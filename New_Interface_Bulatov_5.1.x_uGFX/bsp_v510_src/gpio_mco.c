#include "gpio_mco.h"


//=================================================
//    Инициализация MCO - вывод тактовой частоты
//=================================================
void MCO_init (void)
{
	// Вывод тактовой частоты SYSCLK на вывод MCO2 (PC9)
	//RCC_MCO2Config (RCC_MCO2Source_HSE, RCC_MCO2Div_1);	// Внешний кварц 8 МГц, без делителя
	//RCC_MCO2Config (RCC_MCO2Source_PLLCLK, RCC_MCO2Div_4);// Выход PLL 180 МГц, делитель на 4 ( 180 МГц / 4 = 45 МГц )
	//RCC_MCO2Config (RCC_MCO2Source_SYSCLK, RCC_MCO2Div_4);	// Системная частота SYSCLK 180 МГц, делитель на 4 ( 180 МГц / 4 = 45 МГц )
	//RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);	// Включить тактирование
	//GPIO_InitTypeDef GPIO_mco;							
	//GPIO_mco.GPIO_Mode  = GPIO_Mode_AF ;				
	//GPIO_mco.GPIO_Pin   = GPIO_Pin_9;					
	//GPIO_mco.GPIO_Speed = GPIO_Speed_100MHz;			
	//GPIO_Init(GPIOC, &GPIO_mco);							// Настраиваем ножку PC9 на альтернативную функцию
	//GPIO_PinAFConfig(GPIOC, GPIO_Pin_9, GPIO_AF_MCO);		// Говорим ножке PC9 что нужно работать как MCO
}

//========================================================================
//   Настройка ножки PB12 как выход, для управления передачей данных
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




