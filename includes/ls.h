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
void	exec_ls(t_master* m, t_lsls* ls);

// option.c
bool	parse_arguments(t_lsls* lsls, int argc, char **argv);

// out_files.c
void	output_files(t_master* m, t_lsls* ls, size_t len, t_file_item** items);

// out_dirs.c
void	output_dirs(t_master* m, size_t total_len, size_t len, t_file_item** items);

// printf.c
int		yoyo_dprintf(int fd, const char* format, ...);

// utils.c
const char*	yo_basename(const char* path);

// debug.c
void	print_ls(const t_lsls* ls);

// assert.c
void	yoyo_assert(const char* strexp, bool exp, const char* file, unsigned int line, const char* func);
#define YOYO_ASSERT(exp) yoyo_assert(#exp, exp, __FILE__, __LINE__, __func__)

#endif
