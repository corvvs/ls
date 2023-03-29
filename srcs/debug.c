#include "ls.h"

void	print_ls(const t_file_batch* batch) {
	yoyo_dprintf(STDOUT_FILENO, "len: %zu\n", batch->len);
}
