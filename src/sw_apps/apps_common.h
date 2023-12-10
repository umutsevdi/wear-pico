/******************************************************************************

 * File: include/ui/common.h
 *
 * Author: Umut Sevdi
 * Created: 12/01/23
 * Description: Common UI utilities

*****************************************************************************/

#ifndef SW_SCREEN
#define SW_SCREEN

#include "sw_os/state.h"
#include "sw_res/resources.h"
#include "sw_utils/util.h"

#include <DEV_Config.h>
#include <LCD_1in28.h>
#include <Touch_1in28.h>

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
void apps_paint_time(DateTime* dt, int base_x, int base_y, bool show_sec);
/**
 * Returns whether the selected area has been clicked
 * @x_start - starting position x
 * @y_start - starting position y
 * @width   -  of the selection rectangle
 * @height  - of the selection rectangle
 */
bool apps_is_clicked(int x_start, int y_start, int width, int height);

/**
 * Retrieves the status from various sources and draws to the buffer
 * @is_cb - Whether this function is called by a callback
 */
void apps_post_process(bool is_cb);

#define SCR_IS_CANCELLED apps_is_clicked(140, 20, 40, 40)
#define SCR_SCREEN 0, 0, 240, 240
#define SCR_TRAY 20, 13

enum DISP_T {
    DISP_SYNC,    /* The buffer and the screen are fully synchronized */
    DISP_PARTIAL, /* The buffer and screen are partially synchronized,
                            screen data needs updating */
    DISP_REDRAW   /* The screen requires a complete redraw */
};

enum SCREEN_T {
    SCREEN_CLOCK,
    SCREEN_MENU,
    SCREEN_ALARM,
    SCREEN_CHRONO,
    SCREEN_EVENT,
    SCREEN_MEDIA,
    SCREEN_STEP,
    SCREEN_PSAVE,
};

/* Whether a pop-up is enabled or not and its type */
enum POPUP_T {
    POPUP_NONE,   /* No Pop-Up is available */
    POPUP_CALL,   /* Incoming call UI */
    POPUP_NOTIFY, /* Notification UI */
    POPUP_ALARM   /* Alarm UI */
};

/**
 * Screen state
 */
typedef struct {
    UWORD* buffer;   /* pixel buffer for regular images*/
    UWORD* p_buffer; /* buffer for pop-ups.*/
    UDOUBLE buffer_s;
    enum SCREEN_T sstate; /* Active screen */
    enum POPUP_T pstate;
    enum DISP_T redraw; /* Should cause redraw or not */
    int post_time;      /* Post process timer */

    repeating_timer_t __post_timer;
} SwScreen;

/* SwScreen implementation. Singleton object. */
extern SwScreen screen;

/**
 * Initializes the screen
 */
SCR_STATUS apps_init(void);
#endif
