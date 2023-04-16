#include "ls.h"

// uid に対応するユーザ情報を返す.
// キャッシュにあればそれを, なければ新たに取得して返す.
// 返り値は free しないこと.
t_passwd_cache*	retrieve_user(t_cache* cache, uid_t uid) {
	t_passwd_cache*	table = cache->passwd;
	int	index = uid % N_CACHE;
	t_passwd_cache*	entry = &table[index];
	const bool hit = entry->cached && entry->passwd.pw_uid == uid;
	if (!hit) {
		if (entry->cached) {
			free(entry->passwd.pw_name);
		}
		struct passwd*	ud = getpwuid(uid);
		if (ud == NULL) {
			entry->cached = true;
			entry->exists = false;
			char*	name = ft_itoa(uid);
			YOYO_ASSERT(name != NULL);
			entry->passwd.pw_uid = uid;
			entry->passwd.pw_name = name;
			entry->name = name;
			entry->name_len = ft_strlen(name);
		} else {
			char*	name = ft_strdup(ud->pw_name);
			YOYO_ASSERT(name != NULL);
			entry->cached = true;
			entry->exists = true;
			entry->passwd = *ud;
			entry->passwd.pw_name = name;
			entry->name = name;
			entry->name_len = ft_strlen(name);
		}
	}
	return entry;
}

// gid に対応するグループ情報を返す.
// キャッシュにあればそれを, なければ新たに取得して返す.
// 返り値は free しないこと.
t_group_cache*	retrieve_group(t_cache* cache, gid_t gid) {
	t_group_cache*	table = cache->group;
	int	index = gid % N_CACHE;
	t_group_cache*	entry = &table[index];
	const bool hit = entry->cached && entry->group.gr_gid == gid;
	if (!hit) {
		if (entry->cached) {
			free(entry->group.gr_name);
		}
		struct group*	gd = getgrgid(gid);
		if (gd == NULL) {
			entry->cached = true;
			entry->exists = false;
			char*	name = ft_itoa(gid);
			YOYO_ASSERT(name != NULL);
			entry->group.gr_gid = gid;
			entry->group.gr_name = name;
			entry->name = name;
			entry->name_len = ft_strlen(name);
		} else {
			char*	name = ft_strdup(gd->gr_name);
			YOYO_ASSERT(name != NULL);
			entry->cached = true;
			entry->exists = true;
			entry->group = *gd;
			entry->group.gr_name = name;
			entry->name = name;
			entry->name_len = ft_strlen(name);
		}
	}
	// DEBUGINFO("retrieve: %d -> %d -> %s", gid, index, table[index].group.gr_name);
	return entry;
}
