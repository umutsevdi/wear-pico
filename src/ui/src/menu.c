#include "menu.h"
const unsigned char* menu_frames[] = {menu_alarm, menu_stopwatch, menu_events,
                                      menu_media, menu_pedometer};

/**
 * Loads the menu item and initializes it's context
 * @current - selected menu item
 * SCR_STATUS scr_load_alarm(void);
 * SCR_STATUS scr_load_chono(void);
 * SCR_STATUS scr_load_event(void);
 * SCR_STATUS scr_load_media(void);
 * SCR_STATUS scr_load_step(void);
 */
static void _scr_menu_select(enum SW_MENU_T current);

SCR_STATUS scr_load_menu()
{
    SET_MODULE(SCREEN_MENU, TOUCH_GESTURE);

    enum SW_MENU_T current = SW_MENU_ALARM;
    while (true) {
        /*Calling DOWN returns to clock*/
        switch (XY.Gesture) {
        case UP: return SCR_STATUS_OK;
        case LEFT:
            current = current == SW_MENU_SIZE - 1 ? SW_MENU_ALARM : current + 1;
            screen.redraw = DISP_REDRAW;
            break;
        case RIGHT:
            current = current == 0 ? SW_MENU_SIZE - 1 : current - 1;
            screen.redraw = DISP_REDRAW;
            break;
        case CLICK: _scr_menu_select(current); break;
        case DOUBLE_CLICK: /* TODO make screen black */
        case LONG_PRESS: break;
        }

        if (screen.sstate != SCREEN_MENU) {
            screen.sstate = SCREEN_MENU;
            screen.redraw = DISP_REDRAW;
        }

        if (screen.redraw) {
            screen.sstate = SCREEN_MENU;
            SCR_FILL(menu_frames[current]);
            LCD_1IN28_Display(screen.buffer);
            PRINT("%s", , MENU_S(current));
            screen.redraw = DISP_SYNC;
            XY.Gesture = None;
        }
    }
}

static void _scr_menu_select(enum SW_MENU_T current)
{
    switch (current) {
    /*case SW_MENU_ALARM:scr_load_alarm(); break;*/
    case SW_MENU_CHRONO:
        scr_load_chono();
        break;
        /*
    case SW_MENU_EVENT:scr_load_event();break;
    case SW_MENU_MEDIA:scr_load_media();break;
    case SW_MENU_STEP:scr_load_step();
*/
    default: return;
    }
    XY.mode = TOUCH_GESTURE;
    if (Touch_1IN28_init(XY.mode) != 1) WARN(SCR_WARN_TOUCH_FAILED);
}