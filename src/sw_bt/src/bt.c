#include "sw_bt/bt.h"
#include <hardware/uart.h>
#include <pico/stdlib.h>

#define UART_ID uart0
// #define BAUD_RATE 115200
#define BAUD_RATE 9600

/* We are using pins 0 and 1, but see the GPIO function select table in the
 * data-sheet for information on which other pins can be used. */
#define UART_TX_PIN 0
#define UART_RX_PIN 1

typedef struct __BT_FD {
    uart_inst_t* id;
    int baud_rate;
    int tx_pin;
    int rx_pin;
} BtFd;

BtFd bt;

enum bt_status_t bt_init(void)
{
    uart_init(UART_ID, BAUD_RATE);
    gpio_set_function(UART_TX_PIN, GPIO_FUNC_UART);
    gpio_set_function(UART_RX_PIN, GPIO_FUNC_UART);
    while (true) {
        uart_puts(uart0, "hello world");
        sleep_ms(1000);
    }
    return BT_OK;
}

enum bt_fmt_t bt_encode(char* str, size_t str_s) {}

bool bt_is_connected()
{
    char* req = "AT+STATE?\r\n";
    bt_write(req, strlen(req));
    char response[20];
    bt_read(response, 20);
    return (strstr(response, "CONNECTED") != NULL);
}

size_t bt_read(char* str, size_t str_s)
{
    uint i = 0;
    size_t bytes_left;
    while ((bytes_left = uart_is_readable(bt.id)) > 0 && i < str_s)
        str[i++] = uart_getc(bt.id);
    return i;
}

size_t bt_write(char* str, size_t str_s)
{
    uint i = 0;
    size_t bytes_left;
    while ((bytes_left = uart_is_writable(bt.id)) > 0 && i < str_s)
        str[i++] = uart_getc(bt.id);
    return i;
}
