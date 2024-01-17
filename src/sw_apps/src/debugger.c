#include "sw_apps/apps.h"
#include "sw_bt/bt.h"
#include "sw_os/state.h"

enum app_status_t apps_load_log(void)
{
#define NOTIFY_COLOR 0x9ce5
#define NOTIFY_TEXT_COLOR 0x8c7
    static char __log_copy[LOG_BUFFER_S] = {0};
    int sec = 0;
    while (true) {
        if (!apps_poll_popup()) screen.redraw = DISP_REDRAW;
        if (apps_set_titlebar(SCREEN_LOG, POPUP_NONE)) {
            XY.x_point = 0;
            XY.y_point = 0;
            apps_post_process(false);
            apps_draw(res_get_popup_notify(), 40, 65);
        }
        if (sec != state.dt.second) {
            screen.redraw = DISP_PARTIAL;
            sec = state.dt.second;
        }
        if (apps_is_exited()) return APP_OK;

        if (screen.redraw) {
            char* array[15] = {0};
            memcpy(__log_copy, LOG_BUFFER, LOG_BUFFER_S);
            int array_s = strwrap(__log_copy, strnlen(__log_copy, LOG_BUFFER_S),
                                  40, array, 14);
            if (array_s != -1) {
                for (int i = 0; i < array_s; i++)
                    Paint_DrawString_EN(55, 70 + 9 * i, array[i], &Font8,
                                        NOTIFY_TEXT_COLOR, COLOR_FG);
            }

            LCD_1IN28_Display(screen.buffer);
            screen.redraw = DISP_SYNC;
        }
    }
}
