#include "ls.h"

// uid に対応するユーザ情報を返す.
// キャッシュにあればそれを, なければ新たに取得して返す.
// 返り値は free しないこと.
struct passwd*	retrieve_user(t_cache* cache, uid_t uid) {
	t_passwd_cache*	table = cache->passwd;
	int	index = uid % N_CACHE;
	const bool hit = table[index].cached && table[index].passwd.pw_uid == uid;
	if (!hit) {
		if (table[index].cached) {
			free(table[index].passwd.pw_name);
		}
		struct passwd*	ud = getpwuid(uid);
		if (ud == NULL) {
			table[index].cached = true;
			table[index].exists = false;
			char*	name = ft_itoa(uid);
			YOYO_ASSERT(name != NULL);
			table[index].passwd.pw_uid = uid;
			table[index].passwd.pw_name = name;
		} else {
			char*	name = ft_strdup(ud->pw_name);
			YOYO_ASSERT(name != NULL);
			table[index].cached = true;
			table[index].exists = true;
			table[index].passwd = *ud;
			table[index].passwd.pw_name = name;
		}
	}
	return &table[index].passwd;
}

// gid に対応するグループ情報を返す.
// キャッシュにあればそれを, なければ新たに取得して返す.
// 返り値は free しないこと.
struct group*	retrieve_group(t_cache* cache, gid_t gid) {
	t_group_cache*	table = cache->group;
	int	index = gid % N_CACHE;
	const bool hit = table[index].cached && table[index].group.gr_gid == gid;
	if (!hit) {
		if (table[index].cached) {
			free(table[index].group.gr_name);
		}
		struct group*	gd = getgrgid(gid);
		if (gd == NULL) {
			table[index].cached = true;
			table[index].exists = false;
			char*	name = ft_itoa(gid);
			YOYO_ASSERT(name != NULL);
			table[index].group.gr_gid = gid;
			table[index].group.gr_name = name;
		} else {
			char*	name = ft_strdup(gd->gr_name);
			YOYO_ASSERT(name != NULL);
			table[index].cached = true;
			table[index].exists = true;
			table[index].group = *gd;
			table[index].group.gr_name = name;
		}
	}
	// DEBUGINFO("retrieve: %d -> %d -> %s", gid, index, table[index].group.gr_name);
	return &table[index].group;
}
