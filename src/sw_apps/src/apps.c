#include "sw_apps/apps.h"

static void _step_display();

enum app_status_t apps_lock_screen()
{
    SET_MODULE(SCREEN_LOCK, TOUCH_GESTURE);
    screen.sstate = SCREEN_LOCK;
    Paint_Clear(BLACK);
    DEV_SET_PWM(0);
    LCD_1IN28_Display(screen.buffer);
    XY.Gesture = None;
    while (true) {
        if (state.popup.type != POPUP_NONE) {
            XY.Gesture = UP;
            DEV_SET_PWM(100);
            if (!apps_poll_popup(POPUP_NONE)) return APP_OK;
        }
        if (XY.Gesture == DOUBLE_CLICK) {
            /* Returns with up gesture to trigger down
             * on menu and return to clock screen safely
             */
            XY.Gesture = UP;
            DEV_SET_PWM(100);
            return APP_OK;
        }
        sleep_ms(200);
    }
}

enum app_status_t apps_load_media()
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
        if (!apps_poll_popup(POPUP_NONE)) screen.redraw = DISP_REDRAW;
        if (apps_is_exited()) return APP_OK;
        if (clicked) {
            if (apps_is_clicked(BTN_PLAY_PAUSE)) {
                WARN(BTN_PLAY_PAUSE);
                state.media.is_playing = !state.media.is_playing;
                screen.redraw = DISP_REDRAW;
            } else if (apps_is_clicked(BTN_NEXT)) {
                WARN(BTN_NEXT);
                screen.redraw = DISP_PARTIAL;
            } else if (apps_is_clicked(BTN_PREV)) {
                WARN(BTN_PREV);
                screen.redraw = DISP_PARTIAL;
                screen.redraw = DISP_PARTIAL;
            }
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

enum app_status_t apps_load_step()
{
    SET_MODULE(SCREEN_STEP, TOUCH_POINT);

    int sec = 0;
    while (true) {
        if (!apps_poll_popup(POPUP_NONE)) screen.redraw = DISP_REDRAW;
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
        if (apps_is_exited()) return APP_OK;

        if (screen.redraw) {
            _step_display();
            LCD_1IN28_Display(screen.buffer);
            screen.redraw = DISP_SYNC;
        }
    }
}

static void _step_display()
{
    int base_x = 40;
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
