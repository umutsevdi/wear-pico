/******************************************************************************

 * File: include/peripherals.h
 *
 * Author: Umut Sevdi
 * Created: 12/05/23
 * Description: Additional peripheral control library

*****************************************************************************/
#ifndef SW_PERIPHERALS
#define SW_PERIPHERALS
#include "util.h"

#define BUZZER 18
#define LED 19

void sw_peripheral_init(void);

void sw_peripheral_toggle(int peripheral, bool value);
#endif// !SW_PERIPHERALS
