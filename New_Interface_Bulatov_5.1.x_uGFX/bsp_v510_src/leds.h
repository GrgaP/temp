#ifndef LEDS_H		// Блокируем повторное включение этого модуля
#define LEDS_H

#include "main.h"

#define LED_PORT      GPIOF
#define HL1 /*LED_KEYBOARD*/  GPIO_Pin_6
#define HL2 /*LED_DMA_RX */   GPIO_Pin_7
#define HL3 /*LED_DMA_TX*/    GPIO_Pin_8
#define HL4 /*LED_USB*/       GPIO_Pin_9
#define HL5 /*LED_BT*/        GPIO_Pin_10

//Прототипы функций
void LEDs_Init (void);

#endif