#include "GUI_Paint.h"
#include "sw_apps/apps.h"
#include "sw_bt/bt.h"
#include "sw_os/dev.h"

static const int POPUP_CALL_ITER = 3;
static const int POPUP_ALARM_ITER = 2;
static const int POPUP_CALL_FLAG = DEV_BUZZER | DEV_LED | DEV_VIB;
static const int POPUP_NOTIFY_FLAG = DEV_BUZZER | DEV_VIB;
static const int POPUP_ALARM_FLAG = DEV_BUZZER | DEV_LED;
#define NOTIFY_COLOR 0x9ce5
#define NOTIFY_TEXT_COLOR 0x8c7

static enum app_status_t _load_call();
static enum app_status_t _load_alarm();
static enum app_status_t _load_notification();
int64_t _call_notify(int32_t id, void* data);
int64_t _call_ring(int32_t id, void* data);

enum app_status_t apps_poll_popup()
{
    /* Reject the pop-up request if when there is none or the requested pop-ups
     * priority is lower */
    if (state.__popup_req.type == POPUP_NONE
        || state.popup.type >= state.__popup_req.type) {
        state.__popup_req.type = POPUP_NONE;
        return APP_NO_POPUP;
    }
    /* store the status to revert */
    int mode_old = XY.mode;
    Popup popup_old = state.popup;
    state.popup = state.__popup_req;
    state.__popup_req.type = POPUP_NONE;

    enum app_status_t r;
    switch (state.popup.type) {
    case POPUP_CALL: r = _load_call(); break;
    case POPUP_ALARM: r = _load_alarm(); break;
    case POPUP_NOTIFY: r = _load_notification(); break;
    default: return ERROR(APP_ERROR_INVALID_POPUP);
    }
    /* Revert the module configurations */
    XY.mode = mode_old;
    XY.Gesture = None;
    XY.x_point = 0;
    XY.y_point = 0;
    if (Touch_1IN28_init(XY.mode) != 1) WARN(SCR_WARN_TOUCH_FAILED);
    state.popup = popup_old;
    return r;
}

static enum app_status_t _load_call()
{
#define BTN_ACCEPT 52, 140, 48, 48
#define BTN_DISMISS 140, 140, 48, 48
    SET_MODULE(POPUP_CALL, TOUCH_POINT);
    bool clicked;
    int x = 0, y = 0;
    while (true) {
        if (x != XY.x_point || y != XY.y_point) {
            x = XY.x_point;
            y = XY.y_point;
            clicked = true;
        }
        if (state.popup.value.caller.is_over) return APP_OK;
        if (apps_is_exited()) return APP_OK;

        if (clicked) {
            if (apps_is_clicked(BTN_ACCEPT)) {
                if (bt_send_resp(BT_RESP_CALL_OK)) return APP_OK;
            } else if (apps_is_clicked(BTN_DISMISS)) {
                if (bt_send_resp(BT_RESP_CALL_CANCEL)) return APP_OK;
            }
        }

        if (apps_set_titlebar(0, POPUP_CALL)) {
            XY.x_point = 0;
            XY.y_point = 0;
            apps_draw(res_get_popup_call(), 52, 140);
            PRINT("%s", , state.popup.value.caller.name);
            Paint_DrawString_EN(
                0, 90,
                strcenter(state.popup.value.caller.name,
                          strnlen(state.popup.value.caller.name, 15), 15),
                &Font24, COLOR_BG, COLOR_FG);
            apps_post_process(false);
        }

        if (screen.redraw) {
            LCD_1IN28_Display(screen.buffer);
            screen.redraw = DISP_SYNC;
        }
        clicked = false;
        os_dev_notify_d(POPUP_CALL_ITER, DEV_BUZZER | DEV_LED | DEV_VIB, 250,
                        60);
        sleep_ms(125);
    }
}

static enum app_status_t _load_alarm()
{
#define BTN_ALARM_DISMISS 40, 156, 160, 35
    SET_MODULE(POPUP_ALARM, TOUCH_POINT);

    bool clicked;
    int x = 0, y = 0;
    while (true) {
        if (x != XY.x_point || y != XY.y_point) {
            x = XY.x_point;
            y = XY.y_point;
            clicked = true;
        }
        if (!apps_poll_popup()) screen.redraw = DISP_REDRAW;

        if (apps_is_exited()) return APP_OK;

        if (clicked && apps_is_clicked(BTN_ALARM_DISMISS)) { return APP_OK; }
        if (apps_set_titlebar(0, POPUP_ALARM)) {
            XY.x_point = 0;
            XY.y_point = 0;
            apps_draw(res_get_popup_alarm(), 40, 156);
            apps_paint_time(&state.popup.value.alarm, 20, 70, false);
            apps_post_process(false);
        }
        if (screen.redraw) {
            LCD_1IN28_Display(screen.buffer);
            screen.redraw = DISP_SYNC;
        }
        clicked = false;
        os_dev_notify(POPUP_ALARM_ITER, POPUP_ALARM_FLAG);
        sleep_ms(125);
    }
}

static enum app_status_t _load_notification()
{
    SET_MODULE(POPUP_NOTIFY, TOUCH_POINT);

    bool clicked;
    int x = 0, y = 0;
    os_dev_notify(POPUP_ALARM_ITER, POPUP_NOTIFY_FLAG);
    while (true) {
        if (x != XY.x_point || y != XY.y_point) {
            x = XY.x_point;
            y = XY.y_point;
            clicked = true;
        }
        if (!apps_poll_popup()) screen.redraw = DISP_REDRAW;

        if (apps_is_exited()) return APP_OK;

        if (apps_set_titlebar(0, POPUP_NOTIFY)) {
            XY.x_point = 0;
            XY.y_point = 0;
            apps_draw(res_get_popup_notify(), 40, 65);
            Paint_DrawString_EN(
                76, 40,
                strcenter(state.popup.value.notify.title,
                          strnlen(state.popup.value.notify.title, 12), 12),
                &Font12, NOTIFY_COLOR, COLOR_FG);

            char* array[8] = {0};
            int array_s = strwrap(state.popup.value.notify.text,
                                  strnlen(state.popup.value.notify.text, 128),
                                  20, array, 7);
            if (array_s != -1) {
                for (int i = 0; i < array_s; i++)
                    Paint_DrawString_EN(50, 70 + 12 * i, array[i], &Font12,
                                        NOTIFY_TEXT_COLOR, COLOR_FG);
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
