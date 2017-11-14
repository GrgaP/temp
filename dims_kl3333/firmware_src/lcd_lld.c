#include "stm32f7xx_hal.h"
#include "cmsis_os.h"

UART_HandleTypeDef huart1;
UART_HandleTypeDef huart3;
DMA_HandleTypeDef hdma_usart1_tx;
DMA_HandleTypeDef hdma_usart1_rx;
DMA_HandleTypeDef hdma_usart3_rx;
DMA_HandleTypeDef hdma_usart3_tx;
osThreadId uart_run_Handle;
osMessageQId myQueue01Handle;
osTimerId uart1_timerHandle;
osTimerId uart3_timerHandle;

osSemaphoreId uart1_rx_idle;
osSemaphoreId uart1_rx_cplt;
osSemaphoreId uart1_rxh_cplt;
osSemaphoreId uart1_tx_cplt;
osSemaphoreId uart1_txh_cplt;

osSemaphoreId uart3_rx_idle;
osSemaphoreId uart3_rx_cplt;
osSemaphoreId uart3_rxh_cplt;
osSemaphoreId uart3_tx_cplt;
osSemaphoreId uart3_txh_cplt;

typedef enum {
	IDLE,
	ACTIVE,
	COMPLETE,	
}uart_sm_states;

uart_sm_states uart1_tx_state, uart1_rx_state, uart3_tx_state, uart3_rx_state; 

typedef enum uart_drv_states{
	UART_UNINIT,
	UART_STOP,
	UART_READY,
}uart1_drv_state, uart3_drv_state;


uint8_t uart1_rx_buffer[64];
uint8_t uart1_tx_buffer[64];

uint8_t uart3_rx_buffer[512];
uint8_t uart3_tx_buffer[64];

void Uart_run_Task(void const * argument);
static void uart1_timeout(void const * argument);
static void uart3_timeout(void const * argument);

void HAL_UART_MspInit(UART_HandleTypeDef* huart)
{

  GPIO_InitTypeDef GPIO_InitStruct;
  if(huart->Instance==USART1)
  {
  /* USER CODE BEGIN USART1_MspInit 0 */

  /* USER CODE END USART1_MspInit 0 */
    /* Peripheral clock enable */
    __USART1_CLK_ENABLE();
  
    /**USART1 GPIO Configuration    
    PA9     ------> USART1_TX
    PA10     ------> USART1_RX 
    */
    GPIO_InitStruct.Pin = GPIO_PIN_9|GPIO_PIN_10;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF7_USART1;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    /* Peripheral DMA init*/
  
    hdma_usart1_tx.Instance = DMA2_Stream7;
    hdma_usart1_tx.Init.Channel = DMA_CHANNEL_4;
    hdma_usart1_tx.Init.Direction = DMA_MEMORY_TO_PERIPH;
    hdma_usart1_tx.Init.PeriphInc = DMA_PINC_DISABLE;
    hdma_usart1_tx.Init.MemInc = DMA_MINC_ENABLE;
    hdma_usart1_tx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
    hdma_usart1_tx.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
    hdma_usart1_tx.Init.Mode = DMA_NORMAL;
    hdma_usart1_tx.Init.Priority = DMA_PRIORITY_LOW;
    hdma_usart1_tx.Init.FIFOMode = DMA_FIFOMODE_DISABLE;
    HAL_DMA_Init(&hdma_usart1_tx);

    __HAL_LINKDMA(huart,hdmatx,hdma_usart1_tx);

    hdma_usart1_rx.Instance = DMA2_Stream2;
    hdma_usart1_rx.Init.Channel = DMA_CHANNEL_4;
    hdma_usart1_rx.Init.Direction = DMA_PERIPH_TO_MEMORY;
    hdma_usart1_rx.Init.PeriphInc = DMA_PINC_DISABLE;
    hdma_usart1_rx.Init.MemInc = DMA_MINC_ENABLE;
    hdma_usart1_rx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
    hdma_usart1_rx.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
    hdma_usart1_rx.Init.Mode = DMA_NORMAL;
    hdma_usart1_rx.Init.Priority = DMA_PRIORITY_LOW;
    hdma_usart1_rx.Init.FIFOMode = DMA_FIFOMODE_DISABLE;
    HAL_DMA_Init(&hdma_usart1_rx);

    __HAL_LINKDMA(huart,hdmarx,hdma_usart1_rx);

  /* USER CODE BEGIN USART1_MspInit 1 */

  /* USER CODE END USART1_MspInit 1 */
  }
  else if(huart->Instance==USART3)
  {
  /* USER CODE BEGIN USART3_MspInit 0 */

  /* USER CODE END USART3_MspInit 0 */
    /* Peripheral clock enable */
    __USART3_CLK_ENABLE();
  
    /**USART3 GPIO Configuration    
    PC10     ------> USART3_TX
    PC11     ------> USART3_RX 
    */
    GPIO_InitStruct.Pin = GPIO_PIN_10|GPIO_PIN_11;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF7_USART3;
    HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

    /* Peripheral DMA init*/
  
    hdma_usart3_rx.Instance = DMA1_Stream1;
    hdma_usart3_rx.Init.Channel = DMA_CHANNEL_4;
    hdma_usart3_rx.Init.Direction = DMA_PERIPH_TO_MEMORY;
    hdma_usart3_rx.Init.PeriphInc = DMA_PINC_DISABLE;
    hdma_usart3_rx.Init.MemInc = DMA_MINC_ENABLE;
    hdma_usart3_rx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
    hdma_usart3_rx.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
    hdma_usart3_rx.Init.Mode = DMA_NORMAL;
    hdma_usart3_rx.Init.Priority = DMA_PRIORITY_HIGH;
    hdma_usart3_rx.Init.FIFOMode = DMA_FIFOMODE_DISABLE;
    HAL_DMA_Init(&hdma_usart3_rx);

    __HAL_LINKDMA(huart,hdmarx,hdma_usart3_rx);

    hdma_usart3_tx.Instance = DMA1_Stream3;
    hdma_usart3_tx.Init.Channel = DMA_CHANNEL_4;
    hdma_usart3_tx.Init.Direction = DMA_MEMORY_TO_PERIPH;
    hdma_usart3_tx.Init.PeriphInc = DMA_PINC_DISABLE;
    hdma_usart3_tx.Init.MemInc = DMA_MINC_ENABLE;
    hdma_usart3_tx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
    hdma_usart3_tx.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
    hdma_usart3_tx.Init.Mode = DMA_NORMAL;
    hdma_usart3_tx.Init.Priority = DMA_PRIORITY_LOW;
    hdma_usart3_tx.Init.FIFOMode = DMA_FIFOMODE_DISABLE;
    HAL_DMA_Init(&hdma_usart3_tx);

    __HAL_LINKDMA(huart,hdmatx,hdma_usart3_tx);

  /* Peripheral interrupt init*/
    HAL_NVIC_SetPriority(USART3_IRQn, 5, 0);
    HAL_NVIC_EnableIRQ(USART3_IRQn);
  /* USER CODE BEGIN USART3_MspInit 1 */

  /* USER CODE END USART3_MspInit 1 */
  }

}

void HAL_UART_MspDeInit(UART_HandleTypeDef* huart)
{

  if(huart->Instance==USART1)
  {
  /* USER CODE BEGIN USART1_MspDeInit 0 */

  /* USER CODE END USART1_MspDeInit 0 */
    /* Peripheral clock disable */
    __USART1_CLK_DISABLE();
  
    /**USART1 GPIO Configuration    
    PA9     ------> USART1_TX
    PA10     ------> USART1_RX 
    */
    HAL_GPIO_DeInit(GPIOA, GPIO_PIN_9|GPIO_PIN_10);

    /* Peripheral DMA DeInit*/
    HAL_DMA_DeInit(huart->hdmatx);
    HAL_DMA_DeInit(huart->hdmarx);
  /* USER CODE BEGIN USART1_MspDeInit 1 */

  /* USER CODE END USART1_MspDeInit 1 */
  }
  else if(huart->Instance==USART3)
  {
  /* USER CODE BEGIN USART3_MspDeInit 0 */

  /* USER CODE END USART3_MspDeInit 0 */
    /* Peripheral clock disable */
    __USART3_CLK_DISABLE();
  
    /**USART3 GPIO Configuration    
    PC10     ------> USART3_TX
    PC11     ------> USART3_RX 
    */
    HAL_GPIO_DeInit(GPIOC, GPIO_PIN_10|GPIO_PIN_11);

    /* Peripheral DMA DeInit*/
    HAL_DMA_DeInit(huart->hdmarx);
    HAL_DMA_DeInit(huart->hdmatx);

    /* Peripheral interrupt DeInit*/
    HAL_NVIC_DisableIRQ(USART3_IRQn);

  /* USER CODE BEGIN USART3_MspDeInit 1 */

  /* USER CODE END USART3_MspDeInit 1 */
  }

}

/* USART1 init function */
static void MX_USART1_UART_Init(void)
{

  huart1.Instance = USART1;
  huart1.Init.BaudRate = 115200;
  huart1.Init.WordLength = UART_WORDLENGTH_8B;
  huart1.Init.StopBits = UART_STOPBITS_1;
  huart1.Init.Parity = UART_PARITY_NONE;
  huart1.Init.Mode = UART_MODE_TX_RX;
  huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart1.Init.OverSampling = UART_OVERSAMPLING_16;
  huart1.Init.OneBitSampling = UART_ONEBIT_SAMPLING_DISABLED;
  huart1.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  HAL_UART_Init(&huart1);

}

/* USART3 init function */
static void MX_USART3_UART_Init(void)
{

  huart3.Instance = USART3;
  huart3.Init.BaudRate = 9600;
  huart3.Init.WordLength = UART_WORDLENGTH_8B;
  huart3.Init.StopBits = UART_STOPBITS_1;
  huart3.Init.Parity = UART_PARITY_NONE;
  huart3.Init.Mode = UART_MODE_TX_RX;
  huart3.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart3.Init.OverSampling = UART_OVERSAMPLING_16;
  huart3.Init.OneBitSampling = UART_ONEBIT_SAMPLING_DISABLED;
  huart3.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  HAL_UART_Init(&huart3);
}

void uart_lld_init()
{
	
  MX_USART1_UART_Init();
  MX_USART3_UART_Init();
  
    /* Create the semaphores(s) */
  /* definition and creation of uart1_rx_idle */
  osSemaphoreDef(uart1_rx_idle);
  uart1_rx_idle = osSemaphoreCreate(osSemaphore(uart1_rx_idle), 2);
	/* Create the semaphores(s) */
  /* definition and creation of uart1_rx_ok */
  osSemaphoreDef(uart1_rx_cplt);
  uart1_rx_cplt = osSemaphoreCreate(osSemaphore(uart1_rx_cplt), 2);
   	/* Create the semaphores(s) */
  /* definition and creation of uart1_rx_ok */
  osSemaphoreDef(uart1_rxh_cplt);
  uart1_rxh_cplt = osSemaphoreCreate(osSemaphore(uart1_rxh_cplt), 2);
  	/* Create the semaphores(s) */
  /* definition and creation of uart1_tx_ok */
  osSemaphoreDef(uart1_tx_cplt);
  uart1_tx_cplt = osSemaphoreCreate(osSemaphore(uart1_tx_cplt), 2);
   	/* Create the semaphores(s) */
  /* definition and creation of uart1_tx_ok */
  osSemaphoreDef(uart1_txh_cplt);
  uart1_txh_cplt = osSemaphoreCreate(osSemaphore(uart1_txh_cplt), 2);
    /* Create the timer(s) */
  /* definition and creation of uart1_timer */
  osTimerDef(uart1_timer, uart1_timeout);
  uart1_timerHandle = osTimerCreate(osTimer(uart1_timer), osTimerOnce, NULL);
  
  
  /* Create the semaphores(s) */
  /* definition and creation of uart3_rx_idle */
  osSemaphoreDef(uart3_rx_idle);
  uart3_rx_idle = osSemaphoreCreate(osSemaphore(uart3_rx_idle), 2);
	/* Create the semaphores(s) */
  /* definition and creation of uart1_rx_ok */
  osSemaphoreDef(uart3_rx_cplt);
  uart3_rx_cplt = osSemaphoreCreate(osSemaphore(uart1_rx_cplt), 2);
   	/* Create the semaphores(s) */
  /* definition and creation of uart1_rx_ok */
  osSemaphoreDef(uart3_rxh_cplt);
  uart3_rxh_cplt = osSemaphoreCreate(osSemaphore(uart1_rxh_cplt), 2);
  	/* Create the semaphores(s) */
  /* definition and creation of uart1_tx_ok */
  osSemaphoreDef(uart3_tx_cplt);
  uart3_tx_cplt = osSemaphoreCreate(osSemaphore(uart1_tx_cplt), 2);
   	/* Create the semaphores(s) */
  /* definition and creation of uart1_tx_ok */
  osSemaphoreDef(uart3_txh_cplt);
  uart3_txh_cplt = osSemaphoreCreate(osSemaphore(uart1_txh_cplt), 2);
	/* Create the timer(s) */
  /* definition and creation of uart3_timer */
  osTimerDef(uart3_timer, uart3_timeout);
  uart3_timerHandle = osTimerCreate(osTimer(uart3_timer), osTimerOnce, NULL);
  
	/* Create the thread(s) */
  /* definition and creation of defaultTask */
  osThreadDef(uart_run_Task, Uart_run_Task, osPriorityNormal, 0, 128);
  uart_run_Handle = osThreadCreate(osThread(uart_run_Task), NULL);  
  
    /* Create the queue(s) */
  /* definition and creation of myQueue01 */
  osMessageQDef(myQueue01, 16, uint8_t);
  myQueue01Handle = osMessageCreate(osMessageQ(myQueue01), NULL);
	
}

/*			HAL_UART_DMAStop(huart);
			if(check_rx_data())
				;
			else
				;*/
			
void HAL_UART_RxIdleCallback(UART_HandleTypeDef *huart)
{
  USART_TypeDef* uart_n = huart->Instance;
  
	switch((uint32_t)uart_n)
	{
		case (uint32_t) USART1:
		{
			osSemaphoreRelease(uart1_rx_idle);
		}; break;
		case (uint32_t) USART3:
		{
			osSemaphoreRelease(uart3_rx_idle);
		}; break;
		default:
		break;
	};
}
/*
void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
{
	switch(huart->Instance)
	{
		case UART1:
			osSemaphoreRelease(uart1_tx_cplt); break;
		case UART3:
			osSemaphoreRelease(uart3_tx_cplt); break;
		default:
		break;
	};
}

void HAL_UART_TxHalfCpltCallback(UART_HandleTypeDef *huart)
{
	switch(huart->Instance)
	{
		case UART1:
			osSemaphoreRelease(uart1_txh_cplt); break;
		case UART3:
			osSemaphoreRelease(uart3_txh_cplt); break;
		default:
		break;
	};
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	switch(huart->Instance)
	{
		case UART1:
			osSemaphoreRelease(uart1_rx_cplt); break;
		case UART3:
			osSemaphoreRelease(uart3_rx_cplt); break;
		default:
		break;
	};
}

void HAL_UART_RxHalfCpltCallback(UART_HandleTypeDef *huart)
{
	switch(huart->Instance)
	{
		case UART1:
			osSemaphoreRelease(uart1_rxh_cplt); break;
		case UART3:
			osSemaphoreRelease(uart3_rxh_cplt); break;
		default:
		break;
	};
}

void HAL_UART_ErrorCallback(UART_HandleTypeDef *huart)
{
	switch(huart->Instance)
	{
		case UART1:
		{}; break;
		case UART3:
		{
			
		}; break
		default:
		break;
	};
	
}

*/
static uint16_t valid_check(uint8_t* buffer, uint16_t size)
{
	uint16_t i, retval = 0x01;
	
	if(buffer[0] == 0x24)//start symbol is "$"
	{
		for(i=0; i<size; i++)
		{
			if((buffer[i] !=  0x2F)&&(buffer[i+1] != 0x6e))//searching of stop symbols "/" and "n"
				break;
			else
			{
				if(i== size-1)
					retval = 0x00;
			}
							
		}
	}
	else
		retval =  0x00;
	
	return retval;	
};

static void uart1_sm(void)
{
	
}

static void uart3_sm(void)
{
  static uint16_t buff_item_num = 0;
    
	switch(uart3_rx_state)
	{
          case IDLE:
          {
		//Check line state
		HAL_UART_Receive_DMA(&huart3, uart3_rx_buffer, sizeof(uart3_rx_buffer));  
		uart3_rx_state = ACTIVE;				
          }; 
          break;
          case ACTIVE:
          {
		osSemaphoreWait(uart3_rx_idle, osWaitForever);
		HAL_UART_DMAStop(&huart3);
                
                buff_item_num = sizeof(uart3_rx_buffer) - (uint16_t)(huart3.hdmarx->Instance->NDTR);
                
                if(buff_item_num == 0)
                  
                
                //if(huart3.hdmarx->CNDTR != 0x00)
                                                  
                
                
		uart3_rx_state = COMPLETE;	
                  uart3_rx_state = IDLE;
		//rxc - to COMPLETE
		//rxhc - to ACTIVE
		//rxidle - to COMPLETE
		
		
		
          }; break;
          case COMPLETE:
          {
		valid_check(uart3_rx_buffer, buff_item_num);//validation checking of received data (go to user function)
		
		//copy received data to user app (go to user function)		
		
          }; break;
        default:
                 break;
/*	
	switch(uart3_tx_state)
	case IDLE:
	{
		//waiting data from user app
		HAL_UART_Receive_DMA(&huart1, uart1_rx_buffer, sizeof(uart1_rx_buffer));  
		uart1_rx_state = ACTIVE;	
	}; break;
	case ACTIVE:
	{
		if()//txc waiting			
			//go to complete
		
	}; break;
	case COMPLETE
	{
		//call user app function
		
		
	}; break;
	default
		break;
		*/
        }
}
/*
static inline void uart1_start_rcv()
{
	
	//start timer for timeout waiting
}*/
/*
static inline void uart3_start_rcv()
{
	HAL_UART_Receive_DMA(&huart3, uart3_rx_buffer, sizeof(uart3_rx_buffer));
	uart3_rx_state = ACTIVE;
	//start timer for timeout waiting
}
*/
/* StartDefaultTask function */
void Uart_run_Task(void const * argument)
{

  /* USER CODE BEGIN 5 */
  
  //uart3_tx_();
  //uart1_rx_waiting_start();
  //uart3_rx_waiting_start();
  
  

  
  /* Infinite loop */
  for(;;)
  {
	uart1_sm(); 
	uart3_sm();
	
    osDelay(10);
  }
  /* USER CODE END 5 */ 
}

/* uart1_timeout function */
static void uart1_timeout(void const * argument)
{
  /* USER CODE BEGIN uart1_timeout */
  
  /* USER CODE END uart1_timeout */
}

/* uart3_timeout function */
static void uart3_timeout(void const * argument)
{
  /* USER CODE BEGIN uart3_timeout */
  
  /* USER CODE END uart3_timeout */
}