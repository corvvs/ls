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

void	print_xattr_lines(t_master* m, const t_long_format_measure* measure, const t_file_item* item) {
	(void)m;
	char*	buf = malloc(sizeof(char) * (item->xattr_len + 1));
	YOYO_ASSERT(buf != NULL);
	ssize_t list_len = my_listxattr(item->path, buf, item->xattr_len + 1);
	if (list_len < 0) {
		return;
	}
	char*	key = buf;
	while (*key) {
		// [拡張属性を取得]
		errno = 0;
		ssize_t value_len = my_getxattr(item->path, key, NULL, 0);
		if (value_len < 0) {
			break;
		}
		// [拡張属性を出力]
		// 幅はファイルサイズのものを使う(なんで？)
		const uint64_t w = number_width(value_len);
		const uint64_t mw = measure->size_width;
		yoyo_dprintf(STDOUT_FILENO, "\t%s\t", key);
		if (mw >= w) {
			print_spaces(mw - w);
		}
		yoyo_dprintf(STDOUT_FILENO, "%zu \n", value_len);
		m->lines_out += 1;
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
