#include "ls.h"

static bool	set_short_option(t_global_option* option, char c) {
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
			option->color = YO_COLOR_AUTO;
			break;
		case 'd':
			option->show_dir_as_file = true;
			break;
		default:
			return false;
	}
	return true;
}

static bool	set_long_option(t_global_option* option, char* str) {
	(void)option;
	char*	key_end = str + yo_strlen_to(str, '=');
	char*	value = *key_end ? key_end + 1 : NULL;
	*key_end = 0;
	if (yo_starts_with("color", str) != NULL) {
		if (value == NULL || ft_strcmp(value, "auto") == 0) {
			option->color = YO_COLOR_AUTO;
		} else if (ft_strcmp(value, "none") == 0) {
			option->color = YO_COLOR_NONE;
		} else if (ft_strcmp(value, "always") == 0) {
			option->color = YO_COLOR_ALWAYS;
		} else {
			return false;
		}
	}
	return true;
}

bool	is_short_option(const char* str) {
	return (str[0] == '-' && str[1] != '-' && str[1]);
}

bool	is_long_option(const char* str) {
	return (str[0] == '-' && str[1] == '-' && str[2] != '-' && str[2]);
}

// argc, argv を読み取り, 初期設定を行う
bool	parse_arguments(t_file_batch* batch, int argc, char **argv) {
	batch->opt->tty = isatty(STDOUT_FILENO);
	batch->opt->color = YO_COLOR_NONE;
	int i;
	for (i = 1; i < argc; ++i) {
		char*	s = argv[i];
		if (is_short_option(s)) {
			for (size_t k = 1; s[k]; ++k) {
				if (!set_short_option(batch->opt, s[k])) {
					DEBUGERR("error?: %c", s[k]);
					return false;
				}
			}
		} else if (is_long_option(s)) {
			if (!set_long_option(batch->opt, s + 2)) {
				DEBUGERR("error?: %s", s);
				return false;
			}
		} else {
			break;
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
