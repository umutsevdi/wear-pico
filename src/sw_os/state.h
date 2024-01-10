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

union PopupValue {
    struct caller {
        char name[25];
        bool is_over;
    } caller;
    struct {
        char title[13];
        char text[128];
    } notify;
    DateTime alarm;
};

typedef struct {
    enum popup_t type;
    union PopupValue value;
} Popup;

typedef struct {
    DateTime dt;
    repeating_timer_t __dt_timer;
    bool show_sec;
    bool is_connected;
    absolute_time_t __last_connected;
    Popup popup;
    Popup __popup_req;
    struct {
        /* GPIO pin stack to prevent early cancels
         * Each set/reset is added to the stack */
        int stack[20];
        int16_t temp;
        int16_t dist_acc;
        int16_t dist_gyro;
        repeating_timer_t __step_timer;
    } dev;
    struct {
        bool on_charge;
        int pct;
    } bat;
    struct {
        Alarm list[4];
        short len;
    } alarms;
    struct {
        DateTime dt;
        bool enabled;
        repeating_timer_t timer;
    } chrono;
    struct {
        bool is_playing;
        char song[30];
        char artist[30];
        bool is_fetched;
    } media;
    int32_t step;
} SwState;

extern SwState state;

DateTime os_get_time();
void os_init();

void os_request_popup(Popup p);
#endif
