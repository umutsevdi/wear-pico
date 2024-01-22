#include "sw_bt/bt.h"
#include "sw_os/state.h"
#include <hardware/irq.h>
#include <hardware/regs/intctrl.h>
#include <hardware/uart.h>
#include <pico/stdlib.h>
#define POLLING_ON 0
#define POLLING_OFF 10

typedef struct __BT_FD {
    uart_inst_t* id;
    int baud_rate;
    int tx_pin;
    int rx_pin;
    bool is_enabled;
    char packet[240];// max size of a Bluetooth payload
    uint cursor;
} BtFd;

BtFd bt = {0};

static inline bool is_str_complete()
{
    if (bt.cursor < 4) { return false; }
    return bt.packet[bt.cursor] == '\0' && bt.packet[bt.cursor - 1] == '\n'
           && bt.packet[bt.cursor - 2] == '\r'
           && bt.packet[bt.cursor - 3] == '|';
}

void on_uart_rx()
{
    if (uart_is_readable(bt.id) > 0 && bt.cursor < 240) {
        bt.packet[bt.cursor++] = uart_getc(bt.id);
    }
    if (is_str_complete()) {
        PRINT("RECEIVED [%s]%u", , bt.packet, bt.cursor);
        bt_handle_req(bt.packet, bt.cursor - 2);
        memset(bt.packet, '\0', 240);
        bt.cursor = 0;
        state.__last_connected = get_absolute_time();
    }
}

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
    };
    uart_init(bt.id, bt.baud_rate);
    gpio_set_function(bt.tx_pin, GPIO_FUNC_UART);
    gpio_set_function(bt.rx_pin, GPIO_FUNC_UART);

    irq_set_exclusive_handler(UART0_IRQ, on_uart_rx);
    irq_set_enabled(UART0_IRQ, true);
    uart_set_irq_enables(bt.id, true, false);
}

bool bt_send_resp(enum bt_resp_t response)
{
    char err_str[10];
    if (response != BT_RESP_STEP) {
        snprintf(err_str, 9, "%d|", response);
    } else {
        snprintf(err_str, 9, "%d|%d|", response, state.dev.step);
    }
    return bt_write(err_str, 10) > 0;
}

size_t bt_write(char* str, size_t str_s)
{
    if (!bt_is_writable()) { return 0; }
    size_t bytes = strnlen(str, str_s);
    PRINT(Start sending);
    uart_write_blocking(bt.id, (uint8_t*)str, bytes);
    PRINT("sent [%s]", , str);
    return bytes;
}

bool bt_is_readable() { return uart_is_readable(bt.id); }
bool bt_is_writable() { return uart_is_writable(bt.id); }
