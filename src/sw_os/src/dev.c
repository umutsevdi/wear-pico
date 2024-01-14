#include "sw_os/dev.h"
#include "hardware/gpio.h"
#include "pico/time.h"
#include "sw_os/state.h"
#include <stdlib.h>

const int pins[] = {
    [DEV_NONE] = 0,
    [DEV_BUZZER] = 18,
    [DEV_LED] = 19,
    [DEV_VIB] = 20,
};

void os_dev_init(void)
{
    gpio_init(pins[DEV_LED]);
    gpio_set_dir(pins[DEV_LED], true);
    gpio_init(pins[DEV_BUZZER]);
    gpio_set_dir(pins[DEV_BUZZER], true);
    gpio_init(pins[DEV_VIB]);
    gpio_set_dir(pins[DEV_VIB], true);
}

static void _notify_iter(int32_t flag, int in)
{
    if (flag & DEV_VIB) { gpio_put(pins[DEV_VIB], true); }
    if (flag & DEV_LED) { gpio_put(pins[DEV_LED], true); }
    bool value = true;
    if (flag & DEV_BUZZER) {
        for (int i = 0; i < in; i++) {
            gpio_put(pins[DEV_BUZZER], value);
            value = !value;
            sleep_ms(1);
        }
    } else {
        sleep_ms(in);
    }

    if (flag & DEV_LED) { gpio_put(pins[DEV_LED], false); }
    if (flag & DEV_VIB) { gpio_put(pins[DEV_VIB], false); }
}

void os_dev_notify_d(int count, int32_t flag, int in_ms, int out_ms)
{
    for (short i = 0; i < count; i++) {
        _notify_iter(flag, in_ms);
        sleep_ms(out_ms);
    }
    gpio_put(pins[DEV_BUZZER], false);
}

void os_dev_notify(int count, int32_t flag)
{
    os_dev_notify_d(count, flag, 150, 150);
}
