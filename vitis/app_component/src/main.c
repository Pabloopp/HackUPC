#include "xparameters.h"
#include <stdint.h>
#include <string.h>
#include "xuartps.h"
#include "uart.h"

#define NJ_USE_LIBC 0
#define NJ_INCLUDE_HEADER_ONLY
#include "nanojpeg.c"

#include "tinyjpeg.h"
#define ENCODING_QUALITY 90

int main () {
    volatile uint32_t *LED = (uint32_t *) XPAR_XGPIO_0_BASEADDR; //Debug led
    *LED = 1;
    XUartPs *uart0;
    uint8_t tx_buffer[RX_BUFF_SIZE];
    uint8_t rx_buffer[TX_BUFF_SIZE];
    uint32_t image_size = 0;
    init_uart(uart0);
    njInit();
    while (1)
    {
        memset(rx_buffer, 0, sizeof(rx_buffer));//Clear rx buffer
        image_size = get_image(uart0, rx_buffer);

        // Decode JPEG
        if (njDecode(rx_buffer, image_size) != 0) {
            continue;
        }
        int width = njGetWidth();
        int height = njGetHeight();
        const uint8_t *rgb_data = njGetImage();

        // Convolution here

        // Encode JPEG
        tinyjpeg_t jpeg;
        tinyjpeg_init(&jpeg, width, height);
        uint8_t *encoded_data = (uint8_t *)malloc(width * height * 4);
        size_t encoded_size = tinyjpeg_encode(&jpeg, rgb_data, encoded_data, width, height, ENCODING_QUALITY);

        memcpy(tx_buffer, encoded_data, sizeof(rx_buffer));
        send_frame(uart0, tx_buffer, encoded_size);

        njDone();
        free(encoded_data);
    }
    return 1;
}

