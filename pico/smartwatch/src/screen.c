#include "screen.h"
#include "util.h"

#include "DEV_Config.h"
#include "Debug.h"
#include "GUI_Paint.h"
#include "hardware/adc.h"
#include "pico/stdlib.h"
#include "screen_data.h"

#include "LCD_1in28.h"
#include "Touch_1in28.h"

Touch_1IN28_XY XY;

typedef enum _SW_SCR_STATUS {
    SW_SCR_OK,
    SW_SCR_ALLOC,

} SW_SCR_STATUS;

UBYTE flag = 0, flgh = 0, l;
UWORD x, y;
struct repeating_timer timer_capture_t;

bool sw_scr_cb_refresh(struct repeating_timer* t)
{
    l++;//Determine continuous or single point
    if (l == 253) {
        l = 252;
    }
    return true;
}

void Touch_INT_callback(uint gpio, uint32_t events)
{
    if (XY.mode == 0) {
        XY.Gesture = DEV_I2C_Read_Byte(address, 0x01);
        flag = TOUCH_IRQ;
    } else {
        flag = TOUCH_IRQ;
        XY = Touch_1IN28_Get_Point();
    }
}

static SW_SCR_STATUS _sw_scr_init(UWORD** img_ptr, UDOUBLE img_s)
{
    if (DEV_Module_Init() != 0) {
        return -1;
    }
    LCD_1IN28_Init(HORIZONTAL);
    LCD_1IN28_Clear(WHITE);
    //backlight settings
    DEV_SET_PWM(100);
    //open interrupt
    DEV_IRQ_SET(DEV_I2C_INT, GPIO_IRQ_EDGE_RISE | GPIO_IRQ_EDGE_FALL, true,
                &Touch_INT_callback);

    if ((*img_ptr = (UWORD*)malloc(img_s)) == NULL) {
        return ERROR(SW_SCR_ALLOC);
    }
    Paint_NewImage((UBYTE*)*img_ptr, LCD_1IN28.WIDTH, LCD_1IN28.HEIGHT, 0,
                   WHITE);
    Paint_SetScale(65);
    Paint_Clear(WHITE);
    Paint_SetRotate(ROTATE_0);
    LCD_1IN28_Display(*img_ptr);
}

int sw_scr_run(void)
{
    UDOUBLE img_s = LCD_1IN28_HEIGHT * LCD_1IN28_WIDTH * 2;
    UWORD* img;
    if (!_sw_scr_init(&img, img_s)) {
        WARN(EXIT);
        return 1;
    }
    add_repeating_timer_us(1000, sw_scr_cb_refresh, NULL, &timer_capture_t);

    // /*1.Create a new image cache named IMAGE_RGB and fill it with white*/

    Paint_DrawPoint(50, 41, BLACK, DOT_PIXEL_1X1, DOT_FILL_RIGHTUP);// 240 240
    Paint_DrawPoint(50, 46, BLACK, DOT_PIXEL_2X2, DOT_FILL_RIGHTUP);
    Paint_DrawPoint(50, 51, BLACK, DOT_PIXEL_3X3, DOT_FILL_RIGHTUP);
    Paint_DrawPoint(50, 56, BLACK, DOT_PIXEL_4X4, DOT_FILL_RIGHTUP);
    Paint_DrawPoint(50, 61, BLACK, DOT_PIXEL_5X5, DOT_FILL_RIGHTUP);

    LCD_1IN28_Display(img);
    Paint_DrawString_EN(50, 100, "Umut Sevdi", &Font20, 0x000f, 0xfff0);
    Paint_DrawString_EN(50, 120, "İlkay Özer", &Font20, WHITE, BLUE);

    LCD_1IN28_Display(img);
    DEV_Delay_ms(1500);

    Paint_DrawImage(gImage_1inch28_1, 0, 0, 240, 240);
}
