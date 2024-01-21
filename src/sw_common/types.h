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

typedef struct {
    char* bytes;
    uint32_t len;
} String;

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

bool dt_decode(const char buffer[15], DateTime* dt);

bool dt_encode(const DateTime* dt, char buffer[15]);

/* Returns how many days in given month/year combination */
int dt_number_of_days(DateTime* dt);
/* Returns the day of the week based on given day/month/year. */
int dt_get_day_int(DateTime* dt);
/* Returns the string representation of the day of the week based on given day/month/year. */
const char* dt_get_day(DateTime* dt);

/**
 * Compares given dates based on given flag
 * Example:
 * - date_cmp(dt1, dt2, DT_WC_YEAR | DT_WC_MONTH); -> Compare YEAR and MONTH
 * - date_cmp(dt1, dt2, ~dt1->flag);               -> Compare valid dt1 fields
 * - date_cmp(dt1, dt2, ~(dt2->flag | dt1->flag)); -> Compare common fields
 * @returns
 * *  1 if dt1 is after dt2
 * *  0 if dt1 and dt2 are equal
 * * -1 if dt1 is before dt2
 */
int dt_cmp(const DateTime* dt1, const DateTime* dt2, int16_t flag);

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
    SCREEN_STEP,
    SCREEN_CALENDAR,
    SCREEN_NOTE,
    SCREEN_LOG,

    SCREEN_T_SIZE
};

#define screen_to_str(screen)                                                  \
    screen == SCREEN_CLOCK      ? "SCREEN_CLOCK"                               \
    : screen == SCREEN_LOCK     ? "SCREEN_LOCK"                                \
    : screen == SCREEN_MENU     ? "SCREEN_MENU"                                \
    : screen == SCREEN_ALARM    ? "SCREEN_ALARM"                               \
    : screen == SCREEN_CHRONO   ? "SCREEN_CHRONO"                              \
    : screen == SCREEN_MEDIA    ? "SCREEN_MEDIA"                               \
    : screen == SCREEN_STEP     ? "SCREEN_STEP"                                \
    : screen == SCREEN_CALENDAR ? "SCREEN_CALENDAR"                            \
    : screen == SCREEN_NOTE     ? "SCREEN_NOTE"                                \
    : screen == SCREEN_LOG      ? "SCREEN_LOG"                                 \
                                : "SCREEN_NONE"
enum menu_t {
    MENU_ALARM,
    MENU_CHRONO,
    MENU_MEDIA,
    MENU_STEP,
    MENU_CALENDAR,
    MENU_NOTE,
    MENU_LOG,

    MENU_T_SIZE
};

#define menu_to_str(menu)                                                      \
    menu == MENU_ALARM      ? "MENU_ALARM"                                     \
    : menu == MENU_CHRONO   ? "MENU_CHRONO"                                    \
    : menu == MENU_MEDIA    ? "MENU_MEDIA"                                     \
    : menu == MENU_STEP     ? "MENU_STEP"                                      \
    : menu == MENU_CALENDAR ? "MENU_CALENDAR"                                  \
    : menu == MENU_NOTE     ? "MENU_NOTE"                                      \
    : menu == MENU_LOG      ? "MENU_LOG"                                       \
                            : "MENU_NONE"

/* Whether a pop-up is enabled or not and its type */
enum popup_t {
    POPUP_NONE,     /* No Pop-Up is available */
    POPUP_ALARM,    /* Alarm UI */
    POPUP_CALL,     /* Incoming call UI */
    POPUP_NOTIFY,   /* Notification UI */
    POPUP_REMINDER, /* Reminder UI */

    POPUP_T_SIZE
};

#define popup_to_str(popup)                                                    \
    popup == POPUP_NONE       ? "POPUP_NONE"                                   \
    : popup == POPUP_ALARM    ? "POPUP_ALARM"                                  \
    : popup == POPUP_CALL     ? "POPUP_CALL"                                   \
    : popup == POPUP_REMINDER ? "POPUP_REMINDER"                               \
    : popup == POPUP_NOTIFY   ? "POPUP_NOTIFY"                                 \
                              : "POPUP_NONE"

typedef struct {
    bool is_active;
    DateTime at;
} Alarm;

typedef struct {
    DateTime at;
    char title[31];
} Event;

#endif
