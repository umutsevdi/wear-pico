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
#define PRINT(FMT, ARGS...) printf("%s" #FMT "\r\n", _file_fmt(__FILE__) ARGS)
#define WARN(CODE)                                                             \
    do {                                                                       \
        fprintf(stderr, "%s#%s():%d " #CODE "\r\n", _file_fmt(__FILE__),       \
                __func__, __LINE__);                                           \
    } while (0)

#define ERROR(CODE)                                                            \
    (fprintf(stderr, "%s#%s():%d " #CODE "\r\n", _file_fmt(__FILE__),          \
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
const char* day_of_the_week(DateTime* dt);

/**
 * Trims the first 9 characters of a file URI
 * - /app/src/sw_apps/src/apps.c -> /sw_apps/src/apps.c
 */
const char* _file_fmt(const char* str);
#endif
