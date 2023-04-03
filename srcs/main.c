#include "ls.h"

static void	destroy(t_master* m) {
	free(m->root->path);
	for (size_t i = 0; i < N_CACHE; ++i) {
		if (m->cache.passwd[i].cached) {
			free(m->cache.passwd[i].passwd.pw_name);
		}
	}
	for (size_t i = 0; i < N_CACHE; ++i) {
		if (m->cache.group[i].cached) {
			free(m->cache.group[i].group.gr_name);
		}
	}
}

int main(int argc, char **argv) {
	// argc がまともな値ではない場合は即座に終了する
	if (argc < 1) {
		return 1;
	}

	// [初期設定]
	t_global_option	option = {};
	t_file_batch		root = (t_file_batch){
		.is_root = true,
		.opt = &option,
	};
	if (!parse_arguments(&root, argc, argv)) {
		yoyo_dprintf(STDERR_FILENO, "error!!\n");
		return 1;
	}
	t_master	m = (t_master){
#ifdef __MACH__
		.exec_name = yo_basename(argv[0]),
#else
		.exec_name = argv[0],
#endif
		.root = &root,
		.opt = &option,
		.cache = {},
	};
	m.cache.current_unixtime_s = time(NULL);
	// [lsのメイン処理開始]
	list_files(&m, &root);
	// [後始末]
	destroy(&m);
}
