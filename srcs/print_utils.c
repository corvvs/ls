#include "ls.h"
#include "color.h"

int	print_filename_body(const char*	name, t_quote_type qt) {
#ifdef __MACH__
	(void)qt;
	return yoyo_dprintf(STDOUT_FILENO, "%s", name);
#else
	if (qt == YO_QT_NONE) {
		// クオートなし
		return yoyo_dprintf(STDOUT_FILENO, "%s", name);
	} else if (qt == YO_QT_DQ) {
		// ダブルクオート
		return yoyo_dprintf(STDOUT_FILENO, "\"%s\"", name);
	} else {
		// シングルクオート
		int rv = 0;
		rv += yoyo_dprintf(STDOUT_FILENO, "'");
		for (size_t i = 0; name[i]; ++i) {
			char c = name[i];
			if (c == '\'') {
				rv += yoyo_dprintf(STDOUT_FILENO, "'\\%c'", c);
			} else {
				rv += yoyo_dprintf(STDOUT_FILENO, "%c", c);
			}
		}
		rv += yoyo_dprintf(STDOUT_FILENO, "'");
		return rv;
	}
#endif
}

// ** カラーリングについて **

// [macOS]

//  LSCOLORS The value of this variable describes what color to use for which attribute when colors are enabled with CLICOLOR or COLORTERM.  This string is a concatenation of pairs of the format fb, where f is the
//
//           foreground color and b is the background color.
//
//           The color designators are as follows:
//                 a     black
//                 b     red
//                 c     green
//                 d     brown
//                 e     blue
//                 f     magenta
//                 g     cyan
//                 h     light grey
//                 A     bold black, usually shows up as dark grey
//                 B     bold red
//                 C     bold green
//                 D     bold brown, usually shows up as yellow
//                 E     bold blue
//                 F     bold magenta
//                 G     bold cyan
//                 H     bold light grey; looks like bright white
//                 x     default foreground or background
//
//           Note that the above are standard ANSI colors.  The actual display may differ depending on the color capabilities of the terminal in use.
//
//           The order of the attributes are as follows:
//
//                 1.   directory
//                 2.   symbolic link
//                 3.   socket
//                 4.   pipe
//                 5.   executable
//                 6.   block special
//                 7.   character special
//                 8.   executable with setuid bit set
//                 9.   executable with setgid bit set
//                 10.  directory writable to others, with sticky bit
//                 11.  directory writable to others, without sticky bit
//
//           The default is "exfxcxdxbxegedabagacad", i.e., blue foreground and default background for regular directories, black foreground and red background for setuid executables, etc.

// [Linux]

// # Below are the color init strings for the basic file types.
// # One can use codes for 256 or more colors supported by modern terminals.
// # The default color codes use the capabilities of an 8 color terminal
// # with some additional attributes as per the following codes:
// # Attribute codes:
// # 00=none 01=bold 04=underscore 05=blink 07=reverse 08=concealed
// # Text color codes:
// # 30=black 31=red 32=green 33=yellow 34=blue 35=magenta 36=cyan 37=white
// # Background color codes:
// # 40=black 41=red 42=green 43=yellow 44=blue 45=magenta 46=cyan 47=white
// #NORMAL 00 # no color code at all
// #FILE 00 # regular file: use no color at all
// RESET 0 # reset to "normal" color
// DIR 01;34 # directory
// LINK 01;36 # symbolic link. (If you set this to 'target' instead of a
//  # numerical value, the color is as for the file pointed to.)
// MULTIHARDLINK 00 # regular file with more than one link
// FIFO 40;33 # pipe
// SOCK 01;35 # socket
// DOOR 01;35 # door
// BLK 40;33;01 # block device driver
// CHR 40;33;01 # character device driver
// ORPHAN 40;31;01 # symlink to nonexistent file, or non-stat'able file ...
// MISSING 00 # ... and the files they point to
// SETUID 37;41 # file that is setuid (u+s)
// SETGID 30;43 # file that is setgid (g+s)
// CAPABILITY 30;41 # file with capability
// STICKY_OTHER_WRITABLE 30;42 # dir that is sticky and other-writable (+t,o+w)
// OTHER_WRITABLE 34;42 # dir that is other-writable (o+w) and not sticky
// STICKY 37;44 # dir with the sticky bit set (+t) and not other-writable
// # This is for files with execute permission:
// EXEC 01;32

int	print_filename(const t_file_batch* batch, const t_file_item* item) {
	const t_global_option* option = batch->opt;
	static bool	colored = false;
	const char*	color;
	const char*	suffix = TX_RST;
	const bool	was_colored = colored;
	if (option->color == YO_COLOR_NONE) {
		// 色がない時
		color = YO_COLOR_REGULAR;
		suffix = "";
	} else if (item->actual_file_type == YO_FT_LINK) {
		// 正常なシンボリックリンク
		color = YO_COLOR_GOODLINK;
		colored = true;
	} else if (item->actual_file_type == YO_FT_BAD_LINK) {
		// 不正なシンボリックリンク
		color = YO_COLOR_BADLINK;
		colored = true;
	} else if (item->nominal_file_type == YO_FT_CHAR_DEVICE) {
		// キャラクタデバイス
		color = YO_COLOR_CHAR_DEVICE;
		colored = true;
	} else if (item->nominal_file_type == YO_FT_BLOCK_DEVICE) {
		// ブロックデバイス
		color = YO_COLOR_BLOCK_DEVICE;
		colored = true;
	} else if (item->nominal_file_type == YO_FT_PIPE) {
		// 名前付きパイプ
		color = YO_COLOR_PIPE;
		colored = true;
	} else if (item->nominal_file_type == YO_FT_DOOR) {
		// 名前付きパイプ
		color = YO_COLOR_DOOR;
		colored = true;
	} else if (item->nominal_file_type == YO_FT_SOCKET) {
		// ソケット
		color = YO_COLOR_SOCK;
		colored = true;
	} else if (item->nominal_file_type == YO_FT_DIR) {
		// ディレクトリ
		if (item->st.st_mode & S_IWOTH) {
			// other が書き込み可能なディレクトリ
			// IS DIR and OTHER WRITABLE and STICKY
			if (item->st.st_mode & S_ISVTX) {
				color = YO_COLOR_DIR_WRITABLE_STICKY;
			} else {
				color = YO_COLOR_DIR_WRITABLE_NON_STICKY;
			}
		} else {
			color = YO_COLOR_DIR;
		}
		colored = true;
#ifdef __MACH__
	// 8.   executable with setuid bit set
	// 9.   executable with setgid bit set
	} else if ((item->st.st_mode & S_IXUSR) && (item->st.st_mode & S_ISUID)) {
		// 実行可能 かつ setuid-ed
		color = YO_COLOR_UID;
		colored = true;
	} else if ((item->st.st_mode & S_IXGRP) && (item->st.st_mode & S_ISGID)) {
		// 実行可能 かつ setgid-ed
		color = YO_COLOR_GID;
		colored = true;
#else
	// SETUID 37;41 # file that is setuid (u+s)
	// SETGID 30;43 # file that is setgid (g+s)
	} else if (item->st.st_mode & S_ISUID) {
		// setuid-ed
		color = YO_COLOR_UID;
		colored = true;
	} else if (item->st.st_mode & S_ISGID) {
		// setgid-ed
		color = YO_COLOR_GID;
		colored = true;
#endif
	} else if (item->st.st_mode & S_IXUSR) {
		// オーナーが実行可能
		color = YO_COLOR_EXE;
		colored = true;
	} else if (item->st.st_mode & S_IXGRP) {
		// グループが実行可能
		color = YO_COLOR_EXE;
		colored = true;
	} else {
		color = YO_COLOR_REGULAR;
		suffix = "";
	}
	(void)colored;
	(void)was_colored;
#ifdef __MACH__
#else
	if (!was_colored && colored) {
		yoyo_dprintf(STDOUT_FILENO, "%s", TX_RST);
	}
#endif
	yoyo_dprintf(STDOUT_FILENO, "%s", color);
	int size = print_filename_body(item->name, item->quote_type);
	yoyo_dprintf(STDOUT_FILENO, "%s", suffix);
	return size;
}

#define SPACES "                                                                                                   "

void	print_spaces(uint64_t n) {
	while (n > 0) {
		uint64_t	m = n > 80 ? 80 : n;
		n -= m;
		yoyo_print_direct(STDOUT_FILENO, SPACES, m);
	}
}

void	print_error(t_master* m, const char* operation, const char* path, int status) {
	(void)operation;
#ifdef __MACH__
	(void)status;
	yoyo_dprintf(STDERR_FILENO, "%s: %s: %s\n", m->exec_name, path, strerror(errno));
	// yoyo_dprintf(STDERR_FILENO, "%s: %s %s: %s\n", m->exec_name, operation, path, strerror(errno));
	m->exit_status = 1;
#else
	yoyo_dprintf(STDERR_FILENO, "%s: %s '%s': %s\n", m->exec_name, operation, path, strerror(errno));
	m->exit_status = MAX(m->exit_status, status);
#endif
}

void	print_short_option_error(t_master* m, char c) {
#ifdef __MACH__
	yoyo_dprintf(STDERR_FILENO, "%s: invalid option -- %c\n", m->exec_name, c);
	m->exit_status = 1;
#else
	yoyo_dprintf(STDERR_FILENO, "%s: invalid option -- '%c'\n", m->exec_name, c);
	m->exit_status = MAX(m->exit_status, 2);
#endif
}

void	print_long_option_error(t_master* m, const char* option) {
#ifdef __MACH__
	yoyo_dprintf(STDERR_FILENO, "%s: unrecognized option `%s'\n", m->exec_name, option);
	m->exit_status = 1;
#else
	yoyo_dprintf(STDERR_FILENO, "%s: unrecognized option '%s'\n", m->exec_name, option);
	m->exit_status = MAX(m->exit_status, 2);
#endif
}

