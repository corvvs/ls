#include "ls.h"


static void*	extend_buffer(void* buffer, size_t current_len, size_t extended_len) {
	// DEBUGOUT("extending: %zu -> %zu", current_len, extended_len);
	unsigned char*	extended = malloc(extended_len);
	YOYO_ASSERT(extended != NULL);
	ft_memcpy(extended, buffer, current_len);
	free(buffer);
	return extended;
}


static void	output_dir(t_master* m, const t_file_batch* batch, const t_file_item* dir_item) {
	const char*	dir_path = dir_item->path;
	t_file_batch		info = (t_file_batch){
		.is_root = false,
		.depth = batch->depth + 1,
		.opt = m->opt,
		.len = 0,
		.path = NULL,
	};
	// DEBUGINFO("[%u, %s]", info.depth, dir_item->path);
	errno = 0;
	DIR*	dir = opendir(dir_path);
	if (dir == NULL) {
#ifdef __MACH__
				if (batch->opt->long_format) {
					yoyo_dprintf(STDOUT_FILENO, "total 0\n");
				}
#endif
		print_error(m, "reading directory", dir_path);
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
#ifdef __MACH__
				if (batch->opt->long_format) {
					yoyo_dprintf(STDOUT_FILENO, "total 0\n");
				}
				print_error(m, "reading directory", dir_path);
#else
				print_error(m, "reading directory", dir_path);
				if (batch->opt->long_format) {
					yoyo_dprintf(STDOUT_FILENO, "total 0\n");
				}
#endif
			}
			break;
		}
		// DEBUGOUT("%d: names[%zu] = %s", entry->d_ino, i, entry->d_name);
		if (!m->opt->show_dot_files && entry->d_name[0] == '.') {
			continue;
		}
		char* path = malloc(sizeof(char) * (dir_item->path_len + 1 + ft_strlen(entry->d_name) + 1));
		YOYO_ASSERT(path != NULL);
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
		// DEBUGOUT("%u: names[%zu] = %s", batch->depth, i, path);
		++i;
	}
	closedir(dir);
	info.len = i;
	info.path = names;
	list_files(m, &info);
	for (size_t i = 0; i < info.len; ++i) {
		free(names[i]);
	}
	free(names);
}

void	output_dirs(t_master* m, t_file_batch* batch, size_t total_len, size_t dir_len, t_file_item** items) {
	// DEBUGINFO("depth = %u, total_len = %zu, dir_len = %zu", batch->depth, total_len, dir_len);
	if (total_len == 0) {
		return;
	}
	(void)dir_len;
#ifdef __MACH__
	const bool show_header = (batch->is_root && total_len > 1) || (!batch->is_root && total_len > 0);
#else
	const bool show_header = ((batch->is_root && total_len > 1) || (!batch->is_root && total_len > 0)) 
		|| (batch->is_root && batch->opt->recursive && total_len == 1 && dir_len == 1);
#endif
	for (size_t i = 0; i < total_len; ++i) {
		t_file_item*	item = items[i];
		if (!expand_as_dir(batch, item)) {
			// DEBUGOUT("!! %s", item->path);
			continue;
		}
		const bool show_gap = m->lines_out > 0;
		// DEBUGOUT("?? %zu %s %d %d", i, item->path, show_gap, show_header);
		if (show_gap) {
			yoyo_dprintf(STDOUT_FILENO, "\n");
			m->lines_out += 1;
		}
		// DEBUGOUT("HEADER: %d for %s", show_header, item->path);
		if (show_header) {
			// ヘッダ出力
			t_quote_type	quote_type = determine_quote_type(batch, item->path);
			print_filename_body(item->path, quote_type);
			yoyo_dprintf(STDOUT_FILENO, ":\n");
			m->lines_out += 1;
		}
		output_dir(m,  batch, item);
	}
}
