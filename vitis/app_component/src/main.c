
#include "xparameters.h"
#include <stdint.h>
#include <string.h>
#include "xuartps.h"
#include "uart.h"


void init_uart(XUartPs *uart_instance);
uint32_t get_image(XUartPs *uart, uint8_t *buff);
void send_frame(XUartPs *uart, const u8 *frame_buffer, u32 length);

int main () {
    volatile uint32_t *LED = (uint32_t *) XPAR_XGPIO_0_BASEADDR; //Debug led
    *LED = 1;
    XUartPs uart0;
    uint8_t tx_buffer[RX_BUFF_SIZE];
    uint8_t rx_buffer[TX_BUFF_SIZE];
    //uint8_t tmp_dbg[2];
    uint32_t image_size = 0;
    init_uart(&uart0);
    while (1)
    {
        memset(rx_buffer, 0, RX_BUFF_SIZE);//Clear rx buffer
        image_size = get_image(&uart0, rx_buffer);
        memcpy(tx_buffer, rx_buffer, TX_BUFF_SIZE);
        
        //tmp_dbg[0] = tx_buffer[image_size - 2];
        //tmp_dbg[1] = tx_buffer[image_size - 1];
        send_frame(&uart0, tx_buffer, image_size);
    }
    return 1;
}

void init_uart(XUartPs *uart_instance)
{
    XUartPs_Config *uart_config = XUartPs_LookupConfig(XPAR_UART0_BASEADDR);
    XUartPs_CfgInitialize(uart_instance, uart_config, uart_config ->BaseAddress);
    //TODO set higher baudrate? currently default config
}

uint32_t get_image(XUartPs *uart, uint8_t *buff)
{
    uint8_t partial_bytes[4];
    uint64_t i = 4;
    while (!(partial_bytes[0] == 0xD8 && 
             partial_bytes[1] == 0xFF && 
             partial_bytes[2] == 0xD8 && 
             partial_bytes[3] == 0xFF ))
    {
        partial_bytes[3] = partial_bytes[2];
        partial_bytes[2] = partial_bytes[1];
        partial_bytes[1] = partial_bytes[0];
        partial_bytes[0] = (uint8_t) XUartPs_RecvByte(uart->Config.BaseAddress);
    }
    buff[0] = 0xD8;
    buff[1] = 0xFF;
    buff[2] = 0xD8;
    buff[3] = 0xFF;
    while (!(partial_bytes[0] == 0xD9 && 
             partial_bytes[1] == 0xFF && 
             partial_bytes[2] == 0xD9 && 
             partial_bytes[3] == 0xFF))
    {
        partial_bytes[3] = partial_bytes[2];
        partial_bytes[2] = partial_bytes[1];
        partial_bytes[1] = partial_bytes[0];
        partial_bytes[0] = (uint8_t) XUartPs_RecvByte(uart->Config.BaseAddress);
        buff[i] = partial_bytes[0];
        i++;
    }
    return i;
}
/*
/**
 * Function to send a frame over UART.
 * @param uart: Pointer to the XUartPs instance.
 * @param frame_buffer: Pointer to the frame buffer to be sent.
 */
void send_frame(XUartPs *uart, const u8 *frame_buffer, u32 length){
    u32 total_sent = 0;
    while (total_sent < length) {
        u32 bytes_sent = XUartPs_Send(uart, frame_buffer + total_sent, length - total_sent);
        total_sent += bytes_sent;
    }

}
