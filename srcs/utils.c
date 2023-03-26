#include "ls.h"

const char*	yo_basename(const char* path) {
	char*	last_slash = ft_strrchr(path, '/');
	if (last_slash == NULL) {
		return path;
	} else {
		return last_slash + 1;
	}
}
