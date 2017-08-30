#ifndef POWER_H  // Блокируем повторное включение этого модуля
#define POWER_H

#include "main.h"

extern volatile u8 PWR_OnUser;
extern volatile u32 PWR_Timer;
extern volatile u8 PWR_Flag;
extern volatile u8 pwrDisplay; // отображаемый дисплей 
extern volatile u32 pwrDisplayCnt;

// Смотрю, подключен ли адаптер
#define PWR_OnAdapter (GPIOC->IDR & GPIO_PIN_2)
#define PWR_OnBattery (!PWR_OnAdapter)

// Прототипы функций
void Power_Init (void);
void Power_Ref_On (void);
void Power_Ref_Off (void);
void Power_Main_On (void);
void Power_Main_Off (void);
void Power_DcDc_On (void);
void Power_DcDc_Off (void);
void Power_Disorber_On (void);
void Power_Disorber_Off (void);
void Power_Update (void);

#endif