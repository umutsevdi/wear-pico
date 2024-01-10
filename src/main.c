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
    os_init();
    apps_init();
    multicore_launch_core1(_core1_cb);

    WARN(BT_INIT);
    bt_init();

    int i = 0;
    while (1) {
        os_gyro_fetch();
        i++;
        if (i % 10 == 0) {
            int demo_amount = (state.dev.dist_acc / 10000 - 1);
            state.step += demo_amount > 0 ? demo_amount : 0;
        }
        sleep_ms(125);
    }

    return 0;
}
