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

uint64_t	unixtime_s(const t_stat_time* ts) {
	return unixtime_us(ts) / 1000000;
}

void unixtime_to_date_utc(time_t unix_time, struct tm* time_s) {
	time_t	ss = unix_time % 60;
	unix_time = (unix_time - ss) / 60;
	time_t	mi = unix_time % 60;
	unix_time = (unix_time - mi) / 60;
	time_t	hh = unix_time % 24;
	unix_time = (unix_time - hh) / 24;
	// この時点で, unix_time は日数単位になっている
	time_t	j = unix_time + 2472632;
	time_t	g = (j / DAYS_IN_4_YEARS_CENT);
	time_t	dg = j % DAYS_IN_4_YEARS_CENT;
	time_t	c = (((dg / DAYS_IN_YEAR_CENT) + 1) * 3 / 4);
	time_t	dc = dg - c * DAYS_IN_YEAR_CENT;
	time_t	b = (dc / DAYS_IN_4_YEARS);
	time_t	db = dc % DAYS_IN_4_YEARS;
	time_t	a = (((db / DAYS_IN_YEAR) + 1) * 3 / 4);
	time_t	da = db - a * DAYS_IN_YEAR;
	time_t	y = g * 400 + c * 100 + b * 4 + a;
	time_t	m = ((da * 5 + 308) / 153) - 2;
	time_t	d = da - ((m + 4) * 153 / 5) + 122;
	time_t	yyyy = y - 4800 + ((m + 2) / 12);
	time_t	mm = (m + 2) % 12 + 1;
	time_t	dd = d + 1;

	time_s->tm_year = yyyy;
	time_s->tm_mon = mm;
	time_s->tm_mday = dd;
	time_s->tm_hour = hh;
	time_s->tm_min = mi;
	time_s->tm_sec = ss;
}

// unix_time からローカルの時刻情報を取得する
// localtime を使っていることに注意
// (使用可能関数の中にタイムゾーン情報を得られるものがないため)
void unixtime_to_date_local(time_t unix_time, struct tm* time_s) {
	*time_s = *localtime(&unix_time);
}
