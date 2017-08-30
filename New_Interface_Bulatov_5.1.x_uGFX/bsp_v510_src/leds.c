#include "leds.h"

void LEDs_Init (void)
{
    GPIO_InitTypeDef GPIO_InitStruct;
    
    __GPIOD_CLK_ENABLE();
    __GPIOF_CLK_ENABLE();
    
    GPIO_InitStruct.Pin = GPIO_PIN_5;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_LOW;
    HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);
    
    /*Configure GPIO pins : PF6 PF7 PF8 PF9 */
    GPIO_InitStruct.Pin = GPIO_PIN_6|GPIO_PIN_7|GPIO_PIN_8|GPIO_PIN_9;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_LOW;
    HAL_GPIO_Init(GPIOF, &GPIO_InitStruct);
}
