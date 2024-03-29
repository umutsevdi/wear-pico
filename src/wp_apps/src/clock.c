#include "wp_apps/apps.h"
#include "wp_dev/state.h"
#include "wp_res/resources.h"

/* Updates the values on the SCREEN_CLOCK */
static void _clock_partial(bool* show_sec)
{
    if (*show_sec != state.show_sec) {
        *show_sec = state.show_sec;
        Paint_DrawRectangle(0, 50, 240, 180, COLOR_BG, DOT_PIXEL_1X1,
                            DRAW_FILL_FULL);
    }
    DateTime* dt = &state.dt;
    Paint_DrawString_EN(20, 70, dt_get_day(dt), &Font16, COLOR_BG, COLOR_FG);
    apps_paint_time(dt, state.show_sec ? 0 : 20, state.show_sec ? 100 : 90,
                    state.show_sec);
    char bottom_str[100];
    sprintf(bottom_str, "%d %s %d", dt->day, DATETIME_MONTH(dt->month),
            dt->year % 100);
    Paint_DrawString_EN(90, 160, bottom_str, &Font20, COLOR_BG, COLOR_FG);

    char str[10];
    snprintf(str, 6, "%02d\'C", state.dev.temp);
    Paint_DrawString_EN(170, 70, str, &Font16, COLOR_BG, COLOR_FG);
}

enum app_status_t apps_load_clock()
{
    int min = 0;
    int sec = 0;
    bool show_sec = false;
    while (true) {
        if (!apps_poll_popup()) { screen.redraw = DISP_REDRAW; }
        switch (XY.Gesture) {
        case Down: apps_load(SCREEN_MENU); break;
        case CLICK:
            state.show_sec = !state.show_sec;
            show_sec = !state.show_sec;
            screen.redraw = DISP_REDRAW;
            PRINT("show_seconds:%s", , state.show_sec ? "true" : "false");
            break;
        case DOUBLE_CLICK: apps_load(SCREEN_LOCK); break;
        }
        if (screen.sstate != SCREEN_CLOCK) {
            screen.sstate = SCREEN_CLOCK;
            screen.redraw = DISP_REDRAW;
        } else if (state.show_sec) {
            if (sec != state.dt.second && screen.redraw == DISP_SYNC) {
                screen.redraw = DISP_PARTIAL;
            }
            sec = state.dt.second;
        } else if (min != state.dt.minute && screen.redraw == DISP_SYNC) {
            screen.redraw = DISP_PARTIAL;
            min = state.dt.minute;
        }

        if (screen.redraw) {
            // If full redraw
            if (screen.redraw == DISP_REDRAW) {
                apps_reset();
                apps_draw(res_get_direction(GEST_DIR_U), 96, 20);
            }
            _clock_partial(&show_sec);
            apps_post_process(false);
            LCD_1IN28_Display(screen.buffer);
            screen.redraw = DISP_SYNC;
            XY.Gesture = None;
        }
    }
}
