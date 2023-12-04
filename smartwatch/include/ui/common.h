/******************************************************************************

 * File: include/ui/common.h
 *
 * Author: Umut Sevdi
 * Created: 12/01/23
 * Description: Common UI utilities

*****************************************************************************/

#ifndef SW_SCREEN
#define SW_SCREEN

#include "DEV_Config.h"
#include "Debug.h"
#include "GUI_Paint.h"
#include "LCD_1in28.h"
#include "Touch_1in28.h"
#include "fonts.h"
#include "global.h"
#include "hardware/adc.h"
#include "pico/stdlib.h"
#include "protocol.h"
#include "resources.h"
#include "util.h"
#include <stdio.h>

typedef enum {
    SCR_STATUS_OK,
    SCR_ERROR_ALLOC,
    SCR_WARN_TOUCH_FAILED,
    SCR_ERROR_TIMER_CREATE,
} SCR_STATUS;

#define TOUCH_GESTURE 0
#define TOUCH_POINT 1
#define COLOR_BG 0x42
#define COLOR_FG 0xffff

enum SCREEN_T {
    SCREEN_CLOCK,
    SCREEN_MENU,
    SCREEN_ALARM,
    SCREEN_CHRONO,
    SCREEN_EVENT,
    SCREEN_MEDIA,
    SCREEN_STEP,
};

/* Whether a pop-up is enabled or not and its type */
enum POPUP_T {
    POPUP_NONE,   /* No Pop-Up is available */
    POPUP_CALL,   /* Incoming call UI */
    POPUP_NOTIFY, /* Notification UI */
    POPUP_ALARM   /* Alarm UI */
};

/* Initializes the menu User Interface */
SCR_STATUS scr_load_menu(void);
/* Initializes the Alarm User Interface */
SCR_STATUS scr_load_alarm(void);
/* Initializes the Stopwatch User Interface */
SCR_STATUS scr_load_chono(void);
/* Initializes the Calendar User Interface */
SCR_STATUS scr_load_event(void);
/* Initializes the Media User Interface */
SCR_STATUS scr_load_media(void);
/* Initializes the Pedometer User Interface */
SCR_STATUS scr_load_step(void);

SCR_STATUS scr_load_clock(void);

/******************************************************************************
                            UI Utils
*****************************************************************************/

/**
 * Draws the given DateTime to the position.
 * @dt       - to draw.
 * @base_x   - starting position x
 * @base_y   - starting position y
 * @show_sec - whether to reduce the font size and include seconds
 */
void scr_paint_time(DateTime* dt, int base_x, int base_y, bool show_sec);
/**
 * Returns whether the selected area has been clicked
 * @x_start - starting position x
 * @y_start - starting position y
 * @width   -  of the selection rectangle
 * @height  - of the selection rectangle
 */
bool scr_is_clicked(int x_start, int y_start, int width, int height);
#define SCR_IS_CANCELLED scr_is_clicked(145, 15, 45, 45)

#define SET_MODULE(M, TOUCH_TYPE)                                              \
    printf(__FILE__ "#%s():%d MODULE " #M " \r\n", __func__, __LINE__);        \
    XY.mode = TOUCH_TYPE;                                                      \
    if (Touch_1IN28_init(XY.mode) != 1) WARN(SCR_WARN_TOUCH_FAILED)

/**
 * Screen state
 */
typedef struct {
    UWORD* buffer;   /* pixel buffer for regular images*/
    UWORD* p_buffer; /* buffer for pop-ups.*/
    UDOUBLE buffer_s;
    enum SCREEN_T sstate; /* Active screen */
    enum POPUP_T pstate;
    bool redraw; /* Should cause redraw or not */
} SwScreen;

/* SwScreen implementation. Singleton object. */
extern SwScreen screen;

/**
 * Initializes the screen
 */
SCR_STATUS scr_init(void);
#endif
