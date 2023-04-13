#include "ls.h"

#define BLOCKSIZE_FOR_LINUX_LS 1024
#ifdef __MACH__
# define COL_PADDING 2
#else
# define COL_PADDING 1
#endif

uint64_t	number_width(uint64_t i) {
	if (i == 0) {
		return 1;
	}
	uint64_t	n = 0;
	while (i) {
		i /= 10;
		n += 1;
	}
	return n;
}

// "Total" 用のブロックサイズの計算
static size_t	subtotal_blocks(const t_file_item* item) {
#ifdef __MACH__
	// if (item->nominal_file_type == YO_FT_DIR) {
	// 	return 0;
	// }
	// if (item->nominal_file_type == YO_FT_LINK) {
	// 	return 0;
	// }
	// // 各ファイルのブロック数を8で割った値を切り上げたものの合計
	// if (item->st.st_blocks == 0) {
	// 	return 0;
	// }
	// return CEIL_BY(item->st.st_blocks, 8);
	return item->st.st_blocks;
#else
	// すべてのファイルに割り当てられたブロック数の合計
	const size_t bytes_by_blocks = item->st.st_blocks * 512;
	return bytes_by_blocks / BLOCKSIZE_FOR_LINUX_LS;
#endif
}

// "Total " 部分の出力
static void	print_total_blocks(t_file_batch* batch, size_t len, t_file_item** items) {
	if (batch->is_root) {
		return;
	}
	size_t total_blocks = 0;
	for (size_t i = 0; i < len; ++i) {
		const size_t	blocks = subtotal_blocks(items[i]);
		total_blocks += blocks;
	}
	yoyo_dprintf(STDOUT_FILENO, "total %zu\n", total_blocks);
}

static void	print_filemode_part(const t_file_batch* batch, const t_file_item* item) {
	(void)batch;
	// DEBUGOUT("[%s] %d %d", item->path, item->actual_file_type, item->nominal_file_type);
	char c;
	// 種別
	if (is_dot_dir(item)) {
		c = 'd';
	} else {
		switch (item->actual_file_type) {
			case YO_FT_REGULAR:
				c = '-';
				break;
			case YO_FT_DIR:
				c = 'd';
				break;
			case YO_FT_CHAR_DEVICE:
				c = 'c';
				break;
			case YO_FT_BLOCK_DEVICE:
				c = 'b';
				break;
			case YO_FT_LINK:
			case YO_FT_BAD_LINK:
				c = 'l';
				break;
			default:
				c = '?';
		}
	}
	yoyo_dprintf(STDOUT_FILENO, "%c", c);
	{ // 所有者
		char perm[4] = "---";
		perm[0] = (item->st.st_mode & S_IRUSR) ? 'r' : '-';
		perm[1] = (item->st.st_mode & S_IWUSR) ? 'w' : '-';
		// 実行権限 と setuid の状態に応じて変わる
		const bool is_x = !!(item->st.st_mode & S_IXUSR);
		const bool is_s = !!(item->st.st_mode & S_ISUID);
		perm[2] = (is_x && is_s)
			? 's' : (!is_x && is_s)
			? 'S' : (is_x && !is_s)
			? 'x' : '-';
		yoyo_dprintf(STDOUT_FILENO, "%s", perm);
	}
	{ // グループ
		char perm[4] = "---";
		perm[0] = (item->st.st_mode & S_IRGRP) ? 'r' : '-';
		perm[1] = (item->st.st_mode & S_IWGRP) ? 'w' : '-';
		// 実行権限 と setgid の状態に応じて変わる
		const bool is_x = !!(item->st.st_mode & S_IXGRP);
		const bool is_s = !!(item->st.st_mode & S_ISGID);
		perm[2] = (is_x && is_s)
			? 's' : (!is_x && is_s)
			? 'S' : (is_x && !is_s)
			? 'x' : '-';
		yoyo_dprintf(STDOUT_FILENO, "%s", perm);
	}
	{ // その他
		char perm[4] = "---";
		perm[0] = (item->st.st_mode & S_IROTH) ? 'r' : '-';
		perm[1] = (item->st.st_mode & S_IWOTH) ? 'w' : '-';
		// 実行権限 と スティッキービット の状態に応じて変わる
		const bool is_x = !!(item->st.st_mode & S_IXOTH);
		const bool is_t = !!(item->st.st_mode & S_ISVTX);
		perm[2] = (is_x && is_t)
			? 't' : (!is_x && is_t)
			? 'T' : (is_x && !is_t)
			? 'x' : '-';
		yoyo_dprintf(STDOUT_FILENO, "%s", perm);
	}
	if (batch->bopt.some_has_acl_xattr) {
#ifdef __MACH__
		if (item->xattr_len > 0) {
			yoyo_dprintf(STDOUT_FILENO, "%c", '@');
		} else if (item->acl != NULL) {
			yoyo_dprintf(STDOUT_FILENO, "%c", '+');
		} else {
			yoyo_dprintf(STDOUT_FILENO, "%c", ' ');
		}
#else
		yoyo_dprintf(STDOUT_FILENO, " ");
#endif
	}
}

// リンク数
static uint64_t	get_link_number(const t_file_item* item) {
	return item->st.st_nlink;
}

// リンク数
static void	print_link_number_part(const t_long_format_measure* measure, const t_file_item* item) {
	const uint64_t n = get_link_number(item);
	const uint64_t w = number_width(n);
	print_spaces(measure->link_number_width - w);
	yoyo_dprintf(STDOUT_FILENO, "%zu", item->st.st_nlink);
}

// 所有者名
static char*	get_owner_name(t_cache* cache, const t_file_item* item) {
	struct passwd*	ud = retrieve_user(cache, item->st.st_uid);
	if (ud == NULL) {
		return NULL;
	}
	return ud->pw_name;
}

// 所有者名
static void	print_owner_name(const t_long_format_measure* measure, t_cache* cache, const t_file_item* item) {
	const char*	name = get_owner_name(cache, item);
	if (name == NULL) {
		return;
	}
	const uint64_t w = ft_strlen(name);
	print_spaces(1);
	yoyo_dprintf(STDOUT_FILENO, "%s", name);
	print_spaces(measure->owner_width - w);
}

// グループ名
static char*	get_group_name(t_cache* cache, const t_file_item* item) {
	struct group*	gd = retrieve_group(cache, item->st.st_gid);
	if (gd == NULL) {
		return NULL;
	}
	return gd->gr_name;
}

// グループ名
static void	print_group_name(const t_long_format_measure* measure, t_cache* cache, const t_file_item* item) {
	const char*	name = get_group_name(cache, item);
	if (name == NULL) {
		return;
	}
	const uint64_t w = ft_strlen(name);
	yoyo_dprintf(STDOUT_FILENO, "%s", name);
	print_spaces(measure->group_width - w);
}

// ファイルサイズ
static uint64_t	get_file_size(const t_file_item* item) {
	return item->st.st_size;
}

static void	print_file_size(t_long_format_measure* measure, const t_file_item* item) {
	const uint64_t n = get_file_size(item);
	const uint64_t w = number_width(n);
	print_spaces(measure->size_width - w + COL_PADDING);
	yoyo_dprintf(STDOUT_FILENO, "%zu", item->st.st_size);
}

static void	measure_datetime(t_long_format_measure* measure) {
#ifdef __MACH__
	measure->mon_width = 2;
#else
	measure->mon_width = 3;
#endif
	measure->day_width = 2;
	measure->year_time_width = 5;
}

static void	print_month(const t_long_format_measure* measure, const t_file_item* item) {
	(void)measure;
	yoyo_dprintf(STDOUT_FILENO, " %s", item->time_str.mon);
}

#ifdef __MACH__
# define NEAR_TIME_DAYS (86400 * (365 + 1) / 2)
#else
# define NEAR_TIME_DAYS (86400 * 365 / 2)
#endif

static void	set_time_str_by_ctime(uint64_t ut_s, t_time_string* time_str) {
	const time_t	t_s = ut_s;
	char*	ct = ctime(&t_s);
#ifdef __MACH__
	if (ct[4] == 'J') {
		if (ct[5] == 'a') {
			// Jan
			ft_strlcpy(time_str->mon, " 1", 3);
		} else if (ct[6] == 'n') {
			// Jun
			ft_strlcpy(time_str->mon, " 6", 3);
		} else {
			// Jul
			ft_strlcpy(time_str->mon, " 7", 3);
		}
	} else if (ct[4] == 'F') {
		// Feb 
		ft_strlcpy(time_str->mon, " 2", 3);
	} else if (ct[4] == 'M') {
		if (ct[6] == 'r') {
			// Mar
			ft_strlcpy(time_str->mon, " 3", 3);
		} else {
			// May
			ft_strlcpy(time_str->mon, " 5", 3);
		}
	} else if (ct[4] == 'A') {
		if (ct[5] == 'p') {
			// Apr
			ft_strlcpy(time_str->mon, " 4", 3);
		} else {
			// Aug
			ft_strlcpy(time_str->mon, " 8", 3);
		}
	} else if (ct[4] == 'S') {
		// Sep
		ft_strlcpy(time_str->mon, " 9", 3);
	} else if (ct[4] == 'O') {
		// Oct
		ft_strlcpy(time_str->mon, "10", 3);
	} else if (ct[4] == 'N') {
		// Nov
		ft_strlcpy(time_str->mon, "11", 3);
	} else if (ct[4] == 'D') {
		// Dec
		ft_strlcpy(time_str->mon, "12", 3);
	}
#else
	ft_strlcpy(time_str->mon, ct + 4, 4);
#endif
	ft_strlcpy(time_str->mday, ct + 8, 3);
	ft_strlcpy(time_str->hour, ct + 11, 3);
	ft_strlcpy(time_str->min, ct + 14, 3);
	ft_strlcpy(time_str->year, ct + 20, 5);
}

static void	print_datetime(const t_long_format_measure* measure, t_master* m, t_file_item* item) {
	uint64_t	ut_s = unixtime_s(m->opt->time_access ? &item->st.ATIME : &item->st.MTIME);

	set_time_str_by_ctime(ut_s, &item->time_str);

	const bool show_years = m->cache.current_unixtime_s < ut_s || (m->cache.current_unixtime_s - ut_s) > NEAR_TIME_DAYS;
	print_month(measure, item);
	{
		yoyo_dprintf(STDOUT_FILENO, " %s", item->time_str.mday);
	}
	if (show_years) {
		yoyo_dprintf(STDOUT_FILENO, "  %s", item->time_str.year);
	} else {
		yoyo_dprintf(STDOUT_FILENO, " %s:%s", item->time_str.hour, item->time_str.min);
	}
}

#ifdef __MACH__
static uint64_t	hex_number_width(uint64_t i) {
	if (i == 0) {
		return 1;
	}
	uint64_t	n = 0;
	while (i) {
		i /= 16;
		n += 1;
	}
	return n;
}

static uint64_t get_device_id_width(const t_file_item* item) {
	return item->st.st_rdev == 0 ? 0 : hex_number_width(item->st.st_rdev) + 2;
}

static void	print_device_id(t_long_format_measure* measure, const t_file_item* item) {
	const uint64_t w = item->st.st_rdev == 0 ? 1 : hex_number_width(item->st.st_rdev) + 2;
	print_spaces(measure->size_width - w + COL_PADDING);
	if (item->st.st_rdev) {
		yoyo_dprintf(STDOUT_FILENO, "0x%lx", item->st.st_rdev);
	} else {
		yoyo_dprintf(STDOUT_FILENO, "%lx", item->st.st_rdev);
	}
}

#else

#include <sys/sysmacros.h>
static uint64_t get_device_id_width(const t_file_item* item) {
	unsigned int	mj = major(item->st.st_rdev);
	unsigned int	mn = minor(item->st.st_rdev);
	return number_width(mj) + 2 + number_width(mn);
}

// デバイスID
static void	print_device_id(t_long_format_measure* measure, const t_file_item* item) {
	unsigned int	mj = major(item->st.st_rdev);
	unsigned int	mn = minor(item->st.st_rdev);
	unsigned int	w = number_width(mj) + 2 + number_width(mn);
	print_spaces(measure->size_width - w + COL_PADDING);
	yoyo_dprintf(STDOUT_FILENO, "%u, %u", mj, mn);
}

#endif

// long-format の出力
void	print_long_format(t_master* m, t_file_batch* batch, size_t len, t_file_item** items, size_t len_mesure, t_file_item** items_measure) {
	// DEBUGOUT("len = %zu", len);
	// ["Total:" の出力]
	print_total_blocks(batch, len, items);
	// [幅の測定]
	t_long_format_measure	measure = {};
	for (size_t i = 0; i < len_mesure; ++i) {
		const t_file_item*	item  = items_measure[i];
		{
			uint64_t	link_number = get_link_number(item);
			measure.link_number_width = MAX(measure.link_number_width, number_width(link_number));
		}
		if (!batch->opt->show_group_only) {
			char*		name = get_owner_name(&m->cache, item);
			if (name != NULL) {
				measure.owner_width = MAX(measure.owner_width, ft_strlen(name));
			}
		}
		{
			char*		name = get_group_name(&m->cache, item);
			if (name != NULL) {
				measure.group_width = MAX(measure.group_width, ft_strlen(name));
			}
		}
		// デバイスID or ファイルサイズ
		if (item->actual_file_type == YO_FT_CHAR_DEVICE || item->actual_file_type == YO_FT_BLOCK_DEVICE) {
			measure.size_width = MAX(measure.size_width, get_device_id_width(item));
		} else {
			uint64_t	size = get_file_size(item);
			measure.size_width = MAX(measure.size_width, number_width(size));
		}
	}
	measure_datetime(&measure);
	batch->bopt.some_quoted = false;
	batch->bopt.some_has_acl_xattr = false;
	for (size_t i = 0; i < len; ++i) {
		t_file_item*	item  = items[i];
		if (item->quote_type != YO_QT_NONE) {
			batch->bopt.some_quoted = true;
		}
		if (item->xattr_len > 0) {
			batch->bopt.some_has_acl_xattr = true;
		}
#ifdef __MACH__
		if (item->acl != NULL) {
			batch->bopt.some_has_acl_xattr = true;
		}
#endif
	}
	// [ファイルごとの出力]
	for (size_t i = 0; i < len; ++i) {
		t_file_item*	item  = items[i];
		print_filemode_part(batch, item);
		print_spaces(COL_PADDING - (batch->bopt.some_has_acl_xattr ? 1 : 0));
		print_link_number_part(&measure, item);
		if (batch->opt->show_group_only) {
			// グループ名
			print_spaces(1);
			print_group_name(&measure, &m->cache, item);
		} else {
			// 所有者名
			print_owner_name(&measure, &m->cache, item);
			print_spaces(COL_PADDING);
			print_group_name(&measure, &m->cache, item);
		}
		// ファイルサイズ or デバイス番号
		if (item->actual_file_type == YO_FT_CHAR_DEVICE || item->actual_file_type == YO_FT_BLOCK_DEVICE) {
			print_device_id(&measure, item);
		} else {
			print_file_size(&measure, item);
		}
		// 日時
		print_datetime(&measure, m, item);
		// 名前
		print_spaces(1);
		if (batch->bopt.some_quoted && item->quote_type == YO_QT_NONE) {
			yoyo_dprintf(STDOUT_FILENO, " ");
		}
		print_filename(batch, item);
		// (optional)リンク先
		if (item->link_to) {
			yoyo_dprintf(STDOUT_FILENO, " -> ");
#ifdef __MACH__
			yoyo_dprintf(STDOUT_FILENO, "%s", item->link_to->name);
#else
			print_filename(batch, item->link_to);
#endif
		}
		yoyo_dprintf(STDOUT_FILENO, "\n");
		m->lines_out += 1;

		// (あれば)拡張属性の方法を詳細に表示
		if (batch->opt->show_xattr && item->xattr_len > 0) {
			print_xattr_lines(m, &measure, item);
		}

		// (あれば)ACLの情報を詳細に表示
#ifdef __MACH__
		if (batch->opt->show_acl && item->acl != NULL) {
			print_acl_lines(m, item);
		}
#endif
	}
}
