#include <stdint.h>
#include <stdio.h>

#include <pico/multicore.h>
#include <pico/stdlib.h>
#include <pico/time.h>

#include "common.h"
#include "global.h"
#include "peripherals.h"
#include "util.h"

void run() { scr_load_clock(); }

int main(int argc, char* argv[])
{
    stdio_init_all();
    sw_init_state();
    scr_init();
    sw_peripheral_init();
    multicore_launch_core1(run);

    while (1) {
        //        printf("CORE 0 running with %s\n", __func__);
        sleep_ms(1000);
    }

    return 0;
}
