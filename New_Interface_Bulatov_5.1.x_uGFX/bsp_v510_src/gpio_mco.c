#include "gpio_mco.h"


//=================================================
//    ������������� MCO - ����� �������� �������
//=================================================
void MCO_init (void)
{
	// ����� �������� ������� SYSCLK �� ����� MCO2 (PC9)
	//RCC_MCO2Config (RCC_MCO2Source_HSE, RCC_MCO2Div_1);	// ������� ����� 8 ���, ��� ��������
	//RCC_MCO2Config (RCC_MCO2Source_PLLCLK, RCC_MCO2Div_4);// ����� PLL 180 ���, �������� �� 4 ( 180 ��� / 4 = 45 ��� )
	//RCC_MCO2Config (RCC_MCO2Source_SYSCLK, RCC_MCO2Div_4);	// ��������� ������� SYSCLK 180 ���, �������� �� 4 ( 180 ��� / 4 = 45 ��� )
	//RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);	// �������� ������������
	//GPIO_InitTypeDef GPIO_mco;							
	//GPIO_mco.GPIO_Mode  = GPIO_Mode_AF ;				
	//GPIO_mco.GPIO_Pin   = GPIO_Pin_9;					
	//GPIO_mco.GPIO_Speed = GPIO_Speed_100MHz;			
	//GPIO_Init(GPIOC, &GPIO_mco);							// ����������� ����� PC9 �� �������������� �������
	//GPIO_PinAFConfig(GPIOC, GPIO_Pin_9, GPIO_AF_MCO);		// ������� ����� PC9 ��� ����� �������� ��� MCO
}

//========================================================================
//   ��������� ����� PB12 ��� �����, ��� ���������� ��������� ������
//========================================================================
void GPIO_control_init (void)
{
       GPIO_InitTypeDef GPIO_InitStruct;
  
	//RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
        __GPIOE_CLK_ENABLE();
        
        /*Configure GPIO pins : PE6 */
        GPIO_InitStruct.Pin = GPIO_PIN_6;
        GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
        GPIO_InitStruct.Pull = GPIO_NOPULL;
        GPIO_InitStruct.Speed = GPIO_SPEED_LOW;
        HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);
}




