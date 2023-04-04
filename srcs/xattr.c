#include "ls.h"
#include <sys/types.h>
#include <sys/xattr.h>

void	print_xattr_lines(t_master* m, const t_file_item* item) {
	(void)m;
	char*	buf = malloc(sizeof(char) * (item->xattr_len + 1));
	YOYO_ASSERT(buf != NULL);
	ssize_t list_len = listxattr(item->path, buf, item->xattr_len + 1, XATTR_NOFOLLOW);
	if (list_len < 0) {
		return;
	}
	char*	key = buf;
	while (*key) {
		errno = 0;
		ssize_t value_len = getxattr(item->path, key, NULL, 0, 0, 0);
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
	ssize_t	len = listxattr(path, NULL, 0, XATTR_NOFOLLOW);
	if (len < 0) {
		return 0;
	}
	return len;
}
