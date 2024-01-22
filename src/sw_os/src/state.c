#include "sw_os/state.h"
#include "hardware/gpio.h"
#include "pico/time.h"
#include "sw_bt/bt.h"
#include "sw_common/types.h"
#include "sw_common/util.h"
#include "sw_os/dev.h"

GlobalState state;
repeating_timer_t clock_timer;

extern void os_gyro_init(void);
extern void os_dev_init(void);

absolute_time_t then = {0};

/* Iterate over active alarms, process the first alarm that is about to
 * trigger */
static void _process_alarms()
{
    AlarmList* alarms = &state.alarms;
    for (int i = 0; i < (alarms->len); i++) {
        if (alarms->list[i].is_active
            && !dt_cmp(&state.dt, &alarms->list[i].at,
                       DT_WC_HOUR | DT_WC_MIN)) {
            os_request_popup((Popup){
                .type = POPUP_ALARM,
                .value = (union PopupValue){.alarm = &alarms->list[i]},
            });
            break;
        }
    }
}

/* Iterate over active events, process the first event that is about to
 * trigger */
static void _process_events()
{
    EventList* events = &state.events;
    for (int i = (events->len) - 1; i >= 0; i--) {
        if (!dt_cmp(&state.dt, &events->list[i].at,
                    DT_WC_YEAR | DT_WC_MONTH | DT_WC_DAY | DT_WC_HOUR
                        | DT_WC_MIN)) {
            os_request_popup((Popup){
                .type = POPUP_REMINDER,
                .value = (union PopupValue){.event = events->list[i].title},
            });
            events->len = i;
            break;
        }
    }
}

static bool _os_timer_cb(UNUSED(repeating_timer_t* r))
{
    absolute_time_t now = get_absolute_time();
    if ((absolute_time_diff_us(now, then) / 1000000)) {
        then = now;
        state.dt.second++;
    }
    if (state.dt.second < 60) { return true; }
    state.dt.second = 0;
    state.dt.minute++;
    /* Call every minute  */
    _process_alarms();
    _process_events();
    if (state.dt.minute < 60) { return true; }
    state.dt.minute = 0;
    state.dt.hour++;
    if (state.dt.hour < 24) { return true; }
    state.dt.hour = 0;
    state.dt.day++;
    if (state.dt.day < dt_number_of_days(&state.dt)) { return true; };
    state.dt.day = 1;
    state.dt.month++;
    if (state.dt.month < 12) { return true; }
    state.dt.month = 1;
    state.dt.year++;
    return true;
}

void os_init()
{
    then = get_absolute_time();
    state.bat.pct = 100;
    state.dt = (DateTime){0, 2024, 01, 01, 00, 00, 00};
    state.alarms.len = 0;
    state.chrono.dt.flag = DT_WC_YEAR | DT_WC_MONTH | DT_WC_YEAR | DT_WC_DAY;
    os_dev_init();
    os_gyro_init();
    add_repeating_timer_ms(1000, _os_timer_cb, NULL, &clock_timer);

    state.config.call = DEV_BUZZER | DEV_LED | DEV_VIB;
    state.config.notify = DEV_BUZZER | DEV_VIB;
    state.config.reminder = DEV_BUZZER | DEV_VIB;
    state.config.alarm = DEV_BUZZER | DEV_LED;

    PRINT(OS_INIT);
}

void os_request_popup(Popup p) { state.__popup_req = p; }
