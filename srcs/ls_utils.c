#include "ls.h"

// この item をファイルセクションに表示するべきかどうか
bool	show_in_file_section(const t_file_batch* batch, const t_file_item* item) {
	if (batch->opt->show_everything_as_file && batch->is_root) {
		// `-d`オプションがある場合, ルートの時はなんであっても表示する
		return true;
	}

	if (item->nominal_file_type != YO_FT_DIR) {
		// そもそもディレクトリでないなら表示する
		return true;
	}

	// 以降, ディレクトリまたはディレクトリへのシンボリックリンクの場合

	if (item->actual_file_type == YO_FT_LINK) {
		// ディレクトリへのシンボリックリンクの場合
		// DEBUGOUT("path: %s, actual: %d, nominal: %d", item->path, item->actual_file_type, item->nominal_file_type);

		if (batch->is_root) {
#ifdef __MACH__
			// `-l`か色があるならファイル扱い
			return batch->opt->long_format || batch->opt->color != YO_COLOR_NONE;
#else
			// `-l`ならファイル扱い
			return batch->opt->long_format;
#endif
		}
		return true;
	} else {
		if (is_dot_dir(item)) {
			// `.` または `..` という名前のディレクトリの時
			// -> `-a`がないなら表示しない
			if (!batch->opt->show_dot_files) {
				return false;
			}
		}
		if (batch->is_root) {
			return false;
		}
	}
	return true;
}

// この item をディレクトリとして展開すべきかどうか
bool	expand_as_dir(const t_file_batch* batch, const t_file_item* item) {
	if (batch->opt->show_everything_as_file) {
		// `-d` オプションがある場合は展開しない
		return false;
	}
	if (item->nominal_file_type != YO_FT_DIR) {
		// そもそもディレクトリでも, ディレクトリへのリンクでもないなら展開しない
		return false;
	}

	if (item->actual_file_type == YO_FT_LINK) {
		// ディレクトリへのシンボリックリンクの場合
		if (batch->opt->long_format && batch->is_root) {
			// `-l`ないかルートでない時, 「ディレクトリへのシンボリックリンク」を展開する
			// DEBUGOUT("(1) %s", item->path);
			return false;
		}
		if (batch->opt->recursive && !batch->is_root) {
			// `-R` オプションがあり, ルート指定されておらず, さらにシンボリックリンクの場合は展開しない
			// DEBUGOUT("(2) %s", item->path);
			return false;
		}
		if (!batch->opt->long_format && batch->is_root && batch->opt->color != YO_COLOR_NONE) {
			return false;
		}
	} else if (batch->opt->recursive && !batch->is_root && is_dot_dir(item)) {
		// `-R` オプションがあり, ルート指定されておらず, さらに`.` または `..`の場合は展開しない
		// DEBUGOUT("(3) %s", item->path);
		return false;
	}
	// DEBUGOUT("EXPAND %s", item->path);
	return true;
}


