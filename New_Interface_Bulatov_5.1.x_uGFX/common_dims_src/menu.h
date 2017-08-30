#ifndef MENU_H		// ��������� ��������� ��������� ����� ������
#define MENU_H

#include "main.h"

enum NumericModes_Enum	// ������ ����������� ���������� �� �������
{
	DISP_none      = 0,	// ������ ����������
	DISP_bool      = 1,	// ����� ��� (�������)
	DISP_int       = 2,	// ����������� integer
	DISP_float_s16 = 3,	// ����������� float � ������������ � s16
	DISP_float_s8  = 4	// ����������� float � ������������ � s8
};

typedef const struct _TMenuItem	// ��������� ������ ����
{
	char* const	title;		// ��� ������ ����
	u8 const		items_all;	// ����� ���-�� ������� � ���� ����
	const struct	_TMenuItem *parent;		// ��������� �� ���� �������� ������ (���� ��������� ������, ����� NULL)
	const struct	_TMenuItem *submenu;	// ��������� �� ���� ������� ������ (���� ��������� ������, ����� NULL)
	u32 const		event_enter;	// �������� ��� ������� �� ������ ENTER. ���� �������� �� ���������, ����� TASK_NONE
	u8 const		display_type;	// ��� �����������. 1-bool, 2-float, 3-int. ���� ���������� ������, ����� 0
	s16 const		int_from;	// (integer) ����������� �������� / (bool) ����� ����
	s16* const		int_curr;	// (integer) ������� ��������. ��������� �� ����������, ������� ������������� / (bool) ��������� �� ����������, �������� ������� �����. ���� ������������� ������, ����� NULL
	s16 const		int_to;		// (integer) ������������ ��������
	float const	float_from;	// (float) ����������� ��������
	float const	float_to;	// (float) ������������ ��������
	float const	float_step;	// (float) ��� ���������
        uint8_t secrity; // 0 �������� ������ �� �����
} TMenuItem;



typedef union BITS 
{
  uint16_t data;
  struct
  {
    uint16_t bit0     : 1; /* Use for info window */
    uint16_t bit1     : 1; /* Use for level info */
    uint16_t bit2     : 1;
    uint16_t bit3     : 1;
    uint16_t bit4     : 1;
    uint16_t bit5     : 1;
    uint16_t bit6     : 1;
    uint16_t bit7     : 1; 
    uint16_t bit8     : 1;
    uint16_t bit9     : 1;
    uint16_t bit10    : 1;
    uint16_t bit11    : 1;
    uint16_t bit12    : 1;
    uint16_t bit13    : 1;
    uint16_t bit14    : 1;
    uint16_t bit15    : 1; 
  } bits;
} usBits_TypeDef; /* For usage usBits.bits.bit0 = 1 */


#define MENU_FLAGS_DEF (0)
#define MENU_ALPHA_DEF (20)

extern usBits_TypeDef usMenuFlags;
extern uint16_t usMenuAlpha;

extern TMenuItem MainMenu[];
extern TMenuItem Settings_sensor[];
extern TMenuItem Settings_flags[];
extern TMenuItem Settings_buzzer[];
extern TMenuItem Interface[];
extern TMenuItem System_settings[];
extern TMenuItem Settings_explosive[];
extern TMenuItem pExplMenuTnt[]; 
extern TMenuItem pExplMenu1[];
extern TMenuItem pExplMenu2[];
extern TMenuItem pExplMenu3[];
extern TMenuItem pExplMenu4[];
extern TMenuItem pExplMenu5[];

enum MenuState_Enum		// �������� ��������� ����
{
    MenuState_Idle     = 0,
    MenuState_Working  = 1,
    MenuState_Closing  = 2
};
extern volatile u8 MenuState;
extern volatile u8 menu_pos;


////////////////////////////////////////////////
//    ������ ���������� ��������� �������     //
////////////////////////////////////////////////
void Menu_Automat (void);
void Menu_Stop (void);

void Spectrum_Float_s16_IncDec (u8 IncDec, u8 item);
void Float_s16_IncDec (u8 Increment, s16* Int_C, s16 const* Int_F, s16 const* Int_T, float const* Float_F, float const* Float_T, float const* Float_Step);

void Calibration_Pos (u8 IncDec);
void Calibration_Itrv (u8 IncDec);

void vMenuUpdate(void);

#endif
