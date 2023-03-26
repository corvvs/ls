#include "ls.h"

static bool	set_option(t_option* option, char c) {
	switch (c) {
		case 'l':
			option->show_list = true;
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
			break;
		case 'g':
			option->show_group = true;
			break;
		case 'd':
			option->show_dir_as_file = true;
			break;
		default:
			return false;
	}
	return true;
}

bool	parse_option(t_lsls* lsls, int argc, char **argv) {
	int i;
	for (i = 1; i < argc; ++i) {
		char*	s = argv[i];
		if (*s != '-') {
			// not option
			break;
		}
		for (size_t k = 1; s[k]; ++k) {
			if (!set_option(lsls->opt, s[k])) {
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
	lsls->len = len;
	lsls->path = paths;
	return true;
}
