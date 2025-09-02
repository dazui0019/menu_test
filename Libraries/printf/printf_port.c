#include "printf_port.h"

UART_HandleTypeDef* pUART = NULL;       // printf重定向使用

void _putchar(char character)
{
    HAL_UART_Transmit(pUART, (uint8_t*)&character, 1, 100);
}

void retarget_printf(UART_HandleTypeDef* uartx)
{
    pUART = uartx;
}
