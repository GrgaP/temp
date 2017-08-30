#include "keyboard_tmr.h"
#include "menu_calibration.h"


volatile u8 KB_Flag=0;

u8  KeyState=0;  // ����� ������� ������������� ������ ����������
u8  KeyScan=0;   // ��������������� ���������� ��� ������������
u8  KeyOld=0;
u32 KeyPressTime=0;

static u32 sysTickTim6 = 0;

volatile u16 Keyb_Pause = Keyb_MaxPause;	// ������� ����� �������� (ms)
volatile u16 Keyb_HoldTime = 0;			// ������� ����� ��������� ������ (ms)


//=================================================================//
//  ��������� ���������� ������� ��� �������������� ������� �����  //
//  ������� ���������� 100 ��� � �������                           //
//=================================================================//
void SysTimer_Init (void)
{
	// �������� ����������� ����������
	NVIC_SetPriority(TIM6_DAC_IRQn, 3);		// ��������� ���������� ���������� �� TIM6
	NVIC_EnableIRQ(TIM6_DAC_IRQn);			// ��������� ���������� �� TIM6
	// �������� ������ TIM6 (���������� 100 ��� � �������)
	// ��� ��� ������������ ���� ������ ���������� �� 6 ���������� (�� 6 ��������), ����� ���� ������ ���������� 16 ��� � �������
	RCC->APB1ENR |= RCC_APB1ENR_TIM6EN;		// �������� ������������ ������� TIM6
	TIM6->PSC = 54000-1;					// ( 45 ��� * 2 ) = 90 ��� / ������������ 45000 = 2000 ����� � �������
	TIM6->ARR = 20-1;						// ������� ��������� ( ��������� ���������� ������ 20 �����, ������ 2000 / 20 = 100 ��� � ������� )
	TIM6->DIER |= TIM_DIER_UIE;				// ��������� ��������� ���������� �� TIM6
	TIM6->CR1 |= TIM_CR1_CEN;				// ������ �����
}


//=============================================//
//   ������������� GPIO ��������� ����������   //
//=============================================//
void Keyboard_init (void)
{
    __GPIOE_CLK_ENABLE();
    __GPIOG_CLK_ENABLE();  
  
    GPIO_InitTypeDef GPIO_InitStruct;
  
    /*Configure GPIO pins : PE2 PE3 PE4 (KEYB_READ 0/1/2) */
  GPIO_InitStruct.Pin = GPIO_PIN_2|GPIO_PIN_3|GPIO_PIN_4;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);
	
  /*Configure GPIO pins : PG9 PG13 PG14 (KEYB_SW 0/1/2) */
  GPIO_InitStruct.Pin = GPIO_PIN_9|GPIO_PIN_13|GPIO_PIN_14;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_LOW;
  HAL_GPIO_Init(GPIOG, &GPIO_InitStruct);
  
  HAL_GPIO_WritePin(GPIOG, GPIO_PIN_9|GPIO_PIN_13|GPIO_PIN_14, GPIO_PIN_SET);
}

//============================================//
//  ���������� ���������� ���������� �������  //
//  ���������� 100 ��� � �������              //
//  ������������� �����:                      //
//  -������ ����������                        //
//  -���������� ��������                      //
//  -���������� ����������                    //
//============================================//
void TIM6_DAC_IRQHandler (void)
{
	TIM6->SR &= ~TIM_SR_UIF;	// ������ ����, ����� ���������� ����� ����������� ���������
	
	// �������� ��������� �������� �������� � ����
	if (SETPARAMS.iKey==0)
	{
		Keyb_HoldTime=0;
		TIM4->CR1 &= ~TIM_CR1_CEN;	// ������ �� ������ - ������������ � ��������� ������
	}
	else
	{
		if (Keyb_HoldTime<Keyb_SpeedupTime) Keyb_HoldTime+=10;
	}
	
	// ������ �������� ������ USART1/DMA
	if (--usart_RX_timeout==0) UsartState=UsartState_TimError;
	
	// ������ ��������� ���
	if (batt_check_timer) batt_check_timer--; else
	{
		tasks |= TASK_BATT_CHECK;	// ���������� �� ������� ������ ���������
		batt_check_timer=BATT_CHECK_INTERVAL;	// restart timer
	}
	
	// ������ �����������/������� ������� ���
	if (batt_show_timer) batt_show_timer--; else
	{
		if (batt_percent<20) {if (batt_show) batt_show=0; else batt_show=1;}	// blink batt icon
		else {batt_show=1;}														// show batt icon
		tasks |= TASK_BATT_ICON;	// ���������� �� ������� ������ ���
		batt_show_timer=BATT_SHOW_INTERVAL;	// restart timer
	}
	
	// ����������� �������� � �������� ������� ���������
	if (Grad_Lvl_Trgt >= (Grad_Lvl_Curr+Grad_Stp_Up)) Grad_Lvl_Curr+=Grad_Stp_Up;	// ����
	if (Grad_Lvl_Trgt <= (Grad_Lvl_Curr-Grad_Stp_Down)) Grad_Lvl_Curr-=Grad_Stp_Down;	// ����
	
	// ��������� �����������
	HAL_GPIO_WritePin(GPIOF, GPIO_PIN_6, !(HAL_GPIO_ReadPin(GPIOF,GPIO_PIN_6)));//GPIO_ToggleBits (LED_PORT, HL1);
	
	KB_Flag=1;    Keyboard_Scan(); // ����� ����������
	PWR_Flag=1;   Power_Update(); // ���������� ��������
	BL_Flag=1;    //Backlight_Update(); // ���������� ����������
	BUZ_Flag=1;   Buzzer_Update(); // ���������� ��������
	DISOR_Flag=1; Disorber_Update();// ���������� ����������
        sysTickTim6++;
}

WaitStatus WaitUntilStTim6(TickCountType* LastTime, TickCountType msTime)
{
  if((sysTickTim6 - *LastTime) < msTime) return(WAIT);
  else 
  {
    *LastTime = sysTickTim6; 
    return(READY);
  };
}

TickCountType GetTickCountStTim6(void)
{
  return(sysTickTim6);
}



//===============================================//
//  ������� ������������ ����������              //
//  ��������� 100 ��� � �������                  //
//  ������������ ����������� �� 4 �������        //
//  ������� ������������ 100/4=25 ��� � �������  //
//===============================================//
void Keyboard_Scan (void)
{
	if (KB_Flag)
	{
		switch (KeyState)
		{
			// ������ 1
			case(0):
			{
				KeyScan=0;  // ��������� ��������� ����������
				HAL_GPIO_WritePin(GPIOG, GPIO_PIN_9, GPIO_PIN_RESET);  // ������ 0 ���������� � �����
			} break;
			
			// ������ 2
			case(1):
			{
				// �������� ������ �� ��������
                              
				if (!((KEY_PORT->IDR) & KEY_RD_0)) KeyScan|=KEY_U;
				if (!((KEY_PORT->IDR) & KEY_RD_1)) KeyScan|=KEY_L;
				if (!((KEY_PORT->IDR) & KEY_RD_2)) KeyScan|=KEY_RET;
                              
				//if (!((KEY_PORT->IDR) & KEY_RD_0)) KeyScan|=KEY_L;
				//if (!((KEY_PORT->IDR) & KEY_RD_1)) KeyScan|=KEY_D;
				//if (!((KEY_PORT->IDR) & KEY_RD_2)) KeyScan|=KEY_U;
				HAL_GPIO_WritePin(GPIOG, GPIO_PIN_9, GPIO_PIN_SET);  // ������ 0 �������� �� �����
			} break;
			
			// ������ 3
			case(2):
			{
				HAL_GPIO_WritePin(GPIOG, GPIO_PIN_13, GPIO_PIN_RESET); // ������ 1 ���������� � �����
			} break;
			
			// ������ 4
			case(3):
			{
				// �������� ������ �� ��������
				if (!((KEY_PORT->IDR) & KEY_RD_0)) KeyScan|=KEY_D;
				if (!((KEY_PORT->IDR) & KEY_RD_1)) KeyScan|=KEY_R;
				if (!((KEY_PORT->IDR) & KEY_RD_2)) KeyScan|=KEY_ENT;
				//if (!((KEY_PORT->IDR) & KEY_RD_0)) KeyScan|=KEY_ENT;
				//if (!((KEY_PORT->IDR) & KEY_RD_1)) KeyScan|=KEY_RET;
				//if (!((KEY_PORT->IDR) & KEY_RD_2)) KeyScan|=KEY_R;
				HAL_GPIO_WritePin(GPIOG, GPIO_PIN_13, GPIO_PIN_SET); // ������ 1 �������� �� �����
				KeyOld = SETPARAMS.iKey;  // ��� ������, ������� �����
				SETPARAMS.iKey = KeyScan;  // ��� ������, ������� ������
				// �������� ���� ��� ������� ������
				if ((SETPARAMS.iKey) && (SETPARAMS.iKey!=KeyOld)) Buzzer_Keyb();
			}
		};
		
		// �������� � ���������� "�������"
		if (KeyState>=3) KeyState=0; else KeyState++;
		
		// ������ ��������� ������
		if (SETPARAMS.iKey==0) KeyPressTime=0;  // ���� ������ �� ������, ��������� ������
		if (KeyOld==SETPARAMS.iKey) KeyPressTime++; else KeyPressTime=0;  // ���� ������ ������ �������, ������ �����
		if ((KeyPressTime==KeyPowerOn) && (SETPARAMS.iKey==KEY_RET))  // ���� ������ "RETURN" ���������� 1 �������
		{
                  if (PWR_OnUser) {PWR_OnUser=0; DisplayMode=DisplayMode_Starting;} else {PWR_OnUser=1; DisplayMode=DisplayMode_Starting;}  // ���������/���������� �������
			Buzzer_Keyb();
		}
		KB_Flag=0;
	}
}


void Control_Automat (void)
{
	// ������ �����������
/**/if (DisplayMode==DisplayMode_Logo)
	{
		// ������������ �� �������������
		if ((SETPARAMS.iKey==KEY_RET) && Keyb_SpeedDelay()) DisplayMode=DisplayMode_Spectrum;
		// ���� � ����
		if ((SETPARAMS.iKey==KEY_ENT) && Keyb_SpeedDelay()) DisplayMode=DisplayMode_Menu;
	}
	
/**/if (DisplayMode==DisplayMode_PowerOff)
	{
		if (PWR_OnUser) DisplayMode=DisplayMode_Spectrum;
	}
	if (PWR_OnUser==0) DisplayMode=DisplayMode_PowerOff;
	
	
/**/if (DisplayMode==DisplayMode_Calibration)
	{
          vMenuClbAutomat();
	}
	
/**/if (DisplayMode==DisplayMode_Spectrum)
	{
		// ����� ������� ����������� ��� ����������� �������
		if ((SETPARAMS.iKey & (KEY_U|KEY_D|KEY_L|KEY_R)) && Keyb_SpeedDelay())
		{
			if (SETPARAMS.iKey==KEY_U) Spectrum_Float_s16_IncDec(0, 4);
			if (SETPARAMS.iKey==KEY_D) Spectrum_Float_s16_IncDec(1, 4);
			if (SETPARAMS.iKey==KEY_L) Spectrum_Float_s16_IncDec(0, 5);
			if (SETPARAMS.iKey==KEY_R) Spectrum_Float_s16_IncDec(1, 5);
		}
		// ������������ �� ��������
		if ((SETPARAMS.iKey==KEY_RET) && Keyb_SpeedDelay()) {DisplayMode=DisplayMode_Gradient;}
		// ���� � ����
		if ((SETPARAMS.iKey==KEY_ENT) && Keyb_SpeedDelay()) DisplayMode=DisplayMode_Menu;
	}
	
/**/if (DisplayMode==DisplayMode_Gradient)
	{
		// ������������ �� �������������
		if ((SETPARAMS.iKey==KEY_RET) && Keyb_SpeedDelay()) {DisplayMode=DisplayMode_Spectrum;}
		// ���� � ����
		if ((SETPARAMS.iKey==KEY_ENT) && Keyb_SpeedDelay()) DisplayMode=DisplayMode_Menu;
	}
	
/**/if (DisplayMode==DisplayMode_Gan)
	{
		// ������������ �� �������������
		if ((SETPARAMS.iKey==KEY_RET) && Keyb_SpeedDelay()) {DisplayMode=DisplayMode_Spectrum;}
		// ���� � ����
		if ((SETPARAMS.iKey==KEY_ENT) && Keyb_SpeedDelay()) DisplayMode=DisplayMode_Menu;
	}
	
/**/if (DisplayMode==DisplayMode_Menu)
	{
		Menu_Automat();	// ����� �������� "����"
	}
/**/if (DisplayMode==DisplayMode_Starting)
	{

		if ((SETPARAMS.iKey==KEY_ENT) && Keyb_SpeedDelay()) DisplayMode=DisplayMode_Spectrum;
	}
    if (DisplayMode == DisplayMode_Security)
        {
              if ((SETPARAMS.iKey==KEY_RET) && Keyb_SpeedDelay()) 
              {
                ReturnMenu();
                DisplayMode=DisplayMode_Menu;
              }
        }
}


/*
TIM4
����������� ��������� ��������� ��� ����������� �������� ��������
*/
void Keyb_SpeedDelay_Timer_Init (void)
{
	RCC->APB1ENR |= RCC_APB1ENR_TIM4EN;	// ������� ������������ �������
	TIM4->CR1 |= TIM_CR1_OPM;	// ����� ������������ ������������ (��� ������������� ������� "update" ���� ������������)
	TIM4->PSC = 45000-1;		// ( 45 ��� * 2 ) = 90 ��� / ������������ 45000 = 2000 ����� � �������
}


/*
������������ ��� ����������� �������� ��������
*/
u8 Keyb_SpeedDelay (void)
{
	if (!(TIM4->CR1 & TIM_CR1_CEN))
	{
		// ����������� ����� ����� ��������
		Keyb_Pause = (u16) (Keyb_MaxPause-(((float)Keyb_HoldTime/Keyb_SpeedupTime)*(Keyb_MaxPause-Keyb_MinPause)));
		TIM4->ARR = Keyb_Pause*2;	// ��������� �����
		TIM4->CNT=0;				// �������� �������� �������� ��������
		TIM4->EGR |= TIM_EGR_UG;	// �������� ���������� ���������
		TIM4->CR1 |= TIM_CR1_CEN;	// ��������� ������
		return 1;
	}
	return 0;
}
