#include "ls.h"


static void*	extend_buffer(void* buffer, size_t current_len, size_t extended_len) {
	// DEBUGOUT("extending: %zu -> %zu", current_len, extended_len);
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
		yoyo_dprintf(STDERR_FILENO, "%s: %s: %s\n", m->exec_name, dir_path, strerror(errno));
		return;
	}
	// DEBUGOUT("opened dir: %s, %p", dir_path, dir);
	struct dirent	*entry;
	size_t			len = 2;
	char**			names = malloc(sizeof(char*) * len);
	size_t			i = 0;
	YOYO_ASSERT(names != NULL);
	while (true) {
		errno = 0;
		entry = readdir(dir);
		if (entry == NULL) {
			if (errno) {
				yoyo_dprintf(STDERR_FILENO, "%s: %s: %s\n", m->exec_name, dir_path, strerror(errno));
			}
			break;
		}
		// DEBUGOUT("%d: names[%zu] = %s", entry->d_ino, i, entry->d_name);
		if (!m->opt->show_dot_files && entry->d_name[0] == '.') {
			continue;
		}
		char* path = malloc(sizeof(char) * (dir_item->path_len + 1 + ft_strlen(entry->d_name) + 1));
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
			// DEBUGOUT("len -> %zu, %p", len, names);
		}
		names[i] = path;
		++i;
	}
	closedir(dir);
	info.len = i;
	info.path = names;
	exec_ls(m, &info);
	for (size_t i = 0; i < info.len; ++i) {
		free(names[i]);
	}
	free(names);
}

void	output_dirs(t_master* m, size_t total_len, size_t len, t_file_item** items) {
	if (len == 0) {
		return;
	}
	(void)m;
	const bool show_header = total_len > 1;
	const bool has_leading = total_len - len > 0;
	for (size_t i = 0; i < len; ++i) {
		t_file_item*	item = items[i];
		if (has_leading || 0 < i) {
			yoyo_dprintf(STDOUT_FILENO, "\n");
		}
		if (show_header) {
			yoyo_dprintf(STDOUT_FILENO, "%s:\n", item->path);
		}
		output_dir(m, item);
	}
}
