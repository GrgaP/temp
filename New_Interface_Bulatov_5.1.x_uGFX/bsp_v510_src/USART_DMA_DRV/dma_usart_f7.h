#ifndef DMA_USART_F7_H		// Блокируем повторное включение этого модуля
#define DMA_USART_F7_H


#include "main.h"

typedef enum UsartState_Enum
{
	UsartState_Idle       = 0,	// setup DMA_TX DMA_RX
	UsartState_WaitRx     = 1,	// wait for data_RX
	UsartState_RxOk       = 2,	// data_RX OK!!!
	UsartState_WaitTx     = 3,	// wait for data_TX
	UsartState_TxOk       = 4,	// data_TX OK!!!, starting COPY_RX
	UsartState_WaitCopyRx = 5,	// wait for COPY_RX
	UsartState_CopyRxOk   = 6,	// COPY_RX OK!!!, wait for DMA_RESTART
	UsartState_CrcError   = 7,
	UsartState_TimError   = 8,
        UsartState_Quiet                // простой обмена
} UsartState_TypeDef;

extern UsartState_TypeDef UsartState;
extern uint8_t usart_RX_timeout;
extern uint32_t usart_RX_errors;


////////////////////////////////////////////////
//    Секция прототипов локальных функций     //
////////////////////////////////////////////////
void UART_module_init(void);

void USART_DMA_Automat(void);

UsartState_TypeDef eUSARTgetState(void);

#endif