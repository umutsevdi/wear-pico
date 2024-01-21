#include "sw_common/util.h"
#include "sw_os/state.h"
#include <stdarg.h>

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
    if (str_s < str_cap) {
        size_t lpad = (str_cap - str_s) / 2;
        memmove(str + lpad, str, str_s);
        memset(str, ' ', lpad);
        str[str_s + lpad] = '\0';
    }
    return str;
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

const char* __now()
{
    static char string[20];
    snprintf(string, 20, "%02d/%02d/%02d %02d:%02d:%02d", state.dt.day,
             state.dt.month, state.dt.year % 100, state.dt.hour,
             state.dt.minute, state.dt.second);
    return string;
}
