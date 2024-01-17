/**
 * Copyright (c) 2020 Raspberry Pi (Trading) Ltd.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#include "pico/time.h"
#include "sw_os/dev.h"
#include "sw_os/state.h"

#include <hardware/i2c.h>
#include <math.h>
#include <pico/binary_info.h>
#include <pico/stdlib.h>

#define SAMPLE_SIZE 30
uint16_t buffer[SAMPLE_SIZE];
int16_t cursor;
repeating_timer_t step_timer;

static void _mpu6050_reset();
static void _mpu6050_read_raw(int16_t accel[3], int16_t gyro[3], int16_t* temp);

static bool _step_count_cb(repeating_timer_t* r);
static void _step_count_analyze();

void os_gyro_init()
{
#if !defined(i2c_default) || !defined(PICO_DEFAULT_I2C_SDA_PIN)                \
    || !defined(PICO_DEFAULT_I2C_SCL_PIN)
#warning i2c/mpu6050_i2c example requires a board with I2C pins
    puts("Default I2C pins were not defined");
    return 0;
#else
    printf("Hello, MPU6050! Reading raw data from registers...\n");

    // This example will use I2C0 on the default SDA and SCL pins (4, 5 on a Pico)
    i2c_init(i2c_default, 400 * 1000);
    gpio_set_function(PICO_DEFAULT_I2C_SDA_PIN, GPIO_FUNC_I2C);
    gpio_set_function(PICO_DEFAULT_I2C_SCL_PIN, GPIO_FUNC_I2C);
    gpio_pull_up(PICO_DEFAULT_I2C_SDA_PIN);
    gpio_pull_up(PICO_DEFAULT_I2C_SCL_PIN);
    // Make the I2C pins available to picotool
    bi_decl(bi_2pins_with_func(PICO_DEFAULT_I2C_SDA_PIN,
                               PICO_DEFAULT_I2C_SCL_PIN, GPIO_FUNC_I2C));
    _mpu6050_reset();
    add_repeating_timer_ms(80, _step_count_cb, NULL, &step_timer);
#endif
}

/**
 * Taken from https://github.com/raspberrypi/pico-examples/blob/master/i2c/mpu6050_i2c/mpu6050_i2c.c
 */

/* Example code to talk to a MPU6050 MEMS accelerometer and gyroscope

   This is taking to simple approach of simply reading registers. It's perfectly
   possible to link up an interrupt line and set things up to read from the
   inbuilt FIFO to make it more useful.

   NOTE: Ensure the device is capable of being driven at 3.3v NOT 5v. The Pico
   GPIO (and therefor I2C) cannot be used at 5v.

   You will need to use a level shifter on the I2C lines if you want to run the
   board at 5v.

   Connections on Raspberry Pi Pico board, other boards may vary.

   GPIO PICO_DEFAULT_I2C_SDA_PIN (On Pico this is GP4 (pin 6)) -> SDA on MPU6050 board
   GPIO PICO_DEFAULT_I2C_SCL_PIN (On Pico this is GP5 (pin 7)) -> SCL on MPU6050 board
   3.3v (pin 36) -> VCC on MPU6050 board
   GND (pin 38)  -> GND on MPU6050 board
*/

// By default these devices  are on bus address 0x68
static int addr = 0x68;

#ifdef i2c_default
static void _mpu6050_reset()
{
    // Two byte reset. First byte register, second byte data
    // There are a load more options to set up the device in different ways that could be added here
    /* Register: PWR_MGMT_1 (0x6B), Value: 0x80, Action: Reset all internal registers (of MPU6050) to default values */
    uint8_t res[] = {0x6B, 0x80};
    i2c_write_blocking(i2c_default, addr, res, 2, false);
    sleep_ms(200); /* Give the device time to perform the reset */

    /* Register: PWR_MGMT_1 (0x6B), Value: 0x00, Action: Take MPU6050 out of sleep mode (which is the default state after reset) */
    uint8_t wake[] = {0x6B, 0x00};
    i2c_write_blocking(i2c_default, addr, wake, 2, false);
    sleep_ms(200); /* Give the device time to get out of the reset */
}

static void _mpu6050_read_raw(int16_t accel[3], int16_t gyro[3], int16_t* temp)
{
    // For this particular device, we send the device the register we want to read
    // first, then subsequently read from the device. The register is auto incrementing
    // so we don't need to keep sending the register we want, just the first.

    uint8_t buffer[6];

    // Start reading acceleration registers from register 0x3B for 6 bytes
    uint8_t val = 0x3B;
    i2c_write_blocking(i2c_default, addr, &val, 1,
                       true);// true to keep master control of bus
    i2c_read_blocking(i2c_default, addr, buffer, 6, false);

    for (int i = 0; i < 3; i++) {
        accel[i] = (buffer[i * 2] << 8 | buffer[(i * 2) + 1]);
    }

    // Now gyro data from reg 0x43 for 6 bytes
    // The register is auto incrementing on each read
    val = 0x43;
    i2c_write_blocking(i2c_default, addr, &val, 1, true);
    i2c_read_blocking(i2c_default, addr, buffer, 6,
                      false);// False - finished with bus

    for (int i = 0; i < 3; i++) {
        gyro[i] = (buffer[i * 2] << 8 | buffer[(i * 2) + 1]);
        ;
    }

    // Now temperature from reg 0x41 for 2 bytes
    // The register is auto incrementing on each read
    val = 0x41;
    i2c_write_blocking(i2c_default, addr, &val, 1, true);
    i2c_read_blocking(i2c_default, addr, buffer, 2,
                      false);// False - finished with bus

    *temp = buffer[0] << 8 | buffer[1];
}

static void _step_count_analyze()
{
    const uint16_t threshold = 26000;
    bool peaked = false;
    int number_of_steps = 0;
    for (int i = 0; i < SAMPLE_SIZE; i++) {
        if (buffer[i] > threshold && !peaked) {
            peaked = true;
            number_of_steps++;
            PRINT(STEP);
        } else if (buffer[i] < threshold && peaked) {
            peaked = false;
        }
    }
    if (number_of_steps)
        state.dev.step += number_of_steps / 2;
    else
        state.dev.step += number_of_steps;
}

static bool _step_count_cb(repeating_timer_t* r)
{
    struct GyroData* data = &state.dev;
    _mpu6050_read_raw(data->acc, data->gyro, &data->temp);
    data->temp = (data->temp / 340.0) + 36.53;
    buffer[cursor++] = sqrt(pow(data->acc[0], 2) + pow(data->acc[1], 2)
                            + pow(data->acc[2], 2));
    if (cursor >= SAMPLE_SIZE) {
        _step_count_analyze();
        cursor = 0;
    }
}
#endif
