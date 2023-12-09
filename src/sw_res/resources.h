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
extern const unsigned char menu_alarm[];
extern const unsigned char menu_events[];
extern const unsigned char menu_media[];
extern const unsigned char menu_pedometer[];
extern const unsigned char menu_stopwatch[];
extern const unsigned char* menu_frames[];
extern const unsigned char watch[];

extern const unsigned char font60[];
extern const unsigned char font40[];

extern const unsigned char app_stopwatch_start[];
extern const unsigned char app_stopwatch_stop[];

extern const unsigned char tray_none[];
extern const unsigned char tray_stopwatch[];
extern const unsigned char tray_alarm[];
extern const unsigned char tray_notify[];
extern const unsigned char tray_bt_on[];
extern const unsigned char tray_bt_off[];
extern const unsigned char tray_bat_charge[];
extern const unsigned char tray_bat_high[];
extern const unsigned char tray_bat_mid[];
extern const unsigned char tray_bat_low[];
extern const unsigned char tray_bat_crit[];

#endif
