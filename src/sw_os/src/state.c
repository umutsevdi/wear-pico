#include "sw_os/state.h"
#include "hardware/gpio.h"
#include "pico/time.h"
#include "sw_bt/bt.h"
#include "sw_common/types.h"
#include "sw_os/dev.h"

SwState state;

DateTime os_get_time() { return state.dt; }

extern void os_gyro_init(void);
extern void os_dev_init(void);
extern void os_request_popup(Popup p);

absolute_time_t then = {0};

/* Iterate over active alarms, process the first alarm that is about to
 * trigger */
static void _process_alarms()
{
    if (!state.alarms.is_fetched) { return; }
    for (short i = 0; i < state.alarms.len; i++) {
        if (state.alarms.list[i].is_active
            && !dt_cmp(&state.dt, &state.alarms.list[i].at,
                       DT_WC_HOUR | DT_WC_MIN)) {
            os_request_popup((Popup){
                .type = POPUP_ALARM,
                .value = (union PopupValue){.alarm = state.alarms.list[i].at}});
            break;
        }
    }
}

static bool _os_timer_cb(repeating_timer_t* r)
{
    absolute_time_t now = get_absolute_time();
    if ((absolute_time_diff_us(now, then) / 1000000)) {
        then = now;
        state.dt.second++;
        state.is_connected =
            absolute_time_diff_us(state.__last_connected, now) / 1000000 < 30;
    }
    if (state.dt.second > 59) {
        state.dt.second = 0;
        state.dt.minute++;
        if (state.dt.minute > 59) {
            state.dt.minute = 0;
            state.dt.hour++;
            if (state.dt.hour > 23) {
                state.dt.hour = 0;
                state.dt.day++;
            }
        }
        /* Call every minute  */
        _process_alarms();
    }
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
    add_repeating_timer_ms(1000, _os_timer_cb, NULL, &state.__dt_timer);
    PRINT(OS_INIT);
}

void os_request_popup(Popup p) { state.__popup_req = p; }
