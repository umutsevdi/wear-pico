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

/******************************************************************************
                                Modules
*****************************************************************************/

/* Error types for the applications */
enum app_status_t {
    APP_OK,
    APP_WARN_TOUCH_FAILED,
    APP_NO_POPUP,
    APP_ERROR_ALLOC,
    APP_ERROR_TIMER_CREATE,
    APP_ERROR_INVALID_POPUP,
};

/* Initializes the display */
enum app_status_t apps_init(void);

void apps_request_popup(Popup p);

/**
 * Checks for new pop-up events. If there is one loads it as pop-up.
 *
 * @current - Current pop-up type, POPUP_NONE if the current screen is not a
 * pop-up.
 *
 * @returns
 *  - the app_status_t of the loaded pop-up if a pop-up is loaded.
 *  - APP_NO_POPUP - When there is no pop-up event to display
 *  - APP_ERROR_INVALID_POPUP - If the current pop-up state is invalid
 *
 * Pop-ups have priority. If higher priority pop-up is active it will not change
 * POPUP_CALL > POPUP_ALARM > POPUP_NOTIFY
 * If higher priority pop-up is active it will not change
 *
 */
enum app_status_t apps_poll_popup();
/* Loads selected screen module */
enum app_status_t apps_load(enum screen_t s);

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
    enum disp_t redraw;   /* Should cause redraw or not */
    int post_time;        /* Post process timer */
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

#define apps_is_exited() apps_is_clicked(160, 30, 30, 30)

/**
 * Retrieves the status from various sources and draws to the buffer
 * @is_cb - Whether this function is called by a callback
 */
void apps_post_process(bool is_cb);

/* Draws given resource to the screen */
void apps_draw(Resource res, int start_x, int start_y);
/* Clears the screen */
void apps_reset();

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
