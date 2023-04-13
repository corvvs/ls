#ifndef YO_STRUCTURE_H
# define YO_STRUCTURE_H

# include <stdbool.h>
# include <stdlib.h>
# include <stdint.h>
# include <sys/stat.h>
# include <sys/types.h>
# include <pwd.h>
# include <grp.h>
# include <time.h>
# ifdef __MACH__
#  include <uuid/uuid.h>
# endif

typedef enum	e_filetype {
	YO_FT_REGULAR,
	YO_FT_DIR,
	YO_FT_LINK,
	YO_FT_BAD_LINK,
	YO_FT_CHAR_DEVICE,
	YO_FT_BLOCK_DEVICE,
	YO_FT_SOCKET,
	YO_FT_PIPE,
	YO_FT_DOOR,
	YO_FT_WHITEOUT,
	YO_FT_OTHER,
	YO_FT_ERROR,
}	t_filetype;

typedef enum	e_quote_type {
	// クオートなし
	YO_QT_NONE,
	// ダブルクオーテーションで囲む
	YO_QT_DQ,
	// シングルクオーテーションで囲む
	YO_QT_SQ,
}	t_quote_type;

typedef enum	e_color_option {
	YO_COLOR_NONE,
	YO_COLOR_AUTO,
	YO_COLOR_ALWAYS,
}	t_color_option;

typedef struct	s_batch_option {
	bool	some_quoted;
	bool	some_has_acl_xattr;
}	t_batch_option;

typedef struct	s_global_option {
	bool	tty;
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
	bool	show_group_only;
	// -@
	bool	show_xattr;
	// -e
	bool	show_acl;
	// -d
	// Directories are listed as plain files (not searched recursively).
	bool	show_everything_as_file;
	// --col
	// カラーリングする
	t_color_option	color;
}	t_global_option;


#ifdef __MACH__
# include <sys/types.h>
# include <sys/acl.h>
#endif

typedef struct s_time_string {
	char	mon[4];
	char	mday[3];
	char	year[5];
	char	hour[3];
	char	min[3];
}	t_time_string;

typedef struct s_file_item {
	// ファイルのbasename
	char*	name;
	// ファイル名が表示されるときの長さ
	size_t		display_len;
	// ファイル名をクオートして表示すべきか
	t_quote_type	quote_type;
	// ファイルのパス(相対または絶対)
	const char*	path;
	// シンボリックリンクのリンク先
	struct s_file_item*	link_to;
	// path の長さ
	size_t		path_len;
	// 実質的なファイルの種別
	// 元のファイルがシンボリックリンクだった場合, これはリンク自身のファイルタイプとなる
	t_filetype	actual_file_type;
	// 名目上のファイルの種別
	// 元のファイルがシンボリックリンクだった場合, これはリンク先のファイルタイプとなる
	t_filetype	nominal_file_type;
	// stat構造体
	struct stat	st;
	// 時刻文字列構造体
	t_time_string	time_str;
	// errno (errnoがマクロなのでerrnoという名前は使えない)
	int			errn;

	// 拡張属性のバッファサイズ; これが0の場合は拡張属性を持たないということ
	ssize_t		xattr_len;

#ifdef __MACH__
	acl_t		acl;
#endif
}	t_file_item;

// この名前はないわ
typedef struct	s_file_batch {
	bool				is_root;
	unsigned int		depth;
	char**				path;
	size_t				len;
	t_global_option*	opt;
	t_batch_option		bopt;
}	t_file_batch;

typedef struct	s_passwd_cache {
	bool			cached;
	struct passwd	passwd;
}	t_passwd_cache;

typedef struct	s_group_cache {
	bool			cached;
	struct group	group;
}	t_group_cache;

#define N_CACHE	128

typedef struct	s_cache {
	uint64_t		current_unixtime_s;
	t_passwd_cache	passwd[N_CACHE];
	t_group_cache	group[N_CACHE];
}	t_cache;

typedef struct	s_master {
	const char*	exec_name;
	t_file_batch*		root;
	t_global_option*	opt;
	int					exit_status;
	size_t				lines_out;
	t_cache				cache;
}	t_master;

// long-format における各種寸法
typedef struct	s_long_format_measure {
	uint64_t	link_number_width;
	uint64_t	owner_width;
	uint64_t	group_width;
	uint64_t	size_width;
	uint64_t	mon_width;
	uint64_t	day_width;
	uint64_t	year_time_width;
}	t_long_format_measure;

#endif
