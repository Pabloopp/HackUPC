#include "xuartps.h"
#include "xparameters.h"

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
