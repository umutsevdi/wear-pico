#include "wp_apps/apps.h"
#include "wp_dev/state.h"

/* Displays the current step amount */
static void _step_display()
{
    int base_x = 40;
    int base_y = 86;
    char numstr[6] = {0};
    const int x_size = 40;
    const int y_size = 40;
    snprintf(numstr, 6, "%d", state.dev.step);
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
