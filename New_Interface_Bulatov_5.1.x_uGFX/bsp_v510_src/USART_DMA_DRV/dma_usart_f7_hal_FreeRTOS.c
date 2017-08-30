#include "dma_usart_f7_hal.h"
//#include "spectra_data.h"
//#include "stdlib.h"
//#include "spectra_bug.h"

UART_HandleTypeDef huart1;
DMA_HandleTypeDef hdma_usart1_rx;
DMA_HandleTypeDef hdma_usart1_tx;
DMA_HandleTypeDef hdma_memtomem_dma2_stream1;

static osTimerId Timeout_TimerId;

// Состояние автомата по обмену данными USART
static UsartState_TypeDef UsartState = UsartState_Idle;

void DMA_CopyCpltCallback(DMA_HandleTypeDef * hdma);

static void Timeout_TimerCallback(void const *n);

void USART_DMA_AutomatTask(void const * argument);

/**
  //=================================================================
  //         Инициализируем USART1, задаём скорость обмена
  //=================================================================
  */
static void USART1_Init (void)
{
    GPIO_InitTypeDef GPIO_InitStruct;
	
    // Включаем тактирование USART и GPIO
    __USART1_CLK_ENABLE();
    __GPIOA_CLK_ENABLE();
    __GPIOE_CLK_ENABLE();
    __GPIOF_CLK_ENABLE();
	
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
    
    huart1.Instance = USART1;
    huart1.Init.BaudRate = 1000000;
    huart1.Init.WordLength = UART_WORDLENGTH_8B;
    huart1.Init.StopBits = UART_STOPBITS_1;
    huart1.Init.Parity = UART_PARITY_NONE;
    huart1.Init.Mode = UART_MODE_TX_RX;
    huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
    huart1.Init.OverSampling = UART_OVERSAMPLING_16;
    huart1.Init.OneBitSampling = UART_ONEBIT_SAMPLING_ENABLED;
    huart1.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
    HAL_UART_Init(&huart1);
	
	/*-RST-PE6/CTS-PE5-*/
    GPIO_InitStruct.Pin = GPIO_PIN_6;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;
    HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);
	
	HAL_GPIO_WritePin(GPIOE, GPIO_PIN_6, GPIO_PIN_SET);
    
    HAL_GPIO_WritePin(GPIOF, GPIO_PIN_7, GPIO_PIN_RESET);
        
    HAL_NVIC_SetPriority(USART1_IRQn, 5, 0);
    HAL_NVIC_EnableIRQ(USART1_IRQn);
}

/**
  //=======================================================================
  //   Инициализируем DMA2 для обмена данными с платой управления
  //=======================================================================
  */
static void USART_DMA_Init (void)
{
      /* DMA controller clock enable */
      __HAL_RCC_DMA2_CLK_ENABLE();

      /* DMA interrupt init */
      HAL_NVIC_SetPriority(DMA2_Stream5_IRQn, 5, 0);
      HAL_NVIC_EnableIRQ(DMA2_Stream5_IRQn);
  
      HAL_NVIC_SetPriority(DMA2_Stream7_IRQn, 5, 0);
      HAL_NVIC_EnableIRQ(DMA2_Stream7_IRQn);
  
      HAL_NVIC_SetPriority(DMA2_Stream1_IRQn, 5, 0);
      HAL_NVIC_EnableIRQ(DMA2_Stream1_IRQn);
   
      /* configure Rx DMA stream*/
      hdma_usart1_rx.Instance = DMA2_Stream5;
      hdma_usart1_rx.Init.Channel = DMA_CHANNEL_4;
      hdma_usart1_rx.Init.Direction = DMA_PERIPH_TO_MEMORY;
      hdma_usart1_rx.Init.PeriphInc = DMA_PINC_DISABLE;
      hdma_usart1_rx.Init.MemInc = DMA_MINC_ENABLE;
      hdma_usart1_rx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
      hdma_usart1_rx.Init.MemDataAlignment = DMA_MDATAALIGN_HALFWORD;
      hdma_usart1_rx.Init.Mode = DMA_NORMAL;
      hdma_usart1_rx.Init.Priority = DMA_PRIORITY_VERY_HIGH;
      hdma_usart1_rx.Init.FIFOMode = DMA_FIFOMODE_DISABLE;
      hdma_usart1_rx.Init.FIFOThreshold = DMA_FIFO_THRESHOLD_FULL;
      hdma_usart1_rx.Init.MemBurst = DMA_MBURST_SINGLE;
      hdma_usart1_rx.Init.PeriphBurst = DMA_PBURST_SINGLE;
      HAL_DMA_Init(&hdma_usart1_rx);
      
      __HAL_LINKDMA(&huart1,hdmarx,hdma_usart1_rx);
      
      /* configure Tx DMA stream*/
      hdma_usart1_tx.Instance = DMA2_Stream7;
      hdma_usart1_tx.Init.Channel = DMA_CHANNEL_4;
      hdma_usart1_tx.Init.Direction = DMA_MEMORY_TO_PERIPH;
      hdma_usart1_tx.Init.PeriphInc = DMA_PINC_DISABLE;
      hdma_usart1_tx.Init.MemInc = DMA_MINC_ENABLE;
      hdma_usart1_tx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
      hdma_usart1_tx.Init.MemDataAlignment = DMA_MDATAALIGN_HALFWORD;
      hdma_usart1_tx.Init.Mode = DMA_NORMAL;
      hdma_usart1_tx.Init.Priority = DMA_PRIORITY_HIGH;
      hdma_usart1_tx.Init.FIFOMode = DMA_FIFOMODE_DISABLE;
      hdma_usart1_tx.Init.FIFOThreshold = DMA_FIFO_THRESHOLD_FULL;
      hdma_usart1_tx.Init.MemBurst = DMA_MBURST_SINGLE;
      hdma_usart1_tx.Init.PeriphBurst = DMA_PBURST_SINGLE;
      HAL_DMA_Init(&hdma_usart1_tx);

      __HAL_LINKDMA(&huart1,hdmatx,hdma_usart1_tx);
      
      /* configure copying DMA stream*/
      hdma_memtomem_dma2_stream1.Instance = DMA2_Stream1;
      hdma_memtomem_dma2_stream1.Init.Channel = DMA_CHANNEL_3;
      hdma_memtomem_dma2_stream1.Init.Direction = DMA_MEMORY_TO_MEMORY;
      hdma_memtomem_dma2_stream1.Init.PeriphInc = DMA_PINC_ENABLE;
      hdma_memtomem_dma2_stream1.Init.MemInc = DMA_MINC_ENABLE;
      
      //копирование побайтное! - иначе надо изменить задание размара данных для копирования в HAL_DMA_Start_IT
      hdma_memtomem_dma2_stream1.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
      hdma_memtomem_dma2_stream1.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
      
      hdma_memtomem_dma2_stream1.Init.Mode = DMA_NORMAL;
      hdma_memtomem_dma2_stream1.Init.Priority = DMA_PRIORITY_HIGH;
      hdma_memtomem_dma2_stream1.Init.FIFOMode = DMA_FIFOMODE_ENABLE;
      hdma_memtomem_dma2_stream1.Init.FIFOThreshold = DMA_FIFO_THRESHOLD_FULL;
      hdma_memtomem_dma2_stream1.Init.MemBurst = DMA_MBURST_SINGLE;
      hdma_memtomem_dma2_stream1.Init.PeriphBurst = DMA_PBURST_SINGLE;
      hdma_memtomem_dma2_stream1.XferCpltCallback = &DMA_CopyCpltCallback;//регистрирую функцию обратного вызова при завершении процесса копирования
      HAL_DMA_Init(&hdma_memtomem_dma2_stream1);
}

/**
  * Основная функция инициализации. Выполняется как инициализация всей необходимой периферии, так и создание задачи и софт таймера, обеспечивающих функционирование модуля UART.
  */
void UART_module_init()
{
  USART_DMA_Init();
  
  USART1_Init();
  
  /* Create Timeout Timer */
  osTimerDef(Timeout_Timer, Timeout_TimerCallback);
  Timeout_TimerId =  osTimerCreate(osTimer(Timeout_Timer), osTimerOnce, (void *)0);
  
  osThreadDef(UART_DMAUTOMAT, USART_DMA_AutomatTask, osPriorityHigh, 0, 128);
  osThreadCreate(osThread(UART_DMAUTOMAT), NULL);//создаем основную задачу управления обменом данными по uart
}

/*----------------------------------------------------------------------------*/

/**
  * Служебный обработчик прерывания от UART1, без него передающий канал в реализации HAL не работает.
  */
void USART1_IRQHandler (void)
{
  HAL_UART_IRQHandler(&huart1);
}

/**
  * Служебный обработчик прерывания от DMA, для приемного потока.
  */
void DMA2_Stream5_IRQHandler (void)
{
  HAL_DMA_IRQHandler(&hdma_usart1_rx);
}

/**
  * Служебный обработчик прерывания от DMA, для передающего потока. 
  */
void DMA2_Stream7_IRQHandler (void)
{
  HAL_DMA_IRQHandler(&hdma_usart1_tx);
}

/**
  * Служебный обработчик прерывания от DMA, для потока копирования данных. 
  */
void DMA2_Stream1_IRQHandler (void)
{
  HAL_DMA_IRQHandler(&hdma_memtomem_dma2_stream1);
}

/*----------------------------------------------------------------------------*/

/**
  * Обработчик события срабатывания софт таймера - истечение таймаута ожидания приема данных
  */
static void Timeout_TimerCallback(void const *n)
{  
  UsartState=UsartState_TimError; //модифицируем статус модуля
}

/**
  * Обратный вызов после прерывания о успешном завершении приема
  */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
 {
	HAL_GPIO_WritePin(GPIOF, GPIO_PIN_7, GPIO_PIN_RESET);//LED HL2 (DATA_REQ) - OFF
	UsartState = UsartState_RxOk;  //модифицируем статус модуля
 }

/**
  * Обратный вызов после прерывания о успешном завершении передачи
  */
void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
 {
        HAL_GPIO_WritePin(GPIOF, GPIO_PIN_8, GPIO_PIN_RESET);//LED - HL3 OFF
	UsartState = UsartState_TxOk;  //модифицируем статус модуля
 }

/**
  * Обратный вызов после прерывания о успешном завершении копирования
  */
void DMA_CopyCpltCallback(DMA_HandleTypeDef * hdma)
 {
        UsartState = UsartState_CopyRxOk;  //модифицируем статус модуля
        
        //тут надо дать знать vSpectraCalcAutomat о появлении данных для обработки
 }

/*----------------------------------------------------------------------------*/

/**
//=============================================================
//  Задача - автомат по обмену данными через USART1 и DMA2
//  Отправка комманд CMD_SETPARAMS и CMD_SETKEYS
//  Приём команд CMD_GETPARAMS и CMD_GETDATA
//=============================================================
//  Размер команд:
//  CMD_SETPARAMS - 6 байт
//  CMD_GETPARAMS - 6+20 байт
//  CMD_GETDATA - 6+2048 байт // неактулально + SPECCTRA_SIZE
//  CMD_SETKEYS - 6+4 байт
//=============================================================
//  Проверяет контрольные суммы принятых данных
//  Если CRC не совпадает, выводит на дисплей надпись "CRC"
//  Если CRC совпадает, выполняет ряд задач (см.ниже)
//=============================================================
//  Проверяет корректность CmdId==0x02 и CmdId2==0x03
//=============================================================
*/
void USART_DMA_AutomatTask(void const * argument)
{
  portTickType xLastWakeTime;
  
  xLastWakeTime = xTaskGetTickCount();
  
  for(;;)
  {
	switch (UsartState)
	{
		case UsartState_Idle:
		{
                        osTimerStop(Timeout_TimerId);//останаливаю таймер/сбрасываю таймер
                  
			HAL_GPIO_WritePin(GPIOE, GPIO_PIN_6, GPIO_PIN_SET);//////// нога RTS вверх ////////

                        HAL_UART_DMAStop(&huart1);
                        
                        HAL_UART_Receive_DMA(&huart1, (uint8_t*)&UsartGETPARAMS, sizeof(GETPARAMS));
                                                
			SETPARAMS.iCrc = Crc16((uint8_t*)&SETPARAMS, 4); // Расчёт crc SETPARAMS
			SETPARAMS.iCrc2 = Crc16((uint8_t*)&SETPARAMS.iMagicNumber2, 4);  // Расчёт crc SETKEYS			
                        
                        osTimerStart(Timeout_TimerId, 500);//таймаут 500 мс - запустить soft timer на 500 мс
			
                        HAL_GPIO_WritePin(GPIOF, GPIO_PIN_7, GPIO_PIN_SET);//LED HL2 (DATA_REQ) - ON
                        UsartState = UsartState_WaitRx;
			HAL_GPIO_WritePin(GPIOE, GPIO_PIN_6, GPIO_PIN_RESET);//////// нога RTS вниз /////////
		}; break;
		case UsartState_WaitRx:
		{
			
		}; break;
		case UsartState_RxOk:
		{
                        HAL_UART_DMAStop(&huart1);
			HAL_GPIO_WritePin(GPIOF, GPIO_PIN_8, GPIO_PIN_SET);//LED - HL3 ON
			UsartState = UsartState_WaitTx;   
                        
                        HAL_UART_Transmit_DMA(&huart1, (uint8_t*)&SETPARAMS, sizeof(SETPARAMS));
		}; break;
		case UsartState_WaitTx:
		{
			
		}; break;
		case UsartState_TxOk:  // data TX OK!!!, обмен данными завершён
		{
                        HAL_GPIO_WritePin(GPIOE, GPIO_PIN_6, GPIO_PIN_SET);//////// нога RTS вверх ////////
			
			#ifndef SKIP_CRC_CHECKING
			// Проверяю контрольные суммы
			uint16_t checksum1= Crc16((uint8_t*) &UsartGETPARAMS, 4);  // Расчёт crc CMD_GETPARAMS
			uint16_t checksum2= Crc16((uint8_t*) &UsartGETPARAMS.iMagicNumber2, 4);  // Расчёт crc CMD_GETDATA
			if ((UsartGETPARAMS.iCrc != checksum1) || (UsartGETPARAMS.iCrc2 != checksum2))  // Сравниваю принятые CRC с расcчитанными CRC
			{
				// CRC не совпала
				UsartState = UsartState_CrcError;
			}
			else
			{
				// CRC совпала
				// Проверка команд CMD_GETPARAMS и CMD_GETDATA
				if ((UsartGETPARAMS.iCmdId==0x02) && (UsartGETPARAMS.iCmdId2==CMD_GETMULTIDATA))
				{
                                        // Теперь при помощи DMA перекидываю данные из UsartGETPARAMS в GETPARAMS
					#endif
                                        UsartState = UsartState_WaitCopyRx;

                                        HAL_DMA_Start_IT(&hdma_memtomem_dma2_stream1, (uint32_t)&UsartGETPARAMS, (uint32_t)&GETPARAMS, sizeof(GETPARAMS));

					#ifndef SKIP_CRC_CHECKING
				}
				else
				{
					UsartState = UsartState_Idle;  // Сбрасываю статус и начинаю сначала
				}
			}
			#endif
		}; break;
		case UsartState_WaitCopyRx:
		{
			// Жду, пока данные перекинутся из UsartGETPARAMS в GETPARAMS
		}; break;
		case UsartState_CopyRxOk:
		{
			static uint32_t quiet = 0;
                        // Данные перекинулись
                        if (!quiet--)
                         { 
                           UsartState = UsartState_Idle; // Начинаю новый обмен данными
                           quiet = 0;
                         }
                        
		}; break;
		case UsartState_CrcError:
		{
                        //обращение к LCD модулю - будет реализовано через уведомления
                        if (DisplayMode!=DisplayMode_PowerOff)
                        {
                          LCDBUFFER_Str("CRC",185,285,RED,BLACK,2,2);
			}
                        //Task_add(TASK_LCD_UPDATE);
                        
			UsartState = UsartState_Idle;  // Сбрасываю статус и начинаю сначала
		}; break;
		case UsartState_TimError:
		{
                        //обращение к LCD модулю - будет реализовано через уведомления
                        if (DisplayMode!=DisplayMode_PowerOff)
                        {
                          LCDBUFFER_Str("TIM",185,285,RED,BLACK,2,2);
                        }
			//Task_add(TASK_LCD_UPDATE);
                        
			UsartState = UsartState_Idle;  // Сбрасываю статус и начинаю сначала
		}; break;
	};
  vTaskDelayUntil(&xLastWakeTime,(1/portTICK_RATE_MS));
  }
}


UsartState_TypeDef eUSARTgetState(void)
{
  return (UsartState);
}
/*----------------------------------------------------------------------------*/















































