#include "ui/common.h"

DateTime sw_get_dt_v() { return (DateTime){0, 2022, 5, 10, 9, 30, 25}; }

SCR_STATUS scr_load_clock()
{
    SET_MODULE(SCREEN_CLOCK, TOUCH_GESTURE);
    s.sstate = SCREEN_CLOCK;

    while (true) {
        switch (XY.Gesture) {
        case Down: scr_load_menu(); break;
        case CLICK:
            s.show_sec = !s.show_sec;
            s.redraw = true;
            break;
        case DOUBLE_CLICK: /* TODO make screen black */ break;
        }
        if (s.sstate != SCREEN_CLOCK || s.redraw) {
            s.sstate = SCREEN_CLOCK;
            s.redraw = false;
            Paint_DrawImage(watch, 0, 0, 240, 240);
            DateTime dt_v = sw_get_dt_v();
            DateTime* dt = &dt_v;
            Paint_DrawString_EN(20, 70, DATETIME_DAY(dt->day % 7 + 1), &Font16,
                                COLOR_BG, COLOR_FG);
            scr_paint_time(dt, s.show_sec ? 0 : 20, s.show_sec ? 100 : 90);
            char bottom_str[100];
            sprintf(bottom_str, "%d %s %d", dt->day, DATETIME_MONTH(dt->month),
                    dt->year % 100);
            Paint_DrawString_EN(90, 160, bottom_str, &Font20, COLOR_BG,
                                COLOR_FG);
            LCD_1IN28_Display(s.buffer);
            XY.Gesture = None;
        }
    }
}

SCR_STATUS scr_load_chono()
{
    SET_MODULE(SCREEN_CHRONO, TOUCH_POINT);
    s.redraw = true;
    bool is_alarm_set = false;
    int x, y;
    while (true) {
        if (x != XY.x_point || y != XY.y_point) {
            printf("%d %d\n", XY.x_point, XY.y_point);
            // click occured
            x = XY.x_point;
            y = XY.y_point;
            is_alarm_set = !is_alarm_set;
            s.redraw = true;
        }
        if (x > 120 && y > 120) return SCR_STATUS_OK;

        if (s.sstate != SCREEN_CHRONO || s.redraw) {
            s.redraw = false;
            s.sstate = SCREEN_CHRONO;
            Paint_DrawImage(is_alarm_set ? app_stopwatch_start
                                         : app_stopwatch_stop,
                            0, 0, 240, 240);
            LCD_1IN28_Display(s.buffer);
            XY.Gesture = None;
        }
    }
}
