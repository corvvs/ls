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
	} else if (item->st.st_mode & S_ISUID) {
		// 実行可能な uid が制限されている
		color = YO_COLOR_UID;
		colored = true;
	} else if (item->st.st_mode & S_ISGID) {
		// 実行可能な gid が制限されている
		color = YO_COLOR_GID;
		colored = true;
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
		int rv = write(STDOUT_FILENO, SPACES, m);
		(void)rv;
	}
}

void	print_error(t_master* m, const char* operation, const char* path) {
	(void)operation;
#ifdef __MACH__
	yoyo_dprintf(STDERR_FILENO, "%s: %s: %s\n", m->exec_name, path, strerror(errno));
#else
	yoyo_dprintf(STDERR_FILENO, "%s: %s '%s': %s\n", m->exec_name, operation, path, strerror(errno));
#endif
	m->exit_status = 1;
}