#include "ls.h"
#include <sys/types.h>
#include <sys/xattr.h>

ssize_t	my_listxattr(const char* path, char* buf, size_t size) {
#ifdef __MACH__
	return listxattr(path, buf, size, XATTR_NOFOLLOW);
#else
	return llistxattr(path, buf, size);
#endif
}

ssize_t my_getxattr(const char *path, const char *name, void *value, size_t size) {
#ifdef __MACH__
	return getxattr(path, name, value, size, 0, 0);
#else
	return lgetxattr(path, name, value, size);
#endif
}

void	print_xattr_lines(t_master* m, const t_file_item* item) {
	(void)m;
	char*	buf = malloc(sizeof(char) * (item->xattr_len + 1));
	YOYO_ASSERT(buf != NULL);
	ssize_t list_len = my_listxattr(item->path, buf, item->xattr_len + 1);
	if (list_len < 0) {
		return;
	}
	char*	key = buf;
	while (*key) {
		errno = 0;
		ssize_t value_len = my_getxattr(item->path, key, NULL, 0);
		if (value_len < 0) {
			break;
		}
		const uint64_t w = number_width(value_len);
		yoyo_dprintf(STDOUT_FILENO, "\t%s\t ", key);
		if (5 >= w) {
			print_spaces(5 - w);
		}
		yoyo_dprintf(STDOUT_FILENO, "%zu \n", value_len);
		key += ft_strlen(key) + 1;
	}
	free(buf);
}

ssize_t	get_xattr_len(const char* path) {
	ssize_t	len = my_listxattr(path, NULL, 0);
	if (len < 0) {
		return 0;
	}
	return len;
}
