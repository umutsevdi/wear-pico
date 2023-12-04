/******************************************************************************

 * File: include/global.h
 *
 * Author: Umut Sevdi
 * Created: 11/25/23
 * Description: Global clock state

*****************************************************************************/
#ifndef SW_GLOBAL
#define SW_GLOBAL

#include "pico/time.h"
#include "protocol.h"
#include "util.h"
typedef struct {
    DateTime dt;
    uint64_t dt_padding;
    bool is_connected;
    struct {
        char song[30];
        char album[30];
        char artist[30];
    } media;
    bool clock_show_sec;
    struct {
        DateTime dt;
        bool enabled;
        repeating_timer_t timer;
    } chrono;
} SwState;

extern SwState state;

DateTime sw_get_real_time();
void sw_init_state();
#endif
