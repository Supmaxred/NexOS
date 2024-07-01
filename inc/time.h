#ifndef TIME_H_
#define TIME_H_

typedef long int time_t;

#define NULL ((void *)0)

struct tm
{
  int tm_sec;			/* Seconds.	[0-60] (1 leap second) */
  int tm_min;			/* Minutes.	[0-59] */
  int tm_hour;			/* Hours.	[0-23] */
  int tm_mday;			/* Day.		[1-31] */
  int tm_mon;			/* Month.	[0-11] */
  int tm_year;			/* Year	- 1900.  */
  int tm_wday;			/* Day of week.	[0-6] */
  int tm_yday;			/* Days in year.[0-365]	*/
  int tm_isdst;			/* DST.		[-1/0/1]*/
};

struct tm* gmtime(const time_t* timer);
struct tm* localtime(const time_t* timer);
time_t mktime(struct tm* timeptr);
char* asctime(const struct tm* time);
char* ctime(const time_t* timer);
time_t timegm(struct tm* timeptr);
time_t timelocal(struct tm* timeptr);
time_t time(time_t* tim);

#endif