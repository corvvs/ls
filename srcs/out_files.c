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

static bool	check_placement(t_master* m, t_file_batch* batch, unsigned int term_width, unsigned int column_number, size_t len, t_file_item** items) {
	if (column_number == 1) {
		return true;
	}
	if (len < column_number) {
		return false;
	}
	size_t	total_len = 0;
	size_t	max_len = 0;
	size_t	col_cap = CEIL_BY(len, column_number) / column_number;
	size_t	i = 0;
	while (i < len) {
		if (max_len < items[i]->display_len) {
			max_len = items[i]->display_len;
		}
		i += 1;
		if (i % col_cap == 0 || i == len) {
			// DEBUGOUT("total_len += %zu", max_len);
			total_len += max_len;
			if (i < len) {
				if (batch->bopt.some_quoted) {
					total_len += 1;
				} else {
					total_len += 2;
				}
			}
			max_len = 0;
		}
	}
	(void)m;
	// DEBUGOUT("term_width = %u, column_number = %u, total_len = %zu", term_width, column_number, total_len);
	return total_len <= term_width;
}

static unsigned int	determine_column_number(t_master* m, t_file_batch* batch, unsigned int term_width, size_t len, t_file_item** items) {
	unsigned int	max_sufficient = 1;
	unsigned int	min_insufficient = -1;
	unsigned int	n = -1;
	// DEBUGINFO("term_width = %u, len = %zu", term_width, len);
	for (size_t i = 0; i < 40; ++i) {
		unsigned int nn = max_sufficient + (min_insufficient - max_sufficient) / 2;
		if (nn == n) {
			break;
		}
		n = nn;
		// DEBUGOUT("(%u, %u, %u)", max_sufficient, n, min_insufficient);
		if (check_placement(m, batch, term_width, n, len, items)) {
			max_sufficient = n;
		} else {
			min_insufficient = n;
		}
	}
	if (n < 1) {
		n = 1;
	}
	return n;
}

static void	print_column_format(t_master* m, t_file_batch* batch, unsigned int term_width, size_t len, t_file_item** items) {
	unsigned int	column_number = determine_column_number(m, batch, term_width, len, items);
	unsigned int	row_number = CEIL_BY(len, column_number) / column_number;
	// DEBUGINFO("column_number = %u", column_number);

	// 列ごとの最大値を計算
	size_t*	max_lens = malloc(sizeof(size_t) * column_number);
	YOYO_ASSERT(max_lens != NULL);
	for (unsigned int j = 0; j < column_number; ++j) {
		size_t	max_len = 0;
		for (unsigned int i = 0; i < row_number; ++i) {
			unsigned int	k = j * row_number + i;
			if (len <= k) {
				break;
			}
			if (max_len < items[k]->display_len) {
				max_len = items[k]->display_len;
			}
		}
		max_lens[j] = max_len;
	}

	// 列表示をプリント
	for (unsigned int i = 0; i < row_number; ++i) {
		for (unsigned int j = 0; j < column_number; ++j) {
			unsigned int	k = j * row_number + i;
			if (len <= k) {
				break;
			}
			bool end = j + 1 == column_number;
			print_filename(m->opt, batch, items[k], end);
			if (!end) {
				print_spaces(max_lens[j] + 1 - items[k]->display_len);
			}
		}
		yoyo_dprintf(STDOUT_FILENO, "\n");
	}
	free(max_lens);
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
