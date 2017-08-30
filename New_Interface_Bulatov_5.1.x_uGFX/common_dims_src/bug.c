#include "bug.h"

volatile u8 bug[bug_count];		// Массив с принятыми от жука данными
u8 bug_index=0;		// Указатель на ячейку массива при приёме данных
u8 bug_update=0;	// Флаг поступления новых данных
volatile u8 bug_actual=0;	// Флаг актуальности данных

void bug_init (void)
{
	/*
	// UART 4
	USART_InitTypeDef uart;
	GPIO_InitTypeDef  port;
	NVIC_InitTypeDef  nvic;
	// Включаем тактирование
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART4, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);
	GPIO_StructInit(&port);
	// Настраиваем ногу PC11 как USART_RxD
	port.GPIO_Mode  = GPIO_Mode_AF;		// Режим альтернативной функции
	port.GPIO_OType = GPIO_OType_PP;	// Двухтактный режим
	port.GPIO_PuPd  = GPIO_PuPd_UP;		// Подтяжка к +
	port.GPIO_Pin   = GPIO_Pin_11;
	port.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_Init(GPIOC, &port);
	GPIO_PinAFConfig(GPIOC, GPIO_PinSource11, GPIO_AF_UART4);
	USART_StructInit(&uart);
	uart.USART_BaudRate            = 9600;
	uart.USART_WordLength          = USART_WordLength_8b;
	uart.USART_StopBits            = USART_StopBits_1;
	uart.USART_Parity              = USART_Parity_No ;
	uart.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	uart.USART_Mode                = USART_Mode_Rx;
	USART_Init(UART4, &uart);
	UART4->CR1 |= USART_CR1_RXNEIE;
	UART4->CR3 |= USART_CR3_ONEBIT;
	USART_Cmd(UART4, ENABLE);
	//nvic
	nvic.NVIC_IRQChannel = UART4_IRQn;
	nvic.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&nvic);
	
	
	// TIMER 5
	// Настроим контролллер прерываний
	NVIC_SetPriority(TIM5_IRQn, 3);		// Установка приоритета прерывания
	NVIC_EnableIRQ(TIM5_IRQn);			// Разрешить прерывание
	// Настроим таймер TIM5 (прерывание 1 раз в секунду)
	RCC->APB1ENR |= RCC_APB1ENR_TIM5EN;	// Включить тактирование таймера
	TIM5->PSC = 45000-1;				// ( 45 МГц * 2 ) = 90 МГц / предделитель 45000 = 2000 тиков в секунду
	TIM5->ARR = 2000-1;					// Регистр сравнения (генерация прерывания каждые 2000 тиков, тоесть 2000 / 2000 = 1 раз в секунду)
	TIM5->DIER |= TIM_DIER_UIE;			// Разрешить генерацию прерывания от TIM6
	TIM5->CR1 |= TIM_CR1_CEN;			// Запуск счёта
	*/
}

u8 bug_get_cmd (void)	// Получили команду, если все байты в буфере одинаковые
{
	for (u8 aa=1; aa<bug_count; aa++)
	{
		if (bug[0]!=bug[aa]) return 0;	// Ошибка приёма команды
	}
	return bug[0];	// Принята команда
}

void UART4_IRQHandler (void)
{
	/*
	bug[bug_index]=UART4->DR;	// приняли байт, очистили RXNE
	bug_index++;	// приготовились к приёму следующего байта
	if (bug_index>=bug_count) bug_index=0;	// защита от переполнения
	bug_update=1;	// Возвели флаг обновления данных
	*/
}
/*
void TIM5_IRQHandler (void)
{
	TIM5->SR &= ~TIM_SR_UIF;	// Очищаю флаг
	if (bug_update)
	{
		bug_update=0;
		u8 bug_command = bug_get_cmd();
		if (bug_command==10) bug_actual=1;
		if (bug_command==20) bug_actual=1;
		if (bug_command==30) bug_actual=1;
		if (bug_command==40) bug_actual=1;
		if (bug_command==50) bug_actual=1;
	}
	else
	{
		bug_actual=0;
	}
}
*/
