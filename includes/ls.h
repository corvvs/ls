#ifndef YO_LS_H
# define YO_LS_H

# include "libft.h"
# include "common.h"
# include "structure.h"
# include <unistd.h>
# include <stdlib.h>
# include <dirent.h>
# include <sys/stat.h>
# include <string.h>
# include <errno.h>
# include <assert.h>
# include <time.h>

// stat構造体の時刻フィールド名の差異を吸収するためのマクロ
# ifdef __MACH__
#  define ATIME st_atimespec
#  define CTIME st_ctimespec
#  define MTIME st_mtimespec
typedef struct timespec	t_stat_time;
# else
#  define ATIME st_atim
#  define CTIME st_ctim
#  define MTIME st_mtim
typedef struct timespec	t_stat_time;
# endif

// ls.c
void	list_files(t_master* m, t_file_batch* batch);

// option.c
bool	parse_arguments(t_file_batch* batch, int argc, char **argv);

// out_files.c
void	output_files(t_master* m, t_file_batch* batch, size_t len, t_file_item** items);

// out_dirs.c
void	output_dirs(t_master* m, size_t total_len, size_t len, t_file_item** items);

// print_long_format.c
void	print_long_format(t_master* m, t_file_batch* batch, size_t len, t_file_item** items);

// print_column_linux.c
// print_column_macos.c
void	print_column_format(t_master* m, t_file_batch* batch, unsigned int term_width, size_t len, t_file_item** items);

// print_utils.c
int		print_filename(const t_global_option* option, const t_file_batch* batch, const t_file_item* item, bool end);
void	print_spaces(uint64_t n);
void	print_error(const t_master* m, const char* operation, const char* path);

// printf.c
int		yoyo_dprintf(int fd, const char* format, ...);

// cache.c
struct passwd*	retrieve_user(t_cache* cache, uid_t uid);
struct group*	retrieve_group(t_cache* cache, gid_t gid);

// time.c
uint64_t	unixtime_us(const t_stat_time* ts);
uint64_t	unixtime_s(const t_stat_time* ts);
void		unixtime_to_date_utc(time_t unixtime, struct tm* time_s);
void 		unixtime_to_date_local(time_t unix_time, struct tm* time_s);

// utils.c
const char*	yo_basename(const char* path);

// debug.c
void	print_ls(const t_file_batch* batch);

// assert.c
void	yoyo_assert(const char* strexp, bool exp, const char* file, unsigned int line, const char* func);
#define YOYO_ASSERT(exp) yoyo_assert(#exp, exp, __FILE__, __LINE__, __func__)

#endif
