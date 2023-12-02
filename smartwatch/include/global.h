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
    uint64_t dt_padding;
    SwReqBat bat_clock;
    SwReqBat bat_sw;
    bool is_connected;
    struct {
        char song[30];
        char album[30];
        char artist[30];
    } media;
    uint64_t chrono;

} Smartwatch;

DateTime* sw_get_dt();

#endif
