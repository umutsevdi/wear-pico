/******************************************************************************

 * File: sw_bt/bt.h
 *
 * Author: Umut Sevdi
 * Created: 12/29/23
 * Description: Bluetooth communication and protocol module

*****************************************************************************/

#ifndef SW_BT
#define SW_BT

#include "sw_common/util.h"

/* Status codes related to protocol */
enum bt_fmt_t {
    BT_FMT_OK,
    BT_FMT_ERROR_INVALID_INPUT,
    BT_FMT_ERROR_MAGIC,
    BT_FMT_ERROR_PAYLOAD,
    BT_FMT_ERROR_DATE_LEN,
    BT_FMT_ERROR_ALARM_PARSE,
    BT_FMT_ERROR_DATE_PARSE,
    BT_FMT_ERROR_REQ_TYPE,
    BT_FMT_ERROR_EVENT_PARSE,
    BT_FMT_ERROR_EVENT_LIST_PARSE,
    BT_FMT_ERROR_NOTIFY_PARSE,
    BT_FMT_ERROR_BAT_PARSE,
    BT_FMT_ERROR_OSC_PARSE,
};

/* Error types for the applications */
enum bt_status_t {
    BT_OK,
    BT_ERROR_BROKEN_CONNECTION,
};

/* Messages that could be sent by the phone */
enum bt_req_t {
    BT_REQ_CALL_BEGIN, /* Call start event. Payload: "Caller" */
    BT_REQ_CALL_END,   /* Occurs when the Call event is handled by the phone. */
    BT_REQ_NOTIFY,     /* Notification event. Triggers an interrupt.
                        * Payload: Title|Message */
    BT_REQ_REMINDER,
    BT_REQ_OSC,        /* On Song Change Event. Payload: "Song|Album|Artist" */
    BT_REQ_FETCH_DATE, /* Update request. Payload: "{DateTime}" */
    BT_REQ_FETCH_ALARM,
    BT_REQ_STEP,
    BT_REQ_HB,     /* Heartbeat request to show Pico connection is alive */
    BT_REQ_CONFIG, /* Receives the configuration data*/

    BT_REQ_SIZE
};

enum bt_resp_t {
    BT_RESP_OK,
    BT_RESP_ERR,
    BT_RESP_CALL_OK,
    BT_RESP_CALL_CANCEL,
    BT_RESP_OSC_PREV,
    BT_RESP_OSC_PLAY_PAUSE,
    BT_RESP_OSC_NEXT,
    BT_RESP_STEP,
    BT_RESP_SIZE
};

/* Initializes the Bluetooth connection */
void bt_init(void);

bool bt_send_resp(enum bt_resp_t response);

/* Write up to str_s bytes to the Bluetooth connection */
size_t bt_write(char* str, size_t str_s);

bool bt_is_readable();
bool bt_is_writable();

/**
 * Parses and handles the incoming request.
 * @str - string to parse
 * @str_s - size of the string
 * @returns - error code
 */
enum bt_fmt_t bt_handle_req(char* str, size_t str_s);

#endif
