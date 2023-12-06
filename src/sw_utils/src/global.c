#include "global.h"

SwState state;

DateTime sw_get_real_time() { return state.dt; }

void sw_init_state()
{
    state.clock_show_sec = false;
    state.is_connected = true;
    state.dt = (DateTime){0, 2022, 5, 10, 9, 30, 25};
    state.chrono.dt.flag = DT_WC_YEAR | DT_WC_MONTH | DT_WC_YEAR | DT_WC_DAY;
}
