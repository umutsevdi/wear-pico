/******************************************************************************

 * File: include/util.h
 *
 * Author: Umut Sevdi
 * Created: 09/11/23
 * Description: Common functions and data structures for the smartwatch

*****************************************************************************/
#ifndef SW_UTIL
#define SW_UTIL

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

const char* _file_fmt(const char* str);
#define MAXLINE 256
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
#endif
