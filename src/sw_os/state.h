/******************************************************************************

 * File: include/global.h
 *
 * Author: Umut Sevdi
 * Created: 11/25/23
 * Description: Global smartwatch state

*****************************************************************************/
#ifndef SW_STATE
#define SW_STATE

#include <pico/time.h>

typedef enum {
    DT_WC_YEAR = 0b000001,
    DT_WC_MONTH = 0b000010,
    DT_WC_DAY = 0b000100,
    DT_WC_HOUR = 0b001000,
    DT_WC_MIN = 0b010000,
    DT_WC_SEC = 0b100000,
} DT_WC;

/**
 * Represents any date or time with optional fields.
 *
 * String Format:  YYYYmmDDHHMMSS
 *
 * - DateTime structure is generated from an array of 14 characters where each
 * can be a number or '?'.
 * If any date time field starts with a '?' that section is ignored.
 * - After the sturcture is generated DT_WC enum can be used on
 * DateTime.flag to obtain ignored fields.
 *
 *   Example:
 *   - 202112201830??  20 December 2012 18:30
 *   - ????????123015  12:30:15
 */
typedef struct {
    int16_t flag;
    uint16_t year;
    uint8_t month;
    uint8_t day;
    uint8_t hour;
    uint8_t minute;
    uint8_t second;
} DateTime;

const char* day_of_the_week(DateTime* dt);

#define DATETIME_MONTH(m)                                                      \
    m == 1    ? "Jan"                                                          \
    : m == 2  ? "Feb"                                                          \
    : m == 3  ? "Mar"                                                          \
    : m == 4  ? "Apr"                                                          \
    : m == 5  ? "May"                                                          \
    : m == 6  ? "Jun"                                                          \
    : m == 7  ? "Jul"                                                          \
    : m == 8  ? "Aug"                                                          \
    : m == 9  ? "Sep"                                                          \
    : m == 10 ? "Oct"                                                          \
    : m == 11 ? "Nov"                                                          \
    : m == 12 ? "Dec"                                                          \
              : "Null"

typedef struct {
    DateTime dt;
    uint64_t dt_padding;
    repeating_timer_t __dt_timer;
    bool is_connected;
    struct {
        /* stack values for the gpio pins
         * to prevent early cancels;
         * Each set or reset increases/decreases the value, gpio is disabled only at 0;
         */
        int stack[20];
        int16_t temp;
        int16_t dist_acc;
        int16_t dist_gyro;
    } dev;
    struct {
        bool on_charge;
        int pct;
    } bat;
    struct {
        bool is_playing;
        char song[30];
        char album[30];
        char artist[30];
    } media;
    bool clock_show_sec;
    struct {
        DateTime dt;
        bool enabled;
        repeating_timer_t timer;
    } chrono;
    int32_t step;
} SwState;

extern SwState state;

DateTime os_get_time();
void os_init();
#endif
