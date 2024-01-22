#include "GUI_Paint.h"
#include "sw_apps/apps.h"
#include "sw_bt/bt.h"
#include "sw_os/dev.h"
#include "sw_os/state.h"
#include "sw_res/resources.h"

static const int POPUP_CALL_ITER = 3;
static const int POPUP_ALARM_ITER = 2;
#define NOTIFY_COLOR 0x9ce5
#define NOTIFY_TEXT_COLOR 0x8c7

/** Returns the priority of the pop-up type as an integer:
 * - CALL > ALARM > NOTIFY */
static int _popup_value(enum popup_t p);

/* Compares two pop-up types based on their @_popup_value */
static int _popup_cmp(enum popup_t p1, enum popup_t p2);

/* Loads the given pop-up */
static enum app_status_t _popup_load(enum popup_t popup);
static enum app_status_t _load_call();
static enum app_status_t _load_alarm();
static enum app_status_t _load_reminder();
static enum app_status_t _load_notify();

enum app_status_t apps_poll_popup()
{
    /* Reject the pop-up request if when there is none or the requested pop-ups
     * priority is lower */
    if (_popup_cmp(state.popup.type, state.__popup_req.type) >= 0) {
        state.__popup_req.type = POPUP_NONE;
        return APP_NO_POPUP;
    }
    WARN(LOADING_POPUP);
    /* store the status to revert */
    int mode_old = XY.mode;
    Popup popup_old = state.popup;
    state.popup = state.__popup_req;
    state.__popup_req.type = POPUP_NONE;

    enum app_status_t r = _popup_load(state.popup.type);
    /* Revert the module configurations */
    apps_set_module(SCREEN_T_SIZE, popup_old.type, mode_old);
    state.popup = popup_old;
    return r;
}

static int _popup_value(enum popup_t p)
{
    switch (p) {
    case POPUP_NOTIFY: return 1;
    case POPUP_REMINDER: return 2;
    case POPUP_ALARM: return 3;
    case POPUP_CALL: return 4;
    default: return 0;
    }
}

static int _popup_cmp(enum popup_t p1, enum popup_t p2)
{
    if (p1 == p2) { return 0; }
    return _popup_value(p1) > _popup_value(p2) ? 1 : -1;
}

static enum app_status_t _popup_load(enum popup_t popup)
{
    enum app_status_t r;

    if (!_popup_value(popup)) { return ERROR(APP_ERROR_INVALID_POPUP); }
    apps_set_module(SCREEN_T_SIZE, popup, TOUCH_POINT);
    switch (popup) {
    case POPUP_CALL: r = _load_call(); break;
    case POPUP_ALARM: r = _load_alarm(); break;
    case POPUP_REMINDER: r = _load_reminder(); break;
    case POPUP_NOTIFY: r = _load_notify(); break;
    default: break;
    }
    return r;
}

static enum app_status_t _load_call()
{
#define BTN_ACCEPT 52, 140, 48, 48
#define BTN_DISMISS 140, 140, 48, 48
    bool clicked;
    int x = 0, y = 0;
    PRINT("_load_call %s", , state.popup.value.caller.name);
    while (true) {
        if (x != XY.x_point || y != XY.y_point) {
            x = XY.x_point;
            y = XY.y_point;
            clicked = true;
        }
        if (state.popup.value.caller.is_over) { return APP_OK; }
        if (apps_is_exited()) { return APP_OK; }

        if (clicked) {
            if (apps_is_clicked(BTN_ACCEPT)) {
                if (bt_send_resp(BT_RESP_CALL_OK)) return INFO(BT_RESP_CALL_OK);
            } else if (apps_is_clicked(BTN_DISMISS)) {
                if (bt_send_resp(BT_RESP_CALL_CANCEL))
                    return INFO(BT_RESP_CALL_CANCEL);
            }
        }

        if (apps_set_titlebar(0, POPUP_CALL)) {
            XY.x_point = 0;
            XY.y_point = 0;
            apps_draw(res_get_popup_call(), 52, 140);
            Paint_DrawString_EN(
                0, 90,
                strcenter(state.popup.value.caller.name,
                          strnlen(state.popup.value.caller.name, 25), 16),
                &Font24, COLOR_BG, COLOR_FG);
            apps_post_process(false);
        }

        if (screen.redraw) {
            LCD_1IN28_Display(screen.buffer);
            screen.redraw = DISP_SYNC;
        }
        clicked = false;
        os_dev_notify_d(POPUP_CALL_ITER, state.config.call, 250, 60);
        sleep_ms(125);
    }
}

static enum app_status_t _load_alarm()
{
#define BTN_ALARM_DISMISS 40, 156, 160, 35
    bool clicked;
    int x = 0, y = 0;
    PRINT("_load_alarm %02d:%02d", , state.popup.value.alarm->at.hour,
          state.popup.value.alarm->at.minute);
    while (true) {
        if (x != XY.x_point || y != XY.y_point) {
            x = XY.x_point;
            y = XY.y_point;
            clicked = true;
        }

        if (!apps_poll_popup()) { screen.redraw = DISP_REDRAW; }
        if (apps_is_exited()) { return APP_OK; }
        if (clicked && apps_is_clicked(BTN_ALARM_DISMISS)) { return APP_OK; }
        if (apps_set_titlebar(0, POPUP_ALARM)) {
            XY.x_point = 0;
            XY.y_point = 0;
            apps_draw(res_get_popup_alarm(), 40, 156);
            apps_paint_time(&state.popup.value.alarm->at, 20, 70, false);
            apps_post_process(false);
        }
        if (screen.redraw) {
            LCD_1IN28_Display(screen.buffer);
            screen.redraw = DISP_SYNC;
        }
        clicked = false;
        os_dev_notify(POPUP_ALARM_ITER, state.config.alarm);
        sleep_ms(125);
    }
}

static enum app_status_t _load_reminder()
{
    bool clicked;
    int x = 0, y = 0;
    char reminder_title[31] = {0};
    PRINT("_load_reminder %s ", , state.popup.value.event);
    while (true) {
        if (x != XY.x_point || y != XY.y_point) {
            x = XY.x_point;
            y = XY.y_point;
            clicked = true;
        }

        if (!apps_poll_popup()) { screen.redraw = DISP_REDRAW; }
        if (apps_is_exited()) { return APP_OK; }
        if (clicked && apps_is_clicked(BTN_ALARM_DISMISS)) { return APP_OK; }
        if (apps_set_titlebar(0, POPUP_REMINDER)) {
            XY.x_point = 0;
            XY.y_point = 0;
            apps_draw(res_get_popup_alarm(), 40, 156);
            strncpy(reminder_title, state.popup.value.event, 31);
            Paint_DrawString_EN(
                0, 90,
                strcenter(reminder_title, strnlen(reminder_title, 30), 16),
                &Font24, COLOR_BG, COLOR_FG);
            apps_post_process(false);
        }
        if (screen.redraw) {
            LCD_1IN28_Display(screen.buffer);
            screen.redraw = DISP_SYNC;
        }
        clicked = false;
        os_dev_notify(POPUP_ALARM_ITER, state.config.reminder);
        sleep_ms(125);
    }
}

static enum app_status_t _load_notify()
{
    int x = 0, y = 0;
    os_dev_notify(POPUP_ALARM_ITER, state.config.notify);
    absolute_time_t then = get_absolute_time();
    PRINT("_load_notify %s, %s", , state.popup.value.notify.title,
          state.popup.value.notify.text);
    while (true) {
        if (x != XY.x_point || y != XY.y_point) {
            x = XY.x_point;
            y = XY.y_point;
        }
        if (!apps_poll_popup()) { screen.redraw = DISP_REDRAW; }
        if (apps_is_exited()) { return APP_OK; }
        if ((absolute_time_diff_us(get_absolute_time(), then) / 12000000)) {
            return INFO(APP_TIMEOUT_NOTIFY);
        }

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
                                  17, array, 7);
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
    }
}
