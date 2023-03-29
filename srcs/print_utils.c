#include "ls.h"
#include "color.h"

static void	print_filename_body(const t_option* option, const t_file_item* item, bool end) {
#ifdef __MACH__
	(void)option;
	(void)end;
	yoyo_dprintf(STDOUT_FILENO, "%s", item->name);
#else
	if (item->quote_type == YO_QT_NONE) {
		if (option->tty) {
			if (end) {
				yoyo_dprintf(STDOUT_FILENO, " %s", item->name);
			} else {
				yoyo_dprintf(STDOUT_FILENO, " %s ", item->name);
			}
		} else {
			yoyo_dprintf(STDOUT_FILENO, "%s", item->name);
		}
	} else if (item->quote_type == YO_QT_DQ) {
		yoyo_dprintf(STDOUT_FILENO, "\"%s\"", item->name);
	} else {
		yoyo_dprintf(STDOUT_FILENO, "'");
		for (size_t i = 0; item->name[i]; ++i) {
			char c = item->name[i];
			if (c == '\'') {
				yoyo_dprintf(STDOUT_FILENO, "'\\%c'", c);
			} else {
				yoyo_dprintf(STDOUT_FILENO, "%c", c);
			}
		}
		yoyo_dprintf(STDOUT_FILENO, "'");
	}
#endif
}

void	print_filename(const t_option* option, const t_file_item* item, bool end) {
	const char*	color;
	const char*	suffix = TX_RST;
	if (!option->color) {
		color = YO_COLOR_REGULAR;
		suffix = "";
	} else if (item->nominal_file_type == YO_FT_DIR) {
		color = YO_COLOR_DIR;
	} else if (item->actual_file_type == YO_FT_LINK) {
		color = YO_COLOR_GOODLINK;
	} else if (item->actual_file_type == YO_FT_BAD_LINK) {
		color = YO_COLOR_BADLINK;
	} else if (item->st.st_mode & S_IXUSR) {
		color = YO_COLOR_EXE;
	} else {
		color = YO_COLOR_REGULAR;
		suffix = "";
	}
	yoyo_dprintf(STDOUT_FILENO, "%s", color);
	print_filename_body(option, item, end);
	yoyo_dprintf(STDOUT_FILENO, "%s", suffix);
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
