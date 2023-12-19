#include "sw_apps/apps.h"
#include "sw_res/resources.h"

#define MENU_S(CURRENT)                                                        \
    CURRENT == MENU_ALARM    ? "MENU_ALARM"                                    \
    : CURRENT == MENU_CHRONO ? "MENU_CHRONO"                                   \
    : CURRENT == MENU_EVENT  ? "MENU_EVENT"                                    \
    : CURRENT == MENU_MEDIA  ? "MENU_MEDIA"                                    \
    : CURRENT == MENU_STEP   ? "MENU_STEP"                                     \
                             : "NONE"

/**
 * Loads the menu item and initializes it's context
 * @current - selected menu item
 * enum scr_status_t apps_load_alarm(void);
 * enum scr_status_t apps_load_chono(void);
 * enum scr_status_t apps_load_event(void);
 * enum scr_status_t apps_load_media(void);
 * enum scr_status_t apps_load_step(void);
 */
static void _menu_select(enum menu_t current)
{
    switch (current) {
    /*case MENU_ALARM:scr_load_alarm(); break;*/
    case MENU_CHRONO: apps_load_chrono(); break;
    case MENU_MEDIA: apps_load_media(); break;
    case MENU_STEP:
        apps_load_step();
        break;
        /*
    case MENU_EVENT:scr_load_event();break;
*/
    default: return;
    }
    XY.mode = TOUCH_GESTURE;
    if (Touch_1IN28_init(XY.mode) != 1) WARN(SCR_WARN_TOUCH_FAILED);
}

/* Displays the current menu screen */
static void _menu_display(enum menu_t current)
{
    if (screen.redraw == DISP_REDRAW) {
        apps_draw(res_reset(), 0, 0);
        apps_draw(res_get_direction(GEST_DIR_L), 0, 90);
        apps_draw(res_get_direction(GEST_DIR_R), 200, 90);
        apps_draw(res_get_direction(GEST_DIR_D), 96, 202);
    }
    screen.sstate = SCREEN_MENU;
    apps_draw(res_get_menu_screen(current), 40, 40);
    apps_post_process(false);
    LCD_1IN28_Display(screen.buffer);
    PRINT("%s", , MENU_S(current));
    screen.redraw = DISP_SYNC;
    XY.Gesture = None;
}

enum scr_status_t apps_load_menu()
{
    SET_MODULE(SCREEN_MENU, TOUCH_GESTURE);

    enum menu_t current = MENU_ALARM;
    while (true) {
        switch (XY.Gesture) {
        case UP: return SCR_STATUS_OK; /*Calling DOWN returns to clock*/
        case LEFT:
            current = current == MENU_T_SIZE - 1 ? MENU_ALARM : current + 1;
            screen.redraw = DISP_REDRAW;
            break;
        case RIGHT:
            current = current == 0 ? MENU_T_SIZE - 1 : current - 1;
            screen.redraw = DISP_REDRAW;
            break;
        case CLICK: _menu_select(current); break;
        case DOUBLE_CLICK: apps_lock_screen(); break;
        case LONG_PRESS: break;
        }

        if (screen.sstate != SCREEN_MENU) {
            screen.sstate = SCREEN_MENU;
            screen.redraw = DISP_REDRAW;
        }
        if (screen.redraw) _menu_display(current);
    }
}
