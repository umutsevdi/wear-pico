/******************************************************************************

 * File: include/global.h
 *
 * Author: Umut Sevdi
 * Created: 11/25/23
 * Description: Global smartwatch state

*****************************************************************************/
#ifndef SW_STATE
#define SW_STATE

#include "sw_common/util.h"
#include <pico/time.h>

typedef struct {
    DateTime dt;
    uint64_t dt_padding;
    repeating_timer_t __dt_timer;
    bool is_connected;
    struct {
        /* stack values for the gpio pins
         * to prevent early cancels;
         * Each set or reset increases/decreases the value, gpio is disabled only at 0;
         */
        int stack[20];
        int16_t temp;
        int16_t dist_acc;
        int16_t dist_gyro;
    } dev;
    struct {
        bool on_charge;
        int pct;
    } bat;
    struct {
        bool is_playing;
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
    int32_t step;
} SwState;

extern SwState state;

DateTime os_get_time();
void os_init();
#endif
