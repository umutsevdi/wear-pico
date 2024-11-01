/******************************************************************************

 * File: include/apps.h
 *
 * Author: Umut Sevdi
 * Created: 12/06/23
 * Description: Applications created for the smartwatch

*****************************************************************************/
#ifndef WP_APPS
#define WP_APPS

#include "wp_common/util.h"
#include "wp_res/resources.h"
#include <DEV_Config.h>
#include <LCD_1in28.h>
#include <Touch_1in28.h>

#define TOUCH_GESTURE 0
#define TOUCH_POINT 1
#define CANVAS_WIDTH 176
#define CANVAS_HEIGHT 110


/******************************************************************************
                                Modules
*****************************************************************************/

/* Error types for the applications */
enum app_status_t {
    APP_OK,
    APP_WARN_TOUCH_FAILED,
    APP_NO_POPUP,
    APP_TIMEOUT_NOTIFY,
    APP_ERROR_ALLOC,
    APP_ERROR_TIMER_CREATE,
    APP_ERROR_INVALID_POPUP,
    APP_ERROR_INVALID_APP,
    APP_ERROR_OS_UNINITIALIZED,
};

/* Initializes the display */
enum app_status_t apps_init(void);

/* Loads selected screen module */
enum app_status_t apps_load(enum screen_t s);

enum disp_t {
    DISP_SYNC,    /* The buffer is fully synchronized */
    DISP_PARTIAL, /* The buffer is requires update on sections that can be
                     overwritten */
    DISP_REDRAW   /* The screen requires a complete redraw */
};

/* Screen state */
typedef struct {
    UWORD* buffer; /* pixel buffer for regular images*/
    UDOUBLE buffer_s;
    UWORD* canvas_buffer;
    bool is_saved;        /* whether canvas is saved or not */
    enum screen_t sstate; /* Active screen */
    enum disp_t redraw;   /* Should cause redraw or not */
    int post_time;        /* Post process timer */
    repeating_timer_t __post_timer;
} Display;

/* Display implementation. Singleton object. */
extern Display screen;

/**
 * Checks for new pop-up events. If there is one loads it as pop-up.
 *
 * @param current - Current pop-up type, POPUP_NONE if the current screen is not a
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
/******************************************************************************
                                UI Utilities
*****************************************************************************/

/**
 * Draws the given DateTime to the position.
 * @param dt       - to draw.
 * @param base_x   - starting position x
 * @param base_y   - starting position y
 * @param show_sec - whether to reduce the font size and include seconds
 */
void apps_paint_time(DateTime* dt, int base_x, int base_y, bool show_sec);

/**
 * Returns whether the selected area has been clicked
 * @param x_start - starting position x
 * @param y_start - starting position y
 * @param width   -  of the selection rectangle
 * @param height  - of the selection rectangle
 */
bool apps_is_clicked(int x_start, int y_start, int width, int height);

/**
 * apps_is_clicked with extra options
 * @param x - X position of the cursor
 * @param y - Y position of the cursor
 * @param x_start - starting position x
 * @param y_start - starting position y
 * @param width   -  of the selection rectangle
 * @param height  - of the selection rectangle
 */
bool apps_is_clicked_d(int x, int y, int x_start, int y_start, int width,
                       int height);

#define apps_is_exited() apps_is_clicked(160, 30, 30, 30)

/**
 * Retrieves the status from various sources and draws to the buffer
 * @param is_cb - Whether this function is called by a callback
 */
void apps_post_process(bool is_cb);

/* Draws given resource to the screen */
void apps_draw(Resource res, int start_x, int start_y);

/* Clears the screen */
void apps_reset();

/* Inserts the titlebar of the application if the states don't match
 * - If p_title is set to anything other than POPUP_NONE, it will
 * overwrite the screen value
 * @param s_title - Title of the screen
 * @param p_title - Title of the pop-up, if any
 *
 * @returns - Whether titlebar is inserted or not
 */
bool apps_set_titlebar(enum screen_t s_title, enum popup_t p_title);

/**
 * Initializes the given module
 * @param screen_type - Type of the screen, if there is
 * @param popup_type - Type of the popup, if there is
 * @param touch_type - Touch type to set, either TOUCH_GESTURE or TOUCH_POINT
 * @returns - APP_OK if configuration is successful, APP_WARN_TOUCH_FAILED on
 * failure
 */
enum app_status_t apps_set_module(enum screen_t screen_type,
                                  enum popup_t popup_type, int touch_type);
#endif
