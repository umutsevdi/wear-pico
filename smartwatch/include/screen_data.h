/******************************************************************************

 * File: include/screen_data.h
 *
 * Author: Umut Sevdi
 * Created: 11/13/23
 * Description: A C program that handles screen UI

*****************************************************************************/

#pragma once

extern const unsigned char Signal816[];
extern const unsigned char Msg816[];
extern const unsigned char Bat816[];
extern const unsigned char Bluetooth88[];
extern const unsigned char GPRS88[];
extern const unsigned char Alarm88[];

extern const unsigned char gImage_up[];
extern const unsigned char gImage_down[];
extern const unsigned char gImage_right[];
extern const unsigned char gImage_left[];
extern const unsigned char gImage_long_press[];
extern const unsigned char gImage_double_click[];
extern const unsigned char gImage_1inch28_1[];

/******************************************************************************
                               Menu UI
*****************************************************************************/

/* List all alarms */
extern const unsigned char menu_alarm[];
/* List all events */
extern const unsigned char menu_events[];
/* Start/Stop a chronometer */
extern const unsigned char menu_chrono[];
/* Play/Stop or change tracks */
extern const unsigned char menu_media[];
/* List of notifications */
extern const unsigned char menu_notify[];
/* Current step counting and reset button */
extern const unsigned char menu_step[];

/******************************************************************************
                              Screen Icons
*****************************************************************************/

extern const unsigned char screen_clock[];
extern const unsigned char screen_events[];
extern const unsigned char screen_chrono[];
extern const unsigned char screen_media[];
extern const unsigned char screen_notify[];
extern const unsigned char screen_step[];

/******************************************************************************
                             Pop-up Screens
*****************************************************************************/

extern const unsigned char popup_notify[];
extern const unsigned char popup_call[];

/******************************************************************************
                                  Misc
*****************************************************************************/

/* Section that contains battery and notification on top of the screen */
extern const unsigned char top_menu[];
/* Which icon to be used for battery status */
extern const unsigned char top_menu_bat_low[];
extern const unsigned char top_menu_bat_mid[];
extern const unsigned char top_menu_bat_high[];
extern const unsigned char top_menu_bat_full[];
extern const unsigned char top_menu_bat_charging[];
/* Icon to display when there are unread notifications */
extern const unsigned char top_menu_notify[];
/* Connection status icons */
extern const unsigned char top_menu_bth_dc[];
extern const unsigned char top_menu_bth_ok[];
