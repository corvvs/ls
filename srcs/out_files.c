#include "ls.h"
#include <sys/ioctl.h>

static int	get_terminal_width() {
	// STDOUT が tty でない場合は 0 を返してくるっぽい
	struct winsize ws;
	if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws)) {
		DEBUGERR("%d, %s", errno, strerror(errno));
		return -1;
	}
	return ws.ws_col;
}

static void	print_regular_format(t_master* m, t_file_batch* batch, size_t len, t_file_item** items) {
	if (len == 0) {
		return;
	}
	int term_width = batch->opt->tty ? get_terminal_width() : 0;
	if (term_width > 0) {
		// カラム表示
		print_column_format(m, batch, term_width, len, items);
		return;
	}

	// そのまま表示
	for (size_t i = 0; i < len; ++i) {
		t_file_item*	item = items[i];
		print_filename(batch, item);
		yoyo_dprintf(STDOUT_FILENO, "\n");
		m->lines_out += 1;
	}
}

void	output_files(t_master* m, t_file_batch* batch, size_t len, t_file_item** items) {
	(void)m;
	// if (len == 0) {
	// 	return;
	// }
	t_file_item**	col_items = malloc(sizeof(t_file_item*) * len);
	YOYO_ASSERT(col_items != NULL);
	size_t col_len = 0;
	for (size_t i = 0; i < len; ++i) {
		if (!show_in_file_section(batch, items[i])) {
			continue;
		}
		// DEBUGOUT("%u: show as file: %s", batch->depth, items[i]->path);
		col_items[col_len] = items[i];
		col_len += 1;
	}

	if (m->opt->long_format) {
#ifdef __MACH__
		print_long_format(m, batch, col_len, col_items, col_len, col_items);
#else
		print_long_format(m, batch, col_len, col_items, len, items);
#endif
	} else {
		print_regular_format(m, batch, col_len, col_items);
	}
	free(col_items);
}
