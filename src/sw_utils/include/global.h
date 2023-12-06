/******************************************************************************

 * File: include/global.h
 *
 * Author: Umut Sevdi
 * Created: 11/25/23
 * Description: Global clock state

*****************************************************************************/
#ifndef SW_GLOBAL
#define SW_GLOBAL

#include "util.h"
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

#define DATETIME_DAY(d)                                                        \
    d == 1   ? "Monday"                                                        \
    : d == 2 ? "Tuesday"                                                       \
    : d == 3 ? "Wednesday"                                                     \
    : d == 4 ? "Thursday"                                                      \
    : d == 5 ? "Friday"                                                        \
    : d == 6 ? "Saturday"                                                      \
    : d == 7 ? "Sunday"                                                        \
             : "Null"
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
    bool is_connected;
    struct {
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
} SwState;

extern SwState state;

DateTime sw_get_real_time();
void sw_init_state();
#endif
