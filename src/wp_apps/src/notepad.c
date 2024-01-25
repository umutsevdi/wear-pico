#include "wp_apps/apps.h"

#define PAINT_AREA 42, 79, 156, 95
#define BTN_WHITE 66, 204, 15, 15
#define BTN_RED 92, 204, 15, 15
#define BTN_BLUE 119, 204, 15, 15
#define BTN_GREEN 148, 204, 15, 15
#define BTN_ERASER 0, 112, 32, 32
#define BTN_CLEAR 208, 112, 32, 32

enum notepad_color_t {
    NOTEPAD_COLOR_NONE = COLOR_BG,
    NOTEPAD_COLOR_WHITE = WHITE,
    NOTEPAD_COLOR_RED = RED,
    NOTEPAD_COLOR_GREEN = GREEN,
    NOTEPAD_COLOR_BLUE = BLUE
};

/* Assigns the currently selected color. Returns false if no color is selected */
static bool _notepad_get_color(enum notepad_color_t* color)
{
    if (apps_is_clicked(BTN_ERASER)) {
        WARN(BTN_ERASER);
        *color = NOTEPAD_COLOR_NONE;
    } else if (apps_is_clicked(BTN_CLEAR)) {
        WARN(BTN_CLEAR);
        Paint_DrawRectangle(32, 69, 208, 179, NOTEPAD_COLOR_NONE, DOT_PIXEL_2X2,
                            DRAW_FILL_FULL);
    } else if (apps_is_clicked(BTN_WHITE)) {
        WARN(BTN_WHITE);
        *color = NOTEPAD_COLOR_WHITE;
    } else if (apps_is_clicked(BTN_RED)) {
        WARN(BTN_RED);
        *color = NOTEPAD_COLOR_RED;
    } else if (apps_is_clicked(BTN_BLUE)) {
        WARN(BTN_BLUE);
        *color = NOTEPAD_COLOR_BLUE;
    } else if (apps_is_clicked(BTN_GREEN)) {
        WARN(BTN_GREEN);
        *color = NOTEPAD_COLOR_GREEN;
    } else
        return false;
    return true;
}

static void _handle_click(enum notepad_color_t* color, int* x, int* y,
                          int* old_x, int* old_y)
{
    bool is_selected = _notepad_get_color(color);
    if (!is_selected && apps_is_clicked(PAINT_AREA)) {
        DOT_PIXEL px_size =
            *color == NOTEPAD_COLOR_NONE ? DOT_PIXEL_4X4 : DOT_PIXEL_2X2;
        if (apps_is_clicked_d(*old_x, *old_y, PAINT_AREA))
            Paint_DrawLine(*old_x, *old_y, *x, *y, *color, px_size,
                           LINE_STYLE_SOLID);
        else
            Paint_DrawPoint(*x, *y, *color, px_size, DOT_FILL_AROUND);
    }
    screen.redraw = DISP_PARTIAL;
}

enum app_status_t apps_load_notepad()
{
    enum notepad_color_t color = NOTEPAD_COLOR_WHITE;
    bool clicked = false;
    int x = 0, y = 0;
    int old_x = 0;
    int old_y = 0;
    absolute_time_t now;
    absolute_time_t then = get_absolute_time();
    while (true) {
        if (x != XY.x_point || y != XY.y_point) {
            now = get_absolute_time();
            if (absolute_time_diff_us(then, now) / 1000 < 250) {
                old_x = x;
                old_y = y;
            } else {
                old_x = 0;
                old_y = 0;
            }
            then = now;
            now = get_absolute_time();
            x = XY.x_point;
            y = XY.y_point;
            clicked = true;
        }
        if (!apps_poll_popup()) screen.redraw = DISP_REDRAW;
        if (apps_is_exited()) {
            // save to buffer
            memcpy(screen.canvas_buffer, screen.buffer, screen.buffer_s);
            screen.is_saved = true;
            return APP_OK;
        }
        if (clicked) { _handle_click(&color, &x, &y, &old_x, &old_y); }

        if (apps_set_titlebar(SCREEN_NOTE, POPUP_NONE)) {
            XY.x_point = 0;
            XY.y_point = 0;
            if (screen.is_saved)
                memcpy(screen.buffer, screen.canvas_buffer, screen.buffer_s);
            else
                apps_draw(res_get_app_notepad(), 0, 64);

            apps_post_process(false);
        }

        if (screen.redraw) {
            LCD_1IN28_Display(screen.buffer);
            screen.redraw = DISP_SYNC;
        }
        clicked = false;
    }
}
