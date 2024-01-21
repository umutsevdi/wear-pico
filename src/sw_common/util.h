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

#define LOG_BUFFER_S 250
extern char LOG_BUFFER[LOG_BUFFER_S];

#ifndef MIN
#define MIN(A, B) (A) < (B) ? (A) : (B)
#endif
#ifndef MAX
#define MAX(A, B) (A) > (B) ? (A) : (B)
#endif

#define PRINT(FMT, ARGS...)                                                    \
    printf("%17s %s#%s():-%4d - " #FMT "\r\n", __now(), _file_fmt(__FILE__),   \
           __func__, __LINE__ ARGS),                                           \
        __strdump(0, #FMT "\n" ARGS)

#define WARN(CODE)                                                             \
    printf("%17s %s#%s():%-4d - " #CODE "\r\n", __now(), _file_fmt(__FILE__),  \
           __func__, __LINE__),                                                \
        __strdump(0, "WRN:" #CODE "\n")

#define INFO(CODE)                                                             \
    (printf("%17s %s#%s():%-4d - " #CODE "\r\n", __now(), _file_fmt(__FILE__), \
            __func__, __LINE__),                                               \
     __strdump(0, "INF:" #CODE "\n"))

#define ERROR(CODE)                                                            \
    (printf("%17s %s#%s():%-4d - " #CODE "\r\n", __now(), _file_fmt(__FILE__), \
            __func__, __LINE__),                                               \
     __strdump(CODE, "ERR:" #CODE "\n"))

#ifndef UNUSED
#define UNUSED(ARG) __attribute__((unused)) ARG
#endif

/**
 * Centers the given string in-place and returns it
 * @str - string to center
 * @str_s - length of the string
 * @str_cap - max length of the buffer
 */
const char* strcenter(char* str, size_t str_s, size_t str_cap);
/**
 * Trims the first 9 characters of a file URI
 * - /app/src/sw_apps/src/apps.c -> /sw_apps/src/apps.c
 */
const char* _file_fmt(const char* str);

/**
 * Splits the input string into an array of substrings, considering both the
 * line width and '\n' character.
 * When the line is split due to the it's width, last word will be wrapped as
 * whole.
 * @buffer   - input string to split
 * @buffer_s - size of the buffer
 * @width    - maximum line width
 * @array    - array to store substrings.
 * @array_s  - size of the array
 *
 * @return   - actual number of lines in the array, -1 when improper inputs are given.
 *
 * strwrap handles the string in place and affects the original string.
 * Inserts "..." if the array size is exceeded
 */
int strwrap(char* buffer, size_t buffer_s, int width, char** array,
            int array_s);

/* Dumps given text to the log buffer */
int __strdump(int code, const char* fmt, ...);

const char* __now();

#endif
