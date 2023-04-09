#include "ls.h"

static void	swap_item(t_file_item** a, t_file_item** b) {
	t_file_item*	c = *a;
	*a = *b;
	*b = c;
}

static t_filetype	determine_file_type(struct stat* st) {

	if (S_ISREG(st->st_mode)) {
		return YO_FT_REGULAR;
	} else if (S_ISDIR(st->st_mode)) {
		return YO_FT_DIR;
	} else if (S_ISBLK(st->st_mode)) {
		return YO_FT_BLOCK_DEVICE;
	} else if (S_ISCHR(st->st_mode)) {
		return YO_FT_CHAR_DEVICE;
	} else if (S_ISLNK(st->st_mode)) {
		return YO_FT_LINK;
	} else {
		return YO_FT_OTHER;
	}
}

static int	compare_times(const t_stat_time* ta, const t_stat_time* tb) {
	int64_t diff = unixtime_sort(tb) - unixtime_sort(ta);
	return diff > 0 ? +1 : diff < 0 ? -1 : 0;
}

static int	compare_entries(const t_global_option* option, const t_file_item* pa, const t_file_item* pb) {
	if (option->sort_by_time) {
		int diff;
		if (option->time_access) {
			diff = compare_times(&pa->st.ATIME, &pb->st.ATIME);
		} else {
			diff = compare_times(&pa->st.MTIME, &pb->st.MTIME);
		}
		// DEBUGOUT("BY TIME: %s - %s: %d", pa->name, pb->name, diff);
		if (option->sort_reverse) {
			diff = -diff;
		}
		if (diff != 0) {
			return diff;
		}
	}
	int diff = ft_strcmp(pa->name, pb->name);
	if (option->sort_reverse) {
		diff = -diff;
	}
	return diff;
}

// クイックソート
static void	quick_sort(const t_global_option* option, size_t len, t_file_item** pointers) {
	if (len <= 1) {
		return;
	}
	// [ピボットを選ぶ]
	t_file_item* pivot = pointers[len / 2];
	// [ピボットより小さいものを左に、大きいものを右に集める]
	size_t	i = 0;
	size_t	j = len - 1;
	while (i <= j) {
		while (compare_entries(option, pointers[i], pivot) < 0) {
			++i;
		}
		while (compare_entries(option, pointers[j], pivot) > 0) {
			--j;
		}
		if (i <= j) {
			swap_item(&pointers[i], &pointers[j]);
			++i;
			--j;
		}
	}
	// [再帰的にソートする]
	quick_sort(option, j + 1, pointers);
	quick_sort(option, len - i, pointers + i);
}

// ファイルをオプションに応じてソートする
static void	sort_entries(t_global_option* option, size_t len, t_file_item** pointers) {
	if (option->sort_in_fs) {
		return;
	}
	quick_sort(option, len, pointers);
}

// item が `.` または `..` かどうか
bool	is_dot_dir(const t_file_item* item) {
	return item->nominal_file_type == YO_FT_DIR &&
		(ft_strncmp(item->name, ".", 2) == 0 || ft_strncmp(item->name, "..", 3) == 0);
}

static bool	set_item(t_master* m, t_file_batch* batch, char* path, t_file_item* item, bool trace_link);

// シンボリックリンク item のリンク先に関する情報を取得する
static bool	trace_simlink(t_master* m, t_file_batch* batch, t_file_item* link_item, const char* path) {
	// [リンク先の名前を取得する]
	char	name_buf[PATH_MAX + 1];
	errno = 0;
	ssize_t actual_len = readlink(path, name_buf, PATH_MAX);
	// DEBUGOUT("path = %s, link_len = %zu, actual_len = %zd, errno = %d, %s", path, link_len, actual_len, errno, strerror(errno));
	if (actual_len < 0) {
		return false;
	}
	char* link_to = malloc(sizeof(char) * (actual_len + 1));
	YOYO_ASSERT(link_to != NULL);
	if (link_to == NULL) {
		return false;
	}
	ft_memcpy(link_to, name_buf, actual_len + 1);
	// DEBUGOUT("link_len: %zu, actual_len: %zd", link_len, actual_len);
	link_to[actual_len] = '\0';
	// DEBUGOUT("link_to: %p", link_to);
	// DEBUGOUT("link_to: %s", link_to);
	// [リンク先のパスを取得する]
	errno = 0;
	char*	full_link_to = yo_replace_basename(path, link_to);
	YOYO_ASSERT(full_link_to != NULL);
	// DEBUGINFO("%s + %s -> %s", path, link_to, full_link_to);

	// [リンク先の情報を取得する]
	if (set_item(m, batch, full_link_to, link_item, false)) {
		// リンク先がちゃんと追える
	} else {
		// リンク先が追えない
		link_item->actual_file_type = YO_FT_BAD_LINK;
	}
	link_item->name = link_to;
	free(full_link_to);
	// DEBUGINFO("%s -> %s, a %d, n %d", path, link_to, link_item->actual_file_type, link_item->nominal_file_type);
	return true;
}

static size_t	strquotedlen(const char* s) {
	size_t	len = 0;
	for (size_t i = 0; s[i]; ++i) {
		if (s[i] == '\'') {
			len += 3; // シングルクオート2つとバックスラッシュ1つ
		}
		len += 1;
	}
	return len + 2;
}

#ifdef __MACH__
#else

static bool	should_quote_char(char c) {
	// 以下のいずれかを満たすならクオートすべき
	// - 非表示文字である
	// - 次のいずれかの文字である: (sp) ! * \ " ' ? $ # ; < > = & ( ) [ { } ` ^ | ~
	return !ft_isprint(c) || !!ft_strchr(" !*\\\"'?$#;<>=&()[{}`^|~", c);
}
#endif

t_quote_type	determine_quote_type(const t_file_batch* batch, const char* name) {
	if (!batch->opt->tty) {
		return YO_QT_NONE;
	}
#ifdef __MACH__
	(void)name;
	return YO_QT_NONE;
#else
	bool	has_sq = false;
	bool	has_dq = false;
	bool	has_sp = false;
	bool	has_ex_sp = false;
	for (size_t i = 0; name[i]; ++i) {
		if (should_quote_char(name[i])) {
			if (name[i] == '\'') {
				has_sq = true;
			} else if (name[i] == '"') {
				has_dq = true;
			} else if (name[i] == ' ') {
				has_sp = true;
			} else {
				has_ex_sp = true;
			}
		}
	}
	if (!has_sq && !has_dq && !has_sp && !has_ex_sp) {
		return YO_QT_NONE;
	} else if (has_sq && has_sp && !has_ex_sp) {
		return YO_QT_DQ;
	} else if (has_sq && !has_dq && !has_sp && !has_ex_sp) {
		return YO_QT_DQ;
	} else {
		return YO_QT_SQ;
	}
#endif
}

static size_t	determine_name_len(const char* name, t_quote_type qt) {
	switch (qt) {
		case YO_QT_NONE:
			return ft_strlen(name);
		case YO_QT_DQ:
			return ft_strlen(name) + 2;
		case YO_QT_SQ:
		default:
			return strquotedlen(name);
	}
}


static void	determine_file_name(const t_file_batch* batch, t_file_item* item, char* path) {
	char* name = batch->is_root ? path : yo_basename(path);
	item->quote_type = determine_quote_type(batch, name);
	item->name = name;
	item->path = path;
	item->path_len = ft_strlen(path);
	item->display_len = determine_name_len(name, item->quote_type);
}

static bool	set_item(t_master* m, t_file_batch* batch, char* path, t_file_item* item, bool trace_link) {
	ft_bzero(item, sizeof(t_file_item));
	determine_file_name(batch, item, path);
	errno = 0;
	int rv = trace_link ? lstat(path, &item->st) : stat(path, &item->st);
	if (rv) {
		// DEBUGERR("errno = %d, %s", errno, strerror(errno));
		return false;
	}

	// 拡張属性の取得
	if (trace_link && batch->opt->long_format) {
		item->xattr_len = get_xattr_len(path);
	}

#ifdef __MACH__
	// ACLの取得
	if (trace_link && batch->opt->long_format) {
		item->acl = acl_get_link_np(path, ACL_TYPE_EXTENDED);
	}
#endif

	t_filetype	ft = determine_file_type(&item->st);
	item->actual_file_type = ft;
	item->nominal_file_type = ft;
	item->errn = errno;
	if (item->quote_type != YO_QT_NONE) {
		batch->bopt.some_quoted = true;
	}
	if (is_dot_dir(item) && !batch->is_root) {
		item->actual_file_type = YO_FT_REGULAR;
	}
	if (ft == YO_FT_LINK && trace_link) {
		t_file_item*	link_item = ft_calloc(sizeof(t_file_item), 1);
		YOYO_ASSERT(link_item != NULL);
		// DEBUGINFO("item: %p", item);
		// DEBUGINFO("item->path: %s", item->path);
		if (!trace_simlink(m, batch, link_item, item->path)) {
			item->actual_file_type = YO_FT_BAD_LINK;
		}
		if (link_item->actual_file_type == YO_FT_BAD_LINK) {
			item->actual_file_type = YO_FT_BAD_LINK;
		}
		item->nominal_file_type = link_item->actual_file_type;
		item->link_to = link_item;
		// DEBUGOUT("link_item = %s", link_item->name);
	}
	// DEBUGOUT("path = %s, actual = %d, nominal = %d", path, item->actual_file_type, item->nominal_file_type);
	return true;
}

void	list_files(t_master* m, t_file_batch* batch) {
	t_file_item*	items = malloc(sizeof(t_file_item) * batch->len);
	YOYO_ASSERT(items != NULL);
	t_file_item**	pointers = malloc(sizeof(t_file_item*) * batch->len);
	YOYO_ASSERT(pointers != NULL);

	// [ファイル情報を読み取る]
	size_t			n_ok = 0;
	size_t			n_dirs = 0;
	batch->bopt.some_quoted = false;
	for (size_t i = 0; i < batch->len; ++i) {
		char*			path = batch->path[i];
		t_file_item*	item = &items[i];

		if (!set_item(m, batch, path, item, true)) {
			print_error(m, "cannot access", path);
			continue;
		}
		pointers[n_ok] = item;
		n_ok += 1;
		// `.`, `..` をディレクトリとして扱うのは, ルートの時だけ.
		if (expand_as_dir(batch, item)) {
			n_dirs += 1;
		}
		// DEBUGOUT("%u, %s, %zu, %zu", batch->depth, item->name, n_ok, n_dirs);
	}
	if (batch->bopt.some_quoted) {
		for (size_t i = 0; i < batch->len; ++i) {
			if (items[i].quote_type == YO_QT_NONE) {
				items[i].display_len += 2;
			}
		}
	}

	// [ファイル情報を(オプションに従って)ソートする]
	sort_entries(m->opt, n_ok, pointers);


	// DEBUGOUT("%u, %zu, %zu", batch->depth, n_ok, n_dirs);
	// [非ディレクトリ情報を出力]
	output_files(m, batch, n_ok, pointers);

	// [ディレクトリ情報を出力]
	if (batch->is_root || batch->opt->recursive) {
		output_dirs(m, batch, n_ok, n_dirs, pointers);
	}

	// [後始末]
	for (size_t i = 0; i < n_ok; ++i) {
		if (items[i].link_to) {
			free(items[i].link_to->name);
			free(items[i].link_to);
		}
#ifdef __MACH__
		if (items[i].acl != NULL) {
			acl_free(items[i].acl);
		}
#endif
	}
	free(items);
	free(pointers);
}
