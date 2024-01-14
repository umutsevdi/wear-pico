#include "sw_common/util.h"
#include "sw_os/state.h"
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#define DT_STRLEN 14

enum IDX {
    IDX_YEAR = 0,
    IDX_MONTH = 4,
    IDX_DAY = 6,
    IDX_HOUR = 8,
    IDX_MIN = 10,
    IDX_SEC = 12,
};

char LOG_BUFFER[LOG_BUFFER_S] = {0};
int log_idx = 0;

const char* _file_fmt(const char* str)
{
    int len = strlen(str);
    if (len <= 8) {
        return str;
    } else if (len > 30) {
        return str + 8 + len - 30;
    }
    return (char*)str + 8;
}

const char* strcenter(char* str, size_t str_s, size_t str_cap)
{
    PRINT("STRING: %s, LEN: %zu CAP: %zu", , str, str_s, str_cap);
    if (str_s < str_cap) {
        size_t lpad = (str_cap - str_s) / 2;
        memmove(str + lpad, str, str_s);
        memset(str, ' ', lpad);
        str[str_s + lpad] = '\0';
    }
    PRINT("STRCENTER->%s", , str);
    return str;
}

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
    if (flag & DT_WC_YEAR) {
        if (dt1->year != dt2->year) { return dt1->year > dt2->year ? 1 : -1; }
    }
    if (flag & DT_WC_MONTH) {
        if (dt1->month != dt2->month) {
            return dt1->month > dt2->month ? 1 : -1;
        }
    }
    if (flag & DT_WC_DAY) {
        if (dt1->day != dt2->day) { return dt1->day > dt2->day ? 1 : -1; }
    }
    if (flag & DT_WC_HOUR) {
        if (dt1->hour != dt2->hour) { return dt1->hour > dt2->hour ? 1 : -1; }
    }
    if (flag & DT_WC_MIN) {
        if (dt1->minute != dt2->minute) {
            return dt1->minute > dt2->minute ? 1 : -1;
        }
    }
    if (flag & DT_WC_SEC) {
        if (dt1->second != dt2->second) {
            return dt1->second > dt2->second ? 1 : -1;
        }
    }
    return 0;
}

int strwrap(char* buffer, size_t buffer_s, int width, char** array, int array_s)
{
    if (width <= 0 || array_s <= 0 || buffer == NULL || array == NULL) {
        return -1;
    }

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
                if (*(last_space - 1) != '.') { last_space[0] = '.'; }
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
    if (str_s > 40) { str[39] = '\n'; }
    if (str_s + log_idx > LOG_BUFFER_S) {
        memmove(LOG_BUFFER, LOG_BUFFER + str_s, LOG_BUFFER_S - str_s);
        log_idx -= str_s;
    }

    memcpy(LOG_BUFFER + log_idx, str, str_s);
    log_idx += str_s;
    return code;
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

bool date_decode(const char buffer[15], DateTime* dt)
{
    if (buffer[DT_STRLEN] != '\0') return false;
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

bool date_encode(const DateTime* dt, char buffer[15])
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
            return false;
        }
    }
    if (!(DT_WC_MONTH & dt->flag)) {
        dt->month = strtoul(str_p[1], &endptr, 10);
        if (*endptr != '\0' || dt->month == 0 || dt->month > 12) {
            return false;
        }
    }
    if (!(DT_WC_DAY & dt->flag)) {
        dt->day = strtoul(str_p[2], &endptr, 10);
        if (*endptr != '\0' || dt->day == 0 || dt->day > 30) { return false; }
    }
    if (!(DT_WC_HOUR & dt->flag)) {
        dt->hour = strtoul(str_p[3], &endptr, 10);
        if (*endptr != '\0' || dt->hour > 24) { return false; }
    }
    if (!(DT_WC_MIN & dt->flag)) {
        dt->minute = strtoul(str_p[4], &endptr, 10);
        if (*endptr != '\0' || dt->minute > 60) { return false; }
    }
    if (!(DT_WC_SEC & dt->flag)) {
        dt->second = strtoul(str_p[5], &endptr, 10);
        if (*endptr != '\0' || dt->second > 60) { return false; }
    }
    return true;
}

const char* __now()
{
    static char string[20];
    snprintf(string, 20, "%02d/%02d/%02d %02d:%02d:%02d", state.dt.day,
             state.dt.month, state.dt.year % 100, state.dt.hour,
             state.dt.minute, state.dt.second);
    return string;
}
