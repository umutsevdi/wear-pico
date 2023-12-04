#include "global.h"
#include "pico/time.h"
#include "ui/common.h"

static SCR_STATUS _scr_chrono_toggle(bool toggle);

SCR_STATUS scr_load_clock()
{
    SET_MODULE(SCREEN_CLOCK, TOUCH_GESTURE);

    while (true) {
        switch (XY.Gesture) {
        case Down: scr_load_menu(); break;
        case CLICK:
            state.clock_show_sec = !state.clock_show_sec;
            screen.redraw = true;
            PRINT("show_seconds:%s", , state.clock_show_sec ? "true" : "false");
            break;
        case DOUBLE_CLICK: /* TODO make screen black */ break;
        }
        if (screen.sstate != SCREEN_CLOCK || screen.redraw) {
            screen.sstate = SCREEN_CLOCK;
            screen.redraw = false;
            Paint_DrawImage(watch, 0, 0, 240, 240);
            DateTime* dt = &state.dt;
            Paint_DrawString_EN(20, 70, DATETIME_DAY(dt->day % 7 + 1), &Font16,
                                COLOR_BG, COLOR_FG);
            scr_paint_time(dt, state.clock_show_sec ? 0 : 20,
                           state.clock_show_sec ? 100 : 90,
                           state.clock_show_sec);
            char bottom_str[100];
            sprintf(bottom_str, "%d %s %d", dt->day, DATETIME_MONTH(dt->month),
                    dt->year % 100);
            Paint_DrawString_EN(90, 160, bottom_str, &Font20, COLOR_BG,
                                COLOR_FG);
            LCD_1IN28_Display(screen.buffer);
            XY.Gesture = None;
        }
    }
}

SCR_STATUS scr_load_chono()
{
    SET_MODULE(SCREEN_CHRONO, TOUCH_POINT);
    screen.redraw = true;
    int x, y;
    while (true) {
        if (SCR_IS_CANCELLED) return SCR_STATUS_OK;

        bool is_clicked = false;
        // Start/Stop Button
        if (scr_is_clicked(40, 160, 160, 40) && XY.x_point != x
            && XY.y_point != y) {
            x = XY.x_point;
            y = XY.y_point;
            screen.redraw = true;
            SCR_STATUS status = _scr_chrono_toggle(!state.chrono.enabled);
            if (!status) return status;
            is_clicked = true;
        }

        if (screen.sstate != SCREEN_CHRONO || screen.redraw) {
            screen.redraw = false;
            /* Prevent full redraw to improve performance */
            if (screen.sstate != SCREEN_CHRONO || is_clicked) {
                screen.sstate = SCREEN_CHRONO;
                Paint_DrawImage(state.chrono.enabled ? app_stopwatch_stop
                                                     : app_stopwatch_start,
                                0, 0, 240, 240);
                XY.x_point = 0;
                XY.y_point = 0;
                is_clicked = false;
            }
            scr_paint_time(&state.chrono.dt, 0, 100, true);
            LCD_1IN28_Display(screen.buffer);
        }
    }
}

static bool _scr_chrono_cb(repeating_timer_t* r)
{
    state.chrono.dt.second += 13;
    if (state.chrono.dt.second > 99) {
        state.chrono.dt.second = 0;
        if (++state.chrono.dt.minute > 59) {
            state.chrono.dt.minute = 0;
            if (++state.chrono.dt.hour > 59) {
                cancel_repeating_timer(r);
                state.chrono.enabled = false;
                WARN(END_EVENT_scr_chrono_cb);
            }
        }
    }
    if (screen.sstate == SCREEN_CHRONO && screen.pstate == POPUP_NONE)
        screen.redraw = true;
}

static SCR_STATUS _scr_chrono_toggle(bool toggle)
{
    state.chrono.enabled = toggle;
    if (toggle) {
        state.chrono.dt.hour = 0;
        state.chrono.dt.minute = 0;
        state.chrono.dt.second = 0;
        memset(&state.chrono.timer, 0, sizeof(repeating_timer_t));
        if (!add_repeating_timer_ms(130, _scr_chrono_cb, NULL,
                                    &state.chrono.timer))
            return ERROR(SCR_ERROR_TIMER_CREATE);
        WARN(REGISTER_EVENT_scr_chrono_cb);
    } else {
        WARN(CANCEL_EVENT_scr_chrono_cb);
        cancel_repeating_timer(&state.chrono.timer);
    }
}
