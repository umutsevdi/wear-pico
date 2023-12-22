#include "GUI_Paint.h"
#include "sw_apps/apps.h"
#include "sw_res/resources.h"
#define ALARM_X 40
#define ALARM_Y 66
#define ALARM_BUTTON_SIZE 164, 36

void _alarm_draw(int idx, Alarm* alarm);

enum app_status_t apps_load_alarm(void)
{
    SET_MODULE(SCREEN_ALARM, TOUCH_POINT);

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
            for (int i = 0; i < state.alarms.len; i++) {
                if (apps_is_clicked(ALARM_X, ALARM_Y + i * 36,
                                    ALARM_BUTTON_SIZE)) {
                    PRINT("ALARM_%d", , i);
                    state.alarms.list[i].is_active =
                        !state.alarms.list[i].is_active;
                    _alarm_draw(i, &state.alarms.list[i]);
                    screen.redraw = DISP_PARTIAL;
                }
            }
        }

        if (apps_set_titlebar(SCREEN_ALARM, POPUP_NONE)) {
            XY.x_point = 0;
            XY.y_point = 0;
            for (int i = 0; i < state.alarms.len; i++)
                _alarm_draw(i, &state.alarms.list[i]);
            apps_post_process(false);
        }

        if (screen.redraw) {
            LCD_1IN28_Display(screen.buffer);
            screen.redraw = DISP_SYNC;
        }
        clicked = false;
    }
}

void _alarm_draw(int idx, Alarm* alarm)
{
    int y = ALARM_Y + idx * 36;
    apps_draw(res_get_app_alarm_button(alarm->is_active), ALARM_X, y);
    char str[6];
    snprintf(str, 6, "%02d:%02d", alarm->at.hour, alarm->at.minute);
    int fg = alarm->is_active ? COLOR_FG : GRAY;
    Paint_DrawString_EN(ALARM_X + 12, y + 10, str, &Font16, COLOR_BG, fg);
}
