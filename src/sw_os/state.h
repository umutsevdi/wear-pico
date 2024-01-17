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

struct GyroData {
    int16_t acc[3];
    int16_t gyro[3];
    int16_t temp;
    int32_t step;
};
struct Battery {
    bool on_charge;
    int pct;
};

struct AlarmList {
    Alarm list[4];
    short len;
    bool is_fetched;
};

struct Chrono {
    DateTime dt;
    bool enabled;
    repeating_timer_t timer;
};

struct Media {
    bool is_playing;
    char song[30];
    char artist[30];
    bool is_fetched;
};

typedef struct {
    DateTime dt;
    repeating_timer_t __dt_timer;
    bool show_sec;
    bool is_connected;
    absolute_time_t __last_connected;
    Popup popup;
    Popup __popup_req;
    struct GyroData dev;
    struct Battery bat;
    struct AlarmList alarms;
    struct Chrono chrono;
    struct Media media;
} SwState;

extern SwState state;

DateTime os_get_time();
void os_init();

void os_request_popup(Popup p);
#endif
