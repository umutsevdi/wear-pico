/**
 * Copyright (c) 2020 Raspberry Pi (Trading) Ltd.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <stdint.h>

typedef struct {
    int16_t acl[3];
    int16_t gyro[3];
    int16_t temp[3];
} GyroData;

/**
 * Initializes the gyroscope module
 */
void os_gyro_init();

void mpu6050_read_raw(int16_t accel[3], int16_t gyro[3], int16_t* temp);
