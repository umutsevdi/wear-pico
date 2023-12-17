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

const char* day_of_the_week(DateTime* dt)
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
