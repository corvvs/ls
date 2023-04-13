#include "ls.h"

#define DAYS_IN_YEAR 365
#define DAYS_IN_YEAR_CENT 36524
#define DAYS_IN_4_YEARS 1461
#define DAYS_IN_4_YEARS_CENT 146097

#ifdef __MACH__
uint64_t	unixtime_us(const t_stat_time* ts) {
	return ts->tv_sec * 1000000 + ts->tv_nsec / 1000;
	// return ts->tv_sec;
}
#else
uint64_t	unixtime_us(const t_stat_time* ts) {
	return ts->tv_sec * 1000000 + ts->tv_nsec / 1000;
}
#endif

#ifdef __MACH__
uint64_t	unixtime_sort(const t_stat_time* ts) {
	return ts->tv_sec * 1000000000 + ts->tv_nsec;
	// return ts->tv_sec;
}
#else
uint64_t	unixtime_sort(const t_stat_time* ts) {
	return ts->tv_sec * 1000000000 + ts->tv_nsec;
}
#endif

uint64_t	unixtime_s(const t_stat_time* ts) {
	return unixtime_us(ts) / 1000000;
}
