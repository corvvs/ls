#include "ls.h"

int main(int argc, char **argv) {
	if (argc < 1) {
		exit(1);
	}

	t_option	option = {};
	t_lsls		root = (t_lsls){
		.is_root = true,
		.opt = &option,
	};
	if (!parse_option(&root, argc, argv)) {
		yoyo_dprintf(STDERR_FILENO, "error!!\n");
		return 1;
	}
	t_master	m = (t_master){
		.exec_name = yo_basename(argv[0]),
		.root = &root,
		.opt = &option,
	};
	exec_ls(&m, &root);
}
