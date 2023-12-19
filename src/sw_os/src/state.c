#include "sw_os/state.h"
#include "hardware/gpio.h"
#include "pico/time.h"
#include "sw_os/dev.h"

SwState state;

DateTime os_get_time() { return state.dt; }

extern void os_gyro_init(void);

/* Iterate over active alarms, process the first alarm that is about to
 * trigger */
static void _process_alarms()
{
    for (short i = 0; i < state.alarms.len; i++) {
        if (state.alarms.list[i].is_active) {
            if (dt_cmp(&state.dt, &state.alarms.list[i].at,
                       state.alarms.list[i].at.flag)
                == 0) {
                // TODO run popup
                break;
            }
        }
    }
}

static bool _os_timer_cb(repeating_timer_t* r)
{
    state.dt.second++;
    if (state.dt.second > 59) {
        state.dt.second -= 59;
        if (++state.dt.minute > 59) {
            state.dt.minute = 0;
            if (++state.dt.hour > 23) {
                state.dt.second = 0;
                state.dt.minute = 0;
                state.dt.hour = 0;
            }
        }
        /* Call every minute  */
        _process_alarms();
    }
    return true;
}

static bool _step_count_cb(repeating_timer_t* r)
{
    static int16_t old = 0;
    GyroData d = os_gyro_fetch();
    int16_t peak = state.dev.dist_acc;
    return true;
}

void os_init()
{
    state.clock_show_sec = false;
    state.is_connected = false;
    state.bat.on_charge = true;
    state.dt = (DateTime){0, 2023, 12, 8, 9, 00, 00};
    state.chrono.dt.flag = DT_WC_YEAR | DT_WC_MONTH | DT_WC_YEAR | DT_WC_DAY;
    gpio_init(DEV_BUZZER);
    gpio_init(DEV_LED);
    gpio_set_dir(DEV_BUZZER, true);
    gpio_set_dir(DEV_LED, true);
    os_gyro_init();
    //    os_dev_set_for(DEV_LED, 1000);
    add_repeating_timer_ms(-1000, _os_timer_cb, NULL, &state.__dt_timer);
    add_repeating_timer_ms(500, _step_count_cb, NULL, &state.dev.__step_timer);
    os_dev_notify();
}
