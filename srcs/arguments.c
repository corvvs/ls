#include "ls.h"

static bool	set_short_option(t_global_option* option, char c) {
	switch (c) {
		case 'G':
			option->color = YO_COLOR_AUTO;
			break;
		case 'R':
			option->recursive = true;
			break;
		case 'a':
			option->show_dot_files = true;
			break;
		case 'd':
			option->show_everything_as_file = true;
			break;
		case 'e':
			option->show_acl = true;
			break;
		case 'f':
			option->sort_in_fs = true;
			option->show_dot_files = true;
			break;
		case 'g':
			option->show_group_only = true;
			option->long_format = true;
			break;
		case 'l':
			option->long_format = true;
			break;
		case 'r':
			option->sort_reverse = true;
			break;
		case 't':
			option->sort_by_time = true;
			break;
		case 'u':
			option->time_access = true;
			break;
		case '@':
			option->show_xattr = true;
			break;
		default:
			return false;
	}
	return true;
}

static bool	set_long_option(t_global_option* option, char* str) {
	// --color のみサポートする
	(void)option;
	char*	key_end = str + yo_strlen_to(str, '=');
	char*	value = *key_end ? key_end + 1 : NULL;
	*key_end = 0;
	if (yo_starts_with("color", str) != NULL) {
		if (value == NULL || ft_strcmp(value, "always") == 0) {
			// always
			option->color = YO_COLOR_ALWAYS;
		} else if (ft_strcmp(value, "none") == 0) {
			// none
			option->color = YO_COLOR_NONE;
		} else if (ft_strcmp(value, "auto") == 0) {
			// auto
			if (option->tty) {
				option->color = YO_COLOR_AUTO;
			} else {
				option->color = YO_COLOR_NONE;
			}
		} else {
			return false;
		}
		return true;
	}
	return false;
}

static bool	is_cutoff(const char* str) {
	return (str[0] == '-' && str[1] == '-' && !str[2]);
}

static bool	is_short_option(const char* str) {
	return (str[0] == '-' && str[1] != '-' && str[1]);
}

static bool	is_long_option(const char* str) {
	return (str[0] == '-' && str[1] == '-' && str[2] != '-' && str[2]);
}

#ifdef __MACH__

static int	compare_paths(const char* pa, const char* pb) {
	return ft_strcmp(pa, pb);
}

// paths を文字列でバブルソートする
static void	sort_paths(char** paths, size_t len) {
	if (len < 1) { return; }
	for (size_t i = 0; i < len - 1; ++i) {
		for (size_t j = i + 1; j < len; ++j) {
			if (compare_paths(paths[i], paths[j]) > 0) {
				char*	tmp = paths[i];
				paths[i] = paths[j];
				paths[j] = tmp;
			}
		}
	}
}

#else
#endif

// argc, argv を読み取り, 初期設定を行う
bool	parse_arguments(t_master* m, t_file_batch* batch, int argc, char **argv) {
	batch->opt->tty = isatty(STDOUT_FILENO);
	batch->opt->color = YO_COLOR_NONE;
	int i;
	for (i = 1; i < argc; ++i) {
		char*	s = argv[i];
		if (is_cutoff(s)) {
			i += 1;
			break;
		} else if (is_short_option(s)) {
			for (size_t k = 1; s[k]; ++k) {
				if (!set_short_option(batch->opt, s[k])) {
					print_short_option_error(m, s[k]);
					return false;
				}
			}
		} else if (is_long_option(s)) {
			if (!set_long_option(batch->opt, s + 2)) {
				print_long_option_error(m, s);
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
	// TODO: macOSにおいてのみ, ここで paths を辞書順ソートする
#ifdef __MACH__
	if (!batch->opt->sort_in_fs) {
		sort_paths(paths, len);
	}
#else
#endif
	batch->len = len;
	batch->path = paths;
	return true;
}
