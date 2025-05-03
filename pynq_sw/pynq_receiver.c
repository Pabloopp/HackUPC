#include "xparameters.h"
#include "xuartps.h"

void init_uart(XUartPs *Uart_instance)
{
    XUartPs_Config *Uart_config = XUartPs_LookupConfig(XPAR_UART0_BASEADDR);
    XUartPs_CfgInitialize(Uart_instance, Uart_config, Uart_config ->BaseAddress);
    //TODO set higher baudrate? currently default config
}

void get_image(XUartPs *Uart, uint8_t *buff)
{
    uint8_t r_byte = 0;
    uint8_t r_byte_prev = 0;
    uint64_t i = 2;
    while (r_byte != 0xD8 && r_byte_prev != 0xFF)
    {
        r_byte_prev = r_byte;
        r_byte = XUartPs_RecvByte(Uart->Config.BaseAddress);
    }
    buff[0] = 0xFF;
    buff[1] = 0xD8;
    while (r_byte != 0xD9 && r_byte_prev != 0xFF)
    {
        r_byte_prev = r_byte;
        r_byte = XUartPs_RecvByte(Uart->Config.BaseAddress);
        buff[i] = r_byte;
        i++;
    }
}