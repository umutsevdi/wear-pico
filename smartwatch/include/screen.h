/******************************************************************************

 * File: smartwatch/include/screen.h
 *
 * Author: Umut Sevdi
 * Created: 11/12/23
 * Description: Touchpad screen utilities

*****************************************************************************/

#ifndef SW_SCREEN
#define SW_SCREEN

#include "DEV_Config.h"
typedef enum {
    SCR_STATUS_OK,
    SCR_ERROR_ALLOC,
    SCR_WARN_TOUCH_FAILED,

} SCR_STATUS;

enum SCREEN_T {
    SCREEN_CLOCK,
    SCREEN_MENU,
    SCREEN_ALARM,
    SCREEN_CHRONO,
    SCREEN_EVENT,
    SCREEN_NOTIFY,
    SCREEN_MEDIA,
    SCREEN_STEP,
};

/* Whether a pop-up is enabled or not. if so which popup is enabled */
enum POPUP_T {
    POPUP_NONE,   /* Pop-Up is disabled, showing regular screen */
    POPUP_CALL,   /* Incoming call UI */
    POPUP_NOTIFY, /* Notification UI */
    POPUP_ALARM   /* Alarm UI */
};

typedef struct {
    /* pixel buffer for regular images*/
    UWORD* buffer;
    /* buffer for pop-ups. So the original screen state is kept */
    UWORD* p_buffer;
    UDOUBLE buffer_s;

    enum SCREEN_T sstate;
    enum POPUP_T pstate;
} SwScreen;

extern SwScreen s;

/**
 * Initializes the externally defined SwScreen s.
 */
SCR_STATUS sw_scr_init(void);
SCR_STATUS load_menu();

int sw_scr_run(void);

#endif
