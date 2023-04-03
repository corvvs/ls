#include "ls.h"

// path の basename 部分先頭のポインタを返す
const char*	yo_basename(const char* path) {
	char*	last_slash = ft_strrchr(path, '/');
	if (last_slash == NULL) {
		return path;
	} else {
		return last_slash + 1;
	}
}

// path の dirname 部分と basename を結合したものを返す
char*	yo_replace_basename(const char* path, const char* basename) {
	char*	last_slash = ft_strrchr(path, '/');
	if (last_slash == NULL) {
		return ft_strdup(basename);
	}
	size_t	dir_len = last_slash - path;
	size_t	base_len = ft_strlen(basename);
	char*	str = malloc(sizeof(char) * (dir_len + base_len + 1));
	YOYO_ASSERT(str != NULL);
	ft_memcpy(str, path, dir_len);
	ft_memcpy(str + dir_len, basename, base_len);
	str[dir_len + base_len] = '\0';
	return str;
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
