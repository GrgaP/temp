///////////////////////////////////////////////////////////////////////////
////  Проект для микроконтроллера STM32F429ZI                          ////
////  Дисплей Newhaven NHD-2.4-240320SF-CTXI#-1 (контроллер ILI9341)   ////
////  Частично используется "StdPeriph Library Driver"                 ////
////                                                                   ////
////  Задействованная периферия:                                       ////
////  USART3 - обмен данными                                           ////
////  DMA1_Stream1 - приём данных из USART3 в UsartGETPARAMS           ////
////  DMA1_Stream3 - передача данных из SETPARAMS в USART3             ////
////  DMA2_Stream0 - передача фреймбуфера в LCD                        ////
////  DMA2_Stream1 - копирование из UsartGETPARAMS в GETPARAMS         ////
////  FMC  - дисплейный интерфейс i8080                                ////
////  TIM1 - генерация ШИМ 4кГц для пищалки                            ////
////  TIM3 - отсчёт временнЫх интервалов пищалки                       ////
////  TIM4 - отсчёт задержек для меню и интерфейса                     ////
////  TIM6 - опрос матричной клавиатуры 3x3 клавиши                    ////
////  ADC1 - измер. заряда батареи (PA4-CH4-батарея, PA6-CH6-опора)    ////
////  IWDG - перезагрузка микроконтроллера                             ////
////                                                                   ////
////  Вывод PB12 - выход для запроса данных от слейва                  ////
////  Вывод PB14 - вход для контроля подключения БП                    ////
////  Вывод PB15 - вход для контроля окончания зарядки АКБ             ////
////  Вывод PF0  - светодиод обработчика DMA1_USART3                   ////
////  Вывод PF1  - светодиод обработчика клавиатуры                    ////
////                                                                   ////
////  Протокол обмена упрощён, отправляется команда SETPARAMS+SETKEYS  ////
////     принимается команда GETPARAMS+GETDATA.                        ////
///////////////////////////////////////////////////////////////////////////


//==========================//
//   Переменные и дефайны   //
//==========================//
#include "main.h"
#include "gfx.h"
#include "gui.h"
#include "virt_num_keyboard.h"
#include "passw_text_edit.h"
#include "security.h"

//#include "gmouse_lld_MCU_board.h"

SETPARAMS_t SETPARAMS = {0xAA,0x01,20,0,{0,0,0,0,0,0,0,0},0,0,0xAA,0x04,4,0,0,0,0,-1,-1, SYSTEM_SETTINGS_DEF};	// Структура SETPARAMS + SETKEYS
GETPARAMS_t GETPARAMS, UsartGETPARAMS;	// Структура GETPARAMS + GETDATA
u8 E_Sel = 0;	// Калибруемое взрывчатое вещество
EXPLOSIVE_t E_Settings[E_COUNT] =	// Настройки взрывчатых веществ
{{"TNT"},	// Задаю только названия в-в
 {"RDX"},	// Остальные поля считываются из памяти при включении
 {"HMX"},
 {"PENT"},
 {"SUB1"},
 {"SUB2"}};

uint16_t pExplColor[E_COUNT] = EXP_COLOR;

/* Type def for explosive detection  */
/* index [0] use for TNT, and not currently used, but use level */
Explosive_TypeDef  Explosive_Set[EXPLOSIV_COUNT];

osThreadId MainTaskHandle;//

void StartFreeRTOS_IdleTickCode(void);//просто задача индикатор
void Main_Task(void const * argument);//основная задач отвечает за запуск вторичных задач



//int16_t rpPrWnd = 10;
//int16_t rpPrOffset = 0;

int16_t rpTest;

void Calibration_Start (void)
{
	LCDBUFFER_Clear();
	Task_add(TASK_LCD_UPDATE);
	MenuState = MenuState_Idle;				// Завершаю работу меню
	DisplayMode = DisplayMode_Calibration;	// Переключаюсь в режим калибровки
	Task_del(TASK_CALIBRATION);
}


void Gan_Start (void)
{
	LCDBUFFER_Clear();
	Task_add(TASK_LCD_UPDATE);
	MenuState = MenuState_Idle;		// Завершаю работу меню
	DisplayMode = DisplayMode_Gan;
	Task_del(TASK_GAN);
}

void vExplLevelCheck(void)
{
  for ( uint32_t num = 0; num < EXPLOSIV_COUNT; num++)
    Explosive_Set[num].usLevelAlarm = ( Explosive_Set[num].usLevelAlarm < SETPARAMS.iTrackValue[2]) ? SETPARAMS.iTrackValue[2] :  Explosive_Set[num].usLevelAlarm;
}

/** System Clock Configuration
*/
void SystemClock_Config(void)
{

  RCC_OscInitTypeDef RCC_OscInitStruct;
  RCC_ClkInitTypeDef RCC_ClkInitStruct;
  RCC_PeriphCLKInitTypeDef PeriphClkInitStruct;

  __PWR_CLK_ENABLE();

  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE|RCC_OSCILLATORTYPE_LSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.LSEState = RCC_LSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 4;
  RCC_OscInitStruct.PLL.PLLN = 216;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 9;
  HAL_RCC_OscConfig(&RCC_OscInitStruct);

  HAL_PWREx_ActivateOverDrive();

  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;
  HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_7);

  PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_LTDC|RCC_PERIPHCLK_RTC
                              |RCC_PERIPHCLK_USART1|RCC_PERIPHCLK_USART3
                              |RCC_PERIPHCLK_I2C3|RCC_PERIPHCLK_CLK48;
  PeriphClkInitStruct.PLLSAI.PLLSAIN = 50;
  PeriphClkInitStruct.PLLSAI.PLLSAIR = 2;
  PeriphClkInitStruct.PLLSAI.PLLSAIQ = 2;
  PeriphClkInitStruct.PLLSAI.PLLSAIP = RCC_PLLSAIP_DIV2;
  PeriphClkInitStruct.PLLSAIDivQ = 1;
  PeriphClkInitStruct.PLLSAIDivR = RCC_PLLSAIDIVR_8;
  PeriphClkInitStruct.RTCClockSelection = RCC_RTCCLKSOURCE_LSE;
  PeriphClkInitStruct.Usart1ClockSelection = RCC_USART1CLKSOURCE_PCLK2;
  PeriphClkInitStruct.Usart3ClockSelection = RCC_USART3CLKSOURCE_PCLK1;
  PeriphClkInitStruct.I2c3ClockSelection = RCC_I2C3CLKSOURCE_PCLK1;
  PeriphClkInitStruct.Clk48ClockSelection = RCC_CLK48SOURCE_PLL;
  HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct);

  HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq()/1000);

  HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);

  /* SysTick_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(SysTick_IRQn, 15, 0);
} 

/**
  * CPU L1-Cache enable
  */
static void CPU_CACHE_Enable (void)
{
	/* Enable I-Cache */
	SCB_EnableICache();

	/* Enable D-Cache */
	SCB_EnableDCache();
}

//===========================================================
//                    Главная программа
//===========================================================


int main (void) 
{
    
        //CPU_CACHE_Enable();			// Enable the CPU Cache
        
	HAL_Init();
        
       /* __PWR_CLK_ENABLE();
        
        __GPIOD_CLK_ENABLE();
        
        static GPIO_InitTypeDef GPIO_InitStruct;
        

        GPIO_InitStruct.Pin = GPIO_PIN_5;
        GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
        GPIO_InitStruct.Pull = GPIO_NOPULL;
        GPIO_InitStruct.Speed = GPIO_SPEED_LOW;
        HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);
        
        HAL_GPIO_WritePin(GPIOD, GPIO_PIN_5, GPIO_PIN_SET);*/
        
        /* Configure the system clock */
        SystemClock_Config();
        
        __GPIOA_CLK_ENABLE();
        __GPIOB_CLK_ENABLE();
        __GPIOC_CLK_ENABLE();
        __GPIOD_CLK_ENABLE();
        __GPIOE_CLK_ENABLE();
        __GPIOF_CLK_ENABLE();
        __GPIOG_CLK_ENABLE();
        
        Power_Init();
        
        DIMS_RTC_CalendarBkupInit();        
           
        Backlight_Init();  // TIM2/3 подсветка дисплея и клавиатуры, шим
        
        Power_Main_On();
                  
        HAL_Delay(100);        
           
	//LCD_init();  // LCD на FMC          
	
	LEDs_Init();
	Backlight_Init();  // TIM3 подсветка дисплея и клавиатуры, шим
	Buzzer_init();	// TIM8 пищалка, шим
	Keyboard_init();
	Keyb_SpeedDelay_Timer_Init();  // TIM4
	
        //UART_module_init();//USART1_Init();
	
	//ADC_init();  // Для измерения заряда АКБ
	//Flash_read_settings(); 
        SysTimer_Init();  // TIM6 системный таймер для запуска задач
        Backlight_Alarm();
        
        //vSecurityInit();

	//About_logo();
        
        //gwinAttachMouse(0);
        
        //geventListenerInit(&gl);
	//gwinAttachListener(&gl);

	//gdispGDrawCircle(GDISP,30,100,20,RED);
        //gdispGDrawCircle(GDISP,55,100,20,GREEN);
        //gdispGDrawCircle(GDISP,80,100,20,BLUE);

        StartFreeRTOS_IdleTickCode();//просто задача индикатор
        
        //osThreadDef(MainTask, Main_Task, osPriorityNormal, 0, 1024);
        //MainTaskHandle = osThreadCreate(osThread(MainTask), NULL);
        
        GUI_start();
        
        osKernelStart();
        
        while (1);
        
}

/**
  * @brief  Code for OS monitoring
  * @param  None
  * @retval None
  */
void FreeRTOS_IdleTickCode (void *pvPrams)
{
  /* init code for USB_DEVICE */
  #define LED_ON_TICK 50 
  #define LED_OFF_TICK 500
  
  HAL_GPIO_WritePin(GPIOD, GPIO_PIN_5, GPIO_PIN_RESET);
  
  for (;;)
  {
     HAL_GPIO_WritePin(GPIOD, GPIO_PIN_5, !(HAL_GPIO_ReadPin(GPIOD,GPIO_PIN_5)));
      vTaskDelay(LED_ON_TICK);
      HAL_GPIO_WritePin(GPIOD, GPIO_PIN_5, !(HAL_GPIO_ReadPin(GPIOD,GPIO_PIN_5)));
      vTaskDelay(LED_OFF_TICK);
  }
}

/**
  * @brief  Launch OS monitoring application
  * @param  None
  * @retval None
  */
void StartFreeRTOS_IdleTickCode(void)
{
  xTaskCreate(  FreeRTOS_IdleTickCode, 
                "FreeRTOS_IdleTick", 
                configMINIMAL_STACK_SIZE, 
                NULL, 
                tskIDLE_PRIORITY, 
                NULL
              );
}

void Main_Task(void const * argument)
{
   for(;;)
   {    
    
               /* HAL_GPIO_WritePin(GPIOD, GPIO_PIN_5, !(HAL_GPIO_ReadPin(GPIOD,GPIO_PIN_5)));
		//перенесено в keybord_tmr Keyboard_Scan();    // Опрос клавиатуры, ускоренный перебор значений??????
		//перенесено в keybord_tmr Power_Update();         // Управление питанием
		//Backlight_Update();   // Управление подсветкой
		//перенесено в keybord_tmr Buzzer_Update();        // Управление пищалкой
		//Disorber_Update();      // Управление дисорбером
		
		USART_DMA_Automat();    // Обмен данными
                vSpectraCalcAutomat();  // Общитываем спектры (находим пики и прочее), историю и т.д. 
		Control_Automat();      // Управление кнопками
                
                if(tasks & TASK_FB_WR_EN) 
                {
                  Display_Automat();      // Отображение на дисплей
                  Task_del(TASK_FB_WR_EN);
                };
               
                battery_check(); 
                vExplLevelCheck();    // проверяем не задан ли уровень поиска пиков меньше возможного
                
                // Типа список задач
		// Если возведён флаг соответствующей задачи, запускаю её на выполнение
		if (tasks & TASK_SETTINGS_WRITE)   Flash_write_settings();	// Сохранение настроек
		if (tasks & TASK_MENU_STOP)        Menu_Stop();			// Выход из меню
		if (tasks & TASK_CALIBRATION)      Calibration_Start();	// Установка границ обнаружения под разные вещества
		if (tasks & TASK_GAN)              Gan_Start();			// Отображение осциллограммы ГАН
		//if (tasks & TASK_BATT_CHECK)       battery_check();		// Измерение уровня заряда батареи
		//if (tasks & TASK_BATT_ICON)        battery_icon();		// Отобразить на дисплее иконку батарейки
		if (tasks & TASK_BUZZER_OFF)       Buzzer_deinit();		// Отключить пищалку
		if (tasks & TASK_BUZZER_ON)        Buzzer_init();		// Включить пищалку
		if (tasks & TASK_DISORBER_ON_OFF)  Disorber_On_Off();	// Включить/отключить десорбер
                                                   Backlight_Alarm();
                if (tasks & TASK_FABRICATION_LOAD) vSetFabricationSettings();		// Вывожу фрейм-буфер на дисплей
                if (tasks & TASK_LCD_UPDATE)       LCD_Update();		// Вывожу фрейм-буфер на дисплей

                while (!LcdBufferNotUsed){ asm("NOP");};*/
          vTaskDelay(5);
        }
}

void HardFault_Handler(void)
{
  HAL_GPIO_WritePin(GPIOD, GPIO_PIN_5, !(HAL_GPIO_ReadPin(GPIOD,GPIO_PIN_5)));
  HAL_Delay(100);
}

/**
  * @brief This function provides accurate delay (in milliseconds) based 
  *        on SysTick counter flag.
  * @note This function is declared as __weak to be overwritten in case of other
  *       implementations in user file.
  * @param Delay: specifies the delay duration in milliseconds.
  * @retval None
  */
void HAL_Delay (__IO uint32_t Delay)
{
  while(Delay) 
  {
    if (SysTick->CTRL & SysTick_CTRL_COUNTFLAG_Msk) 
    {
      Delay--;
    }
  }
}

