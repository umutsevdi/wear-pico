#include "wp_apps/apps.h"
#include "wp_dev/dev.h"
#include "wp_dev/state.h"
#include "wp_res/resources.h"

#define CHRONO_CB_FREQUENCY 13
repeating_timer_t chrono_timer;

/**
 * The callback function for the stopwatch event. 
 * Periodically updates the timer, calls for redraw if the primary screen is
 * SCREEN_CHRONO.
 * Inserts time results as follows:
 * - millisecond -> second
 * - second -> minute
 * - minute -> hour
 */
static bool _chrono_cb(repeating_timer_t* r)
{
    state.chrono.dt.second += CHRONO_CB_FREQUENCY;
    if (state.chrono.dt.second > 99) {
        state.chrono.dt.second -= 99;
        if (++state.chrono.dt.minute > 59) {
            state.chrono.dt.minute = 0;
            if (++state.chrono.dt.hour > 59) {
                state.chrono.dt.second = 0;
                state.chrono.enabled = false;
                cancel_repeating_timer(r);
                INFO(END_EVENT_scr_chrono_cb);
                screen.redraw = DISP_REDRAW;
            }
        }
    }
    if (screen.sstate == SCREEN_CHRONO && state.popup.type == POPUP_NONE) {
        screen.redraw = DISP_PARTIAL;
    }
}

/**
 * Starts/cancels a Stopwatch _scr_chrono_cb event which periodically
 * updates the timer
 * @return - timer status
 */
static enum app_status_t _chrono_toggle()
{
    state.chrono.enabled = !state.chrono.enabled;
    os_dev_notify_d(1, DEV_BUZZER | DEV_LED, 150, 0);
    if (state.chrono.enabled) {
        state.chrono.dt.hour = 0;
        state.chrono.dt.minute = 0;
        state.chrono.dt.second = 0;
        memset(&chrono_timer, 0, sizeof(repeating_timer_t));
        if (!add_repeating_timer_ms(CHRONO_CB_FREQUENCY * -10, _chrono_cb, NULL,
                                    &chrono_timer)) {
            return ERROR(APP_ERROR_TIMER_CREATE);
        }
        INFO(REGISTER_EVENT scr_chrono_cb);
    } else {
        INFO(CANCEL_EVENT scr_chrono_cb);
        cancel_repeating_timer(&chrono_timer);
    }
}

enum app_status_t apps_load_chrono()
{
#define BTN_STOPWATCH 40, 160, 160, 40
    bool clicked;
    int x = 0, y = 0;
    while (true) {
        if (x != XY.x_point || y != XY.y_point) {
            x = XY.x_point;
            y = XY.y_point;
            clicked = true;
        }
        if (!apps_poll_popup()) { screen.redraw = DISP_REDRAW; }
        if (apps_is_exited()) { return APP_OK; }

        if (clicked && apps_is_clicked(BTN_STOPWATCH)) {
            INFO(BTN_STOPWATCH);
            screen.redraw = DISP_REDRAW;
            _chrono_toggle();
        }
        if (apps_set_titlebar(SCREEN_CHRONO, POPUP_NONE)) {
            XY.x_point = 0;
            XY.y_point = 0;
            apps_post_process(false);
        }
        if (screen.redraw) {
            /* Prevent full redraw to improve performance */
            apps_draw(res_get_app_chrono_button(state.chrono.enabled), 40, 160);
            apps_paint_time(&state.chrono.dt, 0, 100, true);
            LCD_1IN28_Display(screen.buffer);
            screen.redraw = DISP_SYNC;
        }
        clicked = false;
    }
}
