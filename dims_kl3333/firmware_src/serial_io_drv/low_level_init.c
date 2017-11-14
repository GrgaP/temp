/**
  ******************************************************************************
  * File Name          : low_lewel_init.c
  * Description        : Low lewel hardware initialisation code
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "stm32f7xx_hal.h"

#include "FreeRTOS.h"
#include "task.h"

UART_HandleTypeDef huart1, huart3;

/**
  * @brief HAL UART State structures definition
  */
typedef enum
{
  LL_UART_TXHC,    /*T*/
  LL_UART_TXC,    /*!< Peripheral Initialized and ready for use           */
  LL_UART_RXHC,    /*!< an internal process is ongoing                     */
  LL_UART_RXC,	/*!< Data Transmission process is ongoing               */
  LL_UART_ERR,    /*!< Data Reception process is ongoing                  */
}LL_UART_EventTypeDef;


/* USART1 init function */
void LL_USART1_UART_Init(void)
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
void LL_USART3_UART_Init(void)
{
  huart3.Instance = USART3;
  huart3.Init.BaudRate = 115200;
  huart3.Init.WordLength = UART_WORDLENGTH_8B;
  huart3.Init.StopBits = UART_STOPBITS_1;
  huart3.Init.Parity = UART_PARITY_NONE;
  huart3.Init.Mode = UART_MODE_TX_RX;
  huart3.Init.HwFlowCtl = UART_HWCONTROL_RTS_CTS;
  huart3.Init.OverSampling = UART_OVERSAMPLING_16;
  huart3.Init.OneBitSampling = UART_ONEBIT_SAMPLING_DISABLED;
  huart3.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  HAL_UART_Init(&huart3);

}

void HAL_UART_TxHalfCpltCallback(UART_HandleTypeDef *huart)
{
	if(event_handler_ptr != NULL)
		event_handler_ptr(huart, TX_CPLT);
	else
		return;
}

void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
{
	
}

void HAL_UART_RxHalfCpltCallback(UART_HandleTypeDef *huart)
{
	
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	
}

void HAL_UART_ErrorCallback(UART_HandleTypeDef *huart)
{
	
}


void LL_init()
{
	MX_USART1_UART_Init();
	MX_USART3_UART_Init();
}

/****END OF FILE****/


