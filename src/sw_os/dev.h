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

enum DEV_T {
    DEV_BUZZER = 18,
    DEV_LED = 19,
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

void os_dev_set(enum DEV_T dev, bool value);

/**
 * Turn on the desired device for the set amount of time.
 * @dev - device to turn on
 * @time_m - duration in millisecond
 */
void os_dev_set_for(enum DEV_T dev, unsigned long time_m);

void os_dev_notify();

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

#endif// !SW_PERIPHERALS
