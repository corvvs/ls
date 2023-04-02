#include "ls.h"

#ifdef __MACH__
#else

static bool	check_placement(t_master* m, t_file_batch* batch, unsigned int term_width, unsigned int column_number, size_t len, t_file_item** items) {
	if (column_number == 1) {
		return true;
	}
	if (len < column_number) {
		return false;
	}
	size_t			total_len = 0;
	size_t			max_len = 0;
	const size_t	col_cap = CEIL_BY(len, column_number) / column_number;
	size_t			i = 0;
	const size_t	margin = batch->bopt.some_quoted ? 1 : 2;
	while (i < len) {
		size_t	dl = items[i]->display_len;
		if (items[i]->quote_type != YO_QT_NONE) {
			dl += 1;
		}
		if (max_len < dl) {
			max_len = dl;
		}
		i += 1;
		if (i % col_cap == 0 || i == len) {
			// DEBUGOUT("total_len += %zu", max_len);
			total_len += max_len;
			if (i < len) {
				total_len += margin;
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

static size_t	indent(size_t cursor_from, size_t cursor_to) {
	// DEBUGOUT("cursor_from = %zu, cursor_to = %zu", cursor_from, cursor_to);
	while (cursor_from < cursor_to) {
		if (cursor_to / 8 > (cursor_from + 1) / 8) {
			yoyo_dprintf(STDOUT_FILENO, "\t");
			cursor_from += 8 - cursor_from % 8;

		} else {
			yoyo_dprintf(STDOUT_FILENO, " ");
			cursor_from += 1;
		}
	}
	return cursor_to < cursor_from ? cursor_from : cursor_to;
}

void	print_column_format(t_master* m, t_file_batch* batch, unsigned int term_width, size_t len, t_file_item** items) {
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
			size_t	dl = items[k]->display_len;
			if (items[k]->quote_type != YO_QT_NONE) {
				dl += 1;
			}
			if (max_len < dl) {
				max_len = dl;
			}
		}
		max_lens[j] = max_len;
	}

	// 列表示をプリント
	const size_t	margin = batch->bopt.some_quoted ? 1 : 2;
	for (unsigned int i = 0; i < row_number; ++i) {
		// 1行ずつ表示していく
		size_t	cursor = 0;
		size_t	next_start = 0;
		size_t	offset = 0;
		for (unsigned int j = 0; j < column_number; ++j) {
			unsigned int	k = j * row_number + i;
			if (len <= k) {
				break;
			}
			const t_file_item*	item = items[k];
			next_start = offset;
			cursor = indent(cursor, next_start);
			if ((batch->bopt.some_quoted && item->quote_type == YO_QT_NONE ? 1 : 0)) {
				yoyo_dprintf(STDOUT_FILENO, " ");
				cursor += 1;
			}

			const size_t		next_k = k + row_number;
			const bool			end = len <= next_k;
			cursor += print_filename(m->opt, batch, item, end);
			// DEBUGOUT("cursor = %zu, offset = %zu, len = %zu, next_k = %zu, end = %d", cursor, offset, len, next_k, end);
			if (end) {
				continue;
			}
			// DEBUGOUT("cursor = %zu, offset = %zu, max_lens[%d] = %zu", cursor, offset, j, max_lens[j]);
			offset += max_lens[j] + margin;
		}
		yoyo_dprintf(STDOUT_FILENO, "\n");
	}
	free(max_lens);
}

#endif