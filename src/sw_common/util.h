/******************************************************************************

 * File: include/util.h
 *
 * Author: Umut Sevdi
 * Created: 09/11/23
 * Description: Common functions and data structures for the smartwatch

*****************************************************************************/
#ifndef SW_UTIL
#define SW_UTIL
#include "sw_common/types.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#define PRINT(FMT, ARGS...)                                                    \
    printf("%s#%30s():%-4d  " #FMT "\r\n", _file_fmt(__FILE__), __func__,      \
           __LINE__ ARGS)
#define WARN(CODE)                                                             \
    printf("%s#%30s():%-4d  " #CODE "\r\n", _file_fmt(__FILE__), __func__,     \
           __LINE__);

#define ERROR(CODE)                                                            \
    (fprintf(stderr, "%s#%30s():%-4d  " #CODE "\r\n", _file_fmt(__FILE__),     \
             __func__, __LINE__),                                              \
     CODE)

#define UNUSED(TYPE, ARGS...)                                                  \
    do {                                                                       \
        TYPE __args[] = {ARGS};                                                \
        __args[0] = __args[0];                                                 \
    } while (0);

/**
 * Centers the given string in-place and returns it
 * @str - string to center
 * @str_s - length of the string
 * @str_cap - max length of the buffer
 */
const char* strcenter(char* str, size_t str_s, size_t str_cap);
/* Returns the string representation of the day of the week based on given day/month/year. */
const char* dt_get_day(DateTime* dt);

/**
 * Compares given dates based on given flag
 * Example:
 * - date_cmp(dt1, dt2, dt1->flag);
 * - date_cmp(dt1, dt2, dt1->flag | dt2->flag);
 * - date_cmp(dt1, dt2, DT_WC_YEAR | DT_WC_MONTH);
 * @returns
 * *  1 if dt1 is after dt2
 * *  0 if dt1 and dt2 are equal
 * * -1 if dt1 is before dt2
 *
 */
int dt_cmp(const DateTime* dt1, const DateTime* dt2, int16_t flag);
/**
 * Trims the first 9 characters of a file URI
 * - /app/src/sw_apps/src/apps.c -> /sw_apps/src/apps.c
 */
const char* _file_fmt(const char* str);

#endif
