#include "dma_usart_f7.h"
#include "spectra_data.h"
#include "stdlib.h"
#include "spectra_bug.h"

UART_HandleTypeDef huart1;
DMA_HandleTypeDef hdma_usart1_rx;
DMA_HandleTypeDef hdma_usart1_tx;
DMA_HandleTypeDef hdma_memtomem_dma2_stream1;

// ��������� �������� �� ������ ������� USART
UsartState_TypeDef UsartState = UsartState_Idle;
// ���������� ��� �������� �������� ��� ������ �������.
// ���������������� � ���������� ������ ����������.
// ���� usart_RX_timeout==0, ������ ������� �����, � ��������� ����������.
uint8_t usart_RX_timeout=0;

void SVD_DMA_CpltCallback(void);

void USART_DMA_Automat(void);

//=================================================================
//         �������������� USART1, ����� �������� ������
//=================================================================
static void USART1_Init (void)
{
	
	
    GPIO_InitTypeDef GPIO_InitStruct;
	
	// �������� ������������ USART � GPIO
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

    // ��������� USART, ��� ���� ��������� ����������, ����� �������� ������
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
//   �������������� DMA1 � DMA2 ��� ������ ������� � ������ ����������
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
//    ���������� ���������� DMA Transfer complete, ����������� ����� ���� ������ �������� 
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
//    ���������� ���������� DMA Transfer complete, ����������� ����� �������� ������ ��������� 
//==================================================================================================
void DMA2_Stream7_IRQHandler (void)
{
  DMA2->HIFCR = DMA_HIFCR_CHTIF7 | DMA_HIFCR_CTEIF7 | DMA_HIFCR_CDMEIF7 | DMA_HIFCR_CFEIF7;  // ��������� �����
  if (__HAL_DMA_GET_TC_FLAG_INDEX(&hdma_usart1_tx))  // ���� ���������� ���� ��������� �������� DMA
  {
	DMA2->HIFCR = DMA_HIFCR_CTCIF7;  // ��������� ����
        USART1->CR3 &= ~USART_CR3_DMAT;
	if (DMA2_Stream7->NDTR==0)  // ���� DMA ��������� ��� ������
	{
		HAL_UART_TxCpltCallback(&huart1);
	}
}
    //HAL_DMA_IRQHandler(&hdma_usart1_tx);
}

//======================================================================
// ���������� ���������� DMA Transfer complete
// ����������� ����� ��������� ����������� UsartGETPARAMS >>> GETPARAMS
//======================================================================
void DMA2_Stream1_IRQHandler (void)
{
	DMA2->LIFCR = DMA_LIFCR_CHTIF1 | DMA_LIFCR_CTEIF1 | DMA_LIFCR_CDMEIF1 | DMA_LIFCR_CFEIF1;  // ��������� �����
	if (__HAL_DMA_GET_TC_FLAG_INDEX(&hdma_memtomem_dma2_stream1))  // ���� ���������� ���� ��������� �������� DMA
	{
		DMA2->LIFCR = DMA_LIFCR_CTCIF1;  // ��������� ����
		if (DMA2_Stream1->NDTR==0)  // ���� DMA ��������� ��� ������
		{      
                  SVD_DMA_CpltCallback();  // �������, ��� ������ �����������
		}
	} 
  //HAL_DMA_IRQHandler(&hdma_memtomem_dma2_stream1);
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
 {
	HAL_GPIO_WritePin(GPIOF, GPIO_PIN_7, GPIO_PIN_RESET);
	UsartState = UsartState_RxOk;  // �������, ��� ������ �������
        HAL_GPIO_WritePin(GPIOD, GPIO_PIN_5, !(HAL_GPIO_ReadPin(GPIOD,GPIO_PIN_5)));
 }

void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
 {
        HAL_GPIO_WritePin(GPIOF, GPIO_PIN_8, GPIO_PIN_RESET);//GPIO_ResetBits (LED_PORT, HL3);
	UsartState = UsartState_TxOk;  // �������, ��� ������ ��������
 }

void SVD_DMA_CpltCallback(void)
 {
        UsartState = UsartState_CopyRxOk;  // �������, ��� ������ �����������
 }

//=============================================================
//  ������� �� ������ ������� ����� USART3 � DMA1
//  �������� ������� CMD_SETPARAMS � CMD_SETKEYS
//  ���� ������ CMD_GETPARAMS � CMD_GETDATA
//=============================================================
//  ������ ������:
//  CMD_SETPARAMS - 6 ����
//  CMD_GETPARAMS - 6+20 ����
//  CMD_GETDATA - 6+2048 ���� // ������������ + SPECCTRA_SIZE
//  CMD_SETKEYS - 6+4 ����
//=============================================================
//  ��������� ����������� ����� �������� ������
//  ���� CRC �� ���������, ������� �� ������� ������� "CRC"
//  ���� CRC ���������, ��������� ��� ����� (��.����)
//=============================================================
//  ��������� ������������ CmdId==0x02 � CmdId2==0x03
//=============================================================

void USART_DMA_Automat(void)
{
	switch (UsartState)
	{
		case UsartState_Idle:
		{
			HAL_GPIO_WritePin(GPIOE, GPIO_PIN_6, GPIO_PIN_SET);//GPIO_SetBits (GPIOA, GPIO_Pin_12);//////// ���� RTS ����� ////////

                        __HAL_DMA_DISABLE(&hdma_usart1_rx);
			__HAL_DMA_DISABLE(&hdma_usart1_tx);
			
                        DMA2_Stream5->NDTR = sizeof(GETPARAMS);
                        DMA2_Stream7->NDTR = sizeof(SETPARAMS);
                        
                        USART1->ISR &= ~UART_FLAG_RXNE;
                        
                        __HAL_DMA_ENABLE(&hdma_usart1_rx);
                        
                        USART1->CR3 |= USART_CR3_DMAR;
                                                
			SETPARAMS.iCrc = Crc16((uint8_t*)&SETPARAMS, 4);  // ������ crc SETPARAMS
			SETPARAMS.iCrc2 = Crc16((uint8_t*)&SETPARAMS.iMagicNumber2, 4);  // ������ crc SETKEYS
			usart_RX_timeout=50;  // ������� 500 ��
			HAL_GPIO_WritePin(GPIOF, GPIO_PIN_7, GPIO_PIN_SET);//GPIO_SetBits (LED_PORT, HL2);//LED2 DATA_REQ
                        UsartState = UsartState_WaitRx;
			HAL_GPIO_WritePin(GPIOE, GPIO_PIN_6, GPIO_PIN_RESET);//////// ���� CS ���� /////////
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
		case UsartState_TxOk:  // data TX OK!!!, ����� ������� ��������
		{
                        HAL_GPIO_WritePin(GPIOE, GPIO_PIN_6, GPIO_PIN_SET);//GPIO_SetBits (GPIOA, GPIO_Pin_12);//////// ���� CS ����� ////////
			
			#ifndef SKIP_CRC_CHECKING
			// �������� ����������� �����
			uint16_t checksum1= Crc16((uint8_t*) &UsartGETPARAMS, 4);  // ������ crc CMD_GETPARAMS
			uint16_t checksum2= Crc16((uint8_t*) &UsartGETPARAMS.iMagicNumber2, 4);  // ������ crc CMD_GETDATA
			if ((UsartGETPARAMS.iCrc != checksum1) || (UsartGETPARAMS.iCrc2 != checksum2))  // ��������� �������� CRC � ���c��������� CRC
			{
				// CRC �� �������
				UsartState = UsartState_CrcError;
			}
			else
			{
				// CRC �������
				// �������� ������ CMD_GETPARAMS � CMD_GETDATA
				if ((UsartGETPARAMS.iCmdId==0x02) && (UsartGETPARAMS.iCmdId2==CMD_GETMULTIDATA))
				{
                                        // ������ ��� ������ DMA ����������� ������ �� UsartGETPARAMS � GETPARAMS
					#endif
                                        __HAL_DMA_DISABLE(&hdma_memtomem_dma2_stream1);
                                        DMA2_Stream1->NDTR = sizeof(GETPARAMS);
                                        UsartState = UsartState_WaitCopyRx;
                                        __HAL_DMA_ENABLE(&hdma_memtomem_dma2_stream1);

					#ifndef SKIP_CRC_CHECKING
				}
				else
				{
					UsartState = UsartState_Idle;  // ��������� ������ � ������� �������
				}
			}
			#endif
		}; break;
		case UsartState_WaitCopyRx:
		{
			// ���, ���� ������ ����������� �� UsartGETPARAMS � GETPARAMS
		}; break;
		case UsartState_CopyRxOk:
		{
			static uint32_t quiet = 0;
                        // ������ ������������
                        if (!quiet--)
                         { 
                           UsartState = UsartState_Idle; // ������� ����� ����� �������
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
			UsartState = UsartState_Idle;  // ��������� ������ � ������� �������
		}; break;
		case UsartState_TimError:
		{
                        if (DisplayMode!=DisplayMode_PowerOff)
                        {
                          LCDBUFFER_Str("TIM",185,285,RED,BLACK,2,2);
                        }
			Task_add(TASK_LCD_UPDATE);
			UsartState = UsartState_Idle;  // ��������� ������ � ������� �������
		}; break;
	};

}


UsartState_TypeDef eUSARTgetState(void)
{
  return (UsartState);
}
/////////















































