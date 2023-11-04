#include <stdint.h>
#include <stdio.h>

#include "pico/multicore.h"
#include "pico/stdlib.h"
#include "pico/time.h"

void run()
{
    while (true) {
        printf("CORE 2 running with %s\n", __func__);
        sleep_ms(1000);
    }
}

bool repeating_alarm_cb(struct repeating_timer* timer)
{
    printf("%d INTERRUPT_BEGIN %s:%3d ->[%s]\n", timer->alarm_id, __func__,
           __LINE__, (char*)timer->user_data);
    fflush(stdout);
    absolute_time_t t = get_absolute_time();
    uint64_t c = 0;
    do {
        c++;
    } while (absolute_time_diff_us(t, get_absolute_time()) < 10000000);

    printf("%ld - %d INTERRUPT_END  %s:%3d ->[%s]\n",

           absolute_time_diff_us(t, get_absolute_time()), timer->alarm_id,
           __func__, __LINE__, (char*)timer->user_data);
    return c > 0;
}

int64_t alarm_cb(int32_t r, void* data)
{
    printf("%d INTERRUPT_BEGIN %s:%3d ->[%s]\n", r, __func__, __LINE__,
           (char*)data);
    sleep_ms(2000);
    printf("\r%d INTERRUPT_END  %s:%3d ->[%s]\n", r, __func__, __LINE__,
           (char*)data);
    add_alarm_in_ms(-10000, alarm_cb, "Hello from callback later", false);
    return 0;
}

int main(int argc, char* argv[])
{
    stdio_init_all();
    repeating_timer_t timer;
    printf("OK\r\n");
    add_repeating_timer_ms(5000, repeating_alarm_cb, "CALLBACK txt", &timer);
    multicore_launch_core1(run);

    while (1) {
        printf("CORE 1 running with %s\n", __func__);
        sleep_ms(1000);
    }

    return 0;
}
