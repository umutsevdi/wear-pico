#include "ui/menu.h"

/**
 * Loads the menu item and initializes it's context
 * @current - selected menu item
 * SCR_STATUS scr_load_alarm(void);
 * SCR_STATUS scr_load_chono(void);
 * SCR_STATUS scr_load_event(void);
 * SCR_STATUS scr_load_media(void);
 * SCR_STATUS scr_load_step(void);
 */
static void sw_scr_select(enum SW_MENU_T current);

SCR_STATUS scr_load_menu()
{
    SET_MODULE(SCREEN_MENU, TOUCH_GESTURE);
    s.sstate = SCREEN_MENU;
    s.redraw = true;

    static const unsigned char* menu_frames[] = {
        menu_alarm, menu_stopwatch, menu_events, menu_media, menu_pedometer};

    enum SW_MENU_T current = SW_MENU_ALARM;
    while (true) {
        /*Calling DOWN returns to clock*/
        switch (XY.Gesture) {
        case UP: return SCR_STATUS_OK;
        case LEFT:
            current = current == SW_MENU_SIZE - 1 ? SW_MENU_ALARM : current + 1;
            s.redraw = true;
            break;
        case RIGHT:
            current = current == 0 ? SW_MENU_SIZE - 1 : current - 1;
            s.redraw = true;
            break;
        case CLICK: sw_scr_select(current); break;
        case DOUBLE_CLICK: /* TODO make screen black */
        case LONG_PRESS: break;
        }
        if (s.sstate != SCREEN_MENU || s.redraw) {
            PRINT("%s", , MENU_S(current));
            s.redraw = false;
            s.sstate = SCREEN_MENU;
            Paint_DrawImage(menu_frames[current], 0, 0, 240, 240);
            LCD_1IN28_Display(s.buffer);
            XY.Gesture = None;
        }
    }
}

static void sw_scr_select(enum SW_MENU_T current)
{
    PRINT("%s", , MENU_S(current));
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
    default: break;
    }
    XY.mode = TOUCH_GESTURE;
    if (Touch_1IN28_init(XY.mode) != 1) WARN(SCR_WARN_TOUCH_FAILED);
}
