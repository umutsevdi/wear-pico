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

void os_dev_set(enum dev_t dev, bool value)
{
    if (value)
        state.dev.stack[dev]++;
    else
        state.dev.stack[dev]--;
    if (state.dev.stack[dev] > 0) {
        gpio_put(dev, 1);
    } else {
        gpio_put(dev, 0);
    }
}

static void _notify_iter(int32_t flag, int in)
{
    if (flag & DEV_LED) os_dev_set(pins[DEV_LED], true);

    bool value = true;
    if (flag & DEV_BUZZER) {
        for (int i = 0; i < in; i++) {
            os_dev_set(pins[DEV_BUZZER], value);
            value = !value;
            sleep_ms(1);
        }
    } else
        sleep_ms(in);

    if (flag & DEV_LED) os_dev_set(pins[DEV_LED], false);
}

struct _NotifyData {
    int count;
    int32_t flag;
    int in;
    int out;
};

int64_t _notify_cb(alarm_id_t id, void* data)
{
    UNUSED(alarm_id_t, id);
    struct _NotifyData* d = (struct _NotifyData*)data;
    for (int i = 0; i < (d->count); i++) {
        _notify_iter(d->flag, d->in);
        sleep_ms(d->out);
    }
    os_dev_set(pins[DEV_BUZZER], false);
    free(data);
    return 0;
}

void os_dev_notify_d(int count, int32_t flag, int in_ms, int out_ms)
{
    struct _NotifyData* d = malloc(sizeof(struct _NotifyData));
    d->count = count;
    d->flag = flag;
    d->in = in_ms;
    d->out = out_ms;
    add_alarm_in_ms(0, _notify_cb, d, true);
}

void os_dev_notify(int count, int32_t flag)
{
    os_dev_notify_d(count, flag, 150, 150);
}
