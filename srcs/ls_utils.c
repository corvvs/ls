#include "ls.h"

// この item をファイルセクションに表示するべきかどうか
bool	show_as_files(const t_file_batch* batch, const t_file_item* item) {
	if (batch->opt->show_dir_as_file) {
		return false;
	}

	// DEBUGOUT("path: %s, actual: %d, nominal: %d", item->path, item->actual_file_type, item->nominal_file_type);
	if (item->nominal_file_type != YO_FT_DIR) {
		return false;
	}
	if (item->actual_file_type == YO_FT_LINK) {
		// `-l`ありかつルートの時のみ, 「ディレクトリへのシンボリックリンク」をディレクトリ扱いしない(=展開して表示しない)
		if (batch->opt->long_format && batch->is_root) {
			return false;
		}
#ifdef __MACH__
		// (macOSのみ) `-l`なしかつルートの時, さらに色付けがある時はディレクトリ扱いしない
		if (!batch->opt->long_format && batch->is_root && batch->opt->color != YO_COLOR_NONE) {
			return false;
		}
#else
#endif
	}
	return true;
}

// この item をディレクトリとして展開すべきかどうか
bool	expand_as_dir(const t_file_batch* batch, const t_file_item* item) {
	if (batch->opt->show_dir_as_file) {
		return false;
	}

	if (batch->opt->recursive && !batch->is_root && item->actual_file_type == YO_FT_LINK) {
		return false;
	}

	return show_as_files(batch, item);
}


