#include "sw_res/resources.h"

static inline Resource _res_get_app_toggle_buttons(bool is_paused,
                                                   const unsigned char* res,
                                                   const int w, const int h);

#define _res_alarm_list_s 35424
#define _res_alarm_buttons_s 30720
#define _res_call_s 19440
#define _res_gest_s 27648
#define _res_media_s 57600
#define _res_menu_s 460800
#define _res_notify_s 85440
#define _res_step_s 24480
#define _res_stopwatch_s 38400
#define _res_titlebar_s 115200
#define _res_tray_s 8580
extern const unsigned char _res_alarm_list[];
extern const unsigned char _res_alarm_button[];
extern const unsigned char _res_call[];
extern const unsigned char _res_gest[];
extern const unsigned char _res_media[];
extern const unsigned char _res_menu[];
extern const unsigned char _res_notify[];
extern const unsigned char _res_step[];
extern const unsigned char _res_stopwatch[];
extern const unsigned char _res_titlebar[];
extern const unsigned char _res_tray[];

Resource res_get_tray(enum tray_t tray)
{
    const int w = 20;
    const int h = 13;
    if (tray < 0 && tray >= TRAY_T_SIZE) return (Resource){NULL, 0, 0};

    return (Resource){_res_tray + 2 * (tray)*w * h, w, h};
}
Resource res_get_titlebar(enum screen_t s_title, enum popup_t p_title)
{
    if ((p_title < 0 && p_title >= POPUP_T_SIZE)
        || (s_title < 0 && s_title >= SCREEN_T_SIZE))
        return (Resource){NULL, 0, 0};

    const int w = 160;
    const int h = 30;
    const unsigned char* img;
    if (p_title == POPUP_NONE)
        img = _res_titlebar + 2 * (s_title - 3) * w * h;
    else
        img = _res_titlebar + 2 * (SCREEN_T_SIZE - 3) * w * h
              + 2 * (p_title - 1) * w * h;
    return (Resource){img, w, h};
}
Resource res_get_menu_screen(enum menu_t selected)
{
    const int w = 160;
    const int h = 160;
    if (selected < 0 && selected >= MENU_T_SIZE) return (Resource){NULL, 0, 0};

    return (Resource){_res_menu + 2 * (selected)*w * h, w, h};
}
Resource res_get_direction(enum gest_dir_t direction)
{
    const int w = 48;
    const int h = 48;
    if (direction < 0 && direction >= GEST_DIR_T_SIZE)
        return (Resource){NULL, 0, 0};

    return (Resource){_res_gest + 2 * (direction)*w * h, w, h};
}

Resource res_get_popup_alarm()
{
    return (Resource){_res_alarm_button, 160, 35};
}

Resource res_get_popup_notify() { return (Resource){_res_notify, 160, 138}; }

Resource res_get_popup_call() { return (Resource){_res_call, 135, 48}; }

Resource res_get_app_alarm_button(bool is_paused)
{
    return _res_get_app_toggle_buttons(is_paused, _res_alarm_list, 164, 36);
}
Resource res_get_app_chrono_button(bool is_paused)
{
    return _res_get_app_toggle_buttons(is_paused, _res_stopwatch, 160, 40);
}
Resource res_get_app_media_button(bool is_paused)
{
    return _res_get_app_toggle_buttons(is_paused, _res_media, 160, 60);
}
Resource res_get_app_step() { return (Resource){_res_step, 160, 51}; }

static inline Resource _res_get_app_toggle_buttons(bool is_paused,
                                                   const unsigned char* res,
                                                   const int w, const int h)
{
    return (Resource){res + 2 * (is_paused)*w * h, w, h};
}
