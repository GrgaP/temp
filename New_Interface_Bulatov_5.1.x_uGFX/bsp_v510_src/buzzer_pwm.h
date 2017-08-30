#ifndef BUZZER_PWM_H		// Блокируем повторное включение этого модуля
#define BUZZER_PWM_H

#include "main.h"

extern volatile u8 BUZ_Flag;

// Прототипы функций
void Buzzer_init (void);
void Buzzer_deinit (void);

void Buzzer_Update (void);
void Buzzer_Keyb (void);
void Buzzer_Tnt_On (void);
void Buzzer_Tnt_Off (void);

#endif