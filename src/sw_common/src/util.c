#include "sw_common/util.h"

const char* _file_fmt(const char* str)
{
    return (strnlen(str, 9) > 8) ? (char*)str + 8 : str;
}

const char* strcenter(char* str, size_t str_s, size_t str_cap)
{
    size_t lpad = (str_cap - str_s) / 2;
    memmove(str + lpad, str, str_s);
    memset(str, ' ', lpad);
    return str;
}

const char* dt_get_day(DateTime* dt)
{
    int d = dt->day;
    int m = dt->month;
    int y = dt->year;
    int dow = (d += m < 3 ? y-- : y - 2,
               23 * m / 9 + d + 4 + y / 4 - y / 100 + y / 400)
              % 7;

    switch (dow) {
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
    if (~flag & DT_WC_YEAR) {
        if (dt1->year != dt2->year) return dt1->year > dt2->year ? 1 : -1;
    }
    if (~flag & DT_WC_MONTH) {
        if (dt1->month != dt2->month) return dt1->month > dt2->month ? 1 : -1;
    }
    if (~flag & DT_WC_DAY) {
        if (dt1->day != dt2->day) return dt1->day > dt2->day ? 1 : -1;
    }
    if (~flag & DT_WC_HOUR) {
        if (dt1->hour != dt2->hour) return dt1->hour > dt2->hour ? 1 : -1;
    }
    if (~flag & DT_WC_MIN) {
        if (dt1->minute != dt2->minute)
            return dt1->minute > dt2->minute ? 1 : -1;
    }
    if (~flag & DT_WC_SEC) {
        if (dt1->second != dt2->second)
            return dt1->second > dt2->second ? 1 : -1;
    }
    return 0;
}
