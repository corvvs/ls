#include "ls.h"
#include "color.h"

static void	out_file(t_file_item* item) {
	const char*	color;
	if (item->nominal_file_type == YO_FT_DIR) {
		color = YO_COLOR_DIR;
	} else if (item->actual_file_type == YO_FT_LINK) {
		color = YO_COLOR_GOODLINK;
	} else if (item->actual_file_type == YO_FT_BAD_LINK) {
		color = YO_COLOR_BADLINK;
	} else if (item->st.st_mode & S_IXUSR) {
		color = YO_COLOR_EXE;
	} else {
		color = YO_COLOR_REGULAR;
	}
	yoyo_dprintf(STDOUT_FILENO, "%s%s%s", color, item->name, TX_RST);
}

void	output_files(t_master* m, size_t len, t_file_item** items) {
	(void)m;
	for (size_t i = 0; i < len; ++i) {
		t_file_item*	item = items[i];
		const char*	suffix = i + 1 == len ? "\n" : "  ";
		out_file(item);
		yoyo_dprintf(STDOUT_FILENO, "%s", suffix);
	}
}
