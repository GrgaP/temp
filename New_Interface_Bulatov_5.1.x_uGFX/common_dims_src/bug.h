#ifndef BUG_H		// Блокируем повторное включение этого модуля
#define BUG_H

#include "main.h"

#define bug_count 5	// Количество одинаковых байт для срабатывания жука

extern volatile u8 bug[bug_count];
extern volatile u8 bug_actual;

////////////////////////////////////////////////
//    Секция прототипов локальных функций     //
////////////////////////////////////////////////
void bug_init (void);
u8 bug_get_cmd (void);

#endif



