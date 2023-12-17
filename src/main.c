#include <stdint.h>
#include <stdio.h>

#include <pico/multicore.h>
#include <pico/stdlib.h>
#include <pico/time.h>

#include "sw_apps/apps.h"
#include "sw_common/util.h"
#include "sw_os/dev.h"
#include "sw_os/state.h"

int main(int argc, char* argv[])
{
    stdio_init_all();
    os_init();
    apps_init();
    multicore_launch_core1((void (*)())apps_load_clock);

    while (1) {
        //        printf("CORE 0 running with %s\n", __func__);
        sleep_ms(1000);
        os_gyro_fetch();
    }

    return 0;
}
