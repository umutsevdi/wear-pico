/******************************************************************************

 * File: sw_common/types.h
 *
 * Author: Umut Sevdi
 * Created: 12/17/23
 * Description: Commonly used types across modules

*****************************************************************************/
#ifndef SW_COMMON_TYPES
#define SW_COMMON_TYPES
#include <stdbool.h>
#include <stdint.h>

typedef enum {
    DT_WC_YEAR = 0b000001,
    DT_WC_MONTH = 0b000010,
    DT_WC_DAY = 0b000100,
    DT_WC_HOUR = 0b001000,
    DT_WC_MIN = 0b010000,
    DT_WC_SEC = 0b100000,
} dtwc_t;

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

bool str_to_date(const char buffer[15], DateTime* dt);

bool date_to_str(const DateTime* dt, char buffer[15]);

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

enum screen_t {
    SCREEN_CLOCK,
    SCREEN_LOCK,
    SCREEN_MENU,
    SCREEN_ALARM,
    SCREEN_CHRONO,
    SCREEN_MEDIA,
    SCREEN_EVENT,
    SCREEN_STEP,
    SCREEN_LOG,

    SCREEN_T_SIZE
};

enum menu_t {
    MENU_ALARM,
    MENU_CHRONO,
    MENU_MEDIA,
    MENU_EVENT,
    MENU_STEP,
    MENU_LOG,

    MENU_T_SIZE
};

/* Whether a pop-up is enabled or not and its type */
enum popup_t {
    POPUP_NONE,   /* No Pop-Up is available */
    POPUP_ALARM,  /* Alarm UI */
    POPUP_CALL,   /* Incoming call UI */
    POPUP_NOTIFY, /* Notification UI */

    POPUP_T_SIZE
};

typedef struct {
    bool is_active;
    DateTime at;
} Alarm;

#endif
