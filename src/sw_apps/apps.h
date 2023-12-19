/******************************************************************************

 * File: include/apps.h
 *
 * Author: Umut Sevdi
 * Created: 12/06/23
 * Description: Applications created for the smartwatch

*****************************************************************************/
#ifndef SW_APPS
#define SW_APPS

#include "sw_common/util.h"
#include "sw_os/state.h"
#include "sw_res/resources.h"
#include <DEV_Config.h>
#include <LCD_1in28.h>
#include <Touch_1in28.h>

#define TOUCH_GESTURE 0
#define TOUCH_POINT 1
#define COLOR_BG 0x42
#define COLOR_FG 0xffff
#define SCR_SCREEN 0, 0, 240, 240
#define SCR_TRAY 20, 13

/******************************************************************************
                                Modules
*****************************************************************************/

/* Error types for the applications */
enum scr_status_t {
    SCR_STATUS_OK,
    SCR_ERROR_ALLOC,
    SCR_WARN_TOUCH_FAILED,
    SCR_ERROR_TIMER_CREATE,
};

/* Initializes the display */
enum scr_status_t apps_init(void);
/* Initializes the menu User Interface */
enum scr_status_t apps_load_menu(void);
/* Initializes the Alarm User Interface */
enum scr_status_t apps_load_alarm(void);
/* Initializes the Stopwatch User Interface */
enum scr_status_t apps_load_chrono(void);
/* Initializes the Calendar User Interface */
enum scr_status_t apps_load_event(void);
/* Initializes the Media User Interface */
enum scr_status_t apps_load_media(void);
/* Initializes the Pedometer User Interface */
enum scr_status_t apps_load_step(void);
/* Initializes the clock User Interface */
enum scr_status_t apps_load_clock(void);
/* Shuts the screen off until its cancellation */
enum scr_status_t apps_lock_screen(void);

#define SET_MODULE(M, TOUCH_TYPE)                                              \
    printf("%s#%s():%d MODULE " #M " \r\n", _file_fmt(__FILE__), __func__,     \
           __LINE__);                                                          \
    XY.mode = TOUCH_TYPE;                                                      \
    if (Touch_1IN28_init(XY.mode) != 1) WARN(SCR_WARN_TOUCH_FAILED);           \
    screen.redraw = DISP_REDRAW;                                               \
    XY.x_point = 0;                                                            \
    XY.y_point = 0;

enum disp_t {
    DISP_SYNC,    /* The buffer is fully synchronized */
    DISP_PARTIAL, /* The buffer is requires update on sections that can be
                     overwritten */
    DISP_REDRAW   /* The screen requires a complete redraw */
};

/* Screen state */
typedef struct {
    UWORD* buffer;   /* pixel buffer for regular images*/
    UWORD* p_buffer; /* buffer for pop-ups.*/
    UDOUBLE buffer_s;
    enum screen_t sstate; /* Active screen */
    enum popup_t pstate;
    enum disp_t redraw; /* Should cause redraw or not */
    int post_time;      /* Post process timer */
    repeating_timer_t __post_timer;
} Display;

/* Display implementation. Singleton object. */
extern Display screen;

/******************************************************************************
                                UI Utilities
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

#define apps_is_exited() apps_is_clicked(140, 20, 40, 40)

/**
 * Retrieves the status from various sources and draws to the buffer
 * @is_cb - Whether this function is called by a callback
 */
void apps_post_process(bool is_cb);

/* Draws given resource to the screen */
void apps_draw(Resource res, int start_x, int start_y);

/* Inserts the titlebar of the application if the states don't match
 * - If p_title is set to anything other than POPUP_NONE, it will
 * overwrite the screen value
 * @s_title - Title of the screen
 * @p_title - Title of the pop-up, if any
 *
 * @returns - Whether titlebar is inserted or not
 */
bool apps_set_titlebar(enum screen_t s_title, enum popup_t p_title);
#endif
