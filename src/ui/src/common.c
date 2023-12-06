#include "common.h"

SwScreen screen = {0};
Touch_1IN28_XY XY;

void scr_paint_time(DateTime* dt, int base_x, int base_y, bool show_sec)
{
    const int x_size = 40;
    const int y_size = show_sec ? 40 : 60;
    int datestr_s = show_sec ? 9 : 6;
    char datestr[9] = {0};
    const unsigned char* font = show_sec ? font40 : font60;
    if (show_sec) {
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
        x += show_sec ? 30 : 40;
    }
}

bool scr_is_clicked(int x_start, int y_start, int width, int height)
{
    int x = XY.x_point;
    int y = XY.y_point;
    return x_start < x && x < (x_start + width) && y_start < y
           && y < (y_start + height);
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

SCR_STATUS scr_init(void)
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
                &sw_scr_touch_cb);

    if ((screen.buffer = (UWORD*)malloc(screen.buffer_s)) == NULL) {
        return ERROR(SCR_ERROR_ALLOC);
    }
    if ((screen.p_buffer = (UWORD*)malloc(screen.buffer_s)) == NULL) {
        return ERROR(SCR_ERROR_ALLOC);
    }

    Paint_NewImage((UBYTE*)screen.buffer, LCD_1IN28.WIDTH, LCD_1IN28.HEIGHT, 0,
                   COLOR_BG);
    Paint_SetScale(65);
    Paint_SetRotate(ROTATE_0);
    LCD_1IN28_Display(screen.buffer);

    add_repeating_timer_us(1000, sw_scr_refresh_cb, NULL, &timer_capture_t);
    return SCR_STATUS_OK;
}
