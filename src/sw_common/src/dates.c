#include "sw_common/types.h"
#include "sw_common/util.h"

#define DT_STRLEN 14

enum IDX {
    IDX_YEAR = 0,
    IDX_MONTH = 4,
    IDX_DAY = 6,
    IDX_HOUR = 8,
    IDX_MIN = 10,
    IDX_SEC = 12,
};

int dt_number_of_days(DateTime* dt)
{
    if (dt->month == 2) {
        return ((dt->year % 4 == 0 && dt->year % 100 != 0)
                || (dt->year % 400 == 0))
                   ? 29
                   : 28;
    }
    if (dt->month == 1 || dt->month == 3 || dt->month == 5 || dt->month == 7
        || dt->month == 8 || dt->month == 10 || dt->month == 12) {
        return 31;
    }
    return 30;
}

int dt_get_day_int(DateTime* dt)
{
    int d = dt->day;
    int m = dt->month;
    int y = dt->year;
    int dow = (d += m < 3 ? y-- : y - 2,
               23 * m / 9 + d + 4 + y / 4 - y / 100 + y / 400)
              % 7;

    return dow;
}

const char* dt_get_day(DateTime* dt)
{
    switch (dt_get_day_int(dt)) {
    case 0: return "Sunday";
    case 1: return "Monday";
    case 2: return "Tuesday";
    case 3: return "Wednesday";
    case 4: return "Thursday";
    case 5: return "Friday";
    case 6: return "Saturday";
    }
    return "Null";
}

int dt_cmp(const DateTime* dt1, const DateTime* dt2, int16_t flag)
{
    if (flag & DT_WC_YEAR && dt1->year != dt2->year) {
        return dt1->year > dt2->year ? 1 : -1;
    }
    if (flag & DT_WC_MONTH && dt1->month != dt2->month) {
        return dt1->month > dt2->month ? 1 : -1;
    }
    if (flag & DT_WC_DAY && dt1->day != dt2->day) {
        return dt1->day > dt2->day ? 1 : -1;
    }
    if (flag & DT_WC_HOUR && dt1->hour != dt2->hour) {
        return dt1->hour > dt2->hour ? 1 : -1;
    }
    if (flag & DT_WC_MIN && dt1->minute != dt2->minute) {
        return dt1->minute > dt2->minute ? 1 : -1;
    }
    if (flag & DT_WC_SEC && dt1->second != dt2->second) {
        return dt1->second > dt2->second ? 1 : -1;
    }
    return 0;
}

/* Parses the given buffer to generate the wildcard flag. Looks for '?' 
 * characters. */
static int16_t _dt_wildcard(const char* b);
/**
 * Maps the fields of given string array to the date time.
 * @dt - to assign
 * @date_s - a string array that contains each field of the struct in a
 * different string.
 * char[6]{year[5], month[3], day[3], hour[3], minute[3], second[3]}
 * @return - whether mapping failed or not
 */
static bool _dt_map(DateTime* dt, char** str_p);

bool dt_encode(const DateTime* dt, char buffer[15])
{
    snprintf(buffer, 14, "%04d%02d%02d%02d%02d%02d", dt->year, dt->month,
             dt->day, dt->hour, dt->minute, dt->second);
    if (dt->flag & DT_WC_YEAR) { memset(buffer, 4, '?'); }
    if (dt->flag & DT_WC_MONTH) { memset(buffer + IDX_MONTH, '?', 2); }
    if (dt->flag & DT_WC_DAY) { memset(buffer + IDX_DAY, '?', 2); }
    if (dt->flag & DT_WC_HOUR) { memset(buffer + IDX_HOUR, '?', 2); }
    if (dt->flag & DT_WC_MIN) { memset(buffer + IDX_MIN, '?', 2); }
    if (dt->flag & DT_WC_SEC) { memset(buffer + IDX_SEC, '?', 2); }
    return true;
}

bool dt_decode(const char buffer[15], DateTime* dt)
{
    if (buffer[DT_STRLEN] != '\0') {
        WARN(STRNLEN);
        return false;
    }
    char year[5] = {0};
    char month[3] = {0};
    char day[3] = {0};
    char hour[3] = {0};
    char min[3] = {0};
    char sec[3] = {0};
    dt->flag = _dt_wildcard(buffer);

    for (size_t i = 0; i < DT_STRLEN; i++) {
        if (i < IDX_MONTH) {
            year[i - IDX_YEAR] = buffer[i];
        } else if (IDX_MONTH <= i && i < IDX_DAY) {
            month[i - IDX_MONTH] = buffer[i];
        } else if (IDX_DAY <= i && i < IDX_HOUR) {
            day[i - IDX_DAY] = buffer[i];
        } else if (IDX_HOUR <= i && i < IDX_MIN) {
            hour[i - IDX_HOUR] = buffer[i];
        } else if (IDX_MIN <= i && i < IDX_SEC) {
            min[i - IDX_MIN] = buffer[i];
        } else if (IDX_SEC <= i) {
            sec[i - IDX_SEC] = buffer[i];
        }
    }
    return _dt_map(dt, (char*[]){year, month, day, hour, min, sec});
}

static int16_t _dt_wildcard(const char* b)
{
    int16_t wc = 0;
    if (b[IDX_YEAR] == '?') { wc |= DT_WC_YEAR; }
    if (b[IDX_MONTH] == '?') { wc |= DT_WC_MONTH; }
    if (b[IDX_DAY] == '?') { wc |= DT_WC_DAY; }
    if (b[IDX_HOUR] == '?') { wc |= DT_WC_HOUR; }
    if (b[IDX_MIN] == '?') { wc |= DT_WC_MIN; }
    if (b[IDX_SEC] == '?') { wc |= DT_WC_SEC; }
    return wc;
}

static bool _dt_map(DateTime* dt, char** str_p)
{
    char* endptr = NULL;
    if (!(DT_WC_YEAR & dt->flag)) {
        dt->year = strtoul(str_p[0], &endptr, 10);
        if (*endptr != '\0' || dt->year == 0 || dt->year > 9999) {
            WARN(YEAR);
            return false;
        }
    }
    if (!(DT_WC_MONTH & dt->flag)) {
        dt->month = strtoul(str_p[1], &endptr, 10);
        if (*endptr != '\0' || dt->month == 0 || dt->month > 12) {
            WARN(MON);
            return false;
        }
    }
    if (!(DT_WC_DAY & dt->flag)) {
        dt->day = strtoul(str_p[2], &endptr, 10);
        if (*endptr != '\0' || dt->day == 0 || dt->day > 31) {
            WARN(DAY);
            return false;
        }
    }
    if (!(DT_WC_HOUR & dt->flag)) {
        dt->hour = strtoul(str_p[3], &endptr, 10);
        if (*endptr != '\0' || dt->hour > 24) {
            WARN(HOUR);
            return false;
        }
    }
    if (!(DT_WC_MIN & dt->flag)) {
        dt->minute = strtoul(str_p[4], &endptr, 10);
        if (*endptr != '\0' || dt->minute > 60) {
            WARN(MIN);
            return false;
        }
    }
    if (!(DT_WC_SEC & dt->flag)) {
        dt->second = strtoul(str_p[5], &endptr, 10);
        if (*endptr != '\0' || dt->second > 60) {
            WARN(SEC);
            return false;
        }
    }
    return true;
}
