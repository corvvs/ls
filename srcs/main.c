#include "ls.h"

int main(int argc, char **argv) {
	// argc がまともな値ではない場合は即座に終了する
	if (argc < 1) {
		exit(1);
	}

	// [初期設定]
	t_option	option = {};
	t_lsls		root = (t_lsls){
		.is_root = true,
		.opt = &option,
	};
	if (!parse_arguments(&root, argc, argv)) {
		yoyo_dprintf(STDERR_FILENO, "error!!\n");
		return 1;
	}
	t_master	m = (t_master){
		.exec_name = yo_basename(argv[0]),
		.root = &root,
		.opt = &option,
	};
	// [lsのメイン処理開始]
	exec_ls(&m, &root);
	// [後始末]
	free(root.path);
}
