#ifndef UART_H
#define UART_H

void init_uart(XUartPs *uart_instance);
void get_image(XUartPs *uart, uint8_t *buff);
void send_frame(XUartPs *uart, const u8 *frame_buffer);
#endif