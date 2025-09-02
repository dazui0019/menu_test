#pragma once

#include "printf.h"
#include "stdint.h"
#include "stm32f411xe.h"
#include "usart.h"

/**
 * @brief       提供给printf.c的接口函数, 在串口重定向时使用
 * @param[in]   character 需要发送的字符
 * @retval      none
*/
void _putchar(char character);

void retarget_printf(UART_HandleTypeDef* uartx);
