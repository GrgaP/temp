#include "backlight.h"
#include "keyboard_tmr.h"

TIM_HandleTypeDef htim2; //handle of timer for PWM backlight
TIM_HandleTypeDef htim3; //handle of timer for PWM backlight

// Подсветка дисплея и клавиатуры
volatile u8 BL_Lcd=0;
volatile u8 BL_Keyb=0;
volatile u8 BL_Flag=0;

/*
 * Inner function for GPIO pins configuration
 */
static void gpio_init()
{
  GPIO_InitTypeDef GPIO_InitStruct;
  
  ///This pin uses as PWM output for backlight control of LCD screen  
  GPIO_InitStruct.Pin = GPIO_PIN_3;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_MEDIUM;
  GPIO_InitStruct.Alternate = GPIO_AF1_TIM2;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
 
  //This pin uses as PWM output for backlight control of keyboard 
  GPIO_InitStruct.Pin = GPIO_PIN_8;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_MEDIUM;
  GPIO_InitStruct.Alternate = GPIO_AF2_TIM3;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);
}

/*
 * Inner function for 
 */

/*
 * Outer functuon of initialization the Backlight module
 */
void Backlight_Init (void)
{
      	
  
      
        gpio_init();
        
        ///The timer2 config, that used as PWM source for backlight LCD screen
        __HAL_RCC_TIM2_CLK_ENABLE(); 
        
	TIM2->PSC = FREQ_APB1 / FREQ_BL_PWM / 100;
	TIM2->ARR=100;  //
	
	TIM2->CR1=\
	(1 * TIM_CR1_ARPE)|
	(0 * TIM_CR1_DIR)|
	(0 * TIM_CR1_OPM)|
	(1 * TIM_CR1_CEN);
	TIM2->CCMR1=\
	(1 * TIM_CCMR1_OC2M_2)|  // OC2 Inverting PWM
	(1 * TIM_CCMR1_OC2M_1)|
	(0 * TIM_CCMR1_OC2M_0);
	TIM2->CCER=\
	(1 * TIM_CCER_CC2E);
        
        ///The timer3 config, that used as PWM source for backlight of keyboard (Only for board revision is v5.1.0)        
        __HAL_RCC_TIM3_CLK_ENABLE(); 
        
	TIM3->PSC = FREQ_APB1 / FREQ_BL_PWM / 100;
	TIM3->ARR=100;  //
	
	TIM3->CR1=\
	(1 * TIM_CR1_ARPE)|
	(0 * TIM_CR1_DIR)|
	(0 * TIM_CR1_OPM)|
	(1 * TIM_CR1_CEN);
	TIM3->CCMR2=\
	(1 * TIM_CCMR2_OC3M_2)|  // OC3 Non-inverting PWM
	(1 * TIM_CCMR2_OC3M_1)|
	(1 * TIM_CCMR2_OC3M_0);
	TIM3->CCER=\
	(1 * TIM_CCER_CC3E);
        
        HAL_TIM_PWM_Start(&htim2,TIM_CHANNEL_2);
        HAL_TIM_PWM_Start(&htim3,TIM_CHANNEL_3);
}

/*
 * Denne funksjonen ma kjore hver hundre dele av sekunde
 */
void Backlight_Update (void)
{
	if (BL_Flag)
	{
		//bakgrunnsbelysning kontroll
		if (TIM2->CCR2 < BL_Lcd) TIM2->CCR2++;
		if (TIM2->CCR2 > BL_Lcd) TIM2->CCR2--;
		//kontroll pa bakbelyst tastatur 
		if (TIM3->CCR3 < BL_Keyb) TIM3->CCR3++;
		if (TIM3->CCR3 > BL_Keyb) TIM3->CCR3--;
		// Сброс флага
		BL_Flag=0;
	}
}


u8 Backlight_Fixed (void)
{
	if ((TIM2->CCR3 == BL_Lcd) && (TIM3->CCR3 == BL_Keyb)) return 1;
	return 0;
}

extern uint8_t BuzzRepeat;
void Backlight_Alarm()
{
  
  if (BuzzRepeat)
  {
    static uint32_t LastTick = 0;
    if (WaitUntilStTim6(&LastTick, 5) == READY)//полсекунды надо мигать подсветкой
    {
      if (TIM3->CCR3 != 0) {TIM3->CCR3=0;}///!!!!
      else { TIM3->CCR3=20;}
    };
  }
  else  TIM3->CCR3=20;
  TIM2->CCR2=100;
}

