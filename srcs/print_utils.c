#include "ls.h"
#include "color.h"

void	print_filename(const t_option* option, const t_file_item* item) {
	const char*	color;
	if (!option->color) {
		color = YO_COLOR_REGULAR;
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
	}
	yoyo_dprintf(STDOUT_FILENO, "%s%s%s", color, item->name, TX_RST);
}
