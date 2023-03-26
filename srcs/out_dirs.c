#include "ls.h"


static void*	extend_buffer(void* buffer, size_t current_len, size_t extended_len) {
	DEBUGOUT("extending: %zu -> %zu", current_len, extended_len);
	unsigned char*	extended = malloc(extended_len);
	YOYO_ASSERT(extended != NULL);
	ft_memcpy(extended, buffer, current_len);
	free(buffer);
	return extended;
}


static void	output_dir(t_master* m, const t_file_item* dir_item) {
	const char*	dir_path = dir_item->path;
	t_lsls		info = (t_lsls){
		.is_root = false,
		.opt = m->opt,
		.len = 0,
		.path = NULL,
	};
	errno = 0;
	DIR*	dir = opendir(dir_path);
	if (dir == NULL) {
		DEBUGERR("failed to opendir: %d %s", errno, strerror(errno));
		return;
	}
	DEBUGOUT("opened dir: %s, %p", dir_path, dir);
	struct dirent	*entry;
	size_t			len = 2;
	char**			names = malloc(sizeof(char*) * len);
	size_t			i = 0;
	YOYO_ASSERT(names != NULL);
	while (true) {
		entry = readdir(dir);
		if (entry == NULL) {
			break;
		}
		char* path = malloc(sizeof(char) * (dir_item->path_len + 1 + entry->d_namlen + 1));
		assert(path != NULL);
		char* tpath = path;
		tpath += ft_strlcpy(tpath, dir_item->path, -1);
		tpath += ft_strlcpy(tpath, "/", -1);
		tpath += ft_strlcpy(tpath, entry->d_name, -1);
		if (len <= i) {
			size_t extended_len = len * 2;
			names = extend_buffer(names, len * sizeof(char*), extended_len * sizeof(char*));
			YOYO_ASSERT(names != NULL);
			len = extended_len;
			DEBUGOUT("len -> %zu, %p", len, names);
		}
		names[i] = path;
		DEBUGOUT("names[%zu] = %s", i, path);
		++i;
	}
	closedir(dir);
	info.len = i;
	info.path = names;
	exec_ls(m, &info);
}

void	output_dirs(t_master* m, size_t total_len, size_t len, t_file_item** items) {
	if (len == 0) {
		return;
	}
	(void)m;
	const bool show_header = total_len > 1;
	if (show_header) {
		yoyo_dprintf(STDOUT_FILENO, "\n");
	}
	for (size_t i = 0; i < len; ++i) {
		t_file_item*	item = items[i];
		if (show_header) {
			yoyo_dprintf(STDOUT_FILENO, "%s:\n", item->name);
		}
		output_dir(m, item);
	}
}
