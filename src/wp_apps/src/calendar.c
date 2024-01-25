#include "wp_apps/apps.h"
#include "wp_dev/state.h"

/* Draws the static content such as name of the month and week days */
static void _draw_static(DateTime* now)
{
    char title[12];
    snprintf(title, 12, "%3s %4d", DATETIME_MONTH(now->month), now->year);
    strcenter(title, strnlen(title, 12), 12);

    Paint_DrawString_EN(54, 60, title, &Font16, COLOR_BG, COLOR_FG);
    Paint_DrawString_EN(25, 80, "Mon Tue Wed Thu Fri Sat Sun", &Font12,
                        COLOR_BG, COLOR_FG);
    Paint_DrawRectangle(20, 95, 228, 170, COLOR_FG, DOT_PIXEL_1X1,
                        DRAW_FILL_EMPTY);
}

/* Draw the days of the month to the screen */
static void _draw_days(DateTime* now, int number_of_days, const int dow_begin)
{
    int j = 100;
    char str[4];
    for (int i = dow_begin; i < number_of_days + dow_begin; i++) {
        snprintf(str, 4, " %02d", i - dow_begin + 1);
        int fg_color = COLOR_FG;
        int bg_color = COLOR_BG;
        if (i % 7 == 6 || i % 7 == 0) {
            if (i - dow_begin + 1 == now->day) {
                bg_color = YELLOW;
            } else {
                bg_color = GBLUE;
            }
            fg_color = BLACK;
        } else if (i - dow_begin + 1 == now->day) {
            fg_color = YELLOW;
        }
        Paint_DrawString_EN(25 + (i - 1) % 7 * 28, j, str, &Font12, bg_color,
                            fg_color);
        if (i % 7 == 0) { j += 14; }
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
            DateTime beginning = {.day = 1,
                                  .month = now.month,
                                  .year = now.year};
            int dow_begin = dt_get_day_int(&beginning);

            _draw_static(&now);
            _draw_days(&now, number_of_days, dow_begin);
            if (screen.redraw) {
                apps_post_process(false);
                LCD_1IN28_Display(screen.buffer);
                screen.redraw = DISP_SYNC;
            }
        }
    }
}
