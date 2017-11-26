#ifndef __LED_LLD_H
#define __LED_LLD_H

#include "stm32f7xx_hal.h"

#define LED_NUM 5

typedef struct {
	GPIO_TypeDef* GPIO_port;
	uint16_t GPIO_pin;
}led_id_td;

static const led_id_td led_tab[LED_NUM]={
	{GPIOF, GPIO_PIN_6}, //LED HL1
	{GPIOF, GPIO_PIN_7}, //LED_HL2
	{GPIOF, GPIO_PIN_8}, //LED_HL3
	{GPIOF, GPIO_PIN_9}, //LED_HL4
	{GPIOD, GPIO_PIN_5}, //LED_HL5
};

#define LED_HL(x) led_tab[x-1]


/*HL - ON-OFF*/
//#define LED(led_id)_ON HAL_GPIO_WritePin(led_id.GPIO_port, led_id.GPIO_pin, GPIO_PIN_SET)
//#define LED(led_id)_OFF HAL_GPIO_WritePin(led_id.GPIO_port, led_id.GPIO_pin, GPIO_PIN_RESET)



void led_lld_on(led_id_td led);

void led_lld_off(led_id_td led);

void led_lld_toggle(led_id_td led);

//Прототипы функций
void led_lld_init (void);

//void led_lld_blink(led_id_td led, uint16_t blink_period);

#endif /* __LED_LLD_H */