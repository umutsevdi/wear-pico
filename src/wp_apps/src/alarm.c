#include "wp_apps/apps.h"
#include "wp_bt/bt.h"
#include "wp_dev/state.h"
#include "wp_res/resources.h"
#define ALARM_X 40
#define ALARM_Y 66
#define ALARM_BUTTON_SIZE 164, 36

/* Draws the idx'th alarm to the screen. */
static void _alarm_draw(Alarm* alarm, int y_pos)
{
    apps_draw(res_get_app_alarm_button(alarm->is_active), ALARM_X, y_pos);
    char str[6];
    snprintf(str, 6, "%02d:%02d", alarm->at.hour, alarm->at.minute);
    int fg = alarm->is_active ? COLOR_FG : GRAY;
    Paint_DrawString_EN(ALARM_X + 12, y_pos + 10, str, &Font16, COLOR_BG, fg);
}

/* Check whether given toggle is updated or not. */
static void _check_toggle(Alarm* alarm, int y_pos)
{
    if (apps_is_clicked(ALARM_X, y_pos, ALARM_BUTTON_SIZE)) {
        alarm->is_active = !alarm->is_active;
        _alarm_draw(alarm, y_pos);
        screen.redraw = DISP_PARTIAL;
    }
}

enum app_status_t apps_load_alarm(void)
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

        if (clicked) {
            for (int i = 0; i < state.alarms.len; i++) {
                _check_toggle(&state.alarms.list[i], ALARM_Y + i * 36);
            }
        }
        if (!state.alarms.is_fetched) {
            PRINT(BT_FETCH_ALARMS);
            state.alarms.is_fetched = true;
            screen.redraw = DISP_REDRAW;
        }
        if (apps_set_titlebar(SCREEN_ALARM, POPUP_NONE)) {
            XY.x_point = 0;
            XY.y_point = 0;
            for (int i = 0; i < state.alarms.len; i++) {
                _alarm_draw(&state.alarms.list[i], ALARM_Y + i * 36);
            }
            apps_post_process(false);
        }

        if (screen.redraw) {
            LCD_1IN28_Display(screen.buffer);
            screen.redraw = DISP_SYNC;
        }
        clicked = false;
    }
}
