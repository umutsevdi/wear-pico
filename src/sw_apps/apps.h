/******************************************************************************

 * File: include/apps.h
 *
 * Author: Umut Sevdi
 * Created: 12/06/23
 * Description: Applications created for the smartwatch

*****************************************************************************/
#ifndef SW_APPS
#define SW_APPS

#include "apps_common.h"

/* Initializes the menu User Interface */
SCR_STATUS apps_load_menu(void);
/* Initializes the Alarm User Interface */
SCR_STATUS apps_load_alarm(void);
/* Initializes the Stopwatch User Interface */
SCR_STATUS apps_load_chono(void);
/* Initializes the Calendar User Interface */
SCR_STATUS apps_load_event(void);
/* Initializes the Media User Interface */
SCR_STATUS apps_load_media(void);
/* Initializes the Pedometer User Interface */
SCR_STATUS apps_load_step(void);

SCR_STATUS apps_load_clock(void);
SCR_STATUS apps_load_power_save(void);

#define SET_MODULE(M, TOUCH_TYPE)                                              \
    printf("%s#%s():%d MODULE " #M " \r\n", _file_fmt(__FILE__), __func__,     \
           __LINE__);                                                          \
    XY.mode = TOUCH_TYPE;                                                      \
    if (Touch_1IN28_init(XY.mode) != 1) WARN(SCR_WARN_TOUCH_FAILED);           \
    screen.redraw = DISP_REDRAW

enum SW_MENU_T {
    SW_MENU_ALARM,
    SW_MENU_CHRONO,
    SW_MENU_EVENT,
    SW_MENU_MEDIA,
    SW_MENU_STEP,
    SW_MENU_SIZE
};
#define MENU_S(CURRENT)                                                        \
    CURRENT == SW_MENU_ALARM    ? "MENU_ALARM"                                 \
    : CURRENT == SW_MENU_CHRONO ? "MENU_CHRONO"                                \
    : CURRENT == SW_MENU_EVENT  ? "MENU_EVENT"                                 \
    : CURRENT == SW_MENU_MEDIA  ? "MENU_MEDIA"                                 \
    : CURRENT == SW_MENU_STEP   ? "MENU_STEP"                                  \
                                : "NONE"
extern const unsigned char* menu_frames[];

#endif
