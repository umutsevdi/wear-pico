/******************************************************************************

 * File: include/peripherals.h
 *
 * Author: Umut Sevdi
 * Created: 12/05/23
 * Description: Additional peripheral control library

*****************************************************************************/
#ifndef SW_DEV
#define SW_DEV
#include "sw_common/util.h"

enum dev_t {
    DEV_NONE,
    DEV_BUZZER = 1,
    DEV_LED = 2,
    DEV_VIB = 4,
};

typedef struct {
    int16_t acc[3];
    int16_t gyro[3];
    int16_t temp;
} GyroData;

#define DEV_S(dev)                                                             \
    dev == DEV_BUZZER ? "DEV_BUZZER"                                           \
    : dev == DEV_LED  ? "DEV_LED"                                              \
                      : "DEV_UNDEFINED"

void os_dev_set(enum dev_t dev, bool value);

/**
 * Create a notification effect with desired
 * GPIOs for 360ms.
 * @count - number of repeat
 * @flag - Which GPIOs to trigger.
 *
 * Example:
 * os_dev_notify(2, DEV_BUZZER | DEV_LED);
 */
void os_dev_notify(int count, int32_t flag);
/**
 * os_dev_notify with custom notification durations. 
 *
 * @in_ms - Active duration of the selected GPIOs
 * @out_ms - Passive duration of the selected GPIOs
 *
 */
void os_dev_notify_d(int count, int32_t flag, int in_ms, int out_ms);

/**
 * Fetches and returns the current values
 * from the Gyroscope.
 * Sets state.dev.dis_acc and state.dev.dis_acc
 * to the square root of the X Y Z vectors.
 * @return GyroData
 */
GyroData os_gyro_fetch();

#define GYRO_S(G)                                                              \
    "Acc{%d,%d,%d}, Gyro{%d,%d,%d}, Temp %d*C\n", G.acc[0], G.acc[1],          \
        G.acc[2], G.gyro[0], G.gyro[1], G.gyro[2], G.temp

void beep_beep();
#endif// !SW_PERIPHERALS
