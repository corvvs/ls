#include "ls.h"

#define BLOCKSIZE_FOR_LINUX_LS 1024

// "Total:" 用のブロックサイズの計算
static size_t	subtotal_blocks(const t_file_item* item) {
#ifdef __MACH__
	if (item->nominal_file_type == YO_FT_DIR) {
		return 0;
	}
	if (item->nominal_file_type == YO_FT_LINK) {
		return 0;
	}
	// 各ファイルのブロック数を8で割った値を切り上げたものの合計
	if (item->st.st_blocks == 0) {
		return 0;
	}
	return CEIL_BY(item->st.st_blocks, 8);
#else
	// すべてのファイルに割り当てられたブロック数の合計
	const size_t bytes_by_blocks = item->st.st_blocks * 512;
	return bytes_by_blocks / BLOCKSIZE_FOR_LINUX_LS;
#endif
}

// ファイル別表示用のブロックサイズの計算
static size_t	individual_blocks(const t_file_item* item) {
#ifdef __MACH__
	// 512バイト単位でファイルサイズを切り上げた値
	return CEIL_BY(item->st.st_size, 512) / 512;
#else
	// ファイルが占めるディスク上の領域を、1KB (1024バイト) ごとにまとめたブロック数
	return CEIL_BY(item->st.st_size, BLOCKSIZE_FOR_LINUX_LS) / BLOCKSIZE_FOR_LINUX_LS;
#endif
}

// "Total: " 部分の出力
static void	print_total_blocks(t_lsls* ls, size_t len, t_file_item** items) {
	if (ls->is_root) {
		return;
	}
	size_t total_blocks = 0;
	for (size_t i = 0; i < len; ++i) {
		const size_t	blocks = subtotal_blocks(items[i]);
		total_blocks += blocks;
		// DEBUGOUT("%zu -> total %zu %s", blocks, total_blocks, items[i]->name);
	}
	yoyo_dprintf(STDOUT_FILENO, "total %zu\n", total_blocks);
}

void	print_permission_part(const t_file_item* item) {
	// ディレクトリ
	char c;
	switch (item->actual_file_type) {
		case YO_FT_REGULAR:
			c = '-';
			break;
		case YO_FT_DIR:
			c = 'd';
			break;
		case YO_FT_LINK:
		case YO_FT_BAD_LINK:
			c = 'l';
			break;
		default:
			c = '?';
	}
	yoyo_dprintf(STDOUT_FILENO, "%c", c);
}

// long-format の出力
void	print_long_format(t_master* m, t_lsls* ls, size_t len, t_file_item** items) {
	(void)m;
	print_total_blocks(ls, len, items);
	// ファイルごとの出力
	for (size_t i = 0; i < len; ++i) {
		const t_file_item*	item  = items[i];
		print_permission_part(item);
		yoyo_dprintf(STDOUT_FILENO, " ");
		yoyo_dprintf(STDOUT_FILENO, "%zu ", subtotal_blocks(item));
		yoyo_dprintf(STDOUT_FILENO, "%zu ", individual_blocks(item));
		print_filename(m->opt, item);
		yoyo_dprintf(STDOUT_FILENO, "\n");
	}
}
