#ifndef BACKLIGHT_H  // Блокируем повторное включение этого модуля
#define BACKLIGHT_H

#include "main.h"

#define FREQ_BL_PWM 1000  // Частота ШИМ

extern volatile u8 BL_Lcd;
extern volatile u8 BL_Keyb;
extern volatile u8 BL_Flag;

// Прототипы функций
void Backlight_Init (void);
void Backlight_Update (void);
u8 Backlight_Fixed (void);
void Backlight_Alarm();

#endif