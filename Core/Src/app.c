#include "app.h"
#include "main.h"
#include "stm32f4xx_hal_gpio.h"
#include "stateMachine.h"

void setup(){
}

void loop(){
    HAL_GPIO_TogglePin(LD0_GPIO_Port, LD0_Pin);
    HAL_Delay(100);
}
