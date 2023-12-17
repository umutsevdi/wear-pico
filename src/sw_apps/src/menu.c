#include "sw_apps/apps.h"
#include "sw_res/resources.h"

/**
 * Loads the menu item and initializes it's context
 * @current - selected menu item
 * SCR_STATUS apps_load_alarm(void);
 * SCR_STATUS apps_load_chono(void);
 * SCR_STATUS apps_load_event(void);
 * SCR_STATUS apps_load_media(void);
 * SCR_STATUS apps_load_step(void);
 */
static void _apps_menu_select(enum SW_MENU_T current);

SCR_STATUS apps_load_menu()
{
    SET_MODULE(SCREEN_MENU, TOUCH_GESTURE);

    enum SW_MENU_T current = SW_MENU_ALARM;
    while (true) {
        /*Calling DOWN returns to clock*/
        switch (XY.Gesture) {
        case UP: return SCR_STATUS_OK;
        case LEFT:
            current =
                current == SW_MENU_T_SIZE - 1 ? SW_MENU_ALARM : current + 1;
            screen.redraw = DISP_REDRAW;
            break;
        case RIGHT:
            current = current == 0 ? SW_MENU_T_SIZE - 1 : current - 1;
            screen.redraw = DISP_REDRAW;
            break;
        case CLICK: _apps_menu_select(current); break;
        case DOUBLE_CLICK: apps_lock_screen(); break;
        case LONG_PRESS: break;
        }

        if (screen.sstate != SCREEN_MENU) {
            screen.sstate = SCREEN_MENU;
            screen.redraw = DISP_REDRAW;
        }

        if (screen.redraw) {
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
    }
}

static void _apps_menu_select(enum SW_MENU_T current)
{
    switch (current) {
    /*case SW_MENU_ALARM:scr_load_alarm(); break;*/
    case SW_MENU_CHRONO: apps_load_chrono(); break;
    case SW_MENU_MEDIA: apps_load_media(); break;
    case SW_MENU_STEP:
        apps_load_step();
        break;
        /*
    case SW_MENU_EVENT:scr_load_event();break;
*/
    default: return;
    }
    XY.mode = TOUCH_GESTURE;
    if (Touch_1IN28_init(XY.mode) != 1) WARN(SCR_WARN_TOUCH_FAILED);
}
