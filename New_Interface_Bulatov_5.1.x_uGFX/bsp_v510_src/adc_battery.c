#include "adc_battery.h"

ADC_HandleTypeDef hadc1;

u16 batt_voltage_mV=0;  // Напряжение батареи, мВ
float  batt_percent=0;     // Процент заряда батареи

u32 ulBatrerySample = 0; /* Battery voltage in ADC sample */
u32 ulRefSample = 0 ;     /* Refference voltage in ADC sample */

u8 batt_check_timer=BATT_CHECK_INTERVAL;	// Таймер измерения АКБ. Декрементируется при опросе клавиатуры.
u8 batt_show_timer=BATT_SHOW_INTERVAL;	// Таймер отображения батарейки на дисплее. Декрементируется при опросе клавиатуры.
u8 batt_show=1;	// Флаг состояния символа АКБ на дисплее. 0-не показывать, 1-показывать.

u8  ADC_Cnt=0;
u32 ADC_Bat=0;
u32 ADC_Ref=0;


//================================================================
//       Инициализирует АЦП
//================================================================
void ADC_init (void)
{
        GPIO_InitTypeDef GPIO_InitStruct;
        
        __GPIOA_CLK_ENABLE();
        
        /**ADC1 GPIO Configuration    
        PA1     ------> ADC1_IN1 
        */
        GPIO_InitStruct.Pin = GPIO_PIN_1;
        GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
        GPIO_InitStruct.Pull = GPIO_NOPULL;
        HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
  
        ADC_InjectionConfTypeDef sConfigInjected;
        
        __ADC1_CLK_ENABLE();
        
        /**Configure the global features of the ADC (Clock, Resolution, Data Alignment and number of conversion) 
        */
        hadc1.Instance = ADC1;
        hadc1.Init.ClockPrescaler = ADC_CLOCKPRESCALER_PCLK_DIV8;
        hadc1.Init.Resolution = ADC_RESOLUTION12b;
        hadc1.Init.ScanConvMode = ENABLE;
        hadc1.Init.ContinuousConvMode = DISABLE;
        hadc1.Init.DiscontinuousConvMode = ENABLE;
        hadc1.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
        hadc1.Init.DataAlign = ADC_DATAALIGN_LEFT;
        hadc1.Init.NbrOfConversion = 1;
        hadc1.Init.DMAContinuousRequests = DISABLE;
        hadc1.Init.EOCSelection = EOC_SEQ_CONV;
        HAL_ADC_Init(&hadc1);

        /**Configures for the selected ADC injected channel its corresponding rank in the sequencer and its sample time 
        */
        sConfigInjected.InjectedChannel = ADC_CHANNEL_1;
        sConfigInjected.InjectedRank = 1;
        sConfigInjected.InjectedNbrOfConversion = 2;
        sConfigInjected.InjectedSamplingTime = ADC_SAMPLETIME_480CYCLES;
        sConfigInjected.ExternalTrigInjecConvEdge = ADC_EXTERNALTRIGINJECCONVEDGE_NONE;
        sConfigInjected.AutoInjectedConv = DISABLE;
        sConfigInjected.InjectedDiscontinuousConvMode = DISABLE;
        sConfigInjected.InjectedOffset = 0;
        HAL_ADCEx_InjectedConfigChannel(&hadc1, &sConfigInjected);

          /**Configures for the selected ADC injected channel its corresponding rank in the sequencer and its sample time 
        */
        sConfigInjected.InjectedChannel = ADC_CHANNEL_VREFINT;
        sConfigInjected.InjectedRank = 2;
        HAL_ADCEx_InjectedConfigChannel(&hadc1, &sConfigInjected);
        
        /* Peripheral interrupt init*/
        HAL_NVIC_SetPriority(ADC_IRQn, 0xE, 0);
        HAL_NVIC_EnableIRQ(ADC_IRQn);
  
        HAL_ADCEx_InjectedStart_IT(&hadc1);//запускаем первое преобразование
  
        while (ADC_Cnt < 32) {/*whait*/}
	{
		ADC_Ref>>=5;  // Усреднение измерений
		ADC_Bat>>=5;
		
		// Расcчитываю напряжение аккумулятора, мВ
		batt_voltage_mV = (u16) (((float)ADC_Bat / (float)ADC_Ref) * REFERENCE_VOLTAGE * VOLTAGE_DIVIDER);
		// Получаю напряжение аккумулятора (в миливольтах)
		u16 temp_voltage=batt_voltage_mV;
		// Расчитываю уровень заряда аккумулятора (в процентах)
		if (temp_voltage<=BATT_VMIN) temp_voltage=0;
		else temp_voltage = (u16) ((temp_voltage-BATT_VMIN) / ((BATT_VMAX-BATT_VMIN)/100.0F));
		// Уровень заряда аккумулятора не может быть больше 100%
		

                
                if (temp_voltage>100) temp_voltage=100;
		// Возвращаю процент заряда
		batt_percent=temp_voltage;
		
		ADC_Ref=0;
		ADC_Bat=0;
		ADC_Cnt=0;
		HAL_ADCEx_InjectedStart_IT(&hadc1);// Запуск преобразования
	}
}


void ADC_IRQHandler (void)
{
  HAL_ADC_IRQHandler(&hadc1);
}

void HAL_ADCEx_InjectedConvCpltCallback(ADC_HandleTypeDef* hadc)
{
	if (ADC_Cnt<32)  // Измерение 32 раза
	{
		ADC_Ref += HAL_ADCEx_InjectedGetValue(&hadc1,2);
		ADC_Bat += (HAL_ADCEx_InjectedGetValue(&hadc1,1)+1);
		ADC_Cnt++;
		HAL_ADCEx_InjectedStart_IT(&hadc1);// Запуск преобразования
	}
        else
        {
          
          ulBatrerySample = ADC_Bat; /* Battery voltage in ADC sample */
          ulRefSample = ADC_Ref;   
          
          ADC_Ref=0;
	  ADC_Bat=0;
	  ADC_Cnt=0;
	  HAL_ADCEx_InjectedStart_IT(&hadc1);// Запуск преобразования
        };
  
}


//=============================================================
//    Функция измеряет уровень заряда аккумулятора (мВ + %)
//=============================================================
void battery_check (void)
{
	//if (batt_voltage_mV==0) ADC1->CR2 |= ADC_CR2_JSWSTART;  // Запуск преобразования
	
	//if (ADC_Cnt>=32)
	{
		
                __HAL_ADC_DISABLE_IT(&hadc1,ADC_IT_JEOC); //ADC_ITConfig(ADC1, ADC_IT_JEOC, DISABLE);
                u32 ulTempBat = ulBatrerySample >> 5 ;
                u32 ulTempRef = ulRefSample >> 5 ;
               	__HAL_ADC_ENABLE_IT(&hadc1,ADC_IT_JEOC); //ADC_ITConfig(ADC1, ADC_IT_JEOC, ENABLE);
                
		// Расcчитываю напряжение аккумулятора, мВ
		batt_voltage_mV = (u16) (((float)ulTempBat / (float)ulTempRef) * REFERENCE_VOLTAGE * VOLTAGE_DIVIDER);
		// Получаю напряжение аккумулятора (в миливольтах)
		float temp_voltage=batt_voltage_mV;
		// Расчитываю уровень заряда аккумулятора (в процентах)
		//if (temp_voltage < BATT_VMIN) temp_voltage=0;
		temp_voltage = ((temp_voltage-BATT_VMIN) / ((BATT_VMAX-BATT_VMIN)/100.0F));
		
                #define SHMIT_TRG_BAT_VOLT_PR 1.5 
                char strBuf[10];
                sprintf(strBuf, "%3.0f%%", temp_voltage);
               // LCDBUFFER_Str_Shadow (strBuf,LCD_BATT_X+1,LCD_BATT_Y+40,WHITE,1,1);
               
                if (  temp_voltage > (batt_percent+SHMIT_TRG_BAT_VOLT_PR)) 
                {
                  temp_voltage =  batt_percent+1;
                  if (temp_voltage > 100) temp_voltage = 100;
                  batt_percent = temp_voltage;
                }
                else if ( temp_voltage < (batt_percent-SHMIT_TRG_BAT_VOLT_PR) )
                {
                  temp_voltage =  batt_percent-1;
                  if (temp_voltage < 0) temp_voltage = 0;
                  batt_percent = temp_voltage;
                }
                
	

	}
	
	// Если аккумулятор разряжен - отключаю питание
	if ((PWR_OnBattery) && (batt_voltage_mV<BATT_VMIN))
	{
		//PWR_OnUser=0;
		//DisplayMode=DisplayMode_PowerOff;
	}
	
	//Task_del(TASK_BATT_CHECK);
}


//================================================================================
// Функция рисует на дисплее символ батарейки, показывающей уровень заряда
// В параметрах задаются координаты вывода символа батарейки (левый верхний угол)
//================================================================================
void battery_icon (void)
{
	u16 batt_scale;
	u16 batt_color;
	
	//LCDBUFFER_Rectangle (LCD_BATT_X, LCD_BATT_Y, LCD_BATT_X+30, LCD_BATT_Y+15, BLACK, 1);	// Очищаю место на дисплее под индикатор
	if (batt_show && batt_voltage_mV)
	{
		LCDBUFFER_Rectangle (LCD_BATT_X+1, LCD_BATT_Y+1, LCD_BATT_X+26, LCD_BATT_Y+13, WHITE, 0);	// "Корпус" батареи
		LCDBUFFER_Rectangle (LCD_BATT_X+26, LCD_BATT_Y+6, LCD_BATT_X+29, LCD_BATT_Y+10, WHITE, 1);	// "Носик" батареи
		if (PWR_OnAdapter)
		{
			batt_scale = 24;  // Заполнить шкалу АКБ полностью
			
                        //if (batt_voltage_mV<BATT_VCHARGED) batt_color=RED;
			//else batt_color=GREEN;
                        if (batt_percent < 100) batt_color=RED;
                        else  batt_color=GREEN;
		}
		else
		{
			batt_scale = (u16) (24.0F / 100.0F * batt_percent);  // Расчёт шкалы АКБ
			batt_color=WHITE;  // Белый цвет если разряжается
		}
		// Вывод уровня заряда на дисплей
		LCDBUFFER_Alfa_Rectangle (LCD_BATT_X+2, LCD_BATT_Y+2, LCD_BATT_X+2+batt_scale, LCD_BATT_Y+13, batt_color, 0.5f);
		//char strBuf[10];
                //sprintf (strBuf, "%3.0f%%", batt_percent);
                //LCDBUFFER_Str_Shadow (strBuf,LCD_BATT_X+30,LCD_BATT_Y+4,WHITE,1,1);
                //LCDBUFFER_Num_Shadow (batt_voltage_mV,LCD_BATT_X+1,LCD_BATT_Y+50,WHITE,1,1);
	}
	///Task_add(TASK_LCD_UPDATE);
	Task_del(TASK_BATT_ICON);
}


