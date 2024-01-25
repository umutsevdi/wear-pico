#include <pico/multicore.h>
#include <pico/stdlib.h>

#include "wp_apps/apps.h"
#include "wp_bt/bt.h"
#include "wp_dev/dev.h"
#include "wp_dev/state.h"

void _core1_cb() { apps_load(SCREEN_CLOCK); }

int main(int argc, char* argv[])
{
    stdio_init_all();
    os_init();
    apps_init();
    multicore_launch_core1(_core1_cb);
    bt_init();
    while (true)
        ;
    return 0;
}
