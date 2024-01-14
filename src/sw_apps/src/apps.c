#include "sw_apps/apps.h"
#include "sw_bt/bt.h"
#include "sw_os/state.h"
#include "sw_res/resources.h"

enum notepad_color_t {
    NOTEPAD_COLOR_NONE = COLOR_BG,
    NOTEPAD_COLOR_WHITE = WHITE,
    NOTEPAD_COLOR_RED = RED,
    NOTEPAD_COLOR_GREEN = GREEN,
    NOTEPAD_COLOR_BLUE = BLUE
};

/* Displays the current step amount */
static void _step_display();

/* Assigns the currently selected color. Returns false if no color is selected */
static bool _notepad_get_color(enum notepad_color_t* color);

enum app_status_t apps_lock_screen()
{
    screen.sstate = SCREEN_LOCK;
    Paint_Clear(BLACK);
    DEV_SET_PWM(0);
    LCD_1IN28_Display(screen.buffer);
    XY.Gesture = None;
    while (true) {
        if (state.popup.type != POPUP_NONE
            || state.__popup_req.type != POPUP_NONE) {
            DEV_SET_PWM(80);
            if (!apps_poll_popup()) return APP_OK;
        }
        if (XY.Gesture == DOUBLE_CLICK) {
            /* Returns with up gesture to trigger down
             * on menu and return to clock screen safely
             */
            XY.Gesture = UP;
            DEV_SET_PWM(100);
            return APP_OK;
        }
        sleep_ms(200);
    }
}

enum app_status_t apps_load_media()
{
#define BTN_PLAY_PAUSE 88, 120, 64, 64
#define BTN_NEXT 152, 120, 48, 64
#define BTN_PREV 40, 120, 48, 64
    bool clicked;
    int x = 0, y = 0;

    while (true) {
        if (x != XY.x_point || y != XY.y_point) {
            x = XY.x_point;
            y = XY.y_point;
            clicked = true;
        }
        if (!apps_poll_popup()) { screen.redraw = DISP_REDRAW; }
        if (apps_is_exited()) { return APP_OK; }
        if (clicked) {
            if (apps_is_clicked(BTN_PLAY_PAUSE)) {
                state.media.is_playing = !state.media.is_playing;
                screen.redraw = DISP_REDRAW;
                bt_send_resp(BT_RESP_OSC_PLAY_PAUSE);
            } else if (apps_is_clicked(BTN_NEXT)) {
                screen.redraw = DISP_REDRAW;
                bt_send_resp(BT_RESP_OSC_NEXT);
            } else if (apps_is_clicked(BTN_PREV)) {
                screen.redraw = DISP_REDRAW;
                bt_send_resp(BT_RESP_OSC_PREV);
            }
        }
        if (!state.media.is_fetched) {
            PRINT(BT_OSC_FETCH);
            state.media.is_fetched = true;
            screen.redraw = DISP_REDRAW;
        }
        if (apps_set_titlebar(SCREEN_MEDIA, POPUP_NONE)) {
            XY.x_point = 0;
            XY.y_point = 0;
        }
        if (screen.redraw) {
            apps_draw(res_get_app_media_button(!state.media.is_playing), 40,
                      120);
            char center[30] = {0};
            int len = strnlen(state.media.song, 30);
            strncpy(center, state.media.song, len);
            Paint_DrawString_EN(5, 80, strcenter(center, len, 16), &Font20,
                                COLOR_BG, COLOR_FG);
            len = strnlen(state.media.artist, 30);
            memset(center, 0, 30);
            strncpy(center, state.media.artist, len);
            Paint_DrawString_EN(50, 185, strcenter(center, len, 20), &Font12,
                                COLOR_BG, COLOR_FG);
            apps_post_process(false);
            LCD_1IN28_Display(screen.buffer);
            screen.redraw = DISP_SYNC;
        }
        clicked = false;
        sleep_ms(50);
    }
}

enum app_status_t apps_load_calendar()
{
    int x = 0, y = 0;

    while (true) {
        if (x != XY.x_point || y != XY.y_point) {
            x = XY.x_point;
            y = XY.y_point;
        }
        if (!apps_poll_popup()) { screen.redraw = DISP_REDRAW; }
        if (apps_is_exited()) { return APP_OK; }
        if (apps_set_titlebar(SCREEN_CALENDAR, POPUP_NONE)) {
            XY.x_point = 0;
            XY.y_point = 0;
            DateTime now = state.dt;
            int number_of_days = dt_number_of_days(&now);
            int dow = dt_get_day_int(&now);
            if (dow == 0) { dow = 7; }

            DateTime beginning = {.day = 1,
                                  .month = now.month,
                                  .year = now.year};
            int dow_begin = dt_get_day_int(&beginning);
            if (dow == 0) { dow = 7; }
            char title[12];
            snprintf(title, 12, "%3s %4d", DATETIME_MONTH(now.month), now.year);
            strcenter(title, strnlen(title, 12), 12);

            Paint_DrawString_EN(54, 60, title, &Font16, COLOR_BG, COLOR_FG);
            Paint_DrawString_EN(25, 80, "Mon Tue Wed Thu Fri Sat Sun", &Font12,
                                COLOR_BG, COLOR_FG);
            int j = 100;

            Paint_DrawRectangle(20, 95, 228, 170, COLOR_FG, DOT_PIXEL_1X1,
                                DRAW_FILL_EMPTY);
            char str[4];
            for (int i = dow_begin; i < number_of_days + dow_begin; i++) {
                snprintf(str, 4, " %02d", i - dow_begin + 1);
                int fg_color = COLOR_FG;
                int bg_color = COLOR_BG;
                if (((i - 1) % 7) > 4) {
                    if ((i - dow == now.day)) {
                        bg_color = YELLOW;
                    } else {
                        bg_color = GBLUE;
                    }
                    fg_color = BLACK;
                } else if (i - dow == now.day) {
                    fg_color = YELLOW;
                }
                Paint_DrawString_EN(25 + (i - 1) % 7 * 28, j, str, &Font12,
                                    bg_color, fg_color);
                if (i % 7 == 0) { j += 14; }
            }
            if (screen.redraw) {
                apps_post_process(false);
                LCD_1IN28_Display(screen.buffer);
                screen.redraw = DISP_SYNC;
            }
        }
    }
}

enum app_status_t apps_load_notepad()
{
#define PAINT_AREA 42, 79, 156, 95
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
        if (clicked) {
            bool is_selected = _notepad_get_color(&color);
            if (!is_selected && apps_is_clicked(PAINT_AREA)) {
                DOT_PIXEL px_size =
                    color == NOTEPAD_COLOR_NONE ? DOT_PIXEL_4X4 : DOT_PIXEL_2X2;
                if (apps_is_clicked_d(old_x, old_y, PAINT_AREA))
                    Paint_DrawLine(old_x, old_y, x, y, color, px_size,
                                   LINE_STYLE_SOLID);
                else
                    Paint_DrawPoint(x, y, color, px_size, DOT_FILL_AROUND);
            }
            screen.redraw = DISP_PARTIAL;
        }

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

enum app_status_t apps_load_step()
{
    int sec = 0;
    while (true) {
        if (!apps_poll_popup()) screen.redraw = DISP_REDRAW;
        if (apps_set_titlebar(SCREEN_STEP, POPUP_NONE)) {
            XY.x_point = 0;
            XY.y_point = 0;
            apps_post_process(false);
            apps_draw(res_get_app_step(), 40, 150);
        }
        if (sec != state.dt.second) {
            screen.redraw = DISP_PARTIAL;
            sec = state.dt.second;
        }
        if (apps_is_exited()) return APP_OK;

        if (screen.redraw) {
            _step_display();
            LCD_1IN28_Display(screen.buffer);
            screen.redraw = DISP_SYNC;
        }
    }
}

enum app_status_t apps_load_log(void)
{
#define NOTIFY_COLOR 0x9ce5
#define NOTIFY_TEXT_COLOR 0x8c7
    int sec = 0;
    while (true) {
        if (!apps_poll_popup()) screen.redraw = DISP_REDRAW;
        if (apps_set_titlebar(SCREEN_LOG, POPUP_NONE)) {
            XY.x_point = 0;
            XY.y_point = 0;
            apps_post_process(false);
            apps_draw(res_get_popup_notify(), 40, 65);
        }
        if (sec != state.dt.second) {
            screen.redraw = DISP_PARTIAL;
            sec = state.dt.second;
        }
        if (apps_is_exited()) return APP_OK;

        if (screen.redraw) {
            char* array[15] = {0};
            int array_s = strwrap(LOG_BUFFER, strnlen(LOG_BUFFER, LOG_BUFFER_S),
                                  40, array, 14);
            if (array_s != -1) {
                for (int i = 0; i < array_s; i++)
                    Paint_DrawString_EN(55, 70 + 9 * i, array[i], &Font8,
                                        NOTIFY_TEXT_COLOR, COLOR_FG);
            }

            LCD_1IN28_Display(screen.buffer);
            screen.redraw = DISP_SYNC;
        }
    }
}

static bool _notepad_get_color(enum notepad_color_t* color)
{
#define BTN_WHITE 66, 204, 15, 15
#define BTN_RED 92, 204, 15, 15
#define BTN_BLUE 119, 204, 15, 15
#define BTN_GREEN 148, 204, 15, 15
#define BTN_ERASER 0, 112, 32, 32
#define BTN_CLEAR 208, 112, 32, 32
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

static void _step_display()
{
    int base_x = 40;
    int base_y = 86;
    char numstr[6] = {0};
    const int x_size = 40;
    const int y_size = 40;
    snprintf(numstr, 6, "%d", state.step);
    strcenter(numstr, strnlen(numstr, 6), 6);
    int x = base_x;
    int y = base_y;
    for (int i = 0; i < 5; i++) {
        const unsigned char* img_ptr = NULL;
        if (numstr[i] >= '0' && numstr[i] <= '9') {
            img_ptr = font40 + 2 * (numstr[i] - '0') * x_size * y_size;
            Paint_DrawImage(img_ptr, x, y, x_size, y_size);
        }
        x += 30;
    }
}
