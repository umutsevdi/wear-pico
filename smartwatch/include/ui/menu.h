/******************************************************************************

 * File: include/menu.h
 *
 * Author: Umut Sevdi
 * Created: 12/01/23
 * Description: App switch menu

*****************************************************************************/
#ifndef SW_MENU
#define SW_MENU

#include "ui/common.h"

enum SW_MENU_T {
    SW_MENU_ALARM,
    SW_MENU_CHRONO,
    SW_MENU_EVENT,
    SW_MENU_MEDIA,
    SW_MENU_STEP,
    SW_MENU_SIZE
};
#define MENU_S(CURRENT)                                                        \
    CURRENT == SW_MENU_ALARM    ? "MENU_ALARM"                                 \
    : CURRENT == SW_MENU_CHRONO ? "MENU_CHRONO"                                \
    : CURRENT == SW_MENU_EVENT  ? "MENU_EVENT"                                 \
    : CURRENT == SW_MENU_MEDIA  ? "MENU_MEDIA"                                 \
    : CURRENT == SW_MENU_STEP   ? "MENU_STEP"                                  \
                                : "NONE"

#endif
