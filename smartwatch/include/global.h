/******************************************************************************

 * File: include/global.h
 *
 * Author: Umut Sevdi
 * Created: 11/25/23
 * Description: Global clock state

*****************************************************************************/
#ifndef SW_GLOBAL
#define SW_GLOBAL

#include "protocol.h"
#include "util.h"
typedef struct SW {
    DateTime dt;
    SwReqBat bat_clock;
    SwReqBat bat_sw;
    bool is_connected;
    struct {
        char* song;
        char* album;
        char* artist;
    } media;

} Smartwatch;

DateTime* sw_get_dt();

#endif
