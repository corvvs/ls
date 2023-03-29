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
	} else if (S_ISLNK(st->st_mode)) {
		return YO_FT_LINK;
	} else {
		return YO_FT_OTHER;
	}
}

// 時間 ta, tb の値に基づき pa, pb を入れ替える.
// その後, 比較がこれで十分かどうかを返す.
static bool	swap_by_time(t_global_option* option, t_file_item** pa, t_stat_time* ta, t_file_item** pb, t_stat_time* tb) {
	int64_t diff = unixtime_us(ta) - unixtime_us(tb);
	if ((!option->sort_reverse && diff > 0) || (option->sort_reverse && diff < 0)) {
		// DEBUGOUT("SWAP BY TIME %s, %llu <-> %s, %llu", (*pa)->name, unixtime_us(ta), (*pb)->name, unixtime_us(tb));
		swap_item(pa, pb);
	}
	return diff != 0;
}

// ファイルをオプションに応じてソートする
void	sort_entries(t_global_option* option, bool distinguish_dir, size_t len, t_file_item** pointers) {
	const bool sort_reverse = option->sort_reverse;
	for (size_t i = 0; i < len; ++i) {
		// DEBUGWARN("n = %zu", len - i);
		bool	swapped = false;
		for (size_t j = 1; j < len - i; ++j) {
			t_file_item** pa = &pointers[j - 1];
			t_file_item** pb = &pointers[j];
			// ディレクトリを後, それ以外を前に
			// (-r が効かない)
			if (distinguish_dir) {
				const bool a_is_dir = (*pa)->actual_file_type == YO_FT_DIR;
				const bool b_is_dir = (*pb)->actual_file_type == YO_FT_DIR;
				if (a_is_dir && !b_is_dir) {
					// DEBUGOUT("%s", "SWAP BY DIR");
					swap_item(pa, pb);
					swapped = true;
					continue;
				}
				if (a_is_dir != b_is_dir) {
					continue;
				}
			}
			if (option->sort_in_fs) {
				continue;
			}
			// タイムスタンプ順にソート
			// (-r が効く)
			if (option->sort_by_time) {
				bool over;
				if (!option->time_access) {
					over = swap_by_time(option, &pointers[j], (&pointers[j]->st.MTIME), &pointers[j - 1], (&pointers[j - 1]->st.MTIME));
					if (over) {
						swapped = true;
						continue;
					}
				} else {
					over = swap_by_time(option, &pointers[j], (&pointers[j]->st.ATIME), &pointers[j - 1], (&pointers[j - 1]->st.ATIME));
					if (over) {
						swapped = true;
						continue;
					}
				}
			}
			int diff;
			diff = ft_strcmp(pointers[j - 1]->name, pointers[j]->name);
			if ((!sort_reverse && diff > 0) || (sort_reverse && diff < 0)) {
				swap_item(pa, pb);
				swapped = true;
			}
			if (diff != 0) {
				continue;
			}
		}
		if (!swapped) {
			break;
		}
	}
}

// item が `.` または `..` かどうか
static bool	is_dot_dir(const t_file_item* item) {
	return ft_strncmp(item->name, ".", 1) == 0 || ft_strncmp(item->name, "..", 2) == 0;
}

// シンボリックリンク item のリンク先に関する情報を取得する
static bool	investigate_simlink(t_file_item* item) {
	assert(item->actual_file_type == YO_FT_LINK);
	const char* path = item->path;

	// [リンク先の名前を取得する]
	size_t link_len = item->st.st_size + 1;
	char* link_to = malloc(link_len);
	assert(link_to != NULL);
	if (link_to == NULL) {
		return false;
	}
	errno = 0;
	ssize_t actual_len = readlink(path, link_to, link_len);
	// DEBUGOUT("path = %s, link_len = %zu, actual_len = %zd, errno = %d, %s", path, link_len, actual_len, errno, strerror(errno));
	if (actual_len < 0) {
		item->actual_file_type = YO_FT_BAD_LINK;
		return true;
	}
	link_to[actual_len] = '\0';
	item->link_to = link_to;

	// [リンク先の情報を取得する]
	struct stat	st;
	errno = 0;
	int rv = lstat(link_to, &st);
	if (rv < 0) {
		// DEBUGERR("link_to = %s, errno = %d, %s", link_to, errno, strerror(errno));
		item->actual_file_type = YO_FT_BAD_LINK;
	}
	return true;
}

#ifdef __MACH__
#else
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

static t_quote_type	should_quote_char(char c) {
	// 以下のいずれかを満たすならクオートすべき
	// - 非表示文字である
	// - 次のいずれかの文字である: (sp) ! * \ " ' ? $ # ; < > = & ( ) [ { } ` ^
	return !ft_isprint(c) || !!ft_strchr(" !*\\\"'?$#;<>=&()[{}`^", c);
}
#endif

static void	determine_file_name(const t_master* m, t_file_item* item, const char* path) {
	const char* name = yo_basename(path);
	item->name = name;
	item->path = path;
	item->quote_type = YO_QT_NONE;
	item->path_len = ft_strlen(path);
	if (!m->opt->tty) {
		return;
	}
#ifdef __MACH__
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
	(void)has_dq;
	if (!has_sq && !has_dq && !has_sp && !has_ex_sp) {
		item->quote_type = YO_QT_NONE;
		item->display_len = ft_strlen(name);
	} else if (has_sq && has_sp && !has_ex_sp) {
		item->quote_type =YO_QT_DQ;
		item->display_len = ft_strlen(name) + 2;
	} else if (has_sq && !has_dq && !has_sp && !has_ex_sp) {
		item->quote_type =YO_QT_DQ;
		item->display_len = ft_strlen(name) + 2;
	} else {
		item->quote_type =YO_QT_SQ;
		item->display_len = strquotedlen(name);
	}
#endif
}

void	list_files(t_master* m, t_file_batch* batch) {
	t_file_item*	items = malloc(sizeof(t_file_item) * batch->len);
	YOYO_ASSERT(items != NULL);
	t_file_item**	pointers = malloc(sizeof(t_file_item*) * batch->len);
	YOYO_ASSERT(pointers != NULL);

	// ディレクトリを区別して処理すべきか？
	batch->bopt.distinguish_dir = !m->opt->show_dir_as_file && (m->opt->recursive || batch->is_root);
	size_t			n_ok = 0;
	size_t			n_dirs = 0;
	// [ファイル情報を読み取る]
	batch->bopt.some_quoted = false;
	for (size_t i = 0; i < batch->len; ++i) {
		const char* path = batch->path[i];
		t_file_item*	item = &items[i];

		errno = 0;
		int rv = lstat(path, &item->st);
		if (rv) {
			yoyo_dprintf(STDERR_FILENO, "%s: %s: %s\n", m->exec_name, path, strerror(errno));
			continue;
		}
		t_filetype	ft = determine_file_type(&item->st);
		item->link_to = NULL;
		pointers[n_ok] = item;
		item->actual_file_type = ft;
		item->nominal_file_type = ft;
		item->errn = errno;
		determine_file_name(m, item, path);
		if (item->quote_type != YO_QT_NONE) {
			batch->bopt.some_quoted = true;
		}
		if (is_dot_dir(item) && !batch->is_root) {
			item->actual_file_type = YO_FT_REGULAR;
		}
		if (ft == YO_FT_LINK) {
			investigate_simlink(item);
		}
		n_ok += 1;
		// `.`, `..` をディレクトリとして扱うのは, ルートの時だけ.
		if (!batch->bopt.distinguish_dir) {
			continue;
		}
		if (item->actual_file_type == YO_FT_DIR) {
			n_dirs += 1;
		}
	}
	for (size_t i = 0; i < batch->len; ++i) {
		if (batch->bopt.some_quoted && items[i].quote_type == YO_QT_NONE) {
			items[i].display_len += 2;
		}
	}

	// [ファイル情報を(オプションに従って)ソートする]
	sort_entries(m->opt, batch->bopt.distinguish_dir, n_ok, pointers);

	// [非ディレクトリ情報を出力]
	size_t	n_no_dirs = n_ok - n_dirs;
	output_files(m, batch, n_no_dirs, pointers);

	// [ディレクトリ情報を出力]
	output_dirs(m, n_ok, n_dirs, pointers + n_no_dirs);

	// [後始末]
	for (size_t i = 0; i < n_ok; ++i) {
		free(items[i].link_to);
	}
	free(items);
	free(pointers);
}
