/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __STOPWATCH_H
#define __STOPWATCH_H

#include "stm32f7xx_hal.h"
#include "main.h"

/* Definition for TIMx clock resources */
#define TIMx                           TIM3
#define TIMx_CLK_ENABLE()              __HAL_RCC_TIM3_CLK_ENABLE()


/* Definition for TIMx's NVIC */
#define TIMx_IRQn                      TIM3_IRQn
#define TIMx_IRQHandler                TIM3_IRQHandler

void sw_module_init(void);

uint32_t sw_get_msec_val(void);

char* sw_get_tim_fmt();

void sw_start(void);

void sw_reset(void);

void sw_stop(void);

#endif /* __STOPWATCH_H */