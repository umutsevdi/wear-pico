#include "sw_apps/apps.h"

Display screen = {0};
Touch_1IN28_XY XY;

/* A callback function that triggers post process automatically every 30
 * seconds if it hasn't been triggered for that amount of time */
static bool _post_process_cb(repeating_timer_t* r);
/* Touchscreen reload timer */
static void _apps_touch_cb(uint gpio, uint32_t events);
/* Screen refresh timer */
static bool _apps_refresh_cb(struct repeating_timer* t);

UBYTE flag = 0, flgh = 0, l;
UWORD x, y;
struct repeating_timer timer_capture_t;

enum app_status_t apps_init(void)
{
    screen.buffer_s = LCD_1IN28_HEIGHT * LCD_1IN28_WIDTH * 2;
    screen.redraw = DISP_REDRAW;
    if (DEV_Module_Init() != 0) { return -1; }
    LCD_1IN28_Init(HORIZONTAL);
    LCD_1IN28_Clear(BLACK);
    //backlight settings
    DEV_SET_PWM(100);
    //open interrupt
    DEV_IRQ_SET(DEV_I2C_INT, GPIO_IRQ_EDGE_RISE | GPIO_IRQ_EDGE_FALL, true,
                &_apps_touch_cb);

    if ((screen.buffer = (UWORD*)malloc(screen.buffer_s)) == NULL) {
        return ERROR(APP_ERROR_ALLOC);
    }
    if ((screen.p_buffer = (UWORD*)malloc(screen.buffer_s)) == NULL) {
        return ERROR(APP_ERROR_ALLOC);
    }

    Paint_NewImage((UBYTE*)screen.buffer, LCD_1IN28.WIDTH, LCD_1IN28.HEIGHT, 0,
                   COLOR_BG);
    Paint_SetScale(65);
    Paint_SetRotate(ROTATE_0);
    LCD_1IN28_Display(screen.buffer);

    add_repeating_timer_ms(1000, _post_process_cb, NULL, &screen.__post_timer);
    add_repeating_timer_us(1000, _apps_refresh_cb, NULL, &timer_capture_t);
    return APP_OK;
}
static bool _post_process_cb(repeating_timer_t* r)
{
    UNUSED(void*, r);
    if (screen.post_time > 0)
        screen.post_time--;
    else
        apps_post_process(true);
}

void _apps_touch_cb(uint gpio, uint32_t events)
{
    UNUSED(int, gpio, events);
    if (XY.mode == 0) {
        XY.Gesture = DEV_I2C_Read_Byte(address, 0x01);
        flag = TOUCH_IRQ;
    } else {
        flag = TOUCH_IRQ;
        XY = Touch_1IN28_Get_Point();
    }
}

bool _apps_refresh_cb(struct repeating_timer* t)
{
    l++;//Determine continuous or single point
    if (l == 253) { l = 252; }
    return true;
}
