#include "ui/common.h"

#include "fonts.h"
#include "global.h"
#include "protocol.h"
#include "util.h"

#include "DEV_Config.h"
#include "Debug.h"
#include "GUI_Paint.h"
#include "hardware/adc.h"
#include "pico/stdlib.h"
#include "resources.h"

#include "LCD_1in28.h"
#include "Touch_1in28.h"
#include <stdio.h>
SwScreen s = {0};
Touch_1IN28_XY XY;

void scr_paint_time(DateTime* dt, int base_x, int base_y)
{
    const int x_size = 40;
    const int y_size = s.show_sec ? 40 : 60;
    int datestr_s = s.show_sec ? 9 : 6;
    char datestr[9] = {0};
    const unsigned char* font = s.show_sec ? font40 : font60;
    if (s.show_sec) {
        snprintf(datestr, datestr_s, "%02d:%02d:%02d", dt->hour, dt->minute,
                 dt->second);
    } else {
        snprintf(datestr, datestr_s, "%02d:%02d", dt->hour, dt->minute);
    }

    int x = base_x;
    int y = base_y;
    for (int i = 0; i < datestr_s - 1; i++) {
        const unsigned char* img_ptr = NULL;
        if (datestr[i] >= '0' && datestr[i] <= '9')
            img_ptr = font + 2 * (datestr[i] - '0') * x_size * y_size;
        else if (datestr[i] == ':')
            img_ptr = font + 2 * 10 * x_size * y_size;

        Paint_DrawImage(img_ptr, x, y, x_size, y_size);
        x += s.show_sec ? 30 : 40;
    }
}

UBYTE flag = 0, flgh = 0, l;
UWORD x, y;
struct repeating_timer timer_capture_t;

bool sw_scr_refresh_cb(struct repeating_timer* t)
{
    l++;//Determine continuous or single point
    if (l == 253) { l = 252; }
    return true;
}

void sw_scr_touch_cb(uint gpio, uint32_t events)
{
    if (XY.mode == 0) {
        XY.Gesture = DEV_I2C_Read_Byte(address, 0x01);
        flag = TOUCH_IRQ;
    } else {
        flag = TOUCH_IRQ;
        XY = Touch_1IN28_Get_Point();
    }
}

SCR_STATUS sw_scr_init(void)
{
    s.buffer_s = LCD_1IN28_HEIGHT * LCD_1IN28_WIDTH * 2;
    s.show_sec = false;
    s.redraw = true;
    if (DEV_Module_Init() != 0) { return -1; }
    LCD_1IN28_Init(HORIZONTAL);
    LCD_1IN28_Clear(BLACK);
    //backlight settings
    DEV_SET_PWM(100);
    //open interrupt
    DEV_IRQ_SET(DEV_I2C_INT, GPIO_IRQ_EDGE_RISE | GPIO_IRQ_EDGE_FALL, true,
                &sw_scr_touch_cb);

    if ((s.buffer = (UWORD*)malloc(s.buffer_s)) == NULL) {
        return ERROR(SCR_ERROR_ALLOC);
    }
    if ((s.p_buffer = (UWORD*)malloc(s.buffer_s)) == NULL) {
        return ERROR(SCR_ERROR_ALLOC);
    }

    Paint_NewImage((UBYTE*)s.buffer, LCD_1IN28.WIDTH, LCD_1IN28.HEIGHT, 0,
                   COLOR_BG);
    Paint_SetScale(65);
    Paint_SetRotate(ROTATE_0);
    LCD_1IN28_Display(s.buffer);

    add_repeating_timer_us(1000, sw_scr_refresh_cb, NULL, &timer_capture_t);
    return SCR_STATUS_OK;
}
