/******************************************************************************

 * File: include/peripherals.h
 *
 * Author: Umut Sevdi
 * Created: 12/05/23
 * Description: Additional peripheral control library

*****************************************************************************/
#ifndef WP_DEV
#define WP_DEV
#include "wp_common/util.h"

enum dev_t {
    DEV_NONE,
    DEV_BUZZER = 1,
    DEV_LED = 2,
    DEV_VIB = 4,
};

#define dev_to_str(dev)                                                        \
    dev == DEV_BUZZER ? "DEV_BUZZER"                                           \
    : dev == DEV_LED  ? "DEV_LED"                                              \
    : dev == DEV_VIB  ? "DEV_VIB"                                              \
                      : "NONE"

/**
 * Create a notification effect with desired
 * GPIOs for 360ms.
 * @param count - number of repeat
 * @param flag - Which GPIOs to trigger.
 *
 * Example:
 * os_dev_notify(2, DEV_BUZZER | DEV_LED);
 */
void os_dev_notify(int count, int32_t flag);
/**
 * os_dev_notify with custom notification durations.
 *
 * @param count - number of repeat
 * @param flag - Which GPIOs to trigger.
 * @param in_ms - Active duration of the selected GPIOs
 * @param out_ms - Passive duration of the selected GPIOs
 *
 */
void os_dev_notify_d(int count, int32_t flag, int in_ms, int out_ms);

#define gyro_to_str(G)                                                         \
    "Acc{%d,%d,%d}, Gyro{%d,%d,%d}, Temp %d*C\n", G.acc[0], G.acc[1],          \
        G.acc[2], G.gyro[0], G.gyro[1], G.gyro[2], G.temp
#endif// !wp_PERIPHERALS
