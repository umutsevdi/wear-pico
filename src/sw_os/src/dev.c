#include "sw_os/dev.h"
#include "hardware/gpio.h"
#include "pico/time.h"
#include "sw_os/state.h"

static int64_t _set_for_cb(int32_t id, void* dev_h);
static bool _notify_cb(repeating_timer_t* r);

void os_dev_set(enum DEV_T dev, bool value)
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

void os_dev_set_for(enum DEV_T dev, unsigned long time_m)
{
    int* dev_h = malloc(sizeof(enum DEV_T));
    *dev_h = dev;
    add_alarm_in_ms(time_m, _set_for_cb, dev_h, true);
    os_dev_set(dev, true);
}

void os_dev_notify()
{
    int* data = malloc(sizeof(int));
    *data = 6;
    repeating_timer_t* timer = calloc(sizeof(repeating_timer_t), 1);
    add_repeating_timer_ms(150, _notify_cb, data, timer);
}

static int64_t _set_for_cb(int32_t id, void* dev_h)
{
    UNUSED(int, id);
    os_dev_set(*(enum DEV_T*)dev_h, false);
    free(dev_h);
    return 1;
}

static bool _notify_cb(repeating_timer_t* r)
{
    int v = *(int*)r->user_data;
    os_dev_set(DEV_LED, v % 2 == 0);
    (*(int*)r->user_data)--;
    if (v - 1 == 0) {
        cancel_repeating_timer(r);
        free(r->user_data);
        free(r);
    }
    return true;
}
