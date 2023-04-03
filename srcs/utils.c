#include "ls.h"

const char*	yo_basename(const char* path) {
	char*	last_slash = ft_strrchr(path, '/');
	if (last_slash == NULL) {
		return path;
	} else {
		return last_slash + 1;
	}
}

const char*	yo_starts_with(const char* str, const char* prefix) {
	if (ft_strncmp(str, prefix, ft_strlen(prefix)) == 0) {
		return str + ft_strlen(prefix);
	} else {
		return NULL;
	}
}

size_t	yo_strlen_to(const char* str, int c) {
	size_t	n = 0;
	while (str[n] && str[n] != c) {
		n += 1;
	}
	return n;
}
