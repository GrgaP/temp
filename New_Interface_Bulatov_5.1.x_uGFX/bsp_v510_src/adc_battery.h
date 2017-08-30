#ifndef ADC_BATTERY_H		// Блокируем повторное включение этого модуля
#define ADC_BATTERY_H

#include "main.h"

#define REFERENCE_VOLTAGE 1200	// Опорное напряжение 1,2В
#define VOLTAGE_DIVIDER 11.0F	// Делитель в цепи измерения напряжения 1:11
//#define ADC_CH_BATT 4			// Канал АЦП, к которому подключено измеряемое напряжение
//#define ADC_CH_REF 6			// Канал АЦП, к которому подключено опорное напряжение

extern u16 batt_voltage_mV;
extern float batt_percent;
extern u8 batt_check_timer;
extern u8 batt_show_timer;
extern u8 batt_show;

////////////////////////////////////////////////
//    Секция прототипов локальных функций     //
////////////////////////////////////////////////
void ADC_init (void);
void battery_check (void);
void battery_icon (void);

#endif