#include "sw_os/state.h"
#include "hardware/gpio.h"
#include "pico/time.h"
#include "sw_os/dev.h"

SwState state;

DateTime os_get_time() { return state.dt; }

extern void os_gyro_init(void);

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
    }
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
    os_dev_notify();
}

const char* day_of_the_week(DateTime* dt)
{
    int d = dt->day;
    int m = dt->month;
    int y = dt->year;
    int dow = (d += m < 3 ? y-- : y - 2,
               23 * m / 9 + d + 4 + y / 4 - y / 100 + y / 400)
              % 7;

    switch (dow) {
    case 0: return "Sunday";
    case 1: return "Monday";
    case 2: return "Tuesday";
    case 3: return "Wednesday";
    case 4: return "Thursday";
    case 5: return "Friday";
    case 6: return "Saturday";
    }
    return "Null";
}
