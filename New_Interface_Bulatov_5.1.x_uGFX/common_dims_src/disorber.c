#include "disorber.h"

volatile u8 DISOR_Flag=0;
u8 Disorber_frac=0;
volatile u8 Disorber_sec=0;


void Disorber_On_Off (void)
{
	LCDBUFFER_Clear();
	///Task_add(TASK_LCD_UPDATE);
	MenuState = MenuState_Idle;  // Закрыть меню
	
	if (Disorber_sec)
	{
		Disorber_sec=0;  // Отключить десорбер
	}
	else
	{
		Disorber_sec=DISORBER_TIME;  // Включить десорбер
	}
	DisplayMode = DisplayMode_Spectrum;
	Task_del(TASK_DISORBER_ON_OFF);
}


// Управление десорбером
// Запускать 100 раз в секунду
void Disorber_Update (void)
{
	if (DISOR_Flag)
	{
		DISOR_Flag=0;
		if (Disorber_frac++ >= 100)
		{
			Disorber_frac=0;
			if (Disorber_sec) {Disorber_sec--; Power_Disorber_On();}
			else {Power_Disorber_Off();}
		}
	}
}

