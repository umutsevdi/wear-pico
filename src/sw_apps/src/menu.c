#include "sw_apps/apps.h"
#include "sw_res/resources.h"

/* Initializes the clock User Interface */
extern enum app_status_t apps_load_clock(void);
/* Shuts the screen off until its cancellation */
extern enum app_status_t apps_lock_screen(void);
/* Initializes the menu User Interface */
extern enum app_status_t apps_load_menu(void);
/* Initializes the Alarm User Interface */
extern enum app_status_t apps_load_alarm(void);
/* Initializes the Stopwatch User Interface */
extern enum app_status_t apps_load_chrono(void);
/* Initializes the Calendar User Interface */
extern enum app_status_t apps_load_calendar(void);
/* Initializes the Media User Interface */
extern enum app_status_t apps_load_media(void);
/* Initializes the notepad User Interface */
extern enum app_status_t apps_load_notepad(void);
/* Initializes the Pedometer User Interface */
extern enum app_status_t apps_load_step(void);
/* Initializes the logger User Interface */
extern enum app_status_t apps_load_log(void);

/* Returns whether given screen requires TOUCH_GESTURE or TOUCH_POINT */
static int _get_touch_type(enum screen_t s);

enum app_status_t apps_load(enum screen_t s)
{
    if (s < 0 || s >= SCREEN_T_SIZE) { return ERROR(APP_ERROR_INVALID_APP); }
    int current_mode = XY.mode;
    apps_set_module(s, POPUP_NONE, _get_touch_type(s));

    enum app_status_t r;
    switch (s) {
    case SCREEN_CLOCK: r = apps_load_clock(); break;
    case SCREEN_LOCK: r = apps_lock_screen(); break;
    case SCREEN_MENU: r = apps_load_menu(); break;
    case SCREEN_ALARM: r = apps_load_alarm(); break;
    case SCREEN_CHRONO: r = apps_load_chrono(); break;
    case SCREEN_MEDIA: r = apps_load_media(); break;
    case SCREEN_LOG: r = apps_load_log(); break;
    case SCREEN_NOTE: r = apps_load_notepad(); break;
    case SCREEN_STEP: r = apps_load_step(); break;
    case SCREEN_CALENDAR: r = apps_load_calendar(); break;
    default: break;
    }

    /* Revert the module configurations */
    XY.mode = current_mode;
    if (Touch_1IN28_init(XY.mode) != 1) WARN(SCR_WARN_TOUCH_FAILED);
    return r;
}

/* Displays the current menu screen */
static void _menu_display(enum menu_t current)
{
    if (screen.redraw == DISP_REDRAW) {
        apps_reset();
        apps_draw(res_get_direction(GEST_DIR_L), 0, 90);
        apps_draw(res_get_direction(GEST_DIR_R), 200, 90);
        apps_draw(res_get_direction(GEST_DIR_D), 96, 202);
    }
    screen.sstate = SCREEN_MENU;
    apps_draw(res_get_menu_screen(current), 40, 40);
    apps_post_process(false);
    LCD_1IN28_Display(screen.buffer);

    PRINT("SELECT %s", , menu_to_str(current));
    screen.redraw = DISP_SYNC;
    XY.Gesture = None;
}

enum app_status_t apps_load_menu()
{
    enum menu_t current = MENU_ALARM;
    while (true) {
        if (!apps_poll_popup()) { screen.redraw = DISP_REDRAW; }
        switch (XY.Gesture) {
        case UP: return APP_OK; /*Calling DOWN returns to clock*/
        case LEFT:
            current = current == MENU_T_SIZE - 1 ? MENU_ALARM : current + 1;
            screen.redraw = DISP_REDRAW;
            break;
        case RIGHT:
            current = current == 0 ? MENU_T_SIZE - 1 : current - 1;
            screen.redraw = DISP_REDRAW;
            break;
        case CLICK: apps_load(current + 3); break;
        case DOUBLE_CLICK: apps_load(SCREEN_LOCK); break;
        case LONG_PRESS: break;
        }

        if (screen.sstate != SCREEN_MENU) {
            screen.sstate = SCREEN_MENU;
            screen.redraw = DISP_REDRAW;
        }
        if (screen.redraw) { _menu_display(current); }
    }
}

static int _get_touch_type(enum screen_t s)
{
    static const int touch_types[SCREEN_T_SIZE] = {
        [SCREEN_CLOCK] = TOUCH_GESTURE, [SCREEN_LOCK] = TOUCH_GESTURE,
        [SCREEN_MENU] = TOUCH_GESTURE,  [SCREEN_ALARM] = TOUCH_POINT,
        [SCREEN_CHRONO] = TOUCH_POINT,  [SCREEN_MEDIA] = TOUCH_POINT,
        [SCREEN_STEP] = TOUCH_POINT,    [SCREEN_CALENDAR] = TOUCH_POINT,
        [SCREEN_NOTE] = TOUCH_POINT,    [SCREEN_LOG] = TOUCH_POINT,
    };
    return touch_types[s];
}
