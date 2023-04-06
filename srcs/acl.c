#ifdef __MACH__

#include "ls.h"
#include <membership.h>

static int parse_guid(acl_entry_t entry, uid_t* id, int* id_type) {
	guid_t*	guid_ptr = (guid_t *)acl_get_qualifier(entry);
	return mbr_uuid_to_id((void*)guid_ptr, id, id_type);
}

static bool	print_acl_file(acl_permset_t* permset) {
	bool	has_leading = false;
	const static acl_perm_t perm_types[] = {
		ACL_READ_DATA,
		ACL_WRITE_DATA,
		ACL_EXECUTE,
		ACL_DELETE,
		ACL_APPEND_DATA,
		ACL_READ_ATTRIBUTES,
		ACL_WRITE_ATTRIBUTES,
		ACL_READ_EXTATTRIBUTES,
		ACL_WRITE_EXTATTRIBUTES,
		ACL_READ_SECURITY,
		ACL_WRITE_SECURITY,
		ACL_CHANGE_OWNER,
	};
	const static char* perm_names[] = {
		"read",
		"write",
		"execute",
		"delete",
		"append",
		"readattr",
		"writeattr",
		"readextattr",
		"writeextattr",
		"readsecurity",
		"writesecurity",
		"chown",
	};
	for (size_t i = 0; i < sizeof(perm_types) / sizeof(perm_types[0]); ++i) {
		if (acl_get_perm_np(*permset, perm_types[i])) {
			yoyo_dprintf(STDOUT_FILENO, "%c%s", has_leading ? ',' : ' ', perm_names[i]);
			has_leading = true;
		}
	}
	return has_leading;
}

static bool	print_acl_dir(acl_permset_t* permset) {
	bool	has_leading = false;
	const static acl_perm_t perm_types[] = {
		ACL_LIST_DIRECTORY,
		ACL_ADD_FILE,
		ACL_SEARCH,
		ACL_ADD_SUBDIRECTORY,
		ACL_DELETE_CHILD,
		ACL_READ_ATTRIBUTES,
		ACL_WRITE_ATTRIBUTES,
		ACL_READ_EXTATTRIBUTES,
		ACL_WRITE_EXTATTRIBUTES,
		ACL_READ_SECURITY,
		ACL_WRITE_SECURITY,
		ACL_CHANGE_OWNER,
		ACL_SYNCHRONIZE,
	};
	const static char* perm_names[] = {
		"list",
		"add_file",
		"search",
		"add_subdirectory",
		"delete_child",
		"readattr",
		"writeattr",
		"readextattr",
		"writeextattr",
		"readsecurity",
		"writesecurity",
		"chown",
		"synch",
	};
	for (size_t i = 0; i < sizeof(perm_types) / sizeof(perm_types[0]); ++i) {
		if (acl_get_perm_np(*permset, perm_types[i])) {
			yoyo_dprintf(STDOUT_FILENO, "%c%s", has_leading ? ',' : ' ', perm_names[i]);
			has_leading = true;
		}
	}
	return has_leading;
}

static bool	print_acl_flags(bool has_leading, acl_entry_t entry) {
	acl_flagset_t flagset;
	acl_get_flagset_np(entry, &flagset);
	const static acl_flag_t flag_types[] = {
		ACL_ENTRY_FILE_INHERIT,
		ACL_ENTRY_DIRECTORY_INHERIT,
		ACL_ENTRY_INHERITED,
		ACL_ENTRY_LIMIT_INHERIT,
		ACL_ENTRY_ONLY_INHERIT,
	};
	const static char* flag_names[] = {
		"file_inherit",
		"directory_inherit",
		"inherited",
		"limit_inherit",
		"only_inherit",
	};
	for (size_t i = 0; i < sizeof(flag_types) / sizeof(flag_types[0]); ++i) {
		if (acl_get_flag_np(flagset, flag_types[i])) {
			yoyo_dprintf(STDOUT_FILENO, "%c%s", has_leading ? ',' : ' ', flag_names[i]);
			has_leading = true;
		}
	}
	return has_leading;
}

static void	process_acl_entry(t_master* m, const t_file_item* item, acl_entry_t entry) {
	{	// 対象
		uid_t	id;
		int		id_type;
		parse_guid(entry, &id, &id_type);
		if (id_type == ID_TYPE_UID) {
			struct passwd* pw = retrieve_user(&m->cache, id);
			yoyo_dprintf(STDOUT_FILENO, "user:%s ", pw->pw_name);
		} else if (id_type == ID_TYPE_GID) {
			struct group* gr = retrieve_group(&m->cache, id);
			yoyo_dprintf(STDOUT_FILENO, "group:%s ", gr->gr_name);
		} else {
			return;
		}
	}

	{	// タグ
		acl_tag_t tag_type;
		acl_get_tag_type(entry, &tag_type);
		switch (tag_type) {
			case ACL_EXTENDED_ALLOW:
				yoyo_dprintf(STDOUT_FILENO, "allow");
				break;
			case ACL_EXTENDED_DENY:
				yoyo_dprintf(STDOUT_FILENO, "deny");
				break;
			default:
				yoyo_dprintf(STDOUT_FILENO, "unknown\n");
				return;
		}
	}

	bool	has_leading;
	{	// パーミッション情報
		acl_permset_t permset;
		acl_get_permset(entry, &permset);
		if (item->nominal_file_type == YO_FT_DIR) {
			has_leading = print_acl_dir(&permset);
		} else {
			has_leading = print_acl_file(&permset);
		}
	}
	{	// フラグ情報
		has_leading = print_acl_flags(has_leading, entry);
	}
	yoyo_dprintf(STDOUT_FILENO, "\n");
	m->lines_out += 1;
}

void	print_acl_lines(t_master* m, const t_file_item* item) {
	acl_entry_t entry;
	int entry_id = ACL_FIRST_ENTRY;
	
	unsigned int	i = 0;
	while (acl_get_entry(item->acl, entry_id, &entry) == 0) {
		yoyo_dprintf(STDOUT_FILENO, " %u: ", i);
		process_acl_entry(m, item, entry);
		entry_id = ACL_NEXT_ENTRY;
		i += 1;
	}
}

#endif
