#include "led_lld.h"

osThreadId Led_blink_threadHandle;

void Led_blink_thread(void const * argument);

typedef struct led_blink_thrd_arg_td{
	led_id_td led_id;
	uint16_t blink_period;
	uint32_t blink_duration;
};

void led_lld_Init (void)
{
    GPIO_InitTypeDef GPIO_InitStruct;
    
    //__GPIOD_CLK_ENABLE();
    //__GPIOF_CLK_ENABLE();
    
   /* GPIO_InitStruct.Pin = GPIO_PIN_5;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_LOW;
    HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);
    
    /*Configure GPIO pins : PF6 PF7 PF8 PF9 
    GPIO_InitStruct.Pin = GPIO_PIN_6|GPIO_PIN_7|GPIO_PIN_8|GPIO_PIN_9;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_LOW;
    HAL_GPIO_Init(GPIOF, &GPIO_InitStruct);*/
	
	for(i=0; i< LEN_NUM; i++)
	{
		GPIO_InitStruct.Pin = led_hl[i].GPIO_pin;
		GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
		GPIO_InitStruct.Pull = GPIO_NOPULL;
		GPIO_InitStruct.Speed = GPIO_SPEED_LOW;
		HAL_GPIO_Init(led_hl[i].GPIO_port;, &GPIO_InitStruct);
	}
	
	//osThreadDef(Led_blink_ThreadId, Led_blink_Thread, osPriorityNormal, 0, 128);
	//Led_blink_threadHandle = osThreadCreate(osThread(Led_blink_threadId), NULL);
	
}

void led_lld_on(led_id_td led)
{
	
}

void led_lld_off(led_id_td led)
{
	
}

void led_lld_toggle(led_id_td led)
{
	
}


void led_lld_blink_start(led_id_td led, uint16_t blink_period, uint32_t blink_duration)
{
	
}

void led_lld_blink_stop(led_id_td led)
{
	
}

void Led_blink_thread(void const * argument)
{
	led_blink_thrd_arg_td blink_parameters;
	uint32_t thrd_live_duration;
	
  /* USER CODE BEGIN 5 */
  /* Infinite loop */
  for(;;)
  {
	  
	led_lld_toggle();  
	  
    osDelay(blink_parameters.blink_period);
  }
  /* USER CODE END 5 */ 
}

