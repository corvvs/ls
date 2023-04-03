#include "ls.h"
#include "color.h"

static int	print_filename_body(const t_global_option* option, const t_file_item* item) {
	(void)option;
	const char*	name = item->name;
#ifdef __MACH__
	return yoyo_dprintf(STDOUT_FILENO, "%s", name);
#else
	if (item->quote_type == YO_QT_NONE) {
		// クオートなし
		return yoyo_dprintf(STDOUT_FILENO, "%s", name);
	} else if (item->quote_type == YO_QT_DQ) {
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

int	print_filename(const t_file_batch* batch, const t_file_item* item, bool link_to) {
	const t_global_option* option = batch->opt;
	static bool	colored = false;
	const char*	color;
	const char*	suffix = TX_RST;
	const bool	was_colored = colored;
	if (option->color == YO_COLOR_NONE) {
		// 色がない時
		color = YO_COLOR_REGULAR;
		suffix = "";
	} else if (item->nominal_file_type == YO_FT_DIR) {
		color = YO_COLOR_DIR;
		colored = true;
	} else if (item->actual_file_type == YO_FT_LINK) {
		color = YO_COLOR_GOODLINK;
		colored = true;
	} else if (item->actual_file_type == YO_FT_BAD_LINK) {
		color = YO_COLOR_BADLINK;
		colored = true;
	} else if (item->st.st_mode & S_ISUID) {
		color = YO_COLOR_UID;
		colored = true;
	} else if (item->st.st_mode & S_ISGID) {
		color = YO_COLOR_GID;
		colored = true;
	} else if (item->st.st_mode & S_IXUSR) {
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
	if (!link_to && batch->bopt.some_quoted && item->quote_type == YO_QT_NONE) {
		yoyo_dprintf(STDOUT_FILENO, " ");
	}
	if (!was_colored && colored) {
		yoyo_dprintf(STDOUT_FILENO, "%s", TX_RST);
	}
#endif
	yoyo_dprintf(STDOUT_FILENO, "%s", color);
	int size = print_filename_body(option, item);
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

void	print_error(const t_master* m, const char* operation, const char* path) {
	(void)operation;
#ifdef __MACH__
	yoyo_dprintf(STDERR_FILENO, "%s: %s: %s\n", m->exec_name, path, strerror(errno));
#else
	yoyo_dprintf(STDERR_FILENO, "%s: %s '%s': %s\n", m->exec_name, operation, path, strerror(errno));
#endif
}