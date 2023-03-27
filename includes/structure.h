#ifndef YO_STRUCTURE_H
# define YO_STRUCTURE_H

# include <stdbool.h>
# include <stdlib.h>
# include <stdint.h>
# include <sys/stat.h>

typedef enum e_filetype {
	YO_FT_REGULAR,
	YO_FT_DIR,
	YO_FT_LINK,
	YO_FT_BAD_LINK,
	YO_FT_OTHER,
	YO_FT_ERROR,
}	t_filetype;

typedef struct	s_option {
	// -l
	bool	long_format;
	// -R
	// サブディレクトリがあれば、再帰的にリスト表示する。
	bool	recursive;
	// -t
	// 表示されるタイムスタンプでソートする。
	bool	sort_by_time;
	// -r
	// ソートの順序を逆にする。
	bool	sort_reverse;
	// -a
	// `.' で始まる名前のファイルをリスト表示に含める
	bool	show_dot_files;
	// -u
	// ソート (-t) やリスト表示 (-l) のとき、修正時刻ではなく最終アクセス時刻を使う。
	bool	time_access;
	// -f
	// Output is not sorted.  This option turns on -a.
	// It also negates the effect of the -r, -S and -t options.
	// As allowed by IEEE Std 1003.1-2008 (“POSIX.1”),
	// this option has no effect on the -d, -l, -R and -s options.
	bool	sort_in_fs;
	// -g
	// where it was used to display the group name in the long (-l) format output.
	bool	show_group;
	// -d
	// Directories are listed as plain files (not searched recursively).
	bool	show_dir_as_file;
	// --col
	// カラーリングする
	bool	color;
}	t_option;

typedef struct s_file_item {
	// ファイルのbasename
	const char*	name;
	// ファイルのパス(相対または絶対)
	const char*	path;
	// シンボリックリンクのリンク先
	char*	link_to;
	// path の長さ
	size_t		path_len;
	// 実質的なファイルの種別
	t_filetype	actual_file_type;
	// 名目上のファイルの種別
	t_filetype	nominal_file_type;
	// stat構造体
	struct stat	st;
	// errno (errnoがマクロなのでerrnoという名前は使えない)
	int			errn;
}	t_file_item;

typedef struct	s_lsls {
	bool		is_root;
	char**		path;
	size_t		len;
	t_option*	opt;
}	t_lsls;

typedef struct	s_master {
	const char*	exec_name;
	t_lsls*		root;
	t_option*	opt;
}	t_master;

#endif
