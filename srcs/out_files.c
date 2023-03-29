#include "ls.h"

static void	print_regular_format(t_master* m, size_t len, t_file_item** items) {
	(void)m;
	for (size_t i = 0; i < len; ++i) {
		t_file_item*	item = items[i];
		print_filename(m->opt, item);
		const char*	suffix = i + 1 == len ? "\n" : "  ";
		yoyo_dprintf(STDOUT_FILENO, "%s", suffix);
	}
}


void	output_files(t_master* m, t_file_batch* batch, size_t len, t_file_item** items) {
	(void)m;
	if (m->opt->long_format) {
		print_long_format(m, batch, len, items);
	} else {
		print_regular_format(m, len, items);
	}
}
