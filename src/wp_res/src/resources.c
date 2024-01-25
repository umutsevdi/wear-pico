#include "wp_res/resources.h"

/**
 * Returns a reference to the index in the given resource. 
 * @idx - index of the image
 * @res - Resource to partition
 * @w - width of the image reference
 * @h - width of the image reference
 * @return a new reference
 */

static inline Resource _get_sprite(int idx, const unsigned char* res,
                                   const int w, const int h)
{
    return (Resource){res + 2 * (idx)*w * h, w, h};
}

extern const unsigned char _res_alarm_button[];
extern const unsigned char _res_alarm_popup[];
extern const unsigned char _res_call[];
extern const unsigned char _res_gest[];
extern const unsigned char _res_media[];
extern const unsigned char _res_menu[];
extern const unsigned char _res_notify[];
extern const unsigned char _res_step[];
extern const unsigned char _res_notepad[];
extern const unsigned char _res_stopwatch[];
extern const unsigned char _res_titlebar[];
extern const unsigned char _res_tray[];

Resource res_get_tray(enum tray_t tray)
{
    const int w = 20;
    const int h = 13;
    if (tray < 0 && tray >= TRAY_T_SIZE) { return (Resource){NULL, 0, 0}; }

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
    if (p_title == POPUP_NONE) {
        img = _res_titlebar + 2 * (s_title - 3) * w * h;
    } else {
        img = _res_titlebar + 2 * (SCREEN_T_SIZE - 3) * w * h
              + 2 * (p_title - 1) * w * h;
    }
    return (Resource){img, w, h};
}

Resource res_get_menu_screen(enum menu_t selected)
{
    const int w = 160;
    const int h = 160;
    if (selected < 0 && selected >= MENU_T_SIZE) {
        return (Resource){NULL, 0, 0};
    }
    return _get_sprite(selected, _res_menu, w, h);
}

Resource res_get_direction(enum gest_dir_t direction)
{
    const int w = 48;
    const int h = 48;
    if (direction < 0 && direction >= GEST_DIR_T_SIZE) {
        return (Resource){NULL, 0, 0};
    }
    return _get_sprite(direction, _res_gest, w, h);
}

Resource res_get_app_notepad() { return (Resource){_res_notepad, 240, 140}; }

Resource res_get_app_alarm_button(bool is_paused)
{
    return _get_sprite(is_paused, _res_alarm_button, 164, 36);
}

Resource res_get_app_chrono_button(bool is_paused)
{
    return _get_sprite(is_paused, _res_stopwatch, 160, 40);
}

Resource res_get_app_media_button(bool is_paused)
{
    return _get_sprite(is_paused, _res_media, 160, 60);
}

Resource res_get_app_step() { return (Resource){_res_step, 160, 51}; }

Resource res_get_popup_alarm() { return (Resource){_res_alarm_popup, 160, 35}; }

Resource res_get_popup_notify() { return (Resource){_res_notify, 160, 138}; }

Resource res_get_popup_call() { return (Resource){_res_call, 135, 48}; }
