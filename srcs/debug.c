#include "ls.h"

void	print_ls(const t_lsls* ls) {
	yoyo_dprintf(STDOUT_FILENO, "len: %zu\n", ls->len);
}
