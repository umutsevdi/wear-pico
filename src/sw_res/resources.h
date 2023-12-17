/******************************************************************************

 * File: sw_res/resources.h
 *
 * Author: Umut Sevdi
 * Created: 12/08/23
 * Description: Access library to the UI resources

*****************************************************************************/
#ifndef SW_RESOURCES
#define SW_RESOURCES
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

enum TRAY_T {
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
Resource res_get_tray(enum TRAY_T tray);
Resource res_get_titlebar(enum SCREEN_T s_title, enum POPUP_T p_title);
enum SW_MENU_T {
    SW_MENU_ALARM,
    SW_MENU_CHRONO,
    SW_MENU_MEDIA,
    SW_MENU_EVENT,
    SW_MENU_STEP,

    SW_MENU_T_SIZE
};
Resource res_get_menu_screen(enum SW_MENU_T selected);
enum GEST_DIR_T {
    GEST_DIR_L,
    GEST_DIR_D,
    GEST_DIR_U,
    GEST_DIR_R,

    GEST_DIR_T_SIZE
};
Resource res_get_direction(enum GEST_DIR_T direction);

Resource res_get_app_chrono_button(bool is_paused);
Resource res_get_app_media_button(bool is_paused);
Resource res_get_app_alarm_button(bool is_paused);

Resource res_get_app_step();
Resource res_reset();

extern const unsigned char font60[];
extern const unsigned char font40[];
#endif
