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
	int term_width = get_terminal_width();
	if (term_width > 0) {
		// カラム表示
		print_column_format(m, batch, term_width, len, items);
		return;
	}

	// そのまま表示
	for (size_t i = 0; i < len; ++i) {
		t_file_item*	item = items[i];
		print_filename(m->opt, batch, item, true);
		const char*	suffix = i + 1 == len ? "\n" : "  ";
		yoyo_dprintf(STDOUT_FILENO, "%s", suffix);
	}
}

void	output_files(t_master* m, t_file_batch* batch, size_t len, t_file_item** items) {
	(void)m;
	if (m->opt->long_format) {
		print_long_format(m, batch, len, items);
	} else {
		print_regular_format(m, batch, len, items);
	}
}
