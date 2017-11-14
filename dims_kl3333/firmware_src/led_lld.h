#ifndef __LED_LLD_H
#define __LED_LLD_H

#include "stm32f7xx_hal.h"

#define LED_NUM 5

#

typedef struct led_id_td
{
	GPIO_TypeDef* GPIO_port;
	uint16_t GPIO_pin;
};

const led_id_td led_hl[LED_NUM]={
	{GPIOF, GPIO_Pin_6}, //LED HL1
	{GPIOF, GPIO_Pin_7}, //LED_HL2
	{GPIOF, GPIO_Pin_8}, //LED_HL3
	{GPIOF, GPIO_Pin_9}, //LED_HL4
	{GPIOD, GPIO_Pin_5}, //LED_HL5
}

#define LED_HL1 led_hl[0];
#define LED_HL2 led_hl[1];
#define LED_HL3 led_hl[2];
#define LED_HL4 led_hl[3];
#define LED_HL5 led_hl[4];

#define LED_HL(NUM) ((NUM > 0)&&(NUM<(LED_NUM+1)))? (led_hl[NUM-1]): (#error "!!!")

/*
const led_id_td led_hl1 = {GPIOF, GPIO_Pin_6};
const led_id_td led_hl2 = {GPIOF, GPIO_Pin_7};
const led_id_td led_hl3 = {GPIOF, GPIO_Pin_8};
const led_id_td led_hl4 = {GPIOF, GPIO_Pin_9};
const led_id_td led_hl5 = {GPIOF, GPIO_Pin_10};
*/


/*HL - ON-OFF*/
#define LED(led_id)_ON HAL_GPIO_WritePin(led_id.GPIO_port, led_id.GPIO_pin, GPIO_PIN_SET)
#define LED(led_id)_OFF HAL_GPIO_WritePin(led_id.GPIO_port, led_id.GPIO_pin, GPIO_PIN_RESET)

//Прототипы функций
void led_lld_init (void);

void led_lld_blink(led_id_td led, uint16_t blink_period);

#endif /* __LED_LLD_H */