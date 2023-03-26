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

// ファイルアイテム(へのポインタ)の配列を, 辞書順にソートする
// time O(len^2), space O(1), stable
// TODO: time O(len log(len))
static void	sort_by_name(size_t len, t_file_item** pointers) {
	for (size_t i = 0; i < len; ++i) {
		for (size_t j = 1; j < len - i; ++j) {
			if (ft_strcmp(pointers[j - 1]->name, pointers[j]->name) > 0) {
				swap_item(&pointers[j - 1], &pointers[j]);
			}
		}
	}
}

// ファイルアイテム(へのポインタ)の配列を, ディレクトリ以外が後に来るようにソートする
// time O(len), space O(1), unstable
// TODO: stable
static void	sort_by_is_dir(size_t len, t_file_item** pointers) {
	size_t	inz = 0;
	for (size_t i = 0; i < len; ++i) {
		while (inz < len && pointers[inz]->actual_file_type != YO_FT_DIR) {
			++inz;
		}
		if (inz == len) {
			break;
		}
		if (pointers[i]->actual_file_type != YO_FT_DIR && inz < i) {
			swap_item(&pointers[i], &pointers[inz]);
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
	DEBUGOUT("path = %s, link_len = %zu, actual_len = %zd, errno = %d, %s", path, link_len, actual_len, errno, strerror(errno));
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
		DEBUGERR("link_to = %s, errno = %d, %s", link_to, errno, strerror(errno));
		item->actual_file_type = YO_FT_BAD_LINK;
	}
	return true;
}

void	exec_ls(t_master* m, t_lsls* ls) {
	t_file_item*	items = malloc(sizeof(t_file_item) * ls->len);
	YOYO_ASSERT(items != NULL);
	t_file_item**	pointers = malloc(sizeof(t_file_item*) * ls->len);
	YOYO_ASSERT(pointers != NULL);

	// ディレクトリを区別して処理すべきか？
	const bool		distinguish_dir = m->opt->recursive || ls->is_root;
	size_t			n_ok = 0;
	size_t			n_dirs = 0;
	// [ファイル情報を読み取る]
	for (size_t i = 0; i < ls->len; ++i) {
		const char* path = ls->path[i];
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
		item->name = yo_basename(path);
		item->path = path;
		item->path_len = ft_strlen(path);
		item->errn = errno;
		if (is_dot_dir(item) && !ls->is_root) {
			item->actual_file_type = YO_FT_REGULAR;
		}
		if (ft == YO_FT_LINK) {
			investigate_simlink(item);
		}
		n_ok += 1;
		// `.`, `..` をディレクトリとして扱うのは, ルートの時だけ.
		if (!distinguish_dir) {
			continue;
		}
		if (item->actual_file_type == YO_FT_DIR) {
			n_dirs += 1;
		}
	}

	// [ファイル情報を(オプションに従って)ソートする]
	sort_by_name(n_ok, pointers);

	// [ファイル情報を ディレクトリ以外 -> ディレクトリ の順にソートする]
	if (distinguish_dir) {
		sort_by_is_dir(n_ok, pointers);
	}

	// [非ディレクトリ情報を出力]
	size_t	n_no_dirs = n_ok - n_dirs;
	// DEBUGOUT("n_ok = %zu, n_dirs = %zu, n_no_dirs = %zu", n_ok, n_dirs, n_no_dirs);
	output_files(m, n_no_dirs, pointers);

	// [ディレクトリ情報を出力]
	output_dirs(m, n_ok, n_dirs, pointers + n_no_dirs);

	// [後始末]
	for (size_t i = 0; i < n_ok; ++i) {
		free(items[i].link_to);
	}
	free(items);
	free(pointers);
}
