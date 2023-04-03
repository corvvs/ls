#include "ls.h"

#ifdef __MACH__

static bool	check_placement(t_master* m, t_file_batch* batch, unsigned int term_width, unsigned int column_number, size_t len, t_file_item** items) {
	(void)batch;
	if (column_number == 1) {
		return true;
	}
	if (len < column_number) {
		return false;
	}
	size_t	max_len = 0;
	size_t	i = 0;
	while (i < len) {
		if (max_len < items[i]->display_len) {
			max_len = items[i]->display_len;
		}
		i += 1;
	}
	// 色があれば8, なければ2
	const size_t	tab_size = batch->opt->color != YO_COLOR_NONE ? 2 : 8;
	size_t			total_len = CEIL_BY(max_len + 1, tab_size) * column_number;
	(void)m;
	// DEBUGOUT("term_width = %u, column_number = %u, max_len = %u, total_len = %zu", term_width, column_number, max_len, total_len);
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

void	print_column_format(t_master* m, t_file_batch* batch, unsigned int term_width, size_t len, t_file_item** items) {
	const unsigned int	column_number = determine_column_number(m, batch, term_width, len, items);
	const unsigned int	row_number = CEIL_BY(len, column_number) / column_number;
	// DEBUGINFO("column_number = %u", column_number);

	// 全体の最大値を求める
	size_t	max_len = 0;
	for (size_t i = 0; i < len; ++i) {
		if (max_len < items[i]->display_len) {
			max_len = items[i]->display_len;
		}
	}
	// 色があれば1, なければ8
	const size_t	tab_size = batch->opt->color != YO_COLOR_NONE ? 1 : 8;
	const size_t	term_tabs = CEIL_BY(max_len + 1, tab_size) / tab_size;

	// 列表示をプリント
	for (unsigned int i = 0; i < row_number; ++i) {
		for (unsigned int j = 0; j < column_number; ++j) {
			unsigned int	k = j * row_number + i;
			if (len <= k) {
				break;
			}
			bool row_end = j + 1 == column_number || len <= k + row_number;
			print_filename(m->opt, batch, items[k], row_end);
			if (!row_end) {
				if (batch->opt->color != YO_COLOR_NONE) {
					// 色がついている時はスペース
					unsigned int spaces = term_tabs * tab_size - items[k]->display_len;
					while (spaces--) {
						yoyo_dprintf(STDOUT_FILENO, " ");
					}
				} else {
					unsigned int tabs = term_tabs - items[k]->display_len / 8;
					while (tabs--) {
						yoyo_dprintf(STDOUT_FILENO, "\t");
					}
				}
			}
		}
		yoyo_dprintf(STDOUT_FILENO, "\n");
	}
}

#else
#endif