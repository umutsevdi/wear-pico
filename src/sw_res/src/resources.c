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
#define _res_clear_s 172800
extern const unsigned char _res_alarm_list[];
extern const unsigned char _res_alarm_buttons[];
extern const unsigned char _res_call[];
extern const unsigned char _res_gest[];
extern const unsigned char _res_media[];
extern const unsigned char _res_menu[];
extern const unsigned char _res_notify[];
extern const unsigned char _res_step[];
extern const unsigned char _res_stopwatch[];
extern const unsigned char _res_titlebar[];
extern const unsigned char _res_tray[];
extern const unsigned char _res_clear[];

Resource res_get_tray(enum TRAY_T tray)
{
    const int w = 20;
    const int h = 13;
    if (tray < 0 && tray >= TRAY_T_SIZE)
        return (Resource){.img = NULL, .width = 0, .height = 0};

    return (Resource){
        .img = _res_tray + 2 * (tray)*w * h,
        .width = w,
        .height = h,
    };
}
Resource res_get_titlebar(enum SCREEN_T s_title, enum POPUP_T p_title)
{
    if ((p_title < 0 && p_title >= POPUP_T_SIZE)
        || (s_title < 0 && s_title >= SCREEN_T_SIZE))
        return (Resource){.img = NULL, .width = 0, .height = 0};

    const int w = 160;
    const int h = 30;
    const unsigned char* img;
    if (p_title == POPUP_NONE)
        img = _res_titlebar + 2 * (s_title - 2) * w * h;
    else
        img = _res_titlebar + 2 * (SCREEN_T_SIZE - 1) * w * h
              + 2 * (p_title)*w * h;
    return (Resource){
        .img = img,
        .width = w,
        .height = h,
    };
}
Resource res_get_menu_screen(enum SW_MENU_T selected)
{
    const int w = 160;
    const int h = 160;
    if (selected < 0 && selected >= SW_MENU_T_SIZE)
        return (Resource){.img = NULL, .width = 0, .height = 0};

    return (Resource){
        .img = _res_menu + 2 * (selected)*w * h,
        .width = w,
        .height = h,
    };
}
Resource res_get_direction(enum GEST_DIR_T direction)
{
    const int w = 48;
    const int h = 48;
    if (direction < 0 && direction >= GEST_DIR_T_SIZE)
        return (Resource){.img = NULL, .width = 0, .height = 0};

    return (Resource){
        .img = _res_gest + 2 * (direction)*w * h,
        .width = w,
        .height = h,
    };
}

Resource res_get_app_chrono_button(bool is_paused)
{
    return _res_get_app_toggle_buttons(is_paused, _res_stopwatch, 160, 40);
}
Resource res_get_app_media_button(bool is_paused)
{
    return _res_get_app_toggle_buttons(is_paused, _res_media, 160, 60);
}
Resource res_get_app_alarm_button(bool is_paused)
{
    return _res_get_app_toggle_buttons(is_paused, _res_alarm_buttons, 164, 36);
}
Resource res_get_app_step()
{
    const int w = 160;
    const int h = 51;
    return (Resource){.img = _res_step, .width = w, .height = h};
}

static inline Resource _res_get_app_toggle_buttons(bool is_paused,
                                                   const unsigned char* res,
                                                   const int w, const int h)
{
    return (Resource){
        .img = res + 2 * (is_paused)*w * h,
        .width = w,
        .height = h,
    };
}

Resource res_reset()
{
    return (Resource){.img = _res_clear,
                      .width = WATCH_HEIGHT,
                      .height = WATCH_WIDTH};
}
