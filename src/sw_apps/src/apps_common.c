#include "sw_apps/apps.h"
#include "GUI_Paint.h"

void apps_paint_time(DateTime* dt, int base_x, int base_y, bool show_sec)
{
    const int x_size = 40;
    const int y_size = show_sec ? 40 : 60;
    int datestr_s = show_sec ? 9 : 6;
    char datestr[9] = {0};
    const unsigned char* font = show_sec ? font40 : font60;
    if (show_sec) {
        snprintf(datestr, datestr_s, "%02d:%02d:%02d", dt->hour, dt->minute,
                 dt->second);
    } else {
        snprintf(datestr, datestr_s, "%02d:%02d", dt->hour, dt->minute);
    }

    int x = base_x;
    int y = base_y;
    for (int i = 0; i < datestr_s - 1; i++) {
        const unsigned char* img_ptr = NULL;
        if (datestr[i] >= '0' && datestr[i] <= '9')
            img_ptr = font + 2 * (datestr[i] - '0') * x_size * y_size;
        else if (datestr[i] == ':')
            img_ptr = font + 2 * 10 * x_size * y_size;

        Paint_DrawImage(img_ptr, x, y, x_size, y_size);
        x += show_sec ? 30 : 40;
    }
}

bool apps_is_clicked(int x_start, int y_start, int width, int height)
{
    const int pad = 10;// padding pixel
    int x = XY.x_point;
    int y = XY.y_point;
    return (x_start - pad) < x && x < (x_start + width + pad)
           && (y_start - pad) < y && y < (y_start + height + pad);
}

void apps_post_process(bool is_cb)
{
    if (screen.sstate == SCREEN_LOCK) return;
    char str[10];
    if (screen.sstate != SCREEN_CLOCK) {
        snprintf(str, 10, "%02d:%02d", state.dt.hour, state.dt.minute);
        Paint_DrawString_EN(102, 12, str, &Font12, COLOR_BG, COLOR_FG);
    } else {
        snprintf(str, 6, "%02d\'C", state.dev.temp);
        Paint_DrawString_EN(170, 70, str, &Font16, COLOR_BG, COLOR_FG);
    }
    Resource tray;
    if (state.bat.on_charge)
        tray = res_get_tray(TRAY_BAT_CHARGE);
    else if (state.bat.pct > 80)
        tray = res_get_tray(TRAY_BAT_HIGH);
    else if (state.bat.pct > 50)
        tray = res_get_tray(TRAY_BAT_MID);
    else if (state.bat.pct > 15)
        tray = res_get_tray(TRAY_BAT_LOW);
    else
        tray = res_get_tray(TRAY_BAT_CRIT);
    apps_draw(tray, 66, 214);
    apps_draw(res_get_tray(state.is_connected ? TRAY_BT_ON : TRAY_BT_OFF), 152,
              214);
    if (screen.sstate != SCREEN_CHRONO)
        apps_draw(res_get_tray(state.chrono.enabled ? TRAY_CHRONO : TRAY_NONE),
                  45, 200);
    // TODO notification
    // TODO alarm
    screen.post_time = 30;
    if (is_cb) {
        WARN(POST_PROCESS_BY_CB);
        LCD_1IN28_Display(screen.buffer);
    }
}


void apps_draw(Resource res, int start_x, int start_y)
{
    if (res.img != NULL)
        Paint_DrawImage(res.img, start_x, start_y, res.width, res.height);
}

bool apps_set_titlebar(enum screen_t s_title, enum popup_t p_title)
{
    if (screen.pstate == POPUP_NONE) {
        if (screen.sstate != s_title || screen.sstate == DISP_REDRAW) {
            screen.sstate = s_title;
            screen.redraw = DISP_REDRAW;
            apps_draw(res_reset(), 0, 0);
            apps_draw(res_get_titlebar(s_title, POPUP_NONE), 40, 30);
            return true;
        }
    } else {
        UNUSED(enum popup_t, p_title);
        //TODO popups
    }
    return false;
}
