#ifndef KEYBOARD_TMR_H		// ��������� ��������� ��������� ����� ������
#define KEYBOARD_TMR_H

#include "main.h"
#include "delay.h"


#define KEY_PORT GPIOE

#define KEY_RD_0 GPIO_PIN_2
#define KEY_RD_1 GPIO_PIN_3
#define KEY_RD_2 GPIO_PIN_4
#define KEY_SW_0 GPIO_PIN_9
#define KEY_SW_1 GPIO_PIN_13
#define KEY_SW_2 GPIO_Pin_14

// ����������� ��� ������� ������ � ��������
#define KEY_U	0x01	// ������ �����
#define KEY_D	0x02	// ������ ����
#define KEY_L	0x04	// ������ �����
#define KEY_R	0x08	// ������ ������
#define KEY_RET	0x10	// ������ RETURN
#define KEY_M	0x20	// ������ MENU
#define KEY_ENT	0x40	// ������ ENTER

extern volatile u8 KB_Flag;

// ��������� �������
void Keyboard_init (void);
void Keyboard_Scan (void);
void Keyb_SpeedDelay_Timer_Init (void);
u8 Keyb_SpeedDelay (void);

void Control_Automat (void);
void SysTimer_Init (void);
WaitStatus WaitUntilStTim6(TickCountType* LastTime, TickCountType msTime);
TickCountType GetTickCountStTim6(void);
#endif