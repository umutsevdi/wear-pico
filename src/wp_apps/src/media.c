#include "wp_apps/apps.h"
#include "wp_bt/bt.h"
#include "wp_dev/state.h"

#define BTN_PLAY_PAUSE 88, 120, 64, 64
#define BTN_NEXT 152, 120, 48, 64
#define BTN_PREV 40, 120, 48, 64

static void _check_buttons()
{
    if (apps_is_clicked(BTN_PLAY_PAUSE)) {
        state.media.is_playing = !state.media.is_playing;
        bt_send_resp(BT_RESP_OSC_PLAY_PAUSE);
    } else if (apps_is_clicked(BTN_NEXT)) {
        bt_send_resp(BT_RESP_OSC_NEXT);
    } else if (apps_is_clicked(BTN_PREV)) {
        bt_send_resp(BT_RESP_OSC_PREV);
    } else {
        return;
    }
    screen.redraw = DISP_REDRAW;
}

/* Draws the currently playing song and its artist to the screen */
static void _draw_media()
{
    apps_draw(res_get_app_media_button(!state.media.is_playing), 40, 120);
    char center[30] = {0};
    int len = strnlen(state.media.song, 30);
    strncpy(center, state.media.song, len);
    Paint_DrawString_EN(5, 80, strcenter(center, len, 16), &Font20, COLOR_BG,
                        COLOR_FG);
    len = strnlen(state.media.artist, 30);
    memset(center, 0, 30);
    strncpy(center, state.media.artist, len);
    Paint_DrawString_EN(50, 185, strcenter(center, len, 20), &Font12, COLOR_BG,
                        COLOR_FG);
}

enum app_status_t apps_load_media()
{
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
        if (clicked) { _check_buttons(); }
        if (!state.media.is_fetched) {
            PRINT(BT_OSC_FETCH);
            state.media.is_fetched = true;
            screen.redraw = DISP_REDRAW;
        }
        if (apps_set_titlebar(SCREEN_MEDIA, POPUP_NONE)) {
            XY.x_point = 0;
            XY.y_point = 0;
        }
        if (screen.redraw) {
            _draw_media();
            apps_post_process(false);
            LCD_1IN28_Display(screen.buffer);
            screen.redraw = DISP_SYNC;
        }
        clicked = false;
        sleep_ms(50);
    }
}
