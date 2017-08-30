#include "dma_usart_f7.h"
#include "spectra_data.h"
#include "stdlib.h"
#include "spectra_bug.h"

UART_HandleTypeDef huart1;
DMA_HandleTypeDef hdma_usart1_rx;
DMA_HandleTypeDef hdma_usart1_tx;
DMA_HandleTypeDef hdma_memtomem_dma2_stream1;

// Состояние автомата по обмену данными USART
UsartState_TypeDef UsartState = UsartState_Idle;
// Переменная для контроля таймаута при обмене данными.
// Декрементируется в прерывании опроса клавиатуры.
// Если usart_RX_timeout==0, значит таймаут вышел, и необходим перезапуск.
uint8_t usart_RX_timeout=0;

void SVD_DMA_CpltCallback(void);

void USART_DMA_Automat(void);

//=================================================================
//         Инициализируем USART1, задаём скорость обмена
//=================================================================
static void USART1_Init (void)
{
	
	
    GPIO_InitTypeDef GPIO_InitStruct;
	
	// Включаем тактирование USART и GPIO
    __USART1_CLK_ENABLE();//
    __GPIOA_CLK_ENABLE();//
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

    // Настройка USART, все поля оставляем дефолтными, кроме скорости обмена
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
        
        //HAL_NVIC_SetPriority(USART1_IRQn, 5, 0);
        //HAL_NVIC_EnableIRQ(USART1_IRQn);        
        
}

//=======================================================================
//   Инициализируем DMA1 и DMA2 для обмена данными с платой управления
//=======================================================================
static void USART_DMA_Init (void)
{
      /* DMA controller clock enable */
      __HAL_RCC_DMA2_CLK_ENABLE();

      /* DMA interrupt init */
      HAL_NVIC_SetPriority(DMA2_Stream5_IRQn, 1, 0);
      HAL_NVIC_EnableIRQ(DMA2_Stream5_IRQn);
  
      HAL_NVIC_SetPriority(DMA2_Stream7_IRQn, 1, 0);
      HAL_NVIC_EnableIRQ(DMA2_Stream7_IRQn);
  
      HAL_NVIC_SetPriority(DMA2_Stream1_IRQn, 1, 0);
      HAL_NVIC_EnableIRQ(DMA2_Stream1_IRQn);
   
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
      
      DMA2_Stream5->PAR = (uint32_t)&(USART1->RDR);
      DMA2_Stream5->M0AR = (uint32_t)&UsartGETPARAMS;

      __HAL_LINKDMA(&huart1,hdmarx,hdma_usart1_rx);
      
      __HAL_DMA_ENABLE_IT(&hdma_usart1_rx,DMA_IT_TC);
      __HAL_DMA_ENABLE_IT(&hdma_usart1_rx,DMA_IT_TE);
      __HAL_DMA_ENABLE_IT(&hdma_usart1_rx,DMA_IT_FE);
      __HAL_DMA_ENABLE_IT(&hdma_usart1_rx,DMA_IT_DME);      
      __HAL_DMA_ENABLE(&hdma_usart1_rx);      
      
	
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
      
      DMA2_Stream7->PAR = (uint32_t)&(USART1->TDR);
      DMA2_Stream7->M0AR = (uint32_t)&SETPARAMS;

      __HAL_LINKDMA(&huart1,hdmatx,hdma_usart1_tx);
      
      __HAL_DMA_ENABLE_IT(&hdma_usart1_tx,DMA_IT_TC);
      __HAL_DMA_ENABLE_IT(&hdma_usart1_tx,DMA_IT_TE);
      __HAL_DMA_ENABLE_IT(&hdma_usart1_tx,DMA_IT_FE);
      __HAL_DMA_ENABLE_IT(&hdma_usart1_tx,DMA_IT_DME);  
	
	// UsartGETPARAMS >>> GETPARAMS
	// DMA_2, Channel_0, Stream_0
	hdma_memtomem_dma2_stream1.Instance = DMA2_Stream1;
	hdma_memtomem_dma2_stream1.Init.Channel = DMA_CHANNEL_3;
	hdma_memtomem_dma2_stream1.Init.Direction = DMA_MEMORY_TO_MEMORY;
	hdma_memtomem_dma2_stream1.Init.PeriphInc = DMA_PINC_ENABLE;
	hdma_memtomem_dma2_stream1.Init.MemInc = DMA_MINC_ENABLE;
	hdma_memtomem_dma2_stream1.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
	hdma_memtomem_dma2_stream1.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
	hdma_memtomem_dma2_stream1.Init.Mode = DMA_NORMAL;
	hdma_memtomem_dma2_stream1.Init.Priority = DMA_PRIORITY_HIGH;
	hdma_memtomem_dma2_stream1.Init.FIFOMode = DMA_FIFOMODE_ENABLE;
	hdma_memtomem_dma2_stream1.Init.FIFOThreshold = DMA_FIFO_THRESHOLD_FULL;
	hdma_memtomem_dma2_stream1.Init.MemBurst = DMA_MBURST_SINGLE;
	hdma_memtomem_dma2_stream1.Init.PeriphBurst = DMA_PBURST_SINGLE;
        HAL_DMA_Init(&hdma_memtomem_dma2_stream1);
        
        DMA2_Stream1->PAR = (uint32_t) &UsartGETPARAMS;
        DMA2_Stream1->M0AR = (uint32_t)&GETPARAMS;;
        
        __HAL_DMA_ENABLE_IT(&hdma_memtomem_dma2_stream1,DMA_IT_TC);
	
}

void UART_module_init()
{
  USART1_Init();
  USART_DMA_Init();
}

//==================================================================================================
//    Обработчик прерывания DMA Transfer complete, срабатывает когда приём данных завершён 
//==================================================================================================
void DMA2_Stream5_IRQHandler (void)
{
  DMA2->HIFCR = DMA_HIFCR_CHTIF5 | DMA_HIFCR_CTEIF5 | DMA_HIFCR_CDMEIF5 | DMA_HIFCR_CFEIF5;
  if(__HAL_DMA_GET_TC_FLAG_INDEX(&hdma_usart1_rx))
  {
    DMA2->HIFCR = DMA_HIFCR_CTCIF5;
    USART1->CR3 &= ~USART_CR3_DMAR;
    if (DMA2_Stream2->NDTR==0)
      {
        HAL_UART_RxCpltCallback(&huart1);
      }
  }
	//HAL_DMA_IRQHandler(&hdma_usart1_rx);
}


//==================================================================================================
//    Обработчик прерывания DMA Transfer complete, срабатывает когда передача данных завершёна 
//==================================================================================================
void DMA2_Stream7_IRQHandler (void)
{
  DMA2->HIFCR = DMA_HIFCR_CHTIF7 | DMA_HIFCR_CTEIF7 | DMA_HIFCR_CDMEIF7 | DMA_HIFCR_CFEIF7;  // Сбрасываю флаги
  if (__HAL_DMA_GET_TC_FLAG_INDEX(&hdma_usart1_tx))  // Если установлен флаг окончания передачи DMA
  {
	DMA2->HIFCR = DMA_HIFCR_CTCIF7;  // Сбрасываю флаг
        USART1->CR3 &= ~USART_CR3_DMAT;
	if (DMA2_Stream7->NDTR==0)  // Если DMA обработал все данные
	{
		HAL_UART_TxCpltCallback(&huart1);
	}
}
    //HAL_DMA_IRQHandler(&hdma_usart1_tx);
}

//======================================================================
// Обработчик прерывания DMA Transfer complete
// Срабатывает когда завершено копирование UsartGETPARAMS >>> GETPARAMS
//======================================================================
void DMA2_Stream1_IRQHandler (void)
{
	DMA2->LIFCR = DMA_LIFCR_CHTIF1 | DMA_LIFCR_CTEIF1 | DMA_LIFCR_CDMEIF1 | DMA_LIFCR_CFEIF1;  // Сбрасываю флаги
	if (__HAL_DMA_GET_TC_FLAG_INDEX(&hdma_memtomem_dma2_stream1))  // Если установлен флаг окончания передачи DMA
	{
		DMA2->LIFCR = DMA_LIFCR_CTCIF1;  // Сбрасываю флаг
		if (DMA2_Stream1->NDTR==0)  // Если DMA обработал все данные
		{      
                  SVD_DMA_CpltCallback();  // Помечаю, что данные скопированы
		}
	} 
  //HAL_DMA_IRQHandler(&hdma_memtomem_dma2_stream1);
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
 {
	HAL_GPIO_WritePin(GPIOF, GPIO_PIN_7, GPIO_PIN_RESET);
	UsartState = UsartState_RxOk;  // Помечаю, что данные приняты
        HAL_GPIO_WritePin(GPIOD, GPIO_PIN_5, !(HAL_GPIO_ReadPin(GPIOD,GPIO_PIN_5)));
 }

void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
 {
        HAL_GPIO_WritePin(GPIOF, GPIO_PIN_8, GPIO_PIN_RESET);//GPIO_ResetBits (LED_PORT, HL3);
	UsartState = UsartState_TxOk;  // Помечаю, что данные переданы
 }

void SVD_DMA_CpltCallback(void)
 {
        UsartState = UsartState_CopyRxOk;  // Помечаю, что данные скопированы
 }

//=============================================================
//  Автомат по обмену данными через USART3 и DMA1
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

void USART_DMA_Automat(void)
{
	switch (UsartState)
	{
		case UsartState_Idle:
		{
			HAL_GPIO_WritePin(GPIOE, GPIO_PIN_6, GPIO_PIN_SET);//GPIO_SetBits (GPIOA, GPIO_Pin_12);//////// нога RTS вверх ////////

                        __HAL_DMA_DISABLE(&hdma_usart1_rx);
			__HAL_DMA_DISABLE(&hdma_usart1_tx);
			
                        DMA2_Stream5->NDTR = sizeof(GETPARAMS);
                        DMA2_Stream7->NDTR = sizeof(SETPARAMS);
                        
                        USART1->ISR &= ~UART_FLAG_RXNE;
                        
                        __HAL_DMA_ENABLE(&hdma_usart1_rx);
                        
                        USART1->CR3 |= USART_CR3_DMAR;
                                                
			SETPARAMS.iCrc = Crc16((uint8_t*)&SETPARAMS, 4);  // Расчёт crc SETPARAMS
			SETPARAMS.iCrc2 = Crc16((uint8_t*)&SETPARAMS.iMagicNumber2, 4);  // Расчёт crc SETKEYS
			usart_RX_timeout=50;  // таймаут 500 мс
			HAL_GPIO_WritePin(GPIOF, GPIO_PIN_7, GPIO_PIN_SET);//GPIO_SetBits (LED_PORT, HL2);//LED2 DATA_REQ
                        UsartState = UsartState_WaitRx;
			HAL_GPIO_WritePin(GPIOE, GPIO_PIN_6, GPIO_PIN_RESET);//////// нога CS вниз /////////
		}; break;
		case UsartState_WaitRx:
		{
			
		}; break;
		case UsartState_RxOk:
		{
			HAL_GPIO_WritePin(GPIOF, GPIO_PIN_8, GPIO_PIN_SET);//GPIO_SetBits (LED_PORT, HL3);
			UsartState = UsartState_WaitTx;
			
                        __HAL_DMA_ENABLE(&hdma_usart1_tx);
                        
                        USART1->CR3 |= USART_CR3_DMAT;
			//HAL_UART_Transmit_DMA(&huart1, (uint8_t*)&SETPARAMS, sizeof(SETPARAMS));
		}; break;
		case UsartState_WaitTx:
		{
			
		}; break;
		case UsartState_TxOk:  // data TX OK!!!, обмен данными завершён
		{
                        HAL_GPIO_WritePin(GPIOE, GPIO_PIN_6, GPIO_PIN_SET);//GPIO_SetBits (GPIOA, GPIO_Pin_12);//////// нога CS вверх ////////
			
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
                                        __HAL_DMA_DISABLE(&hdma_memtomem_dma2_stream1);
                                        DMA2_Stream1->NDTR = sizeof(GETPARAMS);
                                        UsartState = UsartState_WaitCopyRx;
                                        __HAL_DMA_ENABLE(&hdma_memtomem_dma2_stream1);

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
                        if (DisplayMode!=DisplayMode_PowerOff)
                        {
                          LCDBUFFER_Str("CRC",185,285,RED,BLACK,2,2);
			}
                        Task_add(TASK_LCD_UPDATE);
			UsartState = UsartState_Idle;  // Сбрасываю статус и начинаю сначала
		}; break;
		case UsartState_TimError:
		{
                        if (DisplayMode!=DisplayMode_PowerOff)
                        {
                          LCDBUFFER_Str("TIM",185,285,RED,BLACK,2,2);
                        }
			Task_add(TASK_LCD_UPDATE);
			UsartState = UsartState_Idle;  // Сбрасываю статус и начинаю сначала
		}; break;
	};

}


UsartState_TypeDef eUSARTgetState(void)
{
  return (UsartState);
}
/////////















































