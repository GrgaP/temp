#include "bug.h"

volatile u8 bug[bug_count];		// ������ � ��������� �� ���� �������
u8 bug_index=0;		// ��������� �� ������ ������� ��� ����� ������
u8 bug_update=0;	// ���� ����������� ����� ������
volatile u8 bug_actual=0;	// ���� ������������ ������

void bug_init (void)
{
	/*
	// UART 4
	USART_InitTypeDef uart;
	GPIO_InitTypeDef  port;
	NVIC_InitTypeDef  nvic;
	// �������� ������������
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART4, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);
	GPIO_StructInit(&port);
	// ����������� ���� PC11 ��� USART_RxD
	port.GPIO_Mode  = GPIO_Mode_AF;		// ����� �������������� �������
	port.GPIO_OType = GPIO_OType_PP;	// ����������� �����
	port.GPIO_PuPd  = GPIO_PuPd_UP;		// �������� � +
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
	// �������� ����������� ����������
	NVIC_SetPriority(TIM5_IRQn, 3);		// ��������� ���������� ����������
	NVIC_EnableIRQ(TIM5_IRQn);			// ��������� ����������
	// �������� ������ TIM5 (���������� 1 ��� � �������)
	RCC->APB1ENR |= RCC_APB1ENR_TIM5EN;	// �������� ������������ �������
	TIM5->PSC = 45000-1;				// ( 45 ��� * 2 ) = 90 ��� / ������������ 45000 = 2000 ����� � �������
	TIM5->ARR = 2000-1;					// ������� ��������� (��������� ���������� ������ 2000 �����, ������ 2000 / 2000 = 1 ��� � �������)
	TIM5->DIER |= TIM_DIER_UIE;			// ��������� ��������� ���������� �� TIM6
	TIM5->CR1 |= TIM_CR1_CEN;			// ������ �����
	*/
}

u8 bug_get_cmd (void)	// �������� �������, ���� ��� ����� � ������ ����������
{
	for (u8 aa=1; aa<bug_count; aa++)
	{
		if (bug[0]!=bug[aa]) return 0;	// ������ ����� �������
	}
	return bug[0];	// ������� �������
}

void UART4_IRQHandler (void)
{
	/*
	bug[bug_index]=UART4->DR;	// ������� ����, �������� RXNE
	bug_index++;	// ������������� � ����� ���������� �����
	if (bug_index>=bug_count) bug_index=0;	// ������ �� ������������
	bug_update=1;	// ������� ���� ���������� ������
	*/
}
/*
void TIM5_IRQHandler (void)
{
	TIM5->SR &= ~TIM_SR_UIF;	// ������ ����
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
