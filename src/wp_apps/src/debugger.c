#include "wp_apps/apps.h"
#include "wp_dev/state.h"

#define NOTIFY_COLOR 0x9ce5
#define NOTIFY_TEXT_COLOR 0x8c7

static void _fill_logs()
{
    static char __log_copy[LOG_BUFFER_S] = {0};
    char* array[15] = {0};
    memcpy(__log_copy, LOG_BUFFER, LOG_BUFFER_S);
    int array_s =
        strwrap(__log_copy, strnlen(__log_copy, LOG_BUFFER_S), 40, array, 14);
    if (array_s != -1) {
        for (int i = 0; i < array_s; i++)
            Paint_DrawString_EN(55, 70 + 9 * i, array[i], &Font8,
                                NOTIFY_TEXT_COLOR, COLOR_FG);
    }
}

enum app_status_t apps_load_log(void)
{
    int sec = 0;
    while (true) {
        if (!apps_poll_popup()) screen.redraw = DISP_REDRAW;
        if (sec != state.dt.second) {
            screen.redraw = DISP_PARTIAL;
            sec = state.dt.second;
        }
        if (apps_set_titlebar(SCREEN_LOG, POPUP_NONE)) {
            XY.x_point = 0;
            XY.y_point = 0;
            apps_post_process(false);
            apps_draw(res_get_popup_notify(), 40, 65);
        }
        if (apps_is_exited()) return APP_OK;
        if (screen.redraw) {
            _fill_logs();
            LCD_1IN28_Display(screen.buffer);
            screen.redraw = DISP_SYNC;
        }
    }
}
