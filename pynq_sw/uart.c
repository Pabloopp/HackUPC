#include "xparameters.h"
#include "xuartps.h"

void init_uart(XUartPs *uart_instance)
{
    XUartPs_Config *uart_config = XUartPs_LookupConfig(XPAR_UART0_BASEADDR);
    XUartPs_CfgInitialize(uart_instance, uart_config, uart_config ->BaseAddress);
    //TODO set higher baudrate? currently default config
}

void get_image(XUartPs *uart, uint8_t *buff)
{
    uint8_t r_byte = 0;
    uint8_t r_byte_prev = 0;
    uint64_t i = 2;
    while (r_byte != 0xD8 && r_byte_prev != 0xFF)
    {
        r_byte_prev = r_byte;
        r_byte = XUartPs_RecvByte(uart->Config.BaseAddress);
    }
    buff[0] = 0xFF;
    buff[1] = 0xD8;
    while (r_byte != 0xD9 && r_byte_prev != 0xFF)
    {
        r_byte_prev = r_byte;
        r_byte = XUartPs_RecvByte(uart->Config.BaseAddress);
        buff[i] = r_byte;
        i++;
    }
}

/**
 * Function to send a frame over UART.
 * @param uart: Pointer to the XUartPs instance.
 * @param frame_buffer: Pointer to the frame buffer to be sent.
 */
void send_frame(XUartPs *uart, const u8 *frame_buffer){
    u32 total_sent = 0;
    u32 length = sizeof(frame_buffer);
    while (total_sent < length) {
        u32 bytes_sent = XUartPs_Send(uart, frame_buffer + total_sent, length - total_sent);
        total_sent += bytes_sent;
    }
}
