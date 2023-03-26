#ifndef YO_COLOR_H
# define YO_COLOR_H

# include "common.h"

# ifdef __MACH__ 
#  define YO_COLOR_DIR TX_BLU
#  define YO_COLOR_EXE TX_RED
#  define YO_COLOR_GOODLINK TX_MAZ
#  define YO_COLOR_BADLINK TX_MAZ
#  define YO_COLOR_REGULAR ""
# else
#  define YO_COLOR_DIR TX_BLU TX_BLD
#  define YO_COLOR_EXE TX_GRN TX_BLD
#  define YO_COLOR_GOODLINK TX_CYN TX_BLD
#  define YO_COLOR_BADLINK TX_RED TX_BLD
#  define YO_COLOR_REGULAR ""
# endif

#endif
