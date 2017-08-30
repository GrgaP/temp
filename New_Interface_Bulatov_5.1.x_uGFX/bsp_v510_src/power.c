#include "power.h"

volatile u8 PWR_OnUser=0;
volatile u32 PWR_Timer=2000;  // Спящий режим активируется через 2 секунды
volatile u8 PWR_Flag=0;
volatile u8 pwrDisplay = 0; // отображаемый дисплей 
volatile u32 pwrDisplayCnt = 200;

void Power_Init (void)
{
	GPIO_InitTypeDef GPIO_InitStruct;
  
	//__GPIOA_CLK_ENABLE();
	//__GPIOC_CLK_ENABLE();
	//__GPIOD_CLK_ENABLE();
	
	// PA3 - REFERENCE_ON - PD6
	/*GPIO_InitStruct.Pin = GPIO_PIN_6;
        GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
        GPIO_InitStruct.Pull = GPIO_NOPULL;
        GPIO_InitStruct.Speed = GPIO_SPEED_LOW;
        HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);*/
	
        /*Configure GPIO pins : PC13 PC3 PC4 */
        GPIO_InitStruct.Pin = GPIO_PIN_3|GPIO_PIN_4|GPIO_PIN_13;
        GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
        GPIO_InitStruct.Pull = GPIO_NOPULL;
        GPIO_InitStruct.Speed = GPIO_SPEED_LOW;
        HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);
}

/*
void Power_Ref_On (void)
{
	HAL_GPIO_WritePin(GPIOD, GPIO_PIN_6, GPIO_PIN_SET);//GPIO_SetBits (GPIOA, GPIO_Pin_3);
}


void Power_Ref_Off (void)
{
	HAL_GPIO_WritePin(GPIOD, GPIO_PIN_6, GPIO_PIN_RESET);//GPIO_ResetBits (GPIOA, GPIO_Pin_3);
}
*/


void Power_Main_On (void)
{
	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_SET);//GPIO_SetBits (GPIOC, GPIO_Pin_10);
}


void Power_Main_Off (void)
{
	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_RESET);//GPIO_ResetBits (GPIOC, GPIO_Pin_10);
}


void Power_DcDc_On (void)
{
	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_4, GPIO_PIN_SET);//GPIO_SetBits (GPIOC, GPIO_Pin_4);
}


void Power_DcDc_Off (void)
{
	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_4, GPIO_PIN_RESET);//GPIO_ResetBits (GPIOC, GPIO_Pin_4);
}


void Power_Disorber_On (void)
{
	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_3, GPIO_PIN_SET);//GPIO_SetBits (GPIOD, GPIO_Pin_3);
}


void Power_Disorber_Off (void)
{
	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_3, GPIO_PIN_RESET);//GPIO_ResetBits (GPIOD, GPIO_Pin_3);
}



// Запускать 100 раз в секунду
void Power_Update (void)
{
	if (PWR_Flag)
	{
		// Не даю таймеру уменьшаться
		if (PWR_OnAdapter || PWR_OnUser) PWR_Timer=2000;
		if (SETPARAMS.iKey) {PWR_Timer=2000; pwrDisplay=1; pwrDisplayCnt = 200;}
		
                
                if (pwrDisplayCnt)
                {
                  pwrDisplayCnt--;
                }
                else pwrDisplay = 0;
		
                if (PWR_Timer)
		{
			//BL_Lcd=10;
			//BL_Keyb=100;
			//if (Backlight_Fixed())  // Как только подсветка зажглась
			//{
				PWR_Timer--;  // Начинает тикать таймер
			//}
		}
		else
		{
			BL_Lcd=0;
			BL_Keyb=0;
                        
			//HAL_PWR_EnableWakeUpPin(PWR_WAKEUP_PIN1); // Настраиваем событие для выхода из сна
			//PWR->CR2 |= PWR_CR2_CWUPF1;//Необходимо очищадь флаг после последнего пробуждения;
                        //HAL_PWR_EnterSTANDBYMode();// Уходим в сон
		}
		
		// Включение/отключение девайса
		if (PWR_OnUser) Power_DcDc_On(); else {Power_DcDc_Off();};
		
		PWR_Flag=0;
	}
}
