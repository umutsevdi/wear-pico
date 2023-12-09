#include "sw_apps/apps_common.h"

/**
 * A callback function that triggers post process automatically every 30
 * seconds if it hasn't been triggered for that amount of time
 */
static bool _apps_post_process_cb(repeating_timer_t* r);
/**
 * Touchscreen reload timer
 */
static void _apps_touch_cb(uint gpio, uint32_t events);
static bool _apps_refresh_cb(struct repeating_timer* t);

SwScreen screen = {0};
Touch_1IN28_XY XY;

void apps_paint_time(DateTime* dt, int base_x, int base_y, bool show_sec)
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

bool apps_is_clicked(int x_start, int y_start, int width, int height)
{
    const int pad = 10;// padding pixel
    int x = XY.x_point;
    int y = XY.y_point;
    return (x_start + pad) < x && x < (x_start + width + pad)
           && (y_start + pad) < y && y < (y_start + height + pad);
}

void apps_post_process(bool is_cb)
{
    if (screen.sstate != SCREEN_CLOCK) {
        char str[10];
        snprintf(str, 10, "%02d:%02d", state.dt.hour, state.dt.minute);
        Paint_DrawString_EN(102, 12, str, &Font12, COLOR_BG, COLOR_FG);
    }
    const unsigned char* bat_img;
    if (state.bat.on_charge)
        bat_img = tray_bat_charge;
    else if (state.bat.pct > 80)
        bat_img = tray_bat_high;
    else if (state.bat.pct > 50)
        bat_img = tray_bat_mid;

    else if (state.bat.pct > 15)
        bat_img = tray_bat_low;
    else
        bat_img = tray_bat_crit;
    Paint_DrawImage(bat_img, 66, 214, SCR_TRAY);

    const unsigned char* bt_img = state.is_connected ? tray_bt_on : tray_bt_off;
    Paint_DrawImage(bt_img, 152, 214, SCR_TRAY);
    if (screen.sstate != SCREEN_CHRONO)
        Paint_DrawImage(state.chrono.enabled ? tray_stopwatch : tray_none, 45,
                        200, SCR_TRAY);
    // TODO notification
    // TODO alarm
    screen.post_time = 30;
    if (is_cb) {
        WARN(POST_PROCESS_BY_CB);
        LCD_1IN28_Display(screen.buffer);
    } else {
        WARN(POST_PROCESS);
    }
}

UBYTE flag = 0, flgh = 0, l;
UWORD x, y;
struct repeating_timer timer_capture_t;

SCR_STATUS apps_init(void)
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

    add_repeating_timer_ms(1000, _apps_post_process_cb, NULL,
                           &screen.__post_timer);
    add_repeating_timer_us(1000, _apps_refresh_cb, NULL, &timer_capture_t);
    return SCR_STATUS_OK;
}

static bool _apps_post_process_cb(repeating_timer_t* r)
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
