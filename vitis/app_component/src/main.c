#include "xparameters.h"
#include <stdint.h>
int main () {
    volatile uint32_t *LED = (uint32_t *) XPAR_XGPIO_0_BASEADDR;
    *LED = 1;
    return 1;
}