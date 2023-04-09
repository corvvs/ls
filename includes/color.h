#ifndef YO_COLOR_H
# define YO_COLOR_H

# include "common.h"

# ifdef __MACH__ 
#  define YO_COLOR_DIR TX_BLU
#  define YO_COLOR_DIR_WRITABLE_STICKY "\e[30;42m"
#  define YO_COLOR_DIR_WRITABLE_NON_STICKY TX_GRY BG_YLW
#  define YO_COLOR_UID TX_GRY BG_RED
#  define YO_COLOR_GID TX_GRY BG_CYN
#  define YO_COLOR_EXE TX_RED
#  define YO_COLOR_CHAR_DEVICE TX_BLU BG_YLW
#  define YO_COLOR_BLOCK_DEVICE TX_BLU BG_CYN
#  define YO_COLOR_GOODLINK TX_MAZ
#  define YO_COLOR_BADLINK TX_MAZ
#  define YO_COLOR_REGULAR ""
# else
#  define YO_COLOR_DIR "\e[01;34m"
#  define YO_COLOR_DIR_WRITABLE_STICKY "\e[30;42m"
#  define YO_COLOR_DIR_WRITABLE_NON_STICKY "\e[30;46m"
#  define YO_COLOR_UID "\e[37;41m"
#  define YO_COLOR_GID "\e[30;43m"
#  define YO_COLOR_EXE "\e[01;32m"
#  define YO_COLOR_CHAR_DEVICE "\e[40;33;01m"
#  define YO_COLOR_BLOCK_DEVICE "\e[40;33;01m"
#  define YO_COLOR_GOODLINK "\e[01;36m"
#  define YO_COLOR_BADLINK "\e[40;31;01m"
#  define YO_COLOR_REGULAR ""
# endif

#endif
