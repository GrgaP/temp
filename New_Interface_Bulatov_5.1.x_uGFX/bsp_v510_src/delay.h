#ifndef DELAY_H		// Блокируем повторное включение этого модуля
#define DELAY_H

#include "stm32f7xx.h"
#include "stdint.h"

typedef uint32_t TickCountType;
typedef enum {WAIT = 0, READY = !WAIT} WaitStatus;
 
////////////////////////////////////////////////
//    Секция прототипов локальных функций     //
////////////////////////////////////////////////
void _delay (uint32_t step);
void _delay_ms (uint32_t ms);
void _delay_us (uint32_t us);
WaitStatus WaitUntil(TickCountType* u32LastTime, TickCountType msTime);
TickCountType GetTickCount(void);
#endif /* _DELAY_H */



