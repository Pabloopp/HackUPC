#include "xparameters.h"
#include <stdint.h>
#include <string.h>
#include "xuartps.h"
#include "uart.h"
int main () {
    volatile uint32_t *LED = (uint32_t *) XPAR_XGPIO_0_BASEADDR; //Debug led
    *LED = 1;
    XUartPs *uart0;
    uint8_t tx_buffer[RX_BUFF_SIZE];
    uint8_t rx_buffer[TX_BUFF_SIZE];
    uint32_t image_size = 0;
    init_uart(uart0);
    while (1)
    {
        memset(rx_buffer, 0, sizeof(rx_buffer));//Clear rx buffer
        image_size = get_image(uart0, rx_buffer);
        memcpy(tx_buffer, rx_buffer, sizeof(rx_buffer));
        send_frame(uart0, tx_buffer, image_size);
    }
    return 1;
}

