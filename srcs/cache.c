#include "ls.h"

// uid に対応するユーザ情報を返す.
// キャッシュにあればそれを, なければ新たに取得して返す.
// 返り値は free しないこと.
struct passwd*	retrieve_user(t_cache* cache, uid_t uid) {
	t_passwd_cache*	table = cache->passwd;
	int	index = uid % N_CACHE;
	const bool hit = table[index].cached && table[index].passwd.pw_uid == uid;
	if (!hit) {
		struct passwd*	ud = getpwuid(uid);
		if (ud == NULL) {
			DEBUGFATAL("no user: %d", uid);
			return NULL;
		}
		char*	name = ft_strdup(ud->pw_name);
		if (name == NULL) {
			return NULL;
		}
		if (table[index].cached) {
			free(table[index].passwd.pw_name);
		}
		table[index].cached = true;
		table[index].passwd = *ud;
		table[index].passwd.pw_name = name;
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
		struct group*	gd = getgrgid(gid);
		if (gd == NULL) {
			DEBUGFATAL("no group: %d", gid);
			return NULL;
		}
		char*	name = ft_strdup(gd->gr_name);
		if (name == NULL) {
			return NULL;
		}
		if (table[index].cached) {
			free(table[index].group.gr_name);
		}
		table[index].cached = true;
		table[index].group = *gd;
		table[index].group.gr_name = name;
	}
	// DEBUGINFO("retrieve: %d -> %d -> %s", gid, index, table[index].group.gr_name);
	return &table[index].group;
}
