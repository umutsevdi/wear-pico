#include <stdint.h>
#include <stdio.h>

#include <pico/multicore.h>
#include <pico/stdlib.h>
#include <pico/time.h>

#include "sw_apps/apps.h"
#include "sw_common/util.h"
#include "sw_os/dev.h"
#include "sw_os/state.h"

extern int bt_main();
void _core1_cb() { apps_load(SCREEN_CLOCK); }
int main(int argc, char* argv[])
{
    stdio_init_all();
    os_init();
    apps_init();
    multicore_launch_core1(_core1_cb);

    bt_main();
    while (1) {
        //        printf("CORE 0 running with %s\n", __func__);
        WARN(BT_CORE_LOOP);
        sleep_ms(2000);
        os_gyro_fetch();
        int demo_amount = (state.dev.dist_acc / 10000 - 2);
        state.step += demo_amount > 0 ? demo_amount : 0;
    }

    return 0;
}
