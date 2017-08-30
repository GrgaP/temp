#ifndef BACKLIGHT_H  // ��������� ��������� ��������� ����� ������
#define BACKLIGHT_H

#include "main.h"

#define FREQ_BL_PWM 1000  // ������� ���

extern volatile u8 BL_Lcd;
extern volatile u8 BL_Keyb;
extern volatile u8 BL_Flag;

// ��������� �������
void Backlight_Init (void);
void Backlight_Update (void);
u8 Backlight_Fixed (void);
void Backlight_Alarm();

#endif