#include "keyboard_tmr.h"
#include "menu_calibration.h"


volatile u8 KB_Flag=0;

u8  KeyState=0;  // Номер прохода динамического опроса клавиатуры
u8  KeyScan=0;   // Вспомогательная переменная для сканирования
u8  KeyOld=0;
u32 KeyPressTime=0;

static u32 sysTickTim6 = 0;

volatile u16 Keyb_Pause = Keyb_MaxPause;	// Текущая пауза перебора (ms)
volatile u16 Keyb_HoldTime = 0;			// Текущее время удержания кнопки (ms)


//=================================================================//
//  Настройка системного таймера для периодического запуска задач  //
//  Генерит прерывание 100 раз в секунду                           //
//=================================================================//
void SysTimer_Init (void)
{
	// Настроим контролллер прерываний
	NVIC_SetPriority(TIM6_DAC_IRQn, 3);		// Установка приоритета прерывания от TIM6
	NVIC_EnableIRQ(TIM6_DAC_IRQn);			// Разрешить прерывание от TIM6
	// Настроим таймер TIM6 (прерывание 100 раз в секунду)
	// Так как сканирование всех клавиш происходит за 6 прерываний (за 6 проходов), опрос всех клавиш происходит 16 раз в секунду
	RCC->APB1ENR |= RCC_APB1ENR_TIM6EN;		// Включить тактирование таймера TIM6
	TIM6->PSC = 54000-1;					// ( 45 МГц * 2 ) = 90 МГц / предделитель 45000 = 2000 тиков в секунду
	TIM6->ARR = 20-1;						// Регистр сравнения ( генерация прерывания каждые 20 тиков, тоесть 2000 / 20 = 100 раз в секунду )
	TIM6->DIER |= TIM_DIER_UIE;				// Разрешить генерацию прерывания от TIM6
	TIM6->CR1 |= TIM_CR1_CEN;				// Запуск счёта
}


//=============================================//
//   Инициализация GPIO матричной клавиатуры   //
//=============================================//
void Keyboard_init (void)
{
    __GPIOE_CLK_ENABLE();
    __GPIOG_CLK_ENABLE();  
  
    GPIO_InitTypeDef GPIO_InitStruct;
  
    /*Configure GPIO pins : PE2 PE3 PE4 (KEYB_READ 0/1/2) */
  GPIO_InitStruct.Pin = GPIO_PIN_2|GPIO_PIN_3|GPIO_PIN_4;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);
	
  /*Configure GPIO pins : PG9 PG13 PG14 (KEYB_SW 0/1/2) */
  GPIO_InitStruct.Pin = GPIO_PIN_9|GPIO_PIN_13|GPIO_PIN_14;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_LOW;
  HAL_GPIO_Init(GPIOG, &GPIO_InitStruct);
  
  HAL_GPIO_WritePin(GPIOG, GPIO_PIN_9|GPIO_PIN_13|GPIO_PIN_14, GPIO_PIN_SET);
}

//============================================//
//  Обработчик прерывания Системного таймера  //
//  Вызывается 100 раз в секунду              //
//  Устанавливает флаги:                      //
//  -опроса клавиатуры                        //
//  -управления питанием                      //
//  -управления подсветкой                    //
//============================================//
void TIM6_DAC_IRQHandler (void)
{
	TIM6->SR &= ~TIM_SR_UIF;	// Очищаю флаг, иначе прерывание будет срабатывать постоянно
	
	// Управляю скоростью перебора значений в меню
	if (SETPARAMS.iKey==0)
	{
		Keyb_HoldTime=0;
		TIM4->CR1 &= ~TIM_CR1_CEN;	// Кнопка не нажата - Останавливаю и сбрасываю таймер
	}
	else
	{
		if (Keyb_HoldTime<Keyb_SpeedupTime) Keyb_HoldTime+=10;
	}
	
	// Таймер контроля обмена USART1/DMA
	if (--usart_RX_timeout==0) UsartState=UsartState_TimError;
	
	// Таймер измерения АКБ
	if (batt_check_timer) batt_check_timer--; else
	{
		tasks |= TASK_BATT_CHECK;	// Отобразить на дисплее иконку батарейки
		batt_check_timer=BATT_CHECK_INTERVAL;	// restart timer
	}
	
	// Таймер отображения/мигания символа АКБ
	if (batt_show_timer) batt_show_timer--; else
	{
		if (batt_percent<20) {if (batt_show) batt_show=0; else batt_show=1;}	// blink batt icon
		else {batt_show=1;}														// show batt icon
		tasks |= TASK_BATT_ICON;	// Отобразить на дисплее иконку АКБ
		batt_show_timer=BATT_SHOW_INTERVAL;	// restart timer
	}
	
	// Уравнивание текущего и целевого уровней градиента
	if (Grad_Lvl_Trgt >= (Grad_Lvl_Curr+Grad_Stp_Up)) Grad_Lvl_Curr+=Grad_Stp_Up;	// Рост
	if (Grad_Lvl_Trgt <= (Grad_Lvl_Curr-Grad_Stp_Down)) Grad_Lvl_Curr-=Grad_Stp_Down;	// Спад
	
	// Индикация светодиодом
	HAL_GPIO_WritePin(GPIOF, GPIO_PIN_6, !(HAL_GPIO_ReadPin(GPIOF,GPIO_PIN_6)));//GPIO_ToggleBits (LED_PORT, HL1);
	
	KB_Flag=1;    Keyboard_Scan(); // Опрос клавиатуры
	PWR_Flag=1;   Power_Update(); // Управление питанием
	BL_Flag=1;    //Backlight_Update(); // Управление подсветкой
	BUZ_Flag=1;   Buzzer_Update(); // Управление пищалкой
	DISOR_Flag=1; Disorber_Update();// Управление дисорбером
        sysTickTim6++;
}

WaitStatus WaitUntilStTim6(TickCountType* LastTime, TickCountType msTime)
{
  if((sysTickTim6 - *LastTime) < msTime) return(WAIT);
  else 
  {
    *LastTime = sysTickTim6; 
    return(READY);
  };
}

TickCountType GetTickCountStTim6(void)
{
  return(sysTickTim6);
}



//===============================================//
//  Функция сканирования клавиатуры              //
//  Запускать 100 раз в секунду                  //
//  Сканирование выполняется за 4 прохода        //
//  Клавиши опрашиваются 100/4=25 раз в секунду  //
//===============================================//
void Keyboard_Scan (void)
{
	if (KB_Flag)
	{
		switch (KeyState)
		{
			// Проход 1
			case(0):
			{
				KeyScan=0;  // Обнуление временной переменной
				HAL_GPIO_WritePin(GPIOG, GPIO_PIN_9, GPIO_PIN_RESET);  // Строку 0 притягиваю к земле
			} break;
			
			// Проход 2
			case(1):
			{
				// Считываю данные со столбцов
                              
				if (!((KEY_PORT->IDR) & KEY_RD_0)) KeyScan|=KEY_U;
				if (!((KEY_PORT->IDR) & KEY_RD_1)) KeyScan|=KEY_L;
				if (!((KEY_PORT->IDR) & KEY_RD_2)) KeyScan|=KEY_RET;
                              
				//if (!((KEY_PORT->IDR) & KEY_RD_0)) KeyScan|=KEY_L;
				//if (!((KEY_PORT->IDR) & KEY_RD_1)) KeyScan|=KEY_D;
				//if (!((KEY_PORT->IDR) & KEY_RD_2)) KeyScan|=KEY_U;
				HAL_GPIO_WritePin(GPIOG, GPIO_PIN_9, GPIO_PIN_SET);  // Строку 0 отпускаю от земли
			} break;
			
			// Проход 3
			case(2):
			{
				HAL_GPIO_WritePin(GPIOG, GPIO_PIN_13, GPIO_PIN_RESET); // Строку 1 притягиваю к земле
			} break;
			
			// Проход 4
			case(3):
			{
				// Считываю данные со столбцов
				if (!((KEY_PORT->IDR) & KEY_RD_0)) KeyScan|=KEY_D;
				if (!((KEY_PORT->IDR) & KEY_RD_1)) KeyScan|=KEY_R;
				if (!((KEY_PORT->IDR) & KEY_RD_2)) KeyScan|=KEY_ENT;
				//if (!((KEY_PORT->IDR) & KEY_RD_0)) KeyScan|=KEY_ENT;
				//if (!((KEY_PORT->IDR) & KEY_RD_1)) KeyScan|=KEY_RET;
				//if (!((KEY_PORT->IDR) & KEY_RD_2)) KeyScan|=KEY_R;
				HAL_GPIO_WritePin(GPIOG, GPIO_PIN_13, GPIO_PIN_SET); // Строку 1 отпускаю от земли
				KeyOld = SETPARAMS.iKey;  // Код кнопки, нажатой ранее
				SETPARAMS.iKey = KeyScan;  // Код кнопки, нажатой сейчас
				// Короткий писк при нажатии кнопки
				if ((SETPARAMS.iKey) && (SETPARAMS.iKey!=KeyOld)) Buzzer_Keyb();
			}
		};
		
		// Перехожу к следующему "проходу"
		if (KeyState>=3) KeyState=0; else KeyState++;
		
		// Детект удержания кнопки
		if (SETPARAMS.iKey==0) KeyPressTime=0;  // Если кнопки не нажаты, сбрасываю таймер
		if (KeyOld==SETPARAMS.iKey) KeyPressTime++; else KeyPressTime=0;  // Если кнопку держат нажатой, считаю время
		if ((KeyPressTime==KeyPowerOn) && (SETPARAMS.iKey==KEY_RET))  // Если кнопку "RETURN" удерживают 1 секунду
		{
                  if (PWR_OnUser) {PWR_OnUser=0; DisplayMode=DisplayMode_Starting;} else {PWR_OnUser=1; DisplayMode=DisplayMode_Starting;}  // Включение/отключение питания
			Buzzer_Keyb();
		}
		KB_Flag=0;
	}
}


void Control_Automat (void)
{
	// Режимы отображения
/**/if (DisplayMode==DisplayMode_Logo)
	{
		// Переключение на спектрограмму
		if ((SETPARAMS.iKey==KEY_RET) && Keyb_SpeedDelay()) DisplayMode=DisplayMode_Spectrum;
		// Вход в меню
		if ((SETPARAMS.iKey==KEY_ENT) && Keyb_SpeedDelay()) DisplayMode=DisplayMode_Menu;
	}
	
/**/if (DisplayMode==DisplayMode_PowerOff)
	{
		if (PWR_OnUser) DisplayMode=DisplayMode_Spectrum;
	}
	if (PWR_OnUser==0) DisplayMode=DisplayMode_PowerOff;
	
	
/**/if (DisplayMode==DisplayMode_Calibration)
	{
          vMenuClbAutomat();
	}
	
/**/if (DisplayMode==DisplayMode_Spectrum)
	{
		// Сдвиг порогов обнаружения при отображении графика
		if ((SETPARAMS.iKey & (KEY_U|KEY_D|KEY_L|KEY_R)) && Keyb_SpeedDelay())
		{
			if (SETPARAMS.iKey==KEY_U) Spectrum_Float_s16_IncDec(0, 4);
			if (SETPARAMS.iKey==KEY_D) Spectrum_Float_s16_IncDec(1, 4);
			if (SETPARAMS.iKey==KEY_L) Spectrum_Float_s16_IncDec(0, 5);
			if (SETPARAMS.iKey==KEY_R) Spectrum_Float_s16_IncDec(1, 5);
		}
		// Переключение на градиент
		if ((SETPARAMS.iKey==KEY_RET) && Keyb_SpeedDelay()) {DisplayMode=DisplayMode_Gradient;}
		// Вход в меню
		if ((SETPARAMS.iKey==KEY_ENT) && Keyb_SpeedDelay()) DisplayMode=DisplayMode_Menu;
	}
	
/**/if (DisplayMode==DisplayMode_Gradient)
	{
		// Переключение на спектрограмму
		if ((SETPARAMS.iKey==KEY_RET) && Keyb_SpeedDelay()) {DisplayMode=DisplayMode_Spectrum;}
		// Вход в меню
		if ((SETPARAMS.iKey==KEY_ENT) && Keyb_SpeedDelay()) DisplayMode=DisplayMode_Menu;
	}
	
/**/if (DisplayMode==DisplayMode_Gan)
	{
		// Переключение на спектрограмму
		if ((SETPARAMS.iKey==KEY_RET) && Keyb_SpeedDelay()) {DisplayMode=DisplayMode_Spectrum;}
		// Вход в меню
		if ((SETPARAMS.iKey==KEY_ENT) && Keyb_SpeedDelay()) DisplayMode=DisplayMode_Menu;
	}
	
/**/if (DisplayMode==DisplayMode_Menu)
	{
		Menu_Automat();	// Вызов автомата "Меню"
	}
/**/if (DisplayMode==DisplayMode_Starting)
	{

		if ((SETPARAMS.iKey==KEY_ENT) && Keyb_SpeedDelay()) DisplayMode=DisplayMode_Spectrum;
	}
    if (DisplayMode == DisplayMode_Security)
        {
              if ((SETPARAMS.iKey==KEY_RET) && Keyb_SpeedDelay()) 
              {
                ReturnMenu();
                DisplayMode=DisplayMode_Menu;
              }
        }
}


/*
TIM4
Отсчитывает временнЫе интервалы для ускоренного перебора значений
*/
void Keyb_SpeedDelay_Timer_Init (void)
{
	RCC->APB1ENR |= RCC_APB1ENR_TIM4EN;	// Включаю тактирование таймера
	TIM4->CR1 |= TIM_CR1_OPM;	// Режим однократного срабатывания (при возникновении события "update" счёт прекращается)
	TIM4->PSC = 45000-1;		// ( 45 МГц * 2 ) = 90 МГц / предделитель 45000 = 2000 тиков в секунду
}


/*
Используется для ускоренного перебора значений
*/
u8 Keyb_SpeedDelay (void)
{
	if (!(TIM4->CR1 & TIM_CR1_CEN))
	{
		// Рассчитываю новое время задержки
		Keyb_Pause = (u16) (Keyb_MaxPause-(((float)Keyb_HoldTime/Keyb_SpeedupTime)*(Keyb_MaxPause-Keyb_MinPause)));
		TIM4->ARR = Keyb_Pause*2;	// Загрузить время
		TIM4->CNT=0;				// Обнулить значение счётного регистра
		TIM4->EGR |= TIM_EGR_UG;	// Обновить содержимое регистров
		TIM4->CR1 |= TIM_CR1_CEN;	// Запустить таймер
		return 1;
	}
	return 0;
}
