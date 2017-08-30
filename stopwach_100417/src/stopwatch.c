#include "stopwatch.h"
#include <stdio.h>


/* TIM handle declaration */
TIM_HandleTypeDef    TimHandle;

/* Prescaler declaration */
uint32_t uwPrescalerValue = 0;

typedef enum  {
  SW_STOP,
  SW_RUN,
} SW_STATE_td;

SW_STATE_td sw_state = SW_STOP;

typedef struct {
  uint8_t h_val;
  uint8_t min_val;
  uint8_t sec_val;
  uint16_t msec_val;
}sw_data_td;

sw_data_td sw_data;

static char output_str[9];

/**
  * @brief  This function is executed in case of error occurrence.
  * @param  None
  * @retval None
  */
static void Timer_Error_Handler(void)
{
  /* Turn LED1 on */
  //BSP_LED_On(LED1);
  while (1)
  {
  }
}

static void Timer_Init(void)
{
  /*##-1- Configure the TIM peripheral #######################################*/
  /* -----------------------------------------------------------------------
    In this example TIM3 input clock (TIM3CLK)  is set to APB1 clock (PCLK1) x2,
    since APB1 prescaler is equal to 4.
      TIM3CLK = PCLK1*2
      PCLK1 = HCLK/4
      => TIM3CLK = HCLK/2 = SystemCoreClock/2
    To get TIM3 counter clock at 10 KHz, the Prescaler is computed as follows:
    Prescaler = (TIM3CLK / TIM3 counter clock) - 1
    Prescaler = ((SystemCoreClock/2) /10 KHz) - 1

    Note:
     SystemCoreClock variable holds HCLK frequency and is defined in system_stm32f7xx.c file.
     Each time the core clock (HCLK) changes, user had to update SystemCoreClock
     variable value. Otherwise, any configuration based on this variable will be incorrect.
     This variable is updated in three ways:
      1) by calling CMSIS function SystemCoreClockUpdate()
      2) by calling HAL API function HAL_RCC_GetSysClockFreq()
      3) each time HAL_RCC_ClockConfig() is called to configure the system clock frequency
  ----------------------------------------------------------------------- */

  /* Compute the prescaler value to have TIMx counter clock equal to 10000 Hz */
  uwPrescalerValue = (uint32_t)((SystemCoreClock / 2) / 10000) - 1;

  /* Set TIMx instance */
  TimHandle.Instance = TIMx;

  /* Initialize TIMx peripheral as follows:
       + Period = 10000 - 1
       + Prescaler = ((SystemCoreClock / 2)/10000) - 1
       + ClockDivision = 0
       + Counter direction = Up
  */
  TimHandle.Init.Period            = 10 - 1;
  TimHandle.Init.Prescaler         = uwPrescalerValue;
  TimHandle.Init.ClockDivision     = 0;
  TimHandle.Init.CounterMode       = TIM_COUNTERMODE_UP;
  TimHandle.Init.RepetitionCounter = 0;

  if (HAL_TIM_Base_Init(&TimHandle) != HAL_OK)
  {
    /* Initialization Error */
    Timer_Error_Handler();
  }

  /*##-2- Start the TIM Base generation in interrupt mode ####################*/
  /* Start Channel1 */
  if (HAL_TIM_Base_Start_IT(&TimHandle) != HAL_OK)
  {
    /* Starting Error */
    Timer_Error_Handler();
  }
};

/**
  * @brief  Period elapsed callback in non blocking mode
  * @param  htim : TIM handle
  * @retval None
  */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  //BSP_LED_Toggle(LED1);
  HAL_GPIO_TogglePin(GPIOI, GPIO_PIN_1);
  
  if(sw_state == SW_RUN){
  
    if(sw_data.msec_val<1000)
      sw_data.msec_val++;
    else
    {
      sw_data.msec_val = 0;
      sw_data.sec_val++;
      //HAL_GPIO_TogglePin(GPIOI, GPIO_PIN_1);
    
      if(sw_data.sec_val==60)
      {
        sw_data.sec_val = 0;
        sw_data.min_val++;      
      
        if(sw_data.min_val==60)
        {
          sw_data.min_val = 0;
          sw_data.h_val++;
        
          if(sw_data.h_val==24)
          {
            sw_data.h_val=0;
            //stop timer
          };
        };      
      };    
    };
  
  };
    
/*  
  switch(sw_state)
  {
  case SW_RUN:
    
    break;
  case SW_RESET:
    
    break;
  };*/
  
}

void sw_reset(void);

void sw_module_init(void)
{
  Timer_Init();
  
  sw_reset();
};

uint32_t sw_get_msec_val(void)
{
  return sw_data.msec_val;
};

char* sw_get_tim_fmt(char* string)
{
  sprintf(output_str, "%02d:%02d:%03d", sw_data.min_val, sw_data.sec_val, sw_data.msec_val);
  
  return output_str;
};

void sw_start(void)
{
  sw_state = SW_RUN;
};

void sw_reset(void)
{    
  sw_data.h_val = 0;
  sw_data.min_val = 0;
  sw_data.sec_val = 0;
  sw_data.msec_val = 0;
  
  sw_state = SW_STOP;
};

void sw_stop(void)
{
  sw_state = SW_STOP;
};