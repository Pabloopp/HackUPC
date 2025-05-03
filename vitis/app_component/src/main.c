
#include "xparameters.h"
#include <stdint.h>
#include <string.h>
#include "xuartps.h"
#include "uart.h"

#define MAX_IMAGE_WIDTH  800
#define MAX_IMAGE_HEIGHT 600
#define MAX_IMAGE_SIZE (MAX_IMAGE_WIDTH * MAX_IMAGE_HEIGHT * 3)

void init_uart(XUartPs *uart_instance);
uint32_t get_image(XUartPs *uart, uint8_t *buff);
void send_frame(XUartPs *uart, const u8 *frame_buffer, u32 length);
void receive_bytes(XUartPs *uart, uint8_t *buffer, uint32_t length);
void send_bytes(XUartPs *uart, uint8_t *buffer, uint32_t length);
void convert_to_matrix_rgb(uint8_t *raw_data, uint8_t ***image_matrix, uint32_t height, uint32_t width);
void invert_image_rgb(uint8_t ***image_matrix, uint32_t height, uint32_t width);


int main () {
    volatile uint32_t *LED = (uint32_t *) XPAR_XGPIO_0_BASEADDR; //Debug led
    *LED = 1;
    XUartPs uart;
    uint8_t tx_buffer[MAX_IMAGE_SIZE];
    uint8_t image_buffer[MAX_IMAGE_SIZE];
    //uint8_t tmp_dbg[2];
    
    init_uart(&uart);
    while (1)
    {
        uint32_t height, height_tmp = 0;
        uint32_t width = 0;
        uint8_t h_buf[4], w_buf[4];
        receive_bytes(&uart, h_buf, 4);
        receive_bytes(&uart, w_buf, 4);
        height = h_buf[3] | (h_buf[2] << 8) | (h_buf[1] << 16) | (h_buf[0] << 24);
        width  = w_buf[3] | (w_buf[2] << 8) | (w_buf[1] << 16) | (w_buf[0] << 24);
        //receive_bytes(&uart, (uint8_t *)&height, 4);//Read height
        //receive_bytes(&uart, (uint8_t *)&width, 4);//Read Width
        uint8_t image_matrix[height][width][3];
        uint32_t image_size = height * width * 3;
        receive_bytes(&uart, image_buffer, image_size);//Read image
        convert_to_matrix_rgb(image_buffer, image_matrix, height, width);
        //memset(rx_buffer, 0, 2 * IMAGE_SIZE);//Clear rx buffer
        //image_size = get_image(&uart0, rx_buffer);
        //memcpy(tx_buffer, rx_buffer, TX_BUFF_SIZE);
        invert_image_rgb(image_matrix, height, width);
        send_bytes(&uart, (uint8_t *)&height, 4);
        send_bytes(&uart, (uint8_t *)&width, 4);
        send_bytes(&uart, tx_buffer, image_size);
        //tmp_dbg[0] = tx_buffer[image_size - 2];
        //tmp_dbg[1] = tx_buffer[image_size - 1];
        //send_frame(&uart0, tx_buffer, image_size);
    }
    return 1;
}

void init_uart(XUartPs *uart_instance)
{
    XUartPs_Config *uart_config = XUartPs_LookupConfig(XPAR_UART0_BASEADDR);
    XUartPs_CfgInitialize(uart_instance, uart_config, uart_config ->BaseAddress);
    //TODO set higher baudrate? currently default config
}

void receive_bytes(XUartPs *uart, uint8_t *buffer, uint32_t length) 
{
    uint32_t received = 0;
    while (received < length)
    {
        received += XUartPs_Recv(uart, buffer + received, length - received);
    }
}
/*
uint32_t get_image(XUartPs *uart, uint8_t *buff)
{
    uint8_t partial_bytes[4];
    uint64_t i = 0;
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
*/

/*
/**
 * Function to send a frame over UART.
 * @param uart: Pointer to the XUartPs instance.
 * @param frame_buffer: Pointer to the frame buffer to be sent.
 */
 /*
void send_frame(XUartPs *uart, const u8 *frame_buffer, u32 length){
    u32 total_sent = 0;
    while (total_sent < length) {
        u32 bytes_sent = XUartPs_Send(uart, frame_buffer + total_sent, length - total_sent);
        total_sent += bytes_sent;
    }

}
*/

void send_bytes(XUartPs *uart, uint8_t *buffer, uint32_t length) 
{
    uint32_t sent = 0;
    while (sent < length)
    {
        sent += XUartPs_Send(uart, buffer + sent, length - sent);
    }
}


void convert_to_matrix_rgb(uint8_t *raw_data, uint8_t ***image_matrix, uint32_t height, uint32_t width)
{
    int index = 0;
    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j< width; j++)
        {
            image_matrix[i][j][0] = raw_data[index++];
            image_matrix[i][j][1] = raw_data[index++];
            image_matrix[i][j][2] = raw_data[index++];
        }
    }
}

void invert_image_rgb(uint8_t ***image_matrix, uint32_t height, uint32_t width) {
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            image_matrix[i][j][0] = 255 - image_matrix[i][j][0];  
            image_matrix[i][j][1] = 255 - image_matrix[i][j][1];  
            image_matrix[i][j][2] = 255 - image_matrix[i][j][2]; 
        }
    }
}