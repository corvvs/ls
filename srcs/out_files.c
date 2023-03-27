#include "ls.h"
#include "color.h"

static void	print_filename(const t_option* option, t_file_item* item) {
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

static void	print_regular_format(t_master* m, size_t len, t_file_item** items) {
	(void)m;
	for (size_t i = 0; i < len; ++i) {
		t_file_item*	item = items[i];
		print_filename(m->opt, item);
		const char*	suffix = i + 1 == len ? "\n" : "  ";
		yoyo_dprintf(STDOUT_FILENO, "%s", suffix);
	}
}

#define BLOCKSIZE_FOR_LINUX_LS 1024

static size_t	subtotal_blocks(const t_file_item* item) {
#ifdef __MACH__
	// 各ファイルのブロック数を8で割った値を切り上げたものの合計
	return CEIL_BY(item->st.st_blocks, 8);
#else
	// すべてのファイルに割り当てられたブロック数の合計
	const size_t bytes_by_blocks = item->st.st_blocks * 512;
	return bytes_by_blocks / BLOCKSIZE_FOR_LINUX_LS;
#endif
}

static size_t	individual_blocks(const t_file_item* item) {
#ifdef __MACH__
	// 512バイト単位でファイルサイズを切り上げた値
	return CEIL_BY(item->st.st_blocks, 512) / 512;
#else
	// ファイルが占めるディスク上の領域を、1KB (1024バイト) ごとにまとめたブロック数
	return CEIL_BY(item->st.st_blocks, BLOCKSIZE_FOR_LINUX_LS) / BLOCKSIZE_FOR_LINUX_LS;
#endif
}

static void	print_long_format(t_master* m, t_lsls* ls, size_t len, t_file_item** items) {
	(void)m;
	if (!ls->is_root) {
		size_t total_blocks = 0;
		for (size_t i = 0; i < len; ++i) {
			const size_t	blocks = subtotal_blocks(items[i]);
			total_blocks += blocks;
			// DEBUGOUT("%zu -> total %zu %s", blocks, total_blocks, items[i]->name);
		}
		yoyo_dprintf(STDOUT_FILENO, "total %zu\n", total_blocks);
	}
	for (size_t i = 0; i < len; ++i) {
		yoyo_dprintf(STDOUT_FILENO, "%s blocks: %lu size: %lu\n", items[i]->name, individual_blocks(items[i]), items[i]->st.st_size);
	}
}

void	output_files(t_master* m, t_lsls* ls, size_t len, t_file_item** items) {
	(void)m;
	if (m->opt->long_format) {
		print_long_format(m, ls, len, items);
	} else {
		print_regular_format(m, len, items);
	}
}
