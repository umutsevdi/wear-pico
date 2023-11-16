/******************************************************************************

 * File: include/util.h
 *
 * Author: Umut Sevdi
 * Created: 09/11/23
 * Description: Common functions and data structures for the smartwatch

*****************************************************************************/
#pragma once
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define MAXLINE 256
#define PRINT(FMT, ARGS...) printf(__FILE__ #FMT "\r\n" ARGS)
#define WARN(CODE)                                                             \
    do {                                                                       \
        fprintf(stderr, __FILE__ "#%s():%d " #CODE "\r\n", __func__,           \
                __LINE__);                                                     \
    } while (0)

#define ERROR(CODE)                                                            \
    (fprintf(stderr, __FILE__ "#%s():%d " #CODE "\r\n", __func__, __LINE__),   \
     CODE)
