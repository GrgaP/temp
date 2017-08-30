#include "delay.h"
#include "main.h"

volatile static TickCountType TickCount = 0;

//===========================================================
//              Подпрограмма задержки (такты)
//===========================================================
void _delay (uint32_t step)
{
	while(step>>0)
	{
		--step;
	}
}


//===========================================================
//            Подпрограмма задержки (миллисекунды)
//===========================================================
void _delay_ms (uint32_t ms)
{
	volatile uint32_t nCount;
	
        /*RCC_ClocksTypeDef RCC_Clocks;
	RCC_GetClocksFreq (&RCC_Clocks);
	
	nCount=(RCC_Clocks.HCLK_Frequency/10000)*ms;*/
        
        nCount=( HAL_RCC_GetHCLKFreq()/10000)*ms;
	for (; nCount!=0; nCount--);
}


//===========================================================
//            Подпрограмма задержки (микросекунды)
//===========================================================
void _delay_us (uint32_t us)
{
	volatile uint32_t nCount;
	
        /*RCC_ClocksTypeDef RCC_Clocks;
	RCC_GetClocksFreq (&RCC_Clocks);
	
	nCount=(RCC_Clocks.HCLK_Frequency/10)*us;*/
        
        nCount=( HAL_RCC_GetHCLKFreq()/10)*us;
	for (; nCount!=0; nCount--);
}

//===========================================================
//            Прерывание системного таймера
//===========================================================
void SysTick_Handler(void)
{
  HAL_IncTick();  
  osSystickHandler();
  TickCount++;
}

//===========================================================
//            Подпрограмма отложенного выполнения
//===========================================================
// LastTime должна быть static переменной
WaitStatus WaitUntil(TickCountType* LastTime, TickCountType msTime)
{
  if((TickCount - *LastTime) < msTime) return(WAIT);
  else 
  {
    *LastTime = TickCount; 
    return(READY);
  };
}

TickCountType GetTickCount(void)
{
  return(TickCount);
}



