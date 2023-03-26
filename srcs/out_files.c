#include "ls.h"

void	output_files(t_master* m, size_t len, t_file_item** items) {
	(void)m;
	for (size_t i = 0; i < len; ++i) {
		t_file_item*	item = items[i];
		const char*	suffix = i + 1 == len ? "\n" : "  ";
		yoyo_dprintf(STDOUT_FILENO, "%s%s", item->name, suffix);
	}
}
