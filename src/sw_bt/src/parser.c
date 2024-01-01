#include "sw_bt/bt.h"
#include "sw_common/types.h"
#include "sw_common/util.h"
#include "sw_os/state.h"
#include <stdlib.h>

/******************************************************************************
 ┌────────────────┐                                     ┌───────────┐
 │                │─────────── Connect&DateTime ───────>│           │
 │                │<───────────────── OK ───────────────│           │
 │                │───────────────── Ping ─────────────>│           │
 │ Android Client │<─────────────── Events ─────────────│ SW Server │
 │                │───────────────── Data ─────────────>│           │
 │                │──────────────── Notify ────────────>│           │
 │                │<───────────── NotifiyResp ──────────│           │
 └────────────────┘                                     └───────────┘

 [MAGIC][REQUEST_TYPE]PAYLOAD
 All request/responses start with a magic number to validate, followed
 by a type.
 

*****************************************************************************/

#define BT_MAGIC (27 ^ 91) /* 64 */

/**
 * Parses and splits the incoming request into tokens up to the size of the
 * given array's size. Updates the size to match the actual number of tokens.
 * @str - incoming data
 * @str_s - size of the data
 * @arr - array to insert into
 * @array_s - size of the array
 * @returns - error code
 */
static enum bt_fmt_t _decode_req(char* str, size_t str_s, char** arr,
                                 int* arr_s);

/**
 * Parses and validates the incoming request. Sets the request type,
 * @arr - array of tokens
 * @array_s - size of the array
 * @req - request to set
 * @returns - error code
 */
enum bt_fmt_t _parse_req(char** arr, int arr_s, enum bt_req_t* req);

/**
 * Executes the function corresponding to the request type
 * @arr - array of tokens
 * @arr_s - size of the array
 * @req - request type to select
 * @returns - error code
 */
static enum bt_fmt_t _route_req(char** arr, int arr_s,
                                enum bt_req_t request_type);

enum bt_fmt_t bt_handle_req(char* str, size_t str_s)
{
    char* str_arr[5] = {0};
    enum bt_fmt_t err;
    int size = 5;
    enum bt_req_t request_t;

    err = _decode_req(str, str_s, str_arr, &size);
    if (!err) return err;

    err = _parse_req(str_arr, size, &request_t);
    if (!err) return err;

    return _route_req(str_arr, size, request_t);
}

static enum bt_fmt_t _decode_req(char* str, size_t str_s, char** arr,
                                 int* arr_s)
{
    int str_arr_i = 0;
    size_t i = 0;
    size_t len = strnlen(str, str_s);
    char* old_str = str;
    while (i < len && str_arr_i < *arr_s) {
        if (str[i] == '|') {
            str[i] = '\0';
            arr[str_arr_i] = old_str;
            old_str = &str[i + 1];
            str_arr_i++;
        }
        i++;
    }
    *arr_s = str_arr_i;
    return BT_FMT_OK;
}

enum bt_fmt_t _parse_req(char** arr, int arr_s, enum bt_req_t* req)
{
    if (arr_s > 5) return ERROR(BT_FMT_ERROR_INVALID_INPUT);
    char* endptr;
    long mgc = strtol(arr[0], &endptr, 10);
    if (*endptr != '\0' || mgc != BT_MAGIC) return ERROR(BT_FMT_ERROR_MAGIC);

    long request_type = strtol(arr[0], &endptr, 10);
    if (*endptr != '\0') return ERROR(BT_FMT_ERROR_REQ_TYPE);

    *req = request_type;
    return BT_FMT_OK;
}

static enum bt_fmt_t _handle_fetch_date(char** str, int str_s)
{
    if (str_s < 3) return ERROR(BT_FMT_ERROR_PAYLOAD);
    bool result = str_to_date(str[2], &state.dt);
    state.is_connected = true;
    return result ? BT_FMT_OK : ERROR(BT_FMT_ERROR_DATE_PARSE);
}

static enum bt_fmt_t _handle_call_begin(char** str, int str_s)
{
    if (str_s < 3) return ERROR(BT_FMT_ERROR_PAYLOAD);
    Popup p = (Popup){.type = POPUP_CALL, .value.caller.is_over = false};
    strncpy(p.value.caller.name, str[2], str_s);
    os_request_popup(p);
    return BT_FMT_OK;
}

static enum bt_fmt_t _handle_call_end(char** str, int str_s)
{
    if (state.popup.type == POPUP_CALL) state.popup.value.caller.is_over = true;
    return BT_FMT_OK;
}

static enum bt_fmt_t _handle_notify(char** str, int str_s)
{
    if (str_s < 4) return ERROR(BT_FMT_ERROR_PAYLOAD);
    Popup p = (Popup){.type = POPUP_NOTIFY};
    strncpy(p.value.notify.title, str[2], str_s > 15 ? 15 : str_s);
    strncpy(p.value.notify.text, str[3], str_s > 128 ? 128 : str_s);
    os_request_popup(p);
    return BT_FMT_OK;
}

static enum bt_fmt_t _handle_reminder(char** str, int str_s)
{
    if (str_s < 3) return ERROR(BT_FMT_ERROR_PAYLOAD);
    Popup p = (Popup){.type = POPUP_NOTIFY};
    strncpy(p.value.notify.title, "Reminder", strlen("Reminder"));
    strncpy(p.value.notify.text, str[2], str_s > 128 ? 128 : str_s);
    os_request_popup(p);
    return BT_FMT_OK;
}

static enum bt_fmt_t _handle_osc(char** str, int str_s)
{
    if (str_s < 6) return ERROR(BT_FMT_ERROR_PAYLOAD);
    state.media.is_playing =
        str[2][0] == 't' || str[2][0] == 'T' || str[2][0] == '1';
    strncpy(state.media.song, str[3], str_s > 30 ? 30 : str_s);
    strncpy(state.media.artist, str[4], str_s > 30 ? 30 : str_s);
    strncpy(state.media.album, str[5], str_s > 30 ? 30 : str_s);
    return BT_FMT_OK;
}

static enum bt_fmt_t _handle_fetch_alarm(char** str, int str_s)
{
    if (str_s < 5) return ERROR(BT_FMT_ERROR_PAYLOAD);
    if (str[2][0] < '1' || str[2][0] > '4')
        return ERROR(BT_FMT_ERROR_ALARM_PARSE);
    int alarm_count = str[2][0] - '0';

    state.alarms.len = alarm_count;
    for (int i = 0; i < alarm_count; i++) {
        /* is_active value is set by the watch */
        str_to_date(str[i + 2], &state.alarms.list[i].at);
    }
    return BT_FMT_OK;
}

static enum bt_fmt_t _route_req(char** arr, int arr_s,
                                enum bt_req_t request_type)
{
    switch (request_type) {
    case BT_REQ_CALL_BEGIN: return _handle_call_begin(arr, arr_s);
    case BT_REQ_CALL_END: return _handle_call_end(arr, arr_s);
    case BT_REQ_NOTIFY: return _handle_notify(arr, arr_s);
    case BT_REQ_REMINDER: return _handle_reminder(arr, arr_s);
    case BT_REQ_OSC: return _handle_osc(arr, arr_s);
    case BT_REQ_FETCH_DATE: return _handle_fetch_date(arr, arr_s);
    case BT_REQ_FETCH_ALARM: return _handle_fetch_alarm(arr, arr_s);
    default: return ERROR(BT_FMT_ERROR_REQ_TYPE);
    }
}

// TODO add requests to send to the phone
// TODO - On Change/Pause song
// TODO - On Toggle alarm result
// TODO - On Call response
// TODO - On Request to fetch date
