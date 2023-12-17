#include "sw_common/util.h"

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
