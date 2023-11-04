#include "msg.h"
#include <stdlib.h>
#include <string.h>
#define DATE_TIME_STRUCT_LEN 14

typedef enum {
    IDX_YEAR = 0,
    IDX_MONTH = 4,
    IDX_DAY = 6,
    IDX_HOUR = 8,
    IDX_MIN = 10,
    IDX_SEC = 12,
} DT_IDX;

bool to_date(DateTime* dt, const char* buffer, size_t buffer_s);

static int16_t dt_wildcard(const char* buffer)
{
    int16_t wc = 0;
    if (buffer[IDX_YEAR] == '?')
        wc |= DT_WC_YEAR;
    if (buffer[IDX_MONTH] == '?')
        wc |= DT_WC_MONTH;
    if (buffer[IDX_DAY] == '?')
        wc |= DT_WC_DAY;
    if (buffer[IDX_HOUR] == '?')
        wc |= DT_WC_HOUR;
    if (buffer[IDX_MIN] == '?')
        wc |= DT_WC_MIN;
    if (buffer[IDX_SEC] == '?')
        wc |= DT_WC_SEC;
    return wc;
}

static bool dt_map(DateTime* dt, char** date_s)
{
    char* endptr = NULL;
    if (!(DT_WC_YEAR & dt->flag)) {
        dt->year = strtoul(date_s[0], &endptr, 10);
        if (*endptr != '\0' || dt->year == 0 || dt->year > 9999)
            return false;
    }
    if (!(DT_WC_MONTH & dt->flag)) {
        dt->month = strtoul(date_s[1], &endptr, 10);
        if (*endptr != '\0' || dt->month == 0 || dt->month > 12)
            return false;
    }
    if (!(DT_WC_DAY & dt->flag)) {
        dt->day = strtoul(date_s[2], &endptr, 10);
        if (*endptr != '\0' || dt->day == 0 || dt->day > 30)
            return false;
    }
    if (!(DT_WC_HOUR & dt->flag)) {
        dt->hour = strtoul(date_s[3], &endptr, 10);
        if (*endptr != '\0' || dt->hour > 24) // danger
            return false;
    }
    if (!(DT_WC_MIN & dt->flag)) {
        dt->minute = strtoul(date_s[4], &endptr, 10);
        if (*endptr != '\0' || dt->minute > 60) // danger
            return false;
    }
    if (!(DT_WC_SEC & dt->flag)) {
        dt->second = strtoul(date_s[5], &endptr, 10);
        if (*endptr != '\0' || dt->second > 60) // danger
            return false;
    }
    return true;
}

bool to_date(DateTime* dt, const char* buffer, size_t buffer_s)
{
    if (buffer_s != DATE_TIME_STRUCT_LEN) {
        return false;
    }
    char year[5] = { 0 };
    char month[3] = { 0 };
    char day[3] = { 0 };
    char hour[3] = { 0 };
    char min[3] = { 0 };
    char sec[3] = { 0 };
    dt->flag = dt_wildcard(buffer);

    for (size_t i = 0; i < buffer_s; i++) {
        if (i < IDX_MONTH)
            year[i - IDX_YEAR] = buffer[i];
        else if (IDX_MONTH <= i && i < IDX_DAY)
            month[i - IDX_MONTH] = buffer[i];
        else if (IDX_DAY <= i && i < IDX_HOUR)
            day[i - IDX_DAY] = buffer[i];
        else if (IDX_HOUR <= i && i < IDX_MIN)
            hour[i - IDX_HOUR] = buffer[i];
        else if (IDX_MIN <= i && i < IDX_SEC)
            min[i - IDX_MIN] = buffer[i];
        else if (IDX_SEC <= i)
            sec[i - IDX_SEC] = buffer[i];
    }
    return dt_map(dt, (char*[]) { year, month, day, hour, min, sec });
}
