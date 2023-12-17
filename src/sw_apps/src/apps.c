#include "sw_apps/apps.h"
#include "GUI_Paint.h"

#define CHRONO_CB_FREQUENCY 13

/**
 * Starts/cancels a Stopwatch _scr_chrono_cb event which periodically
 * updates the timer
 * @return - timer status
 */
static SCR_STATUS _apps_chrono_toggle();
/** Updates the values on the SCREEN_CLOCK */
static void _apps_clock_partial();
static void _step_display();

SCR_STATUS apps_load_clock()
{
    SET_MODULE(SCREEN_CLOCK, TOUCH_GESTURE);

    int min = 0;
    int sec = 0;
    bool old_show_sec = false;
    while (true) {
        switch (XY.Gesture) {
        case Down: apps_load_menu(); break;
        case CLICK:
            state.clock_show_sec = !state.clock_show_sec;
            old_show_sec = !state.clock_show_sec;
            screen.redraw = DISP_REDRAW;
            PRINT("show_seconds:%s", , state.clock_show_sec ? "true" : "false");
            break;
        case DOUBLE_CLICK: apps_lock_screen(); break;
        }
        if (screen.sstate != SCREEN_CLOCK) {
            screen.sstate = SCREEN_CLOCK;
            screen.redraw = DISP_REDRAW;
        } else if (state.clock_show_sec) {
            if (sec != state.dt.second) screen.redraw = DISP_PARTIAL;
            sec = state.dt.second;
        } else if (min != state.dt.minute) {
            screen.redraw = DISP_PARTIAL;
            min = state.dt.minute;
        }

        if (screen.redraw) {
            // If full redraw
            if (screen.redraw == DISP_REDRAW) {
                apps_draw(res_reset(), 0, 0);
                apps_draw(res_get_direction(GEST_DIR_U), 96, 20);
            }
            if (old_show_sec != state.clock_show_sec) {
                old_show_sec = state.clock_show_sec;
                Paint_DrawRectangle(0, 50, 240, 180, COLOR_BG, DOT_PIXEL_1X1,
                                    DRAW_FILL_FULL);
            }
            _apps_clock_partial();
            apps_post_process(false);
            LCD_1IN28_Display(screen.buffer);
            screen.redraw = DISP_SYNC;
            XY.Gesture = None;
        }
    }
}

SCR_STATUS apps_lock_screen()
{
    SET_MODULE(SCREEN_LOCK, TOUCH_GESTURE);
    screen.sstate = SCREEN_LOCK;
    Paint_Clear(BLACK);
    DEV_SET_PWM(0);
    LCD_1IN28_Display(screen.buffer);
    XY.Gesture = None;
    while (true) {
        if (XY.Gesture == DOUBLE_CLICK) {
            /* Returns with up gesture to trigger down
             * on menu and return to clock screen safely
             */
            XY.Gesture = UP;
            DEV_SET_PWM(100);
            return SCR_STATUS_OK;
        }
        sleep_ms(200);
    }
}

SCR_STATUS apps_load_chrono()
{
#define BTN_STOPWATCH 40, 160, 160, 40
    SET_MODULE(SCREEN_CHRONO, TOUCH_POINT);

    bool clicked;
    int x = 0, y = 0;
    while (true) {
        if (x != XY.x_point || y != XY.y_point) {
            x = XY.x_point;
            y = XY.y_point;
            clicked = true;
        }
        if (apps_is_exited()) return SCR_STATUS_OK;

        // Start/Stop Button
        if (clicked && apps_is_clicked(BTN_STOPWATCH)) {
            WARN(BTN_STOPWATCH);
            screen.redraw = DISP_REDRAW;
            SCR_STATUS status = _apps_chrono_toggle();
            if (!status) return status;
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

SCR_STATUS apps_load_media()
{
#define BTN_PLAY_PAUSE 88, 120, 64, 64
#define BTN_NEXT 152, 120, 48, 64
#define BTN_PREV 40, 120, 48, 64
    SET_MODULE(SCREEN_MEDIA, TOUCH_POINT);

    bool clicked;
    int x = 0, y = 0;
    while (true) {
        if (x != XY.x_point || y != XY.y_point) {
            x = XY.x_point;
            y = XY.y_point;
            clicked = true;
        }
        if (apps_is_exited()) return SCR_STATUS_OK;

        // Play/Pause Button
        if (clicked && apps_is_clicked(BTN_PLAY_PAUSE)) {
            WARN(BTN_PLAY_PAUSE);
            state.media.is_playing = !state.media.is_playing;
            screen.redraw = DISP_REDRAW;
        } else if (clicked && apps_is_clicked(BTN_NEXT)) {
            WARN(BTN_NEXT);
            screen.redraw = DISP_PARTIAL;
        } else if (clicked && apps_is_clicked(BTN_PREV)) {
            WARN(BTN_PREV);
            screen.redraw = DISP_PARTIAL;
            screen.redraw = DISP_PARTIAL;
        }

        if (apps_set_titlebar(SCREEN_MEDIA, POPUP_NONE)) {
            XY.x_point = 0;
            XY.y_point = 0;
            apps_post_process(false);
        }

        if (screen.redraw) {
            apps_draw(res_get_app_media_button(!state.media.is_playing), 40,
                      120);
            LCD_1IN28_Display(screen.buffer);
            screen.redraw = DISP_SYNC;
        }
        clicked = false;
    }
}

SCR_STATUS apps_load_step()
{
    SET_MODULE(SCREEN_STEP, TOUCH_POINT);

    int sec = 0;
    while (true) {
        if (apps_set_titlebar(SCREEN_STEP, POPUP_NONE)) {
            XY.x_point = 0;
            XY.y_point = 0;
            apps_post_process(false);
            apps_draw(res_get_app_step(), 40, 150);
        }
        if (sec != state.dt.second) {
            screen.redraw = DISP_PARTIAL;
            sec = state.dt.second;
        }
        if (apps_is_exited()) return SCR_STATUS_OK;

        if (screen.redraw) {
            _step_display();
            LCD_1IN28_Display(screen.buffer);
            screen.redraw = DISP_SYNC;
        }
    }
}

static void _step_display()
{
    int base_x = 50;
    int base_y = 86;
    char numstr[6] = {0};
    const int x_size = 40;
    const int y_size = 40;
    snprintf(numstr, 6, "%d", state.step);
    strcenter(numstr, strnlen(numstr, 6), 6);
    int x = base_x;
    int y = base_y;
    for (int i = 0; i < 5; i++) {
        const unsigned char* img_ptr = NULL;
        if (numstr[i] >= '0' && numstr[i] <= '9') {
            img_ptr = font40 + 2 * (numstr[i] - '0') * x_size * y_size;
            Paint_DrawImage(img_ptr, x, y, x_size, y_size);
        }
        x += 30;
    }
}

/**
 * The callback function for the stopwatch event. 
 * Periodically updates the timer, calls for redraw if the primary screen is
 * SCREEN_CHRONO.
 * Inserts time results as follows:
 * - millisecond -> second
 * - second -> minute
 * - minute -> hour
 */
static bool _scr_chrono_cb(repeating_timer_t* r)
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
                WARN(END_EVENT_scr_chrono_cb);
                screen.redraw = DISP_REDRAW;
            }
        }
    }
    if (screen.sstate == SCREEN_CHRONO && screen.pstate == POPUP_NONE)
        screen.redraw = DISP_PARTIAL;
}

static SCR_STATUS _apps_chrono_toggle()
{
    state.chrono.enabled = !state.chrono.enabled;
    if (state.chrono.enabled) {
        state.chrono.dt.hour = 0;
        state.chrono.dt.minute = 0;
        state.chrono.dt.second = 0;
        memset(&state.chrono.timer, 0, sizeof(repeating_timer_t));
        if (!add_repeating_timer_ms(CHRONO_CB_FREQUENCY * -10, _scr_chrono_cb,
                                    NULL, &state.chrono.timer))
            return ERROR(SCR_ERROR_TIMER_CREATE);
        WARN(REGISTER_EVENT_scr_chrono_cb);
    } else {
        WARN(CANCEL_EVENT_scr_chrono_cb);
        cancel_repeating_timer(&state.chrono.timer);
    }
}

static void _apps_clock_partial()
{
    DateTime* dt = &state.dt;
    Paint_DrawString_EN(20, 70, day_of_the_week(dt), &Font16, COLOR_BG,
                        COLOR_FG);
    apps_paint_time(dt, state.clock_show_sec ? 0 : 20,
                    state.clock_show_sec ? 100 : 90, state.clock_show_sec);
    char bottom_str[100];
    sprintf(bottom_str, "%d %s %d", dt->day, DATETIME_MONTH(dt->month),
            dt->year % 100);
    Paint_DrawString_EN(90, 160, bottom_str, &Font20, COLOR_BG, COLOR_FG);
}
