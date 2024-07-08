#include <time.h>
#include <stdint.h>
#include <ke.h>

//gmtime
#define SECMIN 60
#define SECHOUR 3600
#define SECDAY 86400
#define MONPERYEAR 12

#define is_leap(y) ((((y) % 4) == 0 && ((y) % 100) != 0) || ((y) % 400) == 0)

static int ydays[2] = { 365, 366 };

static int mdays[2][12] = {
    { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 },
    { 31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 }
};

static struct tm tm;

static char at_buf[26];

static int32_t timezone = SECHOUR * 3;

time_t time(time_t* tim)
{
    if(tim != 0)
        *tim = ke_systime;
    
    return ke_systime;
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

struct tm* gmtime(const time_t* timer)
{
    time_t t = *timer;
    int days, leap_years;
    int year = 1970;
    int month = 0;
    int current_year_days;


    tm.tm_sec = t % 60;
    t /= 60;
    tm.tm_min = t % 60;
    t /= 60;
    tm.tm_hour = t % 24;
    t /= 24;

    days = (int)t;

    while (days >= (current_year_days = ydays[is_leap(year)])) {
        days -= current_year_days;
        year++;
    }

    if (is_leap(year)) {
        if (days == 59) {
            tm.tm_mon = 1;
            tm.tm_mday = 29;
            tm.tm_year = year - 1900;
            tm.tm_wday = (4 + (t % 7)) % 7;
            return &tm;
        }
        else if (days > 59) {
            days -= 1;
        }
    }
    tm.tm_yday = days;

    // Calculate month and day
    for (month = 0; month < 12; month++) {
        int days_in_month = mdays[is_leap(year)][month];
        if (days < days_in_month) break;
        days -= days_in_month;
    }

    tm.tm_mon = month;
    tm.tm_mday = days + 1;
    tm.tm_year = year - 1900;
    tm.tm_wday = (4 + (t % 7)) % 7;

    return &tm;
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