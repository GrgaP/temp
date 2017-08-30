#ifndef KEYBOARD_TMR_H		// Блокируем повторное включение этого модуля
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

// Определения для удобной работы с кнопками
#define KEY_U	0x01	// кнопка вверх
#define KEY_D	0x02	// кнопка вниз
#define KEY_L	0x04	// кнопка влево
#define KEY_R	0x08	// кнопка вправо
#define KEY_RET	0x10	// кнопка RETURN
#define KEY_M	0x20	// кнопка MENU
#define KEY_ENT	0x40	// кнопка ENTER

extern volatile u8 KB_Flag;

// Прототипы функций
void Keyboard_init (void);
void Keyboard_Scan (void);
void Keyb_SpeedDelay_Timer_Init (void);
u8 Keyb_SpeedDelay (void);

void Control_Automat (void);
void SysTimer_Init (void);
WaitStatus WaitUntilStTim6(TickCountType* LastTime, TickCountType msTime);
TickCountType GetTickCountStTim6(void);
#endif