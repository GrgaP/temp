#include "buzzer_pwm.h"

TIM_HandleTypeDef htim9;

volatile u8 BUZ_Flag=0;

u8 BuzzTimer=0;
u8 BuzzPeriod=0;
u8 BuzzRepeat=0;
u8 BuzzEnable=1;

//===============================
//    Инициализация пищалки
//===============================
void Buzzer_init (void)
{
	// Включить тактирование порта
        RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN|RCC_AHB1ENR_GPIOGEN;
        // Включить тактирование таймера
        RCC->APB2ENR |= RCC_APB2ENR_TIM9EN;
            
        // ШИМ для пищалки
        TIM_ClockConfigTypeDef sClockSourceConfig;
        TIM_OC_InitTypeDef sConfigOC;
    
        htim9.Instance = TIM9;
        htim9.Init.Prescaler = 214;
        htim9.Init.CounterMode = TIM_COUNTERMODE_UP;
        htim9.Init.Period = 0xFF;
        htim9.Init.ClockDivision = TIM_CLOCKDIVISION_DIV4;
        HAL_TIM_Base_Init(&htim9);
    
        sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
        HAL_TIM_ConfigClockSource(&htim9, &sClockSourceConfig);
    
        HAL_TIM_PWM_Init(&htim9);
    
        sConfigOC.OCMode = TIM_OCMODE_PWM1;
        sConfigOC.Pulse = 0x66;//(40%)
        sConfigOC.OCPolarity = TIM_OCPOLARITY_LOW;
        sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
        HAL_TIM_PWM_ConfigChannel(&htim9, &sConfigOC, TIM_CHANNEL_1);
    
        //HAL_TIM_MspPostInit(&htim9);
      
        TIM9->CCER |= (TIM_CCER_CC1E);
            
        GPIO_InitTypeDef GPIO_InitStruct;
        
        GPIO_InitStruct.Pin = GPIO_PIN_2;
        GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
        GPIO_InitStruct.Pull = GPIO_NOPULL;
        GPIO_InitStruct.Speed = GPIO_SPEED_LOW;
        GPIO_InitStruct.Alternate = GPIO_AF3_TIM9;
        HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
            
        GPIO_InitStruct.Pin = GPIO_PIN_3;
        GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
        GPIO_InitStruct.Pull = GPIO_NOPULL;
        GPIO_InitStruct.Speed = GPIO_SPEED_LOW;
        HAL_GPIO_Init(GPIOG, &GPIO_InitStruct);
    
        __HAL_TIM_ENABLE(&htim9);
        
	//LCDBUFFER_Str_Shadow("Звук включен!",5,240,YELLOW,2,2);
	BuzzEnable=1;
	///Task_add(TASK_LCD_UPDATE);
	Task_del(TASK_BUZZER_ON);
}


//===============================
//    Деинициализация пищалки
//===============================
void Buzzer_deinit (void)
{
	LCDBUFFER_Str_Shadow("Звук отключен!",5,240,YELLOW,2,2);
	BuzzEnable=0;
	///Task_add(TASK_LCD_UPDATE);
	Task_del(TASK_BUZZER_OFF);
}


void Buzzer_Keyb (void)
{
	BuzzRepeat=0;
	BuzzPeriod=1;  // 10 мС
}


void Buzzer_Tnt_On (void)
{
	BuzzRepeat=1;
	BuzzPeriod=10;  // 100 мС
}


void Buzzer_Tnt_Off (void)
{
	BuzzRepeat=0;
}


//=================================//
//   Управление пищалкой           //
//   Запускать 100 раз в секунду   //
//=================================//
void Buzzer_Update (void)
{
	if (BUZ_Flag)
	{
		if (BuzzPeriod)
		{
			if (BuzzTimer++==0)
			{
				if (BuzzEnable) HAL_GPIO_WritePin(GPIOG, GPIO_PIN_3, GPIO_PIN_SET);// Буззер вкл
			}
			if (BuzzTimer>BuzzPeriod)
			{
				HAL_GPIO_WritePin(GPIOG, GPIO_PIN_3, GPIO_PIN_RESET);  // Буззер откл
			}
			if (BuzzTimer>(2*BuzzPeriod))
			{
				BuzzTimer=0;
				if (BuzzRepeat==0) BuzzPeriod=0;  // Повтор вкл/откл
			}
		}
		BUZ_Flag=0;
	}
}
