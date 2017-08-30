#ifndef MENU_H		// Блокируем повторное включение этого модуля
#define MENU_H

#include "main.h"

enum NumericModes_Enum	// Режимы отображения информации на дисплее
{
	DISP_none      = 0,	// нечего отображать
	DISP_bool      = 1,	// булев тип (чекбокс)
	DISP_int       = 2,	// отображение integer
	DISP_float_s16 = 3,	// отображение float с конвертацией в s16
	DISP_float_s8  = 4	// отображение float с конвертацией в s8
};

typedef const struct _TMenuItem	// Структура пункта меню
{
	char* const	title;		// Имя пункта меню
	u8 const		items_all;	// Общее кол-во пунктов в этом меню
	const struct	_TMenuItem *parent;		// Указатель на меню верхнего уровня (если указывать некуда, пишем NULL)
	const struct	_TMenuItem *submenu;	// Указатель на меню нижнего уровня (если указывать некуда, пишем NULL)
	u32 const		event_enter;	// Действие при нажатии на кнопку ENTER. Если действие не требуется, пишем TASK_NONE
	u8 const		display_type;	// Тип отображения. 1-bool, 2-float, 3-int. Если отображать нечего, пишем 0
	s16 const		int_from;	// (integer) Минимальное значение / (bool) Номер бита
	s16* const		int_curr;	// (integer) Текущее значение. Указатель на переменную, которая редактируется / (bool) Указатель на переменную, служащую битовым полем. Если редактировать нечего, пишем NULL
	s16 const		int_to;		// (integer) Максимальное значение
	float const	float_from;	// (float) Минимальное значение
	float const	float_to;	// (float) Максимальное значение
	float const	float_step;	// (float) Шаг изменения
        uint8_t secrity; // 0 проверка пароля не нужна
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

enum MenuState_Enum		// Перечень состояний меню
{
    MenuState_Idle     = 0,
    MenuState_Working  = 1,
    MenuState_Closing  = 2
};
extern volatile u8 MenuState;
extern volatile u8 menu_pos;


////////////////////////////////////////////////
//    Секция прототипов локальных функций     //
////////////////////////////////////////////////
void Menu_Automat (void);
void Menu_Stop (void);

void Spectrum_Float_s16_IncDec (u8 IncDec, u8 item);
void Float_s16_IncDec (u8 Increment, s16* Int_C, s16 const* Int_F, s16 const* Int_T, float const* Float_F, float const* Float_T, float const* Float_Step);

void Calibration_Pos (u8 IncDec);
void Calibration_Itrv (u8 IncDec);

void vMenuUpdate(void);

#endif
