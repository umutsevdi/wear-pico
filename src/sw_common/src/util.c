#include "sw_common/util.h"
#include <stdarg.h>
#include <stdio.h>
#include <string.h>

char LOG_BUFFER[128] = {0};
int log_idx = 0;

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
    if (flag & DT_WC_YEAR) {
        if (dt1->year != dt2->year) return dt1->year > dt2->year ? 1 : -1;
    }
    if (flag & DT_WC_MONTH) {
        if (dt1->month != dt2->month) return dt1->month > dt2->month ? 1 : -1;
    }
    if (flag & DT_WC_DAY) {
        if (dt1->day != dt2->day) return dt1->day > dt2->day ? 1 : -1;
    }
    if (flag & DT_WC_HOUR) {
        if (dt1->hour != dt2->hour) return dt1->hour > dt2->hour ? 1 : -1;
    }
    if (flag & DT_WC_MIN) {
        if (dt1->minute != dt2->minute)
            return dt1->minute > dt2->minute ? 1 : -1;
    }
    if (flag & DT_WC_SEC) {
        if (dt1->second != dt2->second)
            return dt1->second > dt2->second ? 1 : -1;
    }
    return 0;
}

int strwrap(char* buffer, size_t buffer_s, int width, char** array, int array_s)
{
    if (width <= 0 || array_s <= 0 || buffer == NULL || array == NULL)
        return -1;

    array[0] = buffer;
    char* last_space = buffer;
    int current_len = 0;
    int current_idx = 1;
    for (size_t i = 0; i < buffer_s; i++) {
        if (buffer[i] == ' ') {
            last_space = buffer + i;
        } else if (buffer[i] == '\n') {
            last_space = buffer + i;
            /* Set the length to the width to trigger wrapping */
            current_len = width;
        }
        if (current_len == width) { /* soft wrap the current word*/
            *last_space = '\0';
            array[current_idx] = (last_space + 1);
            current_idx++;
            current_len = 0;
        } else {
            current_len++;
        }

        if (current_idx > array_s) {
            if (buffer_s > (uint64_t)(last_space - buffer + 4)) {
                if (*(last_space - 1) != '.') last_space[0] = '.';
                last_space[1] = '.';
                last_space[2] = '.';
                last_space[3] = '\0';
            }
            return array_s;
        }
    }

    return current_idx;
}

int __strdump(int code, const char* fmt, ...)
{
    char str[41];
    va_list args;
    va_start(args, fmt);
    vsnprintf(str, 40, fmt, args);

    int str_s = strnlen(str, 40);
    if (str_s > 40) str[39] = '\n';
    if (str_s + log_idx > 128) {
        memmove(LOG_BUFFER, LOG_BUFFER + str_s, 128 - str_s);
        log_idx -= str_s;
    }

    memcpy(LOG_BUFFER + log_idx, str, str_s);
    log_idx += str_s;
    return code;
}

void get_log(char* str, size_t str_s)
{
    memcpy(str, LOG_BUFFER, str_s < 128 ? str_s : 128);
}
