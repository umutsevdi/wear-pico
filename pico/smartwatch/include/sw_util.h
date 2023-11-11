/******************************************************************************

 * File: include/sw_util.h
 *
 * Author: Umut Sevdi
 * Created: 06/11/23
 * Description: Common functions and data structures the smartwatch

*****************************************************************************/
#pragma once
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define MAX_URI_LENGTH 4096
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

typedef enum BR_NET_PROTOCOL {
    BR_PROTOCOL_GOPHER,
    BR_PROTOCOL_GEMINI,
    BR_PROTOCOL_HTTP,
    BR_PROTOCOL_HTTPS,
    BR_PROTOCOL_UNSUPPORTED
} BR_PROTOCOL;
#define BR_PROTOCOL_S(p)                                                       \
    (p == BR_PROTOCOL_HTTP     ? "HTTP"                                        \
     : p == BR_PROTOCOL_HTTPS  ? "HTTPS"                                       \
     : p == BR_PROTOCOL_GEMINI ? "GEMINI"                                      \
     : p == BR_PROTOCOL_GOPHER ? "GOPHER"                                      \
                               : "UNDEFINED")
