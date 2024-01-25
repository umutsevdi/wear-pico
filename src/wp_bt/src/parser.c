#include "wp_bt/bt.h"
#include "wp_common/util.h"
#include "wp_dev/state.h"

/**
 * Parses and splits the incoming request into tokens up to the size of the
 * given array's size. Updates the size to match the actual number of tokens.
 * @str - incoming data
 * @str_s - size of the data
 * @arr - array to insert into
 * @array_s - size of the array
 * @returns - error code
 */
static enum bt_fmt_t _req_decode(char* str, size_t str_s, String* arr,
                                 int* arr_s)
{
    int str_arr_i = 0;
    size_t i = 0;
    size_t len = strnlen(str, str_s);
    char* old_str = str;
    while (i < len && str_arr_i < *arr_s) {
        if (str[i] == '|') {
            str[i] = '\0';
            arr[str_arr_i].bytes = old_str;
            arr[str_arr_i].len = str + i - old_str;
            old_str = &str[i + 1];
            str_arr_i++;
        }
        i++;
    }
    if (i < str_s) { str[i] = '\0'; }
    if (str_arr_i == 0) { return ERROR(BT_FMT_ERROR_INVALID_INPUT); }
    *arr_s = str_arr_i;
    return BT_FMT_OK;
}

/**
 * Parses and validates the incoming request. Sets the request type,
 * @arr - array of tokens
 * @req - request to set
 * @returns - error code
 */
static enum bt_fmt_t _req_parse(String* arr, enum bt_req_t* req)
{
    char* endptr;
    long request_type = strtol(arr[0].bytes, &endptr, 10);
    if (*endptr != '\0') { return ERROR(BT_FMT_ERROR_REQ_TYPE); }

    *req = request_type;
    return BT_FMT_OK;
}

/* Routing functions */
static enum bt_fmt_t _handle_fetch_date(String* str, int str_s);
static enum bt_fmt_t _handle_call_begin(String* str, int str_s);
static enum bt_fmt_t _handle_call_end();
static enum bt_fmt_t _handle_notify(String* str, int str_s);
static enum bt_fmt_t _handle_osc(String* str, int str_s);
static enum bt_fmt_t _handle_fetch_alarm(String* str, int str_s);
static enum bt_fmt_t _handle_reminder(String* str, int str_s);
static enum bt_fmt_t _handle_step(int str_s);
static enum bt_fmt_t _handle_hb(int str_s);
static enum bt_fmt_t _handle_config(String* str, int str_s);

/**
 * Executes the function corresponding to the request type
 * @arr - array of tokens
 * @arr_s - size of the array
 * @req - request type to select
 * @returns - error code
 */
static enum bt_fmt_t _req_route(String* arr, int arr_s,
                                enum bt_req_t request_type)
{
    switch (request_type) {
    case BT_REQ_CALL_BEGIN: return _handle_call_begin(arr, arr_s);
    case BT_REQ_CALL_END: return _handle_call_end();
    case BT_REQ_NOTIFY: return _handle_notify(arr, arr_s);
    case BT_REQ_REMINDER: return _handle_reminder(arr, arr_s);
    case BT_REQ_OSC: return _handle_osc(arr, arr_s);
    case BT_REQ_FETCH_DATE: return _handle_fetch_date(arr, arr_s);
    case BT_REQ_FETCH_ALARM: return _handle_fetch_alarm(arr, arr_s);
    case BT_REQ_STEP: return _handle_step(arr_s);
    case BT_REQ_HB: return _handle_hb(arr_s);
    case BT_REQ_CONFIG: return _handle_config(arr, arr_s);
    default: return ERROR(BT_FMT_ERROR_REQ_TYPE);
    }
}

enum bt_fmt_t bt_handle_req(char* str, size_t str_s)
{
    String str_arr[10] = {0};
    enum bt_fmt_t err;
    int size = 10;
    enum bt_req_t request_t;

    err = _req_decode(str, str_s, str_arr, &size);
    if (err) { return err; }

    err = _req_parse(str_arr, &request_t);
    if (err) { return err; }

    return _req_route(str_arr, size, request_t);
}

static enum bt_fmt_t _handle_fetch_date(String* str, int str_s)
{
    if (str_s < 2) { return ERROR(BT_FMT_ERROR_PAYLOAD); }
    if (!dt_decode(str[1].bytes, &state.dt)) {
        PRINT("fetch_date(%s)", , str[1].bytes);
        return BT_FMT_OK;
    }
    return ERROR(BT_FMT_ERROR_DATE_PARSE);
}

static enum bt_fmt_t _handle_call_begin(String* str, int str_s)
{
    if (str_s < 2) { return ERROR(BT_FMT_ERROR_PAYLOAD); }
    Popup p = (Popup){.type = POPUP_CALL, .value.caller.is_over = false};
    strncpy(p.value.caller.name, str[1].bytes, MIN(str[1].len, 24));
    os_request_popup(p);
    PRINT("received_call(%s)", , str[1].bytes);
    return BT_FMT_OK;
}

static enum bt_fmt_t _handle_call_end()
{
    if (state.popup.type == POPUP_CALL) {
        state.popup.value.caller.is_over = true;
    }
    PRINT("ended_call()");
    return BT_FMT_OK;
}

static enum bt_fmt_t _handle_notify(String* str, int str_s)
{
    if (str_s < 3) { return ERROR(BT_FMT_ERROR_PAYLOAD); }
    Popup p = (Popup){.type = POPUP_NOTIFY};
    strncpy(p.value.notify.title, str[1].bytes, MIN(str[1].len, 12));
    strncpy(p.value.notify.text, str[2].bytes, MIN(str[2].len, 127));
    os_request_popup(p);
    PRINT("notify(%s, %s)", , p.value.notify.title, p.value.notify.text);
    return BT_FMT_OK;
}

static enum bt_fmt_t _handle_osc(String* str, int str_s)
{
    if (str_s < 4) return ERROR(BT_FMT_ERROR_PAYLOAD);
    state.media.is_playing = str[1].bytes[0] == 't';

    strncpy(state.media.song, str[2].bytes, MIN(str[2].len, 30));
    strncpy(state.media.artist, str[3].bytes, MIN(str[3].len, 30));
    state.media.is_fetched = false;
    PRINT("media(%s, %s)", , state.media.song, state.media.artist);
    return BT_FMT_OK;
}

static void _str_to_alarm(const char* str, DateTime* dt)
{
    int digits[5];
    for (int i = 0; i < 4; i++) {
        if (str[i] < '0' || str[i] > '9') return;
        digits[i] = str[i] - '0';
    }
    int hour = digits[0] * 10 + digits[1];
    int min = digits[2] * 10 + digits[3];
    if (hour > 23 || min > 59) return;
    dt->hour = hour;
    dt->minute = min;
}

static enum bt_fmt_t _handle_fetch_alarm(String* str, int str_s)
{
    AlarmList* alarms = &state.alarms;
    if (str_s < 2) { return ERROR(BT_FMT_ERROR_PAYLOAD); }
    int alarm_count = str[1].bytes[0] - '0';
    if (alarm_count < 0 || alarm_count > 4) {
        return ERROR(BT_FMT_ERROR_ALARM_PARSE);
    } else if (alarm_count + 2 > str_s) {
        return ERROR(BT_FMT_ERROR_INVALID_INPUT);
    }
    for (int i = 0; i < alarm_count; i++) {
        _str_to_alarm(str[i + 2].bytes, &alarms->list[i].at);
        PRINT("alarm_%d(%d:%d)", , i, alarms->list[i].at.hour,
              alarms->list[i].at.minute);
    }
    for (int i = alarm_count; i < 4; i++) {
        alarms->list[i].is_active = true;
    }
    alarms->len = alarm_count;
    alarms->is_fetched = false;
    PRINT(FETCH_ALARM_BT);
    return BT_FMT_OK;
}

static enum bt_fmt_t _handle_reminder(String* str, int str_s)
{

    EventList* events = &state.events;
    if (str_s < 2) { return ERROR(BT_FMT_ERROR_PAYLOAD); }
    int event_count = str[1].bytes[0] - '0';
    if (event_count < 0 || event_count > 4) {
        return ERROR(BT_FMT_ERROR_EVENT_LIST_PARSE);
    } else if (event_count * 2 + 2 > str_s) {
        return ERROR(BT_FMT_ERROR_INVALID_INPUT);
    }

    int event_idx = 0;
    for (int i = 2; i < str_s; i++) {
        if (i % 2 == 0) {
            strncpy(events->list[event_idx].title, str[i].bytes,
                    MIN(str[i].len + 1, 30));
        } else {
            if (!dt_decode(str[i].bytes, &events->list[event_idx].at)) {
                events->len = event_idx;
                return ERROR(BT_FMT_ERROR_EVENT_PARSE);
            }
            event_idx++;
        }
    }
    events->len = event_count;
    events->is_fetched = false;
    PRINT(FETCH_REMINDER_BT);
    return BT_FMT_OK;
}

static enum bt_fmt_t _handle_step(int str_s)
{
    if (str_s < 1) { return ERROR(BT_FMT_ERROR_PAYLOAD); }
    bt_send_resp(BT_RESP_STEP);
    PRINT("step()");
    return BT_FMT_OK;
}

static enum bt_fmt_t _handle_hb(int str_s)
{
    if (str_s < 1) { return ERROR(BT_FMT_ERROR_PAYLOAD); }
    bt_send_resp(BT_RESP_OK);
    PRINT("heartbeat()");
    return BT_FMT_OK;
}

extern void DEV_SET_PWM(uint8_t);
static enum bt_fmt_t _handle_config(String* str, int str_s)
{
    if (str_s < 6) { return ERROR(BT_FMT_ERROR_PAYLOAD); }
    char* endptr;
    state.config.brightness = strtol(str[1].bytes, &endptr, 10);
    if (*endptr != '\0') { return ERROR(BT_FMT_ERROR_INVALID_INPUT); }
    state.config.alarm = strtol(str[2].bytes, &endptr, 10);
    if (*endptr != '\0') { return ERROR(BT_FMT_ERROR_INVALID_INPUT); }
    state.config.call = strtol(str[3].bytes, &endptr, 10);
    if (*endptr != '\0') { return ERROR(BT_FMT_ERROR_INVALID_INPUT); }
    state.config.notify = strtol(str[4].bytes, &endptr, 10);
    if (*endptr != '\0') { return ERROR(BT_FMT_ERROR_INVALID_INPUT); }
    state.config.reminder = strtol(str[5].bytes, &endptr, 10);
    if (*endptr != '\0') { return ERROR(BT_FMT_ERROR_INVALID_INPUT); }
    DEV_SET_PWM(state.config.brightness);
    return BT_FMT_OK;
}
