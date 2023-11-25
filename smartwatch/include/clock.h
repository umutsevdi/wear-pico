/******************************************************************************

 * File: include/clock.h
 *
 * Author: Umut Sevdi
 * Created: 10/31/23
 * Description: Clock functionalities

*****************************************************************************/

#ifndef SW_ALARM
#define SW_ALARM

#include "pico/time.h"
#include "pico/types.h"
typedef struct SwAlarm {
    absolute_time_t timeout;
    alarm_callback_t callback;
} SwAlarm;

typedef alarm_callback_t* SwChrono;

void sw_alarm_new(SwAlarm* alarm, alarm_callback_t on_expire);
void sw_alarm_destroy(SwAlarm* alarm);
void sw_chrono_start(SwChrono* chrono);
absolute_time_t sw_chrono_stop();

void sw_();
void sw_();

#endif
