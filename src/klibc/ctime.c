#include <time.h>
#include <stdint.h>
#include <ke.h>

//gmtime
#define SECMIN 60
#define SECHOUR 3600
#define SECDAY 86400
#define MONPERYEAR 12
#define DIFFTIME 0x19db1ded53e8000ULL
#define DIFFDAYS (3 * DAYSPER100YEARS + 17 * DAYSPER4YEARS + 1 * DAYSPERYEAR)
#define DAYSPERYEAR 365
#define DAYSPER4YEARS (4*DAYSPERYEAR+1)
#define DAYSPER100YEARS (25*DAYSPER4YEARS-1)
#define DAYSPER400YEARS (4*DAYSPER100YEARS+1)
#define SECONDSPERDAY (24*60*60)
#define SECONDSPERHOUR (60*60)
#define LEAPDAY 59

static struct tm tm_buf;

static char at_buf[26];

static int32_t timezone = SECHOUR * 3;

static unsigned int g_monthdays[13] = { 0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334, 365 };
static unsigned int g_lpmonthdays[13] = { 0, 31, 60, 91, 121, 152, 182, 213, 244, 274, 305, 335, 366 };

time_t time(time_t* tim)
{
    time_t curtime = ke_systime;

    if(tim != 0)
        *tim = curtime;
    
    return curtime;
}

char* ctime(const time_t* timer)
{
    return asctime(localtime(timer));
}

struct tm* localtime(const time_t* timer)
{
    uint32_t curtm = *timer;
    time_t lt = curtm + timezone;
    return gmtime(&lt);
}

static inline long leapyears_passed(long days)
{
    long quadcenturies, centuries, quadyears;
    quadcenturies = days / DAYSPER400YEARS;
    days -= quadcenturies;
    centuries = days / DAYSPER100YEARS;
    days += centuries;
    quadyears = days / DAYSPER4YEARS;
    return quadyears - centuries + quadcenturies;
}

static inline long leapdays_passed(long days)
{
    return leapyears_passed(days + DAYSPERYEAR - LEAPDAY + 1);
}

static struct tm* _gmtime_raw(struct tm* ptm, time_t time, int do_dst)
{
    unsigned int days, daystoyear, dayinyear, leapdays, leapyears, years, month;
    unsigned int secondinday, secondinhour;
    unsigned int* padays;

    if (time < 0)
        return 0;

    /* Divide into date and time */
    days = (unsigned int)(time / SECONDSPERDAY);
    secondinday = time % SECONDSPERDAY;

    /* Shift to days from 1.1.1601 */
    days += DIFFDAYS;

    /* Calculate leap days passed till today */
    leapdays = leapdays_passed(days);

    /* Calculate number of full leap years passed */
    leapyears = leapyears_passed(days);

    /* Are more leap days passed than leap years? */
    if (leapdays > leapyears)
    {
        /* Yes, we're in a leap year */
        padays = g_lpmonthdays;
    }
    else
    {
        /* No, normal year */
        padays = g_monthdays;
    }

    /* Calculate year */
    years = (days - leapdays) / 365;
    ptm->tm_year = years - 299;

    /* Calculate number of days till 1.1. of this year */
    daystoyear = years * 365 + leapyears;

    /* Calculate the day in this year */
    dayinyear = days - daystoyear;

    /* Shall we do DST corrections? */
    ptm->tm_isdst = 0;
    //if (do_dst)
    //{
    //    int yeartime = dayinyear * SECONDSPERDAY + secondinday;
    //    if (yeartime >= dst_begin && yeartime <= dst_end) // FIXME! DST in winter
    //    {
    //        time -= _dstbias;
    //        days = (unsigned int)(time / SECONDSPERDAY + DIFFDAYS);
    //        dayinyear = days - daystoyear;
    //        ptm->tm_isdst = 1;
    //    }
    //}

    ptm->tm_yday = dayinyear;

    /* dayinyear < 366 => terminates with i <= 11 */
    for (month = 0; dayinyear >= padays[month + 1]; month++);

    /* Set month and day in month */
    ptm->tm_mon = month;
    ptm->tm_mday = 1 + dayinyear - padays[month];

    /* Get weekday */
    ptm->tm_wday = (days + 1) % 7;

    /* Calculate hour and second in hour */
    ptm->tm_hour = secondinday / SECONDSPERHOUR;
    secondinhour = secondinday % SECONDSPERHOUR;

    /* Calculate minute and second */
    ptm->tm_min = secondinhour / 60;
    ptm->tm_sec = secondinhour % 60;

    return ptm;
}

struct tm* gmtime(const time_t* ptime)
{
    if (!ptime)
        return NULL;

    return _gmtime_raw(&tm_buf, *ptime, 0);
}

static void _buf_app(char** buf, const char* str, int len)
{
    while (*str && **buf && len)
    {
        *((*buf)++) = *(str++);
        
        len--;
    }
}

void _buf_appint(char** buf, int num, int width) {
    char temp[10];
    int i = 0;
    while (num > 0 || i < width) {
        temp[i++] = '0' + num % 10;
        num /= 10;
    }
    while (i < width) {
        temp[i++] = '0';
    }
    while (i > 0) {
        _buf_app(buf, &temp[--i], 1);
    }
}

char* asctime(const struct tm* time)
{
    for (int i = 0; i < 25; i++) {
        at_buf[i] = ' ';
    }

    at_buf[25] = '\0';

    char* buf = at_buf;

    _buf_app(&buf, &"SunMonTueWedThuFriSat"[time->tm_wday * 3], 3);
    _buf_app(&buf, " ", 1);
    _buf_app(&buf, &"JanFebMarAprMayJunJulAugSepOctNovDec"[time->tm_mon * 3], 3);
    _buf_app(&buf, " ", 1);

    _buf_appint(&buf, time->tm_mday, 2);
    _buf_app(&buf, " ", 1);

    _buf_appint(&buf, time->tm_hour, 2);
    _buf_app(&buf, ":", 1);
    _buf_appint(&buf, time->tm_min, 2);
    _buf_app(&buf, ":", 1);
    _buf_appint(&buf, time->tm_sec, 2);
    _buf_app(&buf, " ", 1);

    _buf_appint(&buf, time->tm_year + 1900, 4);

    _buf_app(&buf, "\n", 1);

    return at_buf;
}

/*
 *------------------------------------------------------------------
 * tm_cmp --
 *
 *      Compare two `struct tm's.
 *
 * Results:
 *      0 if the times are the same.
 *      Positive if t1 is later than t2.
 *      Negative if t1 is earlier than t2.
 *
 * Side effects:
 *      None.
 *
 *-----------------------------------------------------------------
 */

static int tm_cmp(struct tm* t1, struct tm* t2)
{
    int diff;

    if ((diff = t1->tm_year - t2->tm_year) != 0)
        return diff;

    if ((diff = t1->tm_mon - t2->tm_mon) != 0)
        return diff;

    if ((diff = t1->tm_mday - t2->tm_mday) != 0)
        return diff;

    if ((diff = t1->tm_hour - t2->tm_hour) != 0)
        return diff;

    if ((diff = t1->tm_min - t2->tm_min) != 0)
        return diff;

    if ((diff = t1->tm_sec - t2->tm_sec) != 0)
        return diff;

    return 0;
}


/*
 *----------------------------------------------------------------------
 *
 * mktime --
 *
 *      Convert a local time, contained in a `struct *tp' into a
 *      calander time in the same representation used by time(2).
 *
 * Results:
 *      Returns the calander time, or -1 if it cannot be represented.
 *
 * Side effects:
 *      Trashes localtime's internal buffer.
 *
 *----------------------------------------------------------------------
 */

struct tm* gmtime_custom(const time_t* timep);

time_t mktime(struct tm* timeptr)
{
    register int i, r;
    time_t t = 0;

    struct tm temp = *timeptr; //Make a local copy just in case tp points to localtime's buffer

    if (tm_cmp(&temp, localtime(&t)) == 0) //Check for zero
        return 0;

    // Do a binary search, until the right time is found
    for (i = 31; --i >= 0;) // Start with high bit, work toward zero
    {
        t |= 1L << i; // Set bit, then compare times

        if ((r = tm_cmp(&temp, localtime(&t))) < 0) // If t is later than temp
            t &= ~(1L << i); // t is later, so clear the bit */
        else if (r == 0)
            return t; // Finded time, so return */
    }

    // If we get here, then tp didn't point to valid data.
    return -1;
}

time_t timegm(struct tm* timeptr)
{
    register int i, r;
    time_t t = 0;

    struct tm temp = *timeptr; //Make a local copy just in case tp points to localtime's buffer

    if (tm_cmp(&temp, gmtime(&t)) == 0) //Check for zero
        return 0;

    // Do a binary search, until the right time is found
    for (i = 31; --i >= 0;) // Start with high bit, work toward zero
    {
        t |= 1L << i; // Set bit, then compare times

        if ((r = tm_cmp(&temp, gmtime(&t))) < 0) // If t is later than temp
            t &= ~(1L << i); // t is later, so clear the bit */
        else if (r == 0)
            return t; // Finded time, so return */
    }

    // If we get here, then tp didn't point to valid data.
    return -1;
}

time_t timelocal(struct tm* timeptr)
{
    return mktime(timeptr);
}