#include "ls.h"

int main(int argc, char **argv) {
	// argc がまともな値ではない場合は即座に終了する
	if (argc < 1) {
		return 1;
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
		.cache = {},
	};
	// [lsのメイン処理開始]
	exec_ls(&m, &root);
	// [後始末]
	free(root.path);
	for (size_t i = 0; i < N_CACHE; ++i) {
		if (m.cache.passwd[i].cached) {
			free(m.cache.passwd[i].passwd.pw_name);
		}
	}
	for (size_t i = 0; i < N_CACHE; ++i) {
		if (m.cache.group[i].cached) {
			free(m.cache.group[i].group.gr_name);
		}
	}
}
