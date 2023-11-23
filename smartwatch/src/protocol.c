#include "protocol.h"
#include <stdlib.h>
#define DT_STRLEN 14

enum IDX {
    IDX_YEAR = 0,
    IDX_MONTH = 4,
    IDX_DAY = 6,
    IDX_HOUR = 8,
    IDX_MIN = 10,
    IDX_SEC = 12,
};

/******************************************************************************
                                 COMMON
*****************************************************************************/

/**
 * Parses the given buffer to generate the wildcard flag. Looks for '?' 
 * characters.
 * @buffer - to parse
 * @return - an int16_t wildcard value
 */
static int16_t _dt_wildcard(const char* b);
/**
 * Maps the fields of given string array to the date time.
 * @dt - to assign
 * @date_s - a string array that contains each field of the struct in a
 * different string.
 * char[6]{year[5], month[3], day[3], hour[3], minute[3], second[3]}
 * @return - whether mapping failed or not
 */
static bool _dt_map(DateTime* dt, char** str_p);

enum MSGFMT sw_str_to_date(const char buffer[15], DateTime* dt)
{
    if (buffer[DT_STRLEN] != '\0') return ERROR(MSGFMT_ERROR_DATE_LEN);
    char year[5] = {0};
    char month[3] = {0};
    char day[3] = {0};
    char hour[3] = {0};
    char min[3] = {0};
    char sec[3] = {0};
    dt->flag = _dt_wildcard(buffer);

    for (size_t i = 0; i < DT_STRLEN; i++) {
        if (i < IDX_MONTH)
            year[i - IDX_YEAR] = buffer[i];
        else if (IDX_MONTH <= i && i < IDX_DAY)
            month[i - IDX_MONTH] = buffer[i];
        else if (IDX_DAY <= i && i < IDX_HOUR)
            day[i - IDX_DAY] = buffer[i];
        else if (IDX_HOUR <= i && i < IDX_MIN)
            hour[i - IDX_HOUR] = buffer[i];
        else if (IDX_MIN <= i && i < IDX_SEC)
            min[i - IDX_MIN] = buffer[i];
        else if (IDX_SEC <= i)
            sec[i - IDX_SEC] = buffer[i];
    }
    return _dt_map(dt, (char*[]){year, month, day, hour, min, sec})
               ? MSGFMT_OK
               : ERROR(MSGFMT_ERROR_DATE_PARSE);
}

enum MSGFMT sw_date_to_str(const DateTime* dt, char buffer[15])
{
    snprintf(buffer, 14, "%04d%02d%02d%02d%02d%02d", dt->year, dt->month,
             dt->day, dt->hour, dt->minute, dt->second);
    if (dt->flag & DT_WC_YEAR) memset(buffer, 4, '?');
    if (dt->flag & DT_WC_MONTH) memset(buffer + IDX_MONTH, '?', 2);
    if (dt->flag & DT_WC_DAY) memset(buffer + IDX_DAY, '?', 2);
    if (dt->flag & DT_WC_HOUR) memset(buffer + IDX_HOUR, '?', 2);
    if (dt->flag & DT_WC_MIN) memset(buffer + IDX_MIN, '?', 2);
    if (dt->flag & DT_WC_SEC) memset(buffer + IDX_SEC, '?', 2);
    return MSGFMT_OK;
}

enum MSGFMT sw_event_new(Event* event, char* str, size_t str_s)
{
    int index = 0;
    char* c = str;
    char* oldptr = str;

    enum MSGFMT err;
    while ((c = strchr(oldptr, '|')) != NULL && index < 5
           && oldptr < str + str_s) {
        *c = 0;
        switch (index) {
        case 0:
            if (!(err = sw_str_to_date(oldptr, &event->start))) return err;
            break;
        case 1:
            if (!(err = sw_str_to_date(oldptr, &event->end))) return err;
            break;
        case 2: event->name = oldptr; break;
        case 3: event->desc = oldptr; break;
        case 4: event->location = oldptr; break;
        }
        oldptr = c + 1;
        index++;
    }
    if (index != 5) return ERROR(MSGFMT_ERROR_EVENT_PARSE);
    return MSGFMT_OK;
}

enum MSGFMT sw_param_notify_new(SwReqNotify* notify, char* str, size_t str_s)
{
    int index = 0;
    char* c = NULL;
    char* oldptr = str;
    while ((c = strchr(oldptr, '\n')) != NULL && index < 4
           && oldptr < str + str_s) {
        *c = 0;
        char* endptr;
        switch (index) {
        case 0: notify->app = oldptr; break;
        case 1: notify->title = oldptr; break;
        case 2: notify->message = oldptr; break;
        case 3:
            notify->option_s = strtoul(oldptr, &endptr, 10);
            if (*endptr != '\0') return ERROR(MSGFMT_ERROR_NOTIFY_PARSE);
        }
        oldptr = c + 1;
        index++;
    }
    if (index != 4 || notify->option_s > SW_PARAM_MAX_SIZE)
        return ERROR(MSGFMT_ERROR_NOTIFY_PARSE);
    index = 0;
    memset(notify->option, 0, SW_PARAM_MAX_SIZE);
    while ((c = strchr(oldptr, '\n')) != NULL && notify->option_s > index) {
        *c = 0;
        notify->option[index] = oldptr;
        oldptr = c + 1;
        index++;
    }
    if (notify->option_s != index) {
        free(notify->option);
        return ERROR(MSGFMT_ERROR_NOTIFY_PARSE);
    }
    return MSGFMT_OK;
}

enum MSGFMT sw_event_list_new(SwReqEvent* events, char* str, size_t str_s)
{
    char* c = NULL;
    char* oldptr = str;

    if ((c = strchr(oldptr, '\n')) == NULL || c > str + str_s)
        return ERROR(MSGFMT_ERROR_EVENTLST_PARSE);
    char* endptr;
    events->size = strtoul(oldptr, &endptr, 10);
    if (*endptr != '\0') return ERROR(MSGFMT_ERROR_EVENTLST_PARSE);
    events->list = calloc(events->size, sizeof(Event));
    int index = 0;
    while ((c = strchr(oldptr, '\n')) != NULL && events->size > index
           && c > str + str_s) {
        *c = 0;
        enum MSGFMT err =
            sw_event_new(&events->list[index], oldptr, c - oldptr);
        if (!err) {
            WARN(MSGFMT_ERROR_EVENT_PARSE);
            memset(&events->list[index], 0, sizeof(Event));
        }
        oldptr = c + 1;
        index++;
    }
    if (events->size != index) {
        free(events->list);
        return ERROR(MSGFMT_ERROR_EVENTLST_PARSE);
    }
    return MSGFMT_OK;
}

void sw_event_list_free(SwReqEvent* events)
{
    free(events->list);
    events->size = 0;
    events->list = NULL;
}

enum MSGFMT sw_bat_new(SwReqBat* bat, char* str, size_t str_s)
{
    char* c = NULL;
    char* oldptr = str;

    if ((c = strchr(oldptr, '\n')) == NULL || c > str + str_s)
        return ERROR(MSGFMT_ERROR_BAT_PARSE);
    char* endptr;
    bat->pct = strtof(oldptr, &endptr);
    if (*endptr != '\0') return ERROR(MSGFMT_ERROR_BAT_PARSE);
    bat->is_ac = !strncmp(c, "CHARGING", 8);
    return MSGFMT_OK;
}

enum MSGFMT sw_osc_new(SwReqOSC* osc, char* str, size_t str_s)
{
    int index = 0;
    char* c = NULL;
    char* oldptr = str;
    while ((c = strchr(oldptr, '\n')) != NULL && index < 3
           && oldptr < str + str_s) {
        *c = 0;
        switch (index) {
        case 0: osc->song = oldptr; break;
        case 1: osc->album = oldptr; break;
        case 2: osc->artist = oldptr; break;
        }
        oldptr = c + 1;
        index++;
    }
    if (index != 3) return ERROR(MSGFMT_ERROR_OSC_PARSE);
    return MSGFMT_OK;
}

enum MSGFMT sw_parse_request(SwRequest* req, char* msg, size_t msg_s)
{
    char* endptr = NULL;
    msg[5] = 0;
    msg[11] = 0;
    char* msg_split[] = {&msg[0], &msg[6], &msg[12]};

    uint16_t mgc = strtoul(msg_split[0], &endptr, 10);
    if (*endptr != '\0' || mgc != SW_PK_MAGIC_NUMBER)
        return ERROR(MSGFMT_ERROR_MAGIC);
    req->__magic = mgc;
    int req_type = strtoul(msg_split[1], &endptr, 10);
    if (*endptr != '\0' || req_type >= SW_REQ_SIZE)
        return ERROR(MSGFMT_ERROR_REQ_TYPE);
    req->req_t = req_type;
    req->_raw = msg_split[2];
    req->_raw_s = msg_s - 12;
    switch (req->req_t) {
    case SW_REQ_CONNECT: return sw_str_to_date(req->_raw, &req->pl.connect);
    case SW_REQ_NOTIFY:
        return sw_param_notify_new(&req->pl.notify, req->_raw, req->_raw_s);
    case SW_REQ_CALL_BEGIN:
        memcpy(req->pl.caller, req->_raw, req->_raw_s > 29 ? 29 : req->_raw_s);
        return MSGFMT_OK;
    case SW_REQ_SYNC_EVENT:
        return sw_event_list_new(&req->pl.events, req->_raw, req->_raw_s);
    case SW_REQ_SYNC_BAT:
        return sw_bat_new(&req->pl.bat, req->_raw, req->_raw_s);
    case SW_REQ_SYNC_NOTIFY:
        // TODO add protocol to send multiple notifications
    case SW_REQ_OSC: return sw_osc_new(&req->pl.osc, req->_raw, req->_raw_s);
    default: break;
    }
    return MSGFMT_OK;
}

static int16_t _dt_wildcard(const char* b)
{
    int16_t wc = 0;
    if (b[IDX_YEAR] == '?') wc |= DT_WC_YEAR;
    if (b[IDX_MONTH] == '?') wc |= DT_WC_MONTH;
    if (b[IDX_DAY] == '?') wc |= DT_WC_DAY;
    if (b[IDX_HOUR] == '?') wc |= DT_WC_HOUR;
    if (b[IDX_MIN] == '?') wc |= DT_WC_MIN;
    if (b[IDX_SEC] == '?') wc |= DT_WC_SEC;
    return wc;
}

static bool _dt_map(DateTime* dt, char** str_p)
{
    char* endptr = NULL;
    if (!(DT_WC_YEAR & dt->flag)) {
        dt->year = strtoul(str_p[0], &endptr, 10);
        if (*endptr != '\0' || dt->year == 0 || dt->year > 9999) return false;
    }
    if (!(DT_WC_MONTH & dt->flag)) {
        dt->month = strtoul(str_p[1], &endptr, 10);
        if (*endptr != '\0' || dt->month == 0 || dt->month > 12) return false;
    }
    if (!(DT_WC_DAY & dt->flag)) {
        dt->day = strtoul(str_p[2], &endptr, 10);
        if (*endptr != '\0' || dt->day == 0 || dt->day > 30) return false;
    }
    if (!(DT_WC_HOUR & dt->flag)) {
        dt->hour = strtoul(str_p[3], &endptr, 10);
        if (*endptr != '\0' || dt->hour > 24)// danger
            return false;
    }
    if (!(DT_WC_MIN & dt->flag)) {
        dt->minute = strtoul(str_p[4], &endptr, 10);
        if (*endptr != '\0' || dt->minute > 60)// danger
            return false;
    }
    if (!(DT_WC_SEC & dt->flag)) {
        dt->second = strtoul(str_p[5], &endptr, 10);
        if (*endptr != '\0' || dt->second > 60)// danger
            return false;
    }
    return true;
}
