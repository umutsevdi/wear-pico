/******************************************************************************

 * File: sw_res/resources.h
 *
 * Author: Umut Sevdi
 * Created: 12/08/23
 * Description: Access library to the UI resources

*****************************************************************************/
#ifndef RESOURCES
#define RESOURCES
#define WATCH_HEIGHT 240
#define WATCH_WIDTH 240
#include "sw_common/util.h"

#define COLOR_BG 0x42
#define COLOR_FG 0xffff

/* Coordinate constants */

/* Screen matrix coordinates */
#define SCREEN_C 0, 0, 240, 240
/* Tray x and y coordinates */
#define TRAY_C 20, 13

typedef struct {
    const unsigned char* img;
    size_t width;
    size_t height;
} Resource;
#define res_C(r) r.width, r.height

enum tray_t {
    TRAY_NONE,
    TRAY_BT_ON,
    TRAY_BT_OFF,
    TRAY_CHRONO,
    TRAY_NOTIFY,
    TRAY_ALARM,
    TRAY_BAT_CHARGE,
    TRAY_BAT_HIGH,
    TRAY_BAT_MID,
    TRAY_BAT_LOW,
    TRAY_BAT_CRIT,

    TRAY_T_SIZE
};
Resource res_get_tray(enum tray_t tray);
Resource res_get_titlebar(enum screen_t s_title, enum popup_t p_title);
Resource res_get_menu_screen(enum menu_t selected);
enum gest_dir_t {
    GEST_DIR_L,
    GEST_DIR_D,
    GEST_DIR_U,
    GEST_DIR_R,

    GEST_DIR_T_SIZE
};
Resource res_get_direction(enum gest_dir_t direction);

Resource res_get_app_chrono_button(bool is_paused);
Resource res_get_app_media_button(bool is_paused);
Resource res_get_app_alarm_button(bool is_paused);
Resource res_get_app_step();
Resource res_get_popup_alarm();
Resource res_get_popup_notify();
Resource res_get_popup_call();

extern const unsigned char font60[];
extern const unsigned char font40[];
#endif
