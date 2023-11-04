/******************************************************************************

 * File: include/msg.h
 *
 * Author: Umut Sevdi
 * Created: 10/31/23
 * Description: Messaging protocol definition.
 ┌────────────────┐                                     ┌───────────┐
 │                │─────────── Connect&DateTime ───────>│           │
 │                │<───────────────── OK ───────────────│           │
 │                │───────────────── Ping ─────────────>│           │
 │ Android Client │<─────────────── Events ─────────────│ SW Server │
 │                │───────────────── Data ─────────────>│           │
 │                │──────────────── Notify ────────────>│           │
 │                │<───────────── NotifiyResp ──────────│           │
 └────────────────┘                                     └───────────┘

*****************************************************************************/

#pragma once
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

/**
 * Common types for request and response type enums:
 * # DateTime: Describes the optional date and/or time values. YYYYmmDDHHMMSS
 * Unknown fields are replaced with '?'. 
 *   Example:
 *   - 202112201830??  20 December 2012 18:30 
 *   - ????????123015  12:30:15 
 * # Option: Used on custom buttons. Payload is separated by '|' symbol.
 *   Example:
 *   - Accept|Deny
 *   - Previous|Stop|Next
 *
 */
typedef enum {
    SW_REQ_CONNECT, /* Connection request. Payload: "{DateTime}" */
    SW_REQ_DISCONNECT,
    SW_REQ_NOTIFY,     /* Notification event. Triggers an interrupt.
                        * Payload: App Name\nTitle\nMessage\n{Option} */
    SW_REQ_CALL_BEGIN, /* Call start event. Payload: "Caller" */
    SW_REQ_CALL_EXIT,  /* Occurs when the Call event is handled by the phone. */
    SW_REQ_PING, /* Client periodically asks to server, requested data. Every sec */
    SW_REQ_SYNC_EVENT,  /* Calendar events info. Sends a list of events.
                        * Payload: "Name|{DateTime}|{DateTime}|Desc|Location"*/
    SW_REQ_SYNC_BAT,    /* Sends phone's battery information. 
                        * Payload: "Pct\nStatus" */
    SW_REQ_SYNC_NOTIFY, /* Sends all notifications exist on Android lock screen.
                         * Payload: "Title\nMessage\n{Option}"*/
    SW_REQ_MDI_OSC, /* On Song Change Event. Payload: "Song\nAlbum\nArtist" */
} SW_REQ_TYPE;

typedef enum {
    SW_RESP_NOTIFY,       /* Notification response. Payload: {Index} */
    SW_RESP_CALL_ACCEPT,  /* Accept the call */
    SW_RESP_CALL_DISMISS, /* Reject the call */
    SW_RESP_SYNC,         /* Ask for sync. Payload: EVENT|BAT|NOTIFY|*/
    SW_RESP_MDI_NEXT,     /* Skip to the next song */
    SW_RESP_MDI_PLYPS,    /* Play or pause */
    SW_RESP_MDI_PREV,     /* Skip to the previous song */
} SW_RESP_TYPE;

const uint16_t SW_PK_MAGIC_NUMBER = 2791;

typedef struct {
    uint64_t __magic;
    SW_REQ_TYPE request;
    char* buffer;
    size_t buffer_s;
} SwPacketRequest;

typedef struct {
    uint64_t __magic;
    SW_RESP_TYPE request;
    char* buffer;
    size_t buffer_s;
} SwPacketResponse;

typedef struct {
    int16_t flag;
    uint16_t year;
    uint8_t month;
    uint8_t day;
    uint8_t hour;
    uint8_t minute;
    uint8_t second;
} DateTime;

typedef enum {
    DT_WC_YEAR = 0b000001,
    DT_WC_MONTH = 0b000010,
    DT_WC_DAY = 0b000100,
    DT_WC_HOUR = 0b001000,
    DT_WC_MIN = 0b010000,
    DT_WC_SEC = 0b100000,
} DT_WC;

bool to_date(DateTime* datetime, const char* buffer, size_t buffer_s);
