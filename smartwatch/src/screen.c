#include "screen.h"
#include "fonts.h"
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

#define GESTURE_S(G)                                                           \
    G == UP             ? "UP   "                                              \
    : G == Down         ? "DOWN "                                              \
    : G == LEFT         ? "LEFT "                                              \
    : G == RIGHT        ? "RIGHT"                                              \
    : G == CLICK        ? "CLICK"                                              \
    : G == DOUBLE_CLICK ? "DOUBL"                                              \
    : G == LONG_PRESS   ? "LONG "                                              \
                        : "NONE "

#define MODE_S(M)                                                              \
    M == TOUCH_INIT          ? "INIT"                                          \
    : M == TOUCH_IRQ         ? "IRQ "                                          \
    : M == TOUCH_FUNCTION    ? "FUNC"                                          \
    : M == TOUCH_DRAW        ? "DRAW"                                          \
    : M == TOUCH_OUT_GESTURE ? "GEST"                                          \
    : M == TOUCH_NO_DRAW     ? "NDRW"                                          \
                             : "NONE"

typedef enum _SW_SCR_STATUS {
    SW_SCR_OK,
    SW_SCR_ALLOC,

} SW_SCR_STATUS;

UBYTE flag = 0, flgh = 0, l;
UWORD x, y;
struct repeating_timer timer_capture_t;

char* state_str[] = {"IDLE", "DRAW", "GESTURE", "UP", "DOWN", "LEFT", "RIGHT"};
enum DRAWING_STATE {
    DRAWING_IDLE,
    DRAWING_DRAW,
    DRAWING_GESTURE,
    DRAWING_UP,
    DRAWING_DOWN,
    DRAWING_LEFT,
    DRAWING_RIGHT
};

bool sw_scr_cb_refresh(struct repeating_timer* t)
{
    l++;//Determine continuous or single point
    if (l == 253) { l = 252; }
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
    if (DEV_Module_Init() != 0) { return -1; }
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

    Paint_DrawPoint(130, 220, BLACK, DOT_PIXEL_8X8, DOT_FILL_AROUND);
    LCD_1IN28_Display(img);
    Paint_DrawString_EN(50, 100, "Umut Sevdi", &Font20, BLACK, WHITE);
    Paint_DrawString_EN(50, 120, "Ilkay Ozer", &Font20, BLUE, WHITE);
    Paint_DrawString_EN(50, 40, "Umut Sevdi", &Font8, BLACK, WHITE);
    Paint_DrawString_EN(50, 0, "Lorem Ipsum", &Font12, BLUE, WHITE);
    Paint_DrawString_EN(50, 20, "Lorem Ipsum", &Font16, BLUE, WHITE);
    Paint_DrawString_EN(50, 40, "Lorem Ipsum", &Font20, BLUE, WHITE);
    Paint_DrawString_EN(50, 60, "Lorem Ipsum", &Font24, BLUE, WHITE);

    LCD_1IN28_Display(img);
    DEV_Delay_ms(1500);
    Paint_ClearWindows(35, 35, 205, 208, BLACK);

    Paint_DrawImage(gImage_1inch28_1, 0, 0, 240, 240);
    LCD_1IN28_Display(img);
    DEV_Delay_ms(1500);
    Paint_ClearWindows(35, 35, 205, 208, WHITE);

    XY.mode = 0;
    if (Touch_1IN28_init(XY.mode) == true)
        printf("OK!\r\n");
    else
        printf("NO!\r\n");

    LCD_1IN28_Display(img);
    while (true) {
        printf("{%3d %3d} %s %s \n", XY.x_point, XY.y_point, MODE_S(XY.mode),
               GESTURE_S(XY.Gesture));
        XY.x_point = (XY.x_point > 37) ? XY.x_point : 37;
        XY.y_point = (XY.y_point > 37) ? XY.y_point : 37;

        XY.x_point = (XY.x_point < 205) ? XY.x_point : 205;
        XY.y_point = (XY.y_point < 203) ? XY.y_point : 203;
        Paint_DrawLine(x, y, XY.x_point, XY.y_point, XY.color, DOT_PIXEL_2X2,
                       LINE_STYLE_DOTTED);
        const char* direction = "";

        if (x > 120) {
            direction = "RIGHT";
        } else {
            direction = "LEFT";
        }
        if (y > 120) {
            direction = "UP";
        } else {
            direction = "DOWN";
        }
        switch (XY.Gesture) {
        case None: XY.color = RED; break;
        case UP: XY.color = BLUE; break;
        case Down: XY.color = GREEN; break;
        case LEFT: XY.color = GRAY; break;
        case RIGHT: XY.color = YELLOW; break;
        }
        Paint_DrawString_EN(85, 190, GESTURE_S(XY.Gesture), &Font20, BLACK,
                            XY.color);

        Paint_DrawString_EN(35, 130, direction, &Font24, 0X647C, WHITE);

        x = XY.x_point;
        y = XY.y_point;
        LCD_1IN28_Display(img);
    }
}
