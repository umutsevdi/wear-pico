#include <pico/multicore.h>
#include <pico/stdlib.h>

#include "sw_apps/apps.h"
#include "sw_bt/bt.h"
#include "sw_os/dev.h"
#include "sw_os/state.h"

void _core1_cb() { apps_load(SCREEN_CLOCK); }

int main(int argc, char* argv[])
{
    stdio_init_all();
    sleep_ms(1000);
    os_init();
    apps_init();
    multicore_launch_core1(_core1_cb);

    WARN(BT_INIT);
    bt_init();

    while (1) {
        GyroData data = os_gyro_fetch();
        sleep_ms(50);
    }
    return 0;
}
