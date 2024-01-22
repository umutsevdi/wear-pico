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
    char* event;
    Alarm* alarm;
};

typedef struct {
    enum popup_t type;
    union PopupValue value;
} Popup;

typedef struct {
    int alarm;
    int call;
    int notify;
    int reminder;
    int brightness;
} ConfigData;

typedef struct {
    int16_t acc[3];
    int16_t gyro[3];
    int16_t temp;
    int32_t step;
} GyroData;

typedef struct {
    bool on_charge;
    int pct;
} BatteryData;

typedef struct {
    Alarm list[4];
    short len;
    bool is_fetched;
} AlarmList;

typedef struct {
    Event list[4];
    short len;
    bool is_fetched;
} EventList;

typedef struct {
    DateTime dt;
    bool enabled;
} ChronoData;

typedef struct {
    bool is_playing;
    char song[30];
    char artist[30];
    bool is_fetched;
} MediaData;

typedef struct {
    ConfigData config;
    DateTime dt;
    bool show_sec;
    absolute_time_t __last_connected;
    Popup popup;
    Popup __popup_req;
    GyroData dev;
    BatteryData bat;
    AlarmList alarms;
    EventList events;
    ChronoData chrono;
    MediaData media;
} GlobalState;

extern GlobalState state;

void os_init();
void os_request_popup(Popup p);
#endif
