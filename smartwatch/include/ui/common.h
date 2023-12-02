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

} SCR_STATUS;

enum SCREEN_T {
    SCREEN_CLOCK,
    SCREEN_MENU,
    SCREEN_ALARM,
    SCREEN_CHRONO,
    SCREEN_EVENT,
    SCREEN_MEDIA,
    SCREEN_STEP,
};

/* Whether a pop-up is enabled or not. if so which popup is enabled */
enum POPUP_T {
    POPUP_NONE,   /* Pop-Up is disabled, showing regular screen */
    POPUP_CALL,   /* Incoming call UI */
    POPUP_NOTIFY, /* Notification UI */
    POPUP_ALARM   /* Alarm UI */
};

typedef struct {
    /* pixel buffer for regular images*/
    UWORD* buffer;
    /* buffer for pop-ups. So the original screen state is kept */
    UWORD* p_buffer;
    UDOUBLE buffer_s;

    enum SCREEN_T sstate;
    enum POPUP_T pstate;
    bool redraw;
    bool show_sec;
} SwScreen;

extern SwScreen s;

SCR_STATUS sw_scr_init(void);

#define TOUCH_GESTURE 0
#define TOUCH_POINT 1
#define COLOR_BG 0x42
#define COLOR_FG 0xffff

/******************************************************************************
                            Modules
*****************************************************************************/

SCR_STATUS scr_load_menu(void);
SCR_STATUS scr_load_alarm(void);
SCR_STATUS scr_load_chono(void);
SCR_STATUS scr_load_event(void);
SCR_STATUS scr_load_media(void);
SCR_STATUS scr_load_step(void);

SCR_STATUS scr_load_clock(void);

#define GESTURE_S(G)                                                           \
    G == UP             ? "UP   "                                              \
    : G == Down         ? "DOWN "                                              \
    : G == LEFT         ? "LEFT "                                              \
    : G == RIGHT        ? "RIGHT"                                              \
    : G == CLICK        ? "CLICK"                                              \
    : G == DOUBLE_CLICK ? "DOUBL"                                              \
    : G == LONG_PRESS   ? "LONG "                                              \
                        : "NONE "

#define MODE_S(M)                                                              \
    M == TOUCH_INIT          ? "INIT"                                          \
    : M == TOUCH_IRQ         ? "IRQ "                                          \
    : M == TOUCH_FUNCTION    ? "FUNC"                                          \
    : M == TOUCH_DRAW        ? "DRAW"                                          \
    : M == TOUCH_OUT_GESTURE ? "GEST"                                          \
    : M == TOUCH_NO_DRAW     ? "NDRW"                                          \
                             : "NONE"
#endif

void scr_paint_time(DateTime* dt, int base_x, int base_y);

#define SET_MODULE(M, TOUCH_TYPE)                                              \
    fprintf(stdout, __FILE__ "#%s():%d MODULE " #M " \r\n", __func__,          \
            __LINE__);                                                         \
    XY.mode = TOUCH_TYPE;                                                      \
    if (Touch_1IN28_init(XY.mode) != 1) WARN(SCR_WARN_TOUCH_FAILED)\
