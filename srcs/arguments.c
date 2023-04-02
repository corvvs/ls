#include "ls.h"

static bool	set_option(t_global_option* option, char c) {
	switch (c) {
		case 'l':
			option->long_format = true;
			break;
		case 'R':
			option->recursive = true;
			break;
		case 't':
			option->sort_by_time = true;
			break;
		case 'r':
			option->sort_reverse = true;
			break;
		case 'a':
			option->show_dot_files = true;
			break;
		case 'u':
			option->time_access = true;
			break;
		case 'f':
			option->sort_in_fs = true;
			option->show_dot_files = true;
			break;
		case 'g':
			option->show_group = true;
			break;
		case 'G':
			option->color = true;
			break;
		case 'd':
			option->show_dir_as_file = true;
			break;
		default:
			return false;
	}
	return true;
}

// argc, argv を読み取り, 初期設定を行う
bool	parse_arguments(t_file_batch* batch, int argc, char **argv) {
	batch->opt->tty = isatty(STDOUT_FILENO);
	int i;
	for (i = 1; i < argc; ++i) {
		char*	s = argv[i];
		if (*s != '-') {
			// オプションでない文字列を感知 -> これ以降をファイル名とみなす
			break;
		}
		for (size_t k = 1; s[k]; ++k) {
			if (!set_option(batch->opt, s[k])) {
				DEBUGERR("error?: %c", s[k]);
				return false;
			}
		}
	}
	size_t len = argc - i;
	const bool	no_file_specified = len == 0;
	if (no_file_specified) {
		len += 1;
	}
	char**	paths = malloc(sizeof(char*) * len);
	if (!paths) { return false; }
	if (no_file_specified) {
		paths[0] = ".";
	} else {
		for (int j = 0; i < argc; ++i, ++j) {
			paths[j] = argv[i];
		}
	}
	batch->len = len;
	batch->path = paths;
	return true;
}
