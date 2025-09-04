#include "lcd_port.h"
#include "main.h"
#include "stm32f4xx_hal_gpio.h"

/* Command/data pin set */
#define LCD_RS_CMD            HAL_GPIO_WritePin()
#define LCD_RS_DATA           GPIOX_ODR(LCD_RS) = 1

/* Reset pin set */
#define LCD_RST_ON            GPIOX_ODR(LCD_RST) = 0
#define LCD_RST_OFF           GPIOX_ODR(LCD_RST) = 1

/* Chip select pin set */
#define LCD_CS_ON             GPIOX_ODR(LCD_CS) = 0
#define LCD_CS_OFF            GPIOX_ODR(LCD_CS) = 1

void LCD_IO_Init(void)
{
}

/* Public functions */

void LCD_Delay(uint32_t Delay)
{
    HAL_Delay(Delay);
}

//-----------------------------------------------------------------------------
void LCD_IO_Bl_OnOff(uint8_t Bl)
{
    HAL_GPIO_WritePin(LCD_BL_GPIO_Port, LCD_BL_Pin, Bl);
}
