/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "gfx.h"
//#include "gui.h"

/* External variables --------------------------------------------------------*/
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
UART_HandleTypeDef huart6;
DMA_HandleTypeDef hdma_usart6_tx;
DMA_HandleTypeDef hdma_usart6_rx;

osThreadId defaultTaskHandle;

osThreadId GUI_ThreadId;

osTimerId ts_timer;//таймер опроса сенсорного экрана

/* Private function prototypes -----------------------------------------------*/



/*-RTOS-*/
static void GUI_Thread(void const *argument);
void StartDefaultTask(void const * argument);
static void TS_TimerCallback(void const *n);

/*-SysInit-*/
static void MPU_Config(void);
static void CPU_CACHE_Enable(void);
static void SystemClock_Config(void);


/* USART1 init function */
void MX_USART6_UART_Init(void)
{

  huart6.Instance = USART6;
  huart6.Init.BaudRate = 115200;
  huart6.Init.WordLength = UART_WORDLENGTH_8B;
  huart6.Init.StopBits = UART_STOPBITS_1;
  huart6.Init.Parity = UART_PARITY_NONE;
  huart6.Init.Mode = UART_MODE_TX_RX;
  huart6.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart6.Init.OverSampling = UART_OVERSAMPLING_16;
  huart6.Init.OneBitSampling = UART_ONEBIT_SAMPLING_DISABLED;
  huart6.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  HAL_UART_Init(&huart6);

  HAL_NVIC_SetPriority(USART6_IRQn, 5, 0);
  HAL_NVIC_EnableIRQ(USART6_IRQn);
}

/** 
  * Enable DMA controller clock
  */
void MX_DMA_Init(void) 
{
  /* DMA controller clock enable */
  __HAL_RCC_DMA2_CLK_ENABLE();

  /* DMA interrupt init */
  HAL_NVIC_SetPriority(DMA2_Stream1_IRQn, 5, 0);
  HAL_NVIC_EnableIRQ(DMA2_Stream1_IRQn);
  HAL_NVIC_SetPriority(DMA2_Stream6_IRQn, 5, 0);
  HAL_NVIC_EnableIRQ(DMA2_Stream6_IRQn);
}

/*
 * 
 */
/*void test_func(void)
{
  uint8_t arr1[15] = {0,1,2,3,4,5,6,7,8,9,10,11,12,13,14};
  uint8_t tmp1;

  uint8_t ind_start = 7;
  uint8_t num = 4;
  
  uint8_t i = 0;
  
  while(1)
  {
    tmp1 = arr1[ind_start];
      for(i= ind_start; i<((ind_start+num)-1); i++){arr1[i]=arr1[i+1];};
    arr1[(ind_start+num)-1] = tmp1;
  };
}*/

/**
  * @brief  Main program
  * @param  None
  * @retval None
  */

int main(void)
{
  /* Configure the MPU attributes as Write Through */
  MPU_Config();
  
  /* Enable the CPU Cache */
  CPU_CACHE_Enable();

/* STM32F7xx HAL library initialization*/
  HAL_Init();  

  /* Configure the system clock to 216 Mhz */
  SystemClock_Config();  
  
  /*---Debug section begin---*/
  
  GPIO_InitTypeDef GPIO_InitStruct;
  
  __GPIOI_CLK_ENABLE();
  
  GPIO_InitStruct.Pin = GPIO_PIN_1;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_MEDIUM;
  HAL_GPIO_Init(GPIOI, &GPIO_InitStruct);
  
  HAL_GPIO_WritePin(GPIOI, GPIO_PIN_1, GPIO_PIN_SET);
  HAL_GPIO_WritePin(GPIOI, GPIO_PIN_1, GPIO_PIN_RESET);
  
  /*---Debug section end---*/
  
  MX_DMA_Init();
  MX_USART6_UART_Init(); 
  
  
  /* Create GUI task */
  osThreadDef(GUI, GUI_Thread, osPriorityNormal, 0, 512);
  GUI_ThreadId = osThreadCreate (osThread(GUI), NULL);
 
  /* LCD-GUI init*/
  //SVD_GUI_Init();
  
  DIMS_RTC_CalendarBkupInit();

  /* Create Touch screen Timer */
  osTimerDef(TS_Timer, TS_TimerCallback);
  ts_timer =  osTimerCreate(osTimer(TS_Timer), osTimerPeriodic, (void *)0);
 
  osTimerStart(ts_timer, 100);
  
  /* Start scheduler */
  osKernelStart();
  
  /*-RTOS end-*/
  
  /* We should never get here as control is now taken by the scheduler */
  for(;;);
}

/*----------------------RTOS theread desc begin--------------------------*/

static void GUI_Thread(void const *argument)
{
  //SVD_GUI_back_ctrl_startup();//запуск фонового окна с кнопками
  
  gfxInit();
  
  guiCreate();
 /* Gui background Task */
    while(1) 
    {
      gdispClear(Black);//Erasing a next free framebuffer
      guiEventLoop();//Running drawing operation
      
      gdispFlush();//A drawing of frame buffer has been completed. Need flushing
      
      osDelay(20); /* Nothing left to do for the moment ... Idle processing */
    }
}

/**
  * @brief  Timer callbacsk (40 ms)
  * @param  n: Timer index 
  * @retval None
  */
static void TS_TimerCallback(void const *n)
{  
  //SVD_GUI_TouchUpdate();
}

/*-----------------------RTOS theread desc end-------------------------*/

/**
  * @brief  System Clock Configuration
  *         The system Clock is configured as follow : 
  *            System Clock source            = PLL (HSE)
  *            SYSCLK(Hz)                     = 200000000
  *            HCLK(Hz)                       = 200000000
  *            AHB Prescaler                  = 1
  *            APB1 Prescaler                 = 4
  *            APB2 Prescaler                 = 2
  *            HSE Frequency(Hz)              = 25000000
  *            PLL_M                          = 25
  *            PLL_N                          = 400
  *            PLL_P                          = 2
  *            PLL_Q                          = 8
  *            VDD(V)                         = 3.3
  *            Main regulator output voltage  = Scale1 mode
  *            Flash Latency(WS)              = 6
  * @param  None
  * @retval None
  */
static void SystemClock_Config(void)
{
  RCC_ClkInitTypeDef RCC_ClkInitStruct;
  RCC_OscInitTypeDef RCC_OscInitStruct;
  //RCC_PeriphCLKInitTypeDef PeriphClkInitStruct;
  
  HAL_StatusTypeDef ret = HAL_OK;

  /* Enable HSE Oscillator and activate PLL with HSE as source */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 25;
  RCC_OscInitStruct.PLL.PLLN = 400;  
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 8;

  ret = HAL_RCC_OscConfig(&RCC_OscInitStruct);
  if(ret != HAL_OK)
  {
    while(1) { ; }
  }

  /* Activate the OverDrive to reach the 200 MHz Frequency */
  ret = HAL_PWREx_EnableOverDrive();
  if(ret != HAL_OK)
  {
    while(1) { ; }
  }
  
  /* Select PLL as system clock source and configure the HCLK, PCLK1 and PCLK2 clocks dividers */
  RCC_ClkInitStruct.ClockType = (RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2);
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;  
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;
  
  ret = HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_6);
  if(ret != HAL_OK)
  {
    while(1) { ; }
  }
  
  HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);  

}

/**
  * @brief  CPU L1-Cache enable.
  * @param  None
  * @retval None
  */
static void CPU_CACHE_Enable(void)
{
  /* Enable I-Cache */
  SCB_EnableICache();

  /* Enable D-Cache */
  SCB_EnableDCache();
}

/**
  * @brief  Configure the MPU attributes as Write Through for SRAM1/2.
  * @note   The Base Address is 0x20010000 since this memory interface is the AXI.
  *         The Region Size is 256KB, it is related to SRAM1 and SRAM2  memory size.
  * @param  None
  * @retval None
  */
static void MPU_Config(void)
{
  MPU_Region_InitTypeDef MPU_InitStruct;
  
  /* Disable the MPU */
  HAL_MPU_Disable();

  /* Configure the MPU attributes as WT for SRAM */
  MPU_InitStruct.Enable = MPU_REGION_ENABLE;
  MPU_InitStruct.BaseAddress = 0x20010000;
  MPU_InitStruct.Size = MPU_REGION_SIZE_256KB;
  MPU_InitStruct.AccessPermission = MPU_REGION_FULL_ACCESS;
  MPU_InitStruct.IsBufferable = MPU_ACCESS_NOT_BUFFERABLE;
  MPU_InitStruct.IsCacheable = MPU_ACCESS_CACHEABLE;
  MPU_InitStruct.IsShareable = MPU_ACCESS_NOT_SHAREABLE;
  MPU_InitStruct.Number = MPU_REGION_NUMBER0;
  MPU_InitStruct.TypeExtField = MPU_TEX_LEVEL0;
  MPU_InitStruct.SubRegionDisable = 0x00;
  MPU_InitStruct.DisableExec = MPU_INSTRUCTION_ACCESS_ENABLE;

  HAL_MPU_ConfigRegion(&MPU_InitStruct);

  /* Enable the MPU */
  HAL_MPU_Enable(MPU_PRIVILEGED_DEFAULT);
}

/**
  * @brief This function provides accurate delay (in milliseconds) based 
  *        on SysTick counter flag.
  * @note This function is declared as __weak to be overwritten in case of other
  *       implementations in user file.
  * @param Delay: specifies the delay duration in milliseconds.
  * @retval None
  */
void HAL_Delay (__IO uint32_t Delay)
{
  while(Delay) 
  {
    if (SysTick->CTRL & SysTick_CTRL_COUNTFLAG_Msk) 
    {
      Delay--;
    }
  }
}

#ifdef  USE_FULL_ASSERT

/**
  * @brief  Reports the name of the source file and the source line number
  *   where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t* file, uint32_t line)
{
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
  while (1)
  {}
}
#endif