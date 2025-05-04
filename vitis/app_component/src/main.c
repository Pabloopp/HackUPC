#include "xparameters.h"
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include "xuartps.h"
#include <math.h>

#define MAX_IMAGE_WIDTH  200
#define MAX_IMAGE_HEIGHT 200
#define MAX_IMAGE_SIZE (MAX_IMAGE_WIDTH * MAX_IMAGE_HEIGHT * 3)

void init_uart(XUartPs *uart_instance);
uint32_t get_image(XUartPs *uart, uint8_t *buff);
void send_frame(XUartPs *uart, const u8 *frame_buffer, u32 length);
void receive_bytes(XUartPs *uart, uint8_t *buffer, uint32_t length);
void send_bytes(XUartPs *uart, uint8_t *buffer, uint32_t length);
void convert_to_matrix_rgb(uint8_t *raw_data, uint8_t image_matrix[MAX_IMAGE_HEIGHT][MAX_IMAGE_WIDTH][3], uint32_t height, uint32_t width);
void invert_image_rgb(uint8_t image_matrix[MAX_IMAGE_HEIGHT][MAX_IMAGE_WIDTH][3], uint32_t height, uint32_t width);
void serialize_matrix_rgb(uint8_t image_matrix[MAX_IMAGE_HEIGHT][MAX_IMAGE_WIDTH][3], uint8_t *raw_data, uint32_t height, uint32_t width);
void send_uint32_big_endian(XUartPs *uart, uint32_t value);
uint8_t clamp(float value);
void sobel_convolve(uint32_t height, uint32_t width, uint8_t input[MAX_IMAGE_HEIGHT][MAX_IMAGE_WIDTH][3],
              uint8_t output[MAX_IMAGE_HEIGHT][MAX_IMAGE_WIDTH][3]);


    uint8_t tx_buffer[MAX_IMAGE_SIZE];
    uint8_t image_buffer[MAX_IMAGE_SIZE];
    uint8_t image_matrix[MAX_IMAGE_HEIGHT][MAX_IMAGE_WIDTH][3];
    uint8_t transformed_image_matrix[MAX_IMAGE_HEIGHT][MAX_IMAGE_WIDTH][3];
int main () {
    volatile uint32_t *LED = (uint32_t *) XPAR_XGPIO_0_BASEADDR; //Debug led
    *LED = 1;
    XUartPs uart;
    
    //uint8_t tmp_dbg[2];
    
    init_uart(&uart);
    while (1)
    {
        uint32_t height = 0;
        uint32_t width = 0;
        uint8_t h_buf[4], w_buf[4];
        receive_bytes(&uart, h_buf, 4);
        receive_bytes(&uart, w_buf, 4);
        height = h_buf[3] | (h_buf[2] << 8) | (h_buf[1] << 16) | (h_buf[0] << 24);
        width  = w_buf[3] | (w_buf[2] << 8) | (w_buf[1] << 16) | (w_buf[0] << 24);

        uint32_t image_size = height * width * 3;
        receive_bytes(&uart, image_buffer, image_size);//Read image
        convert_to_matrix_rgb(image_buffer, image_matrix, height, width);
        //invert_image_rgb(image_matrix, height, width);
        sobel_convolve(height, width, image_matrix, transformed_image_matrix);
        serialize_matrix_rgb(transformed_image_matrix, tx_buffer, height, width);
        send_uint32_big_endian(&uart, height);
        send_uint32_big_endian(&uart, width);
        send_bytes(&uart, tx_buffer, image_size);
    }
    return 1;
}

void init_uart(XUartPs *uart_instance)
{
    XUartPs_Config *uart_config = XUartPs_LookupConfig(XPAR_UART0_BASEADDR);
    XUartPs_CfgInitialize(uart_instance, uart_config, uart_config ->BaseAddress);
}

void receive_bytes(XUartPs *uart, uint8_t *buffer, uint32_t length) 
{
    uint32_t received = 0;
    while (received < length)
    {
        received += XUartPs_Recv(uart, buffer + received, length - received);
    }
}

void send_bytes(XUartPs *uart, uint8_t *buffer, uint32_t length) 
{
    uint32_t sent = 0;
    while (sent < length)
    {
        sent += XUartPs_Send(uart, buffer + sent, length - sent);
    }
}

void send_uint32_big_endian(XUartPs *uart, uint32_t value) {
    uint8_t bytes[4];
    bytes[0] = (value >> 24) & 0xFF;
    bytes[1] = (value >> 16) & 0xFF;
    bytes[2] = (value >> 8) & 0xFF;
    bytes[3] = value & 0xFF;
    send_bytes(uart, bytes, 4);
}

void convert_to_matrix_rgb(uint8_t *raw_data, uint8_t image_matrix[MAX_IMAGE_HEIGHT][MAX_IMAGE_WIDTH][3], uint32_t height, uint32_t width)
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

void serialize_matrix_rgb(uint8_t image_matrix[MAX_IMAGE_HEIGHT][MAX_IMAGE_WIDTH][3], uint8_t *raw_data, uint32_t height, uint32_t width) {
    int index = 0;
    for (uint32_t i = 0; i < height; i++) {
        for (uint32_t j = 0; j < width; j++) {
            raw_data[index++] = image_matrix[i][j][0];  
            raw_data[index++] = image_matrix[i][j][1];  
            raw_data[index++] = image_matrix[i][j][2];  
        }
    }
}

void invert_image_rgb(uint8_t image_matrix[MAX_IMAGE_HEIGHT][MAX_IMAGE_WIDTH][3], uint32_t height, uint32_t width) {
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            image_matrix[i][j][0] = 255 - image_matrix[i][j][0];  
            image_matrix[i][j][1] = 255 - image_matrix[i][j][1];  
            image_matrix[i][j][2] = 255 - image_matrix[i][j][2]; 
        }
    }
}

float sobel_gx[3][3] = {
    { -1, 0, 1 },
    { -2, 0, 2 },
    { -1, 0, 1 }
};

float sobel_gy[3][3] = {
    { -1, -2, -1 },
    {  0,  0,  0 },
    {  1,  2,  1 }
};

float sharpen_kernel[3][3] = {
    { 0, -1, 0 },
    { -1, 5, -1 },
    { 0, -1, 0 }
};

uint8_t clamp(float value) {
    if (value < 0) return 0;
    if (value > 255) return 255;
    return (uint8_t)value;
}

void convolve(uint32_t height, uint32_t width, uint8_t input[MAX_IMAGE_HEIGHT][MAX_IMAGE_WIDTH][3],
              uint8_t output[MAX_IMAGE_HEIGHT][MAX_IMAGE_WIDTH][3], float kernel[3][3]) {
    for (int y = 1; y < height - 1; y++) {
        for (int x = 1; x < width - 1; x++) {
            for (int c = 0; c < 3; c++) {
                float sum = 0.0f;

                for (int ky = -1; ky <= 1; ky++) {
                    for (int kx = -1; kx <= 1; kx++) {
                        int iy = y + ky;
                        int ix = x + kx;
                        sum += input[iy][ix][c] * kernel[ky + 1][kx + 1];
                    }
                }

                output[y][x][c] = clamp(sum);
            }
        }
    }
}

void sobel_convolve(uint32_t height, uint32_t width, uint8_t input[MAX_IMAGE_HEIGHT][MAX_IMAGE_WIDTH][3],
                    uint8_t output[MAX_IMAGE_HEIGHT][MAX_IMAGE_WIDTH][3]) {
    for (int y = 1; y < height - 1; y++) {
        for (int x = 1; x < width - 1; x++) {
            for (int c = 0; c < 3; c++) {
                float gx = 0.0f;
                float gy = 0.0f;

                for (int ky = -1; ky <= 1; ky++) {
                    for (int kx = -1; kx <= 1; kx++) {
                        int iy = y + ky;
                        int ix = x + kx;
                        gx += input[iy][ix][c] * sobel_gx[ky + 1][kx + 1];
                        gy += input[iy][ix][c] * sobel_gy[ky + 1][kx + 1];
                    }
                }

                float magnitude = sqrt(gx * gx + gy * gy);
                output[y][x][c] = clamp(magnitude);
            }
        }
    }
}
