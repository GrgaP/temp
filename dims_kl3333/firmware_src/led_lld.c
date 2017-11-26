
#include "led_lld.h"

//osThreadId Led_blink_threadHandle;

void led_lld_on(led_id_td led)
{
	HAL_GPIO_WritePin(led.GPIO_port, led.GPIO_pin, GPIO_PIN_SET);
}

void led_lld_off(led_id_td led)
{
	HAL_GPIO_WritePin(led.GPIO_port, led.GPIO_pin, GPIO_PIN_RESET);
}

void led_lld_toggle(led_id_td led)
{
	HAL_GPIO_WritePin(led.GPIO_port, led.GPIO_pin, !(HAL_GPIO_ReadPin(led.GPIO_port, led.GPIO_pin)));;
}

void led_lld_init(void)
{
	uint8_t i;
	
   for(i=1; i==LED_NUM; i++)
	   led_lld_off(led_tab[i]);
}