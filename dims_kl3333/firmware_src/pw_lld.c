#include "stm32f7xx_hal.h"

void pw_lld_main_on (void)
{
	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_SET);//GPIO_SetBits (GPIOC, GPIO_Pin_10);
}

void pw_lld_main_off (void)
{
	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_RESET);//GPIO_ResetBits (GPIOC, GPIO_Pin_10);
}


void pw_lld_gps_on (void)
{
	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_5, GPIO_PIN_SET);//GPIO_SetBits (GPIOC, GPIO_Pin_10);
}

void pw_lld_gps_off (void)
{
	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_5, GPIO_PIN_RESET);//GPIO_SetBits (GPIOC, GPIO_Pin_10);
}


void pw_lld__dcdc_on (void)
{
	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_4, GPIO_PIN_SET);//GPIO_SetBits (GPIOC, GPIO_Pin_4);
}

void pw_lld_dcdc_off (void)
{
	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_4, GPIO_PIN_RESET);//GPIO_ResetBits (GPIOC, GPIO_Pin_4);
}


void pw_lld_disorber_on (void)
{
	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_3, GPIO_PIN_SET);//GPIO_SetBits (GPIOD, GPIO_Pin_3);
}

void pw_lld_disorber_off (void)
{
	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_3, GPIO_PIN_RESET);//GPIO_ResetBits (GPIOD, GPIO_Pin_3);
}

void pw_lld_init()
{
	pw_lld_main_on();
        pw_lld_gps_on();
	pw_lld_disorber_off();
	pw_lld_dcdc_off();	
}
