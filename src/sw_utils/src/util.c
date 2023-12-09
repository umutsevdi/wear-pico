#include "sw_utils/util.h"
/*
/app/src/sw_apps/src/apps.c
*/
const char* _file_fmt(const char* str)
{
    if (strnlen(str, 9) > 8) return (char*)str + 8;
    return str;
}

