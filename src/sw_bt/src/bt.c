#include "sw_bt/bt.h"
#include <hardware/uart.h>
#include <pico/stdlib.h>

typedef struct __BT_FD {
    uart_inst_t* id;
    int baud_rate;
    int tx_pin;
    int rx_pin;
    bool is_enabled;
    char packet[240];// max size of a Bluetooth payload
    bool packet_lock;
} BtFd;

BtFd bt = {0};

void bt_init(void)
{
    bt = (BtFd){
        .id = uart0,
        .baud_rate = 9600,
        .tx_pin = 0,
        /* We are using pins 0 and 1, but see the GPIO function select table in the
         * data-sheet for information on which other pins can be used. */
        .rx_pin = 1,
        .is_enabled = true,
        .packet_lock = false,
    };
    uart_init(bt.id, bt.baud_rate);
    gpio_set_function(bt.tx_pin, GPIO_FUNC_UART);
    gpio_set_function(bt.rx_pin, GPIO_FUNC_UART);
}

void bt_receive_req()
{
    if (bt.packet_lock) {
        WARN(READ_PACKET_LOCK);
        return;
    }

    bt.packet_lock = true;
    size_t bytes = bt_read(bt.packet, 240);
    if (bytes > 0) bt_handle_req(bt.packet, bytes);
    bt.packet_lock = false;
}

bool bt_send_resp(enum bt_resp_t response)
{
    char err_str[10];
    if (response != BT_RESP_STEP)
        snprintf(err_str, 9, "%d|", response);
    else
        snprintf(err_str, 9, "%d|%d|", response, state.step);
    return bt_write(err_str, 10) > 0;
}

size_t bt_read(char* str, size_t str_s)
{
    //TODO Handle polling for data over 32 bytes
    // respond the code
    if (!bt_is_readable()) return 0;
    memset(str, '\0', str_s);
    uint i = 0;
    size_t bytes_left;
    while ((bytes_left = uart_is_readable(bt.id)) > 0 && i < str_s)
        str[i++] = uart_getc(bt.id);
    PRINT("rec  [%s]", , str);
    if (i > 0) state.__last_connected = get_absolute_time();
    return i;
}

size_t bt_write(char* str, size_t str_s)
{
    if (!bt_is_writable()) return 0;
    uint i = 0;
    size_t bytes_left;
    while ((bytes_left = uart_is_writable(bt.id)) > 0 && i < str_s)
        uart_putc(bt.id, str[i++]);
    PRINT("sent [%s]", , str);
    return i;
}

bool bt_is_readable() { return uart_is_readable(bt.id); }
bool bt_is_writable() { return uart_is_writable(bt.id); }
