#include "msg.h"
#define DT_STRLEN 14

enum IDX {
    IDX_YEAR = 0,
    IDX_MONTH = 4,
    IDX_DAY = 6,
    IDX_HOUR = 8,
    IDX_MIN = 10,
    IDX_SEC = 12,
};

static int16_t _dt_wildcard(const char* buffer);
static bool _dt_map(DateTime* dt, char** date_s);

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
    req->_payload = msg_split[2];
    req->_payload_s = msg_s - 12;
    return MSGFMT_OK;
}

enum MSGFMT sw_date_parse(const char* buffer, DateTime* dt)
{
    if (buffer[DT_STRLEN] != '\0') {
        return ERROR(MSGFMT_ERROR_DATE_LEN);
    }
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
    if (dt->flag & DT_WC_YEAR) {
        memset(buffer, 4, '?');
    }
    if (dt->flag & DT_WC_MONTH) {
        memset(buffer + 4, '?', 2);
    }
    if (dt->flag & DT_WC_DAY) {
        memset(buffer + 6, '?', 2);
    }
    if (dt->flag & DT_WC_HOUR) {
        memset(buffer + 8, '?', 2);
    }
    if (dt->flag & DT_WC_MIN) {
        memset(buffer + 10, '?', 2);
    }
    if (dt->flag & DT_WC_SEC) {
        memset(buffer + 12, '?', 2);
    }
    return MSGFMT_OK;
}

static int16_t _dt_wildcard(const char* buffer)
{
    int16_t wc = 0;
    if (buffer[IDX_YEAR] == '?')
        wc |= DT_WC_YEAR;
    if (buffer[IDX_MONTH] == '?')
        wc |= DT_WC_MONTH;
    if (buffer[IDX_DAY] == '?')
        wc |= DT_WC_DAY;
    if (buffer[IDX_HOUR] == '?')
        wc |= DT_WC_HOUR;
    if (buffer[IDX_MIN] == '?')
        wc |= DT_WC_MIN;
    if (buffer[IDX_SEC] == '?')
        wc |= DT_WC_SEC;
    return wc;
}

static bool _dt_map(DateTime* dt, char** date_str)
{
    char* endptr = NULL;
    if (!(DT_WC_YEAR & dt->flag)) {
        dt->year = strtoul(date_str[0], &endptr, 10);
        if (*endptr != '\0' || dt->year == 0 || dt->year > 9999)
            return false;
    }
    if (!(DT_WC_MONTH & dt->flag)) {
        dt->month = strtoul(date_str[1], &endptr, 10);
        if (*endptr != '\0' || dt->month == 0 || dt->month > 12)
            return false;
    }
    if (!(DT_WC_DAY & dt->flag)) {
        dt->day = strtoul(date_str[2], &endptr, 10);
        if (*endptr != '\0' || dt->day == 0 || dt->day > 30)
            return false;
    }
    if (!(DT_WC_HOUR & dt->flag)) {
        dt->hour = strtoul(date_str[3], &endptr, 10);
        if (*endptr != '\0' || dt->hour > 24)// danger
            return false;
    }
    if (!(DT_WC_MIN & dt->flag)) {
        dt->minute = strtoul(date_str[4], &endptr, 10);
        if (*endptr != '\0' || dt->minute > 60)// danger
            return false;
    }
    if (!(DT_WC_SEC & dt->flag)) {
        dt->second = strtoul(date_str[5], &endptr, 10);
        if (*endptr != '\0' || dt->second > 60)// danger
            return false;
    }
    return true;
}
