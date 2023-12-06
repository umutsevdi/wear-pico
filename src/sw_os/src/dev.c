#include "sw_os/dev.h"
#include "hardware/gpio.h"

void sw_peripheral_init()
{
    gpio_init(BUZZER);
    gpio_init(LED);
    gpio_set_dir(BUZZER, true);
    gpio_set_dir(LED, true);
}

void sw_peripheral_toggle(int peripheral, bool value)
{
    gpio_put(peripheral, value);
}
