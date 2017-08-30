#include "menu.h"
#include "security.h"

// Текст заголовка "Главного меню"
//char* const MainMenu_title = "---Главное меню---";
char* const MainMenu_title = "---Главное меню---";

float fMenuAlpha = ((float)(MENU_ALPHA_DEF))/100.f; 
uint16_t usMenuAlpha = MENU_ALPHA_DEF;

font_t fontTitle, fontTemp; 

usBits_TypeDef usMenuFlags = {MENU_FLAGS_DEF}; 
/* bit0: 1 - show info window, 0 - don't show */

u8 MenuNeedUpdate=0;

TMenuItem MainMenu[]=	// Главное меню
{
	{"Десорбер вкл/откл",	       12,	NULL,	NULL,			TASK_DISORBER_ON_OFF,	DISP_none, 0,0,0,0,0,0,0},
	{"Сохранить настройки",	        0,	NULL,	NULL,			TASK_SETTINGS_WRITE,	DISP_none, 0,0,0,0,0,0,0},
	{"Загр. зав. настр.",	        0,	NULL,	NULL,			TASK_FABRICATION_LOAD,	DISP_none, 0,0,0,0,0,0,0},
        {"Настроить сенсор",		0,	NULL,	Settings_sensor,	TASK_NONE,		DISP_none, 0,0,0,0,0,0,1},
	{"Настроить флаги",		0,	NULL,	Settings_flags,		TASK_NONE,		DISP_none, 0,0,0,0,0,0,1},
	{"Настроить звук",		0,	NULL,	Settings_buzzer,	TASK_NONE,		DISP_none, 0,0,0,0,0,0,0},
	{"Калибровка",		        0,	NULL,	NULL,			TASK_CALIBRATION,       DISP_none, 0,0,0,0,0,0,0},
        {"Настр. вещ.",                 0,      NULL,   Settings_explosive,     TASK_NONE,              DISP_none, 0,0,0,0,0,0,0},
	{"Осциллограмма ГАН",		0,	NULL,	NULL,			TASK_GAN,		DISP_none, 0,0,0,0,0,0,1},
        {"Системные настройки",	        0,	NULL,	System_settings,	TASK_NONE,		DISP_none, 0,0,0,0,0,0,1},
	{"Интерфейс",	                0,	NULL,	Interface,	        TASK_NONE,		DISP_none, 0,0,0,0,0,0,0},
        {"Выход",			0,	NULL,	NULL,			TASK_MENU_STOP,		DISP_none, 0,0,0,0,0,0,0}
};

 
TMenuItem Old_Settings_explosive[] = 
{
  {"1 Win",  	        10, MainMenu, NULL, TASK_NONE, DISP_float_s16,   0,	                 (void*)&Explosive_Set[1].ulWndId,    (EXP_WND_MAX),     0.0F, EXP_WND_MAX_PR,0.1F , 0},
  {"1 Offset",  	0, MainMenu, NULL, TASK_NONE, DISP_float_s16,   -EXP_OFFSET_MAX,	 (void*)&Explosive_Set[1].lOffsetId, (EXP_OFFSET_MAX) ,  -EXP_OFFSET_MAX_PR, EXP_OFFSET_MAX_PR,0.1F, 0},
  {"2 Win",  	        0, MainMenu, NULL, TASK_NONE, DISP_float_s16,   0,	                 (void*)&Explosive_Set[2].ulWndId,    (EXP_WND_MAX),     0.0F, EXP_WND_MAX_PR,0.1F, 0},
  {"2 Offset",  	0, MainMenu, NULL, TASK_NONE, DISP_float_s16,   -EXP_OFFSET_MAX,	 (void*)&Explosive_Set[2].lOffsetId,(EXP_OFFSET_MAX) ,  -EXP_OFFSET_MAX_PR, EXP_OFFSET_MAX_PR,0.1F, 0},
  {"3 Win",  	        4, MainMenu, NULL, TASK_NONE, DISP_float_s16,   0,	                 (void*)&Explosive_Set[3].ulWndId,    (EXP_WND_MAX),     0.0F, EXP_WND_MAX_PR,0.1F, 0},
  {"3 Offset",  	0, MainMenu, NULL, TASK_NONE, DISP_float_s16,   -EXP_OFFSET_MAX,	 (void*)&Explosive_Set[3].lOffsetId, (EXP_OFFSET_MAX) ,  -EXP_OFFSET_MAX_PR, EXP_OFFSET_MAX_PR,0.1F, 0},
  {"4 Win",  	        0, MainMenu, NULL, TASK_NONE, DISP_float_s16,   0,	                 (void*)&Explosive_Set[4].ulWndId,    (EXP_WND_MAX),     0.0F, EXP_WND_MAX_PR,0.1F, 0},
  {"4 Offset",  	0, MainMenu, NULL, TASK_NONE, DISP_float_s16,   -EXP_OFFSET_MAX,	 (void*)&Explosive_Set[4].lOffsetId,(EXP_OFFSET_MAX) ,  -EXP_OFFSET_MAX_PR, EXP_OFFSET_MAX_PR,0.1F, 0},
  {"5 Win",  	        4, MainMenu, NULL, TASK_NONE, DISP_float_s16,   0,	                 (void*)&Explosive_Set[5].ulWndId,    (EXP_WND_MAX),     0.0F, EXP_WND_MAX_PR,0.1F, 0},
  {"5 Offset",  	0, MainMenu, NULL, TASK_NONE, DISP_float_s16,   -EXP_OFFSET_MAX,	 (void*)&Explosive_Set[5].lOffsetId, (EXP_OFFSET_MAX) ,  -EXP_OFFSET_MAX_PR, EXP_OFFSET_MAX_PR,0.1F, 0},
  
} ;

TMenuItem Settings_explosive[] =
{
  { (void*)Explosive_Set[0].Name,     6, MainMenu, pExplMenuTnt,  TASK_NONE, DISP_none, 0 },
  { (void*)Explosive_Set[1].Name,     0, MainMenu, pExplMenu1,    TASK_NONE, DISP_none , 0},
  { (void*)Explosive_Set[2].Name,     0, MainMenu, pExplMenu2,    TASK_NONE, DISP_none , 0},
  { (void*)Explosive_Set[3].Name,     0, MainMenu, pExplMenu3,    TASK_NONE, DISP_none , 0},
  { (void*)Explosive_Set[4].Name,     0, MainMenu, pExplMenu4,    TASK_NONE, DISP_none , 0},
  { (void*)Explosive_Set[5].Name,     0, MainMenu, pExplMenu5,    TASK_NONE, DISP_none , 0},
};




TMenuItem pExplMenuTnt[] = 
{
  {"Window", 3, Settings_explosive, NULL, TASK_NONE, DISP_float_s16,   0,   (void*)&rpIdWnd,    (EXP_WND_MAX),     0.0F, EXP_WND_MAX_PR,0.5F, 0},
  {"Offset", 0, Settings_explosive, NULL, TASK_NONE, DISP_float_s16,   (-EXP_OFFSET_MAX),	 (void*)&rpIdOffset, EXP_OFFSET_MAX ,(-EXP_OFFSET_MAX_PR), EXP_OFFSET_MAX_PR,0.5F, 0},
  {"Level alarm",0, Settings_explosive, NULL, TASK_NONE, DISP_float_s16,0, (void*)&Explosive_Set[0].usLevelAlarm, (32767), 5.0F, 100.0F,0.5F, 0}
};

TMenuItem pExplMenu1[] = 
{
  {"Window", 4, Settings_explosive, NULL, TASK_NONE, DISP_float_s16,   0,   (void*)&Explosive_Set[1].ulWndId,    (EXP_WND_MAX),     0.0F, EXP_WND_MAX_PR,0.5F, 0},
  {"Offset", 0, Settings_explosive, NULL, TASK_NONE, DISP_float_s16,   -EXP_OFFSET_MAX,	 (void*)&Explosive_Set[1].lOffsetId, (EXP_OFFSET_MAX) ,  -EXP_OFFSET_MAX_PR, EXP_OFFSET_MAX_PR,0.5F, 0},
  {"Level alarm",0, Settings_explosive, NULL, TASK_NONE, DISP_float_s16,0, (void*)&Explosive_Set[1].usLevelAlarm, (32767), 5.0F, 100.0F,0.5F, 0},
  {"On",   0, Settings_explosive, NULL, TASK_NONE, DISP_bool,	   0,  (void*)&Explosive_Set[1].eOnOff}
};

TMenuItem pExplMenu2[] = 
{
  {"Window", 4, Settings_explosive, NULL, TASK_NONE, DISP_float_s16,   0,   (void*)&Explosive_Set[2].ulWndId,    (EXP_WND_MAX),     0.0F, EXP_WND_MAX_PR,0.5F, 0},
  {"Offset", 0, Settings_explosive, NULL, TASK_NONE, DISP_float_s16,   -EXP_OFFSET_MAX,	 (void*)&Explosive_Set[2].lOffsetId, (EXP_OFFSET_MAX) ,  -EXP_OFFSET_MAX_PR, EXP_OFFSET_MAX_PR,0.5F, 0},
  {"Level alarm",0, Settings_explosive, NULL, TASK_NONE, DISP_float_s16,0, (void*)&Explosive_Set[2].usLevelAlarm, (32767), 5.0F, 100.0F,0.5F, 0},
  {"On",   0, Settings_explosive, NULL, TASK_NONE, DISP_bool,	   0,  (void*)&Explosive_Set[2].eOnOff}
};

TMenuItem pExplMenu3[] = 
{
  {"Window", 4, Settings_explosive, NULL, TASK_NONE, DISP_float_s16,   0,   (void*)&Explosive_Set[3].ulWndId,    (EXP_WND_MAX),     0.0F, EXP_WND_MAX_PR,0.5F, 0},
  {"Offset", 0, Settings_explosive, NULL, TASK_NONE, DISP_float_s16,   -EXP_OFFSET_MAX,	 (void*)&Explosive_Set[3].lOffsetId, (EXP_OFFSET_MAX) ,  -EXP_OFFSET_MAX_PR, EXP_OFFSET_MAX_PR,0.5F, 0},
  {"Level alarm",0, Settings_explosive, NULL, TASK_NONE, DISP_float_s16,0, (void*)&Explosive_Set[3].usLevelAlarm, (32767), 5.0F, 100.0F,0.5F, 0},
  {"On",   0, Settings_explosive, NULL, TASK_NONE, DISP_bool,	   0,  (void*)&Explosive_Set[3].eOnOff}

};


TMenuItem pExplMenu4[] = 
{
  {"Window", 4, Settings_explosive, NULL, TASK_NONE, DISP_float_s16,   0,   (void*)&Explosive_Set[4].ulWndId,    (EXP_WND_MAX),     0.0F, EXP_WND_MAX_PR,0.5F, 0},
  {"Offset", 0, Settings_explosive, NULL, TASK_NONE, DISP_float_s16,   -EXP_OFFSET_MAX,	 (void*)&Explosive_Set[4].lOffsetId, (EXP_OFFSET_MAX) ,  -EXP_OFFSET_MAX_PR, EXP_OFFSET_MAX_PR,0.5F, 0},
  {"Level alarm",0, Settings_explosive, NULL, TASK_NONE, DISP_float_s16,0, (void*)&Explosive_Set[4].usLevelAlarm, (32767), 5.0F, 100.0F,0.5F, 0},
  {"On",   0, Settings_explosive, NULL, TASK_NONE, DISP_bool,	   0,  (void*)&Explosive_Set[4].eOnOff},

};

TMenuItem pExplMenu5[] = 
{
  {"Window", 4, Settings_explosive, NULL, TASK_NONE, DISP_float_s16,   0,   (void*)&Explosive_Set[5].ulWndId,    (EXP_WND_MAX),     0.0F, EXP_WND_MAX_PR,0.5F, 0},
  {"Offset", 0, Settings_explosive, NULL, TASK_NONE, DISP_float_s16,   -EXP_OFFSET_MAX,	 (void*)&Explosive_Set[5].lOffsetId, (EXP_OFFSET_MAX) ,  -EXP_OFFSET_MAX_PR, EXP_OFFSET_MAX_PR,0.5F, 0},
  {"Level alarm",0, Settings_explosive, NULL, TASK_NONE, DISP_float_s16,0, (void*)&Explosive_Set[5].usLevelAlarm, (32767), 5.0F, 100.0F,0.5F, 0},
  {"On",   0, Settings_explosive, NULL, TASK_NONE, DISP_bool,	   0,  (void*)&Explosive_Set[5].eOnOff}

};




TMenuItem Settings_sensor[]=	// Настройки сенсора
{
	{"Vrf",			11,	MainMenu,	NULL,	TASK_NONE,	DISP_float_s16,	0,		&SETPARAMS.iTrackValue[0],	180,	12.0F,		180.0F,		1.0F, 0},
	{"Vpull, V",	0,	MainMenu,	NULL,	TASK_NONE,	DISP_float_s16,	-1900,	&SETPARAMS.iTrackValue[1],	1900,	-45.0F,		45.0F,		0.1F, 0},
	{"Уровень",	0,	MainMenu,	NULL,	TASK_NONE,	DISP_float_s16,	50,	&SETPARAMS.iTrackValue[2],	32000,	50.0F,	32000.0F,	50.0F, 0},
       	{"Acq, ms",	0,	MainMenu,	NULL,	TASK_NONE,	DISP_float_s16,	1,		&SETPARAMS.iTrackValue[3],	100,	1.0F,		100.0F,		1.0F, 0},
	{"От, V",		0,	MainMenu,	NULL,	TASK_NONE,	DISP_float_s16,	-31768,	&SETPARAMS.iTrackValue[4],	31767,	-45.0F,		45.0F,		0.1F, 0},
	{"До, V",		0,	MainMenu,	NULL,	TASK_NONE,	DISP_float_s16,	-31768,	&SETPARAMS.iTrackValue[5],	31767,	-45.0F,		45.0F,		0.1F, 0},
	{"From, V",	0,	MainMenu,	NULL,	TASK_NONE,	DISP_float_s16,	-31768,	&SETPARAMS.iTrackValue[6],	31767,	-45.0F,		45.0F,		0.1F, 0},
	{"To,   V",	0,	MainMenu,	NULL,	TASK_NONE,	DISP_float_s16,	-31768,	&SETPARAMS.iTrackValue[7],	31767,	-45.0F,		45.0F,		0.1F, 0},
	{"Epull, V",	0,	MainMenu,	NULL,	TASK_NONE,	DISP_float_s8,	-100,	(void*)&SETPARAMS.ePull,	100,	-10.0F,		10.0F,		0.1F, 0},
	{"Gain",		0,	MainMenu,	NULL,	TASK_NONE,	DISP_float_s8,	0,		(void*)&SETPARAMS.Gain,		6,		0.0F,		6.0F,		1.0F, 0},
        {"Уровень Rp",	0,	MainMenu,	NULL,	TASK_NONE,	DISP_float_s16,	1500,	&rpPeakSerchLevel,	32000,	1500.0F,	32000.0F,	50.0F, 0},
};

TMenuItem Settings_flags[]=	// Настройки флагов
{
	{"Power On/Off",	8, MainMenu, NULL, TASK_NONE,	DISP_bool,	0,	(void*)&SETPARAMS.iFlagBits, 0},	// 0-нулевой бит переменной "iFlagBits"
	{"V_RF On/Off",	0, MainMenu, NULL, TASK_NONE,	DISP_bool,	1,	(void*)&SETPARAMS.iFlagBits, 0},	
	{"V_pull On/Off",	0, MainMenu, NULL, TASK_NONE,	DISP_bool,	2,	(void*)&SETPARAMS.iFlagBits, 0},
	{"V_RF +/-",		0, MainMenu, NULL, TASK_NONE,	DISP_bool,	3,	(void*)&SETPARAMS.iFlagBits, 0},	// 3-третий бит переменной "iFlagBits"
	{"Bluetooth",		0, MainMenu, NULL, TASK_NONE,	DISP_bool,	4,	(void*)&SETPARAMS.iFlagBits, 0},
	{"Win.42/84V",		0, MainMenu, NULL, TASK_NONE,	DISP_bool,	5,	(void*)&SETPARAMS.iFlagBits, 0},
	{"Hck",				0, MainMenu, NULL, TASK_NONE,	DISP_bool,	6,	(void*)&SETPARAMS.iFlagBits, 0},
	{"Manual",			0, MainMenu, NULL, TASK_NONE,	DISP_bool,	7,	(void*)&SETPARAMS.iFlagBits, 0}
};

TMenuItem Settings_buzzer[]=	// Настройки звука
{
	{"Отключить звук",	2,	MainMenu,	NULL,	TASK_BUZZER_OFF,	DISP_none, 0},
	{"Включить звук",		0,	MainMenu,	NULL,	TASK_BUZZER_ON,		DISP_none, 0}
};

TMenuItem Interface[]=
{
  {"Menu transp.",      3, MainMenu, NULL, TASK_NONE, DISP_float_s16,      0,  (void*)&usMenuAlpha,  100,0.F,100.F,1.0F, 0},
  {"Info window",     	0, MainMenu, NULL, TASK_NONE, DISP_bool,	   0,  (void*)&usMenuFlags.bits, 0},
  {"Inf Lvl window",    0, MainMenu, NULL, TASK_NONE, DISP_bool,	   1,  (void*)&usMenuFlags.bits, 0},

};

TMenuItem System_settings[]=
{
  {"Gain Reper",        9, MainMenu, NULL, TASK_NONE, DISP_float_s8, 0,  (void*)&SETPARAMS.systemSettings.gainReper,  6,0.0F,6.0F,1.0F, 0},
  {"Gain Analitic",     0, MainMenu, NULL, TASK_NONE, DISP_float_s8, 0,  (void*)&SETPARAMS.systemSettings.gainAnalitic,  6,0.0F,6.0F,1.0F, 0},
  {"Gan Fun",	        0, MainMenu, NULL, TASK_NONE, DISP_bool,     0,	 (void*)&SETPARAMS.systemSettings.ganFan, 0},
  {"Reper Heat",	0, MainMenu, NULL, TASK_NONE, DISP_bool,     0,	 (void*)&SETPARAMS.systemSettings.reperHeatOn, 0},
  {"Reper Win",  	0, MainMenu, NULL, TASK_NONE, DISP_float_s16,0,	 (void*)&rpIdWnd, (SPECTRA_SIZE/4), 0.0F, 25.0F,0.1F, 0},
  {"Reper Offset",  	0, MainMenu, NULL, TASK_NONE, DISP_float_s16,-SPECTRA_SIZE/4,	 (void*)&rpIdOffset,(SPECTRA_SIZE/4) ,-25.0F, 25.0F,0.1F, 0},
  {"Heater Temp.",  	0, MainMenu, NULL, TASK_NONE, DISP_float_s8,0,	 (void*)&SETPARAMS.systemSettings.cHeaterSetTemp, 80 ,-0.0F, 80.0F,1.0F, 0},
  {"Heater Hist.",  	0, MainMenu, NULL, TASK_NONE, DISP_float_s8,0,	 (void*)&SETPARAMS.systemSettings.cHeaterHistTemp, 15 ,-0.0F, 15.0F,1.0F, 0},
  {"Heater On",	        0, MainMenu, NULL, TASK_NONE, DISP_bool,     0,	 (void*)&SETPARAMS.systemSettings.ucHeaterOn, 0},
};


TMenuItem *menu_current = MainMenu;	// Инициализирую указатель, указывает на Главное меню
u8 menu_pos_parent=0;	// Пункт меню, выбранный в меню верхнего уровня
volatile u8 menu_pos=0;	// Выбранный пункт текущего меню
volatile u8 MenuState = MenuState_Idle;


//=======================================================//
//   Действия, которые надо сделать при выходе из меню   //
//=======================================================//
void Menu_Stop (void)
{
	MenuState = MenuState_Closing;
	Task_del(TASK_MENU_STOP);
}

// Преобразование s16-->float
float Menu_Float_s16_read (u8 item)
{
	float f_curr = (((((menu_current+item)->float_to)-((menu_current+item)->float_from))/(((menu_current+item)->int_to)-((menu_current+item)->int_from)))   *    (*((menu_current+item)->int_curr)-((menu_current+item)->int_from)))   +   ((menu_current+item)->float_from);
	return f_curr;
}


// Преобразование s8-->float
float Menu_Float_s8_read (u8 item)
{
	float f_curr = (((((menu_current+item)->float_to)-((menu_current+item)->float_from))/(((menu_current+item)->int_to)-((menu_current+item)->int_from)))   *    (*(s8*)((menu_current+item)->int_curr)-((menu_current+item)->int_from)))   +   ((menu_current+item)->float_from);
	return f_curr;
}


/*
IncDec - увеличиваем или уменьшаем значение
Int_C - указатель на изменяемую переменную s16 Current
Int_F - указатель на константу s16 From
Int_T - указатель на константу s16 To
Float_F - указатель на константу Float From
Float_T - указатель на константу Float To
Float_Step - указатель на константу Float Step (шаг изменения)
*/
void Float_s16_IncDec (u8 Increment, s16* Int_C, s16 const* Int_F, s16 const* Int_T, float const* Float_F, float const* Float_T, float const* Float_Step)
{
	// Конвертирую s16->Float, запоминаю в переменную "Float Current"
	float Float_C = (((*Float_T-*Float_F)/(*Int_T-*Int_F))   *   (*Int_C-*Int_F))   +   *Float_F;
	if (Increment)
	{
		if ((*Float_T-Float_C)>=*Float_Step) Float_C+=*Float_Step;
		else Float_C=*Float_T;	// Достигли максимального значения
	}
	else
	{
		if ((Float_C-*Float_F)>=*Float_Step) Float_C-=*Float_Step;
		else Float_C=*Float_F;	// Достигли минимального значения
	}
	// Конвертирую Float->s16, возвращаю значение
	*Int_C = (s16) roundf(    ((Float_C-*Float_F)    /    ((*Float_T-*Float_F)/(*Int_T-*Int_F)))    +   *Int_F    );
}

/*
IncDec - увеличиваем или уменьшаем значение
Int_C - указатель на изменяемую переменную s8 Current
Int_F - указатель на константу s16 From
Int_T - указатель на константу s16 To
Float_F - указатель на константу Float From
Float_T - указатель на константу Float To
Float_Step - указатель на константу Float Step (шаг изменения)
*/
void Float_s8_IncDec (u8 Increment, s8* Int_C, s16 const* Int_F, s16 const* Int_T, float const* Float_F, float const* Float_T, float const* Float_Step)
{
	// Конвертирую s8->Float, запоминаю в переменную "Float Current"
	float Float_C = (((*Float_T-*Float_F)/(*Int_T-*Int_F))   *   (*Int_C-*Int_F))   +   *Float_F;
	if (Increment)
	{
		if ((*Float_T-Float_C)>=*Float_Step) Float_C+=*Float_Step;
		else Float_C=*Float_T;	// Достигли максимального значения
	}
	else
	{
		if ((Float_C-*Float_F)>=*Float_Step) Float_C-=*Float_Step;
		else Float_C=*Float_F;	// Достигли минимального значения
	}
	// Конвертирую Float->s8, возвращаю значение
	*Int_C = (s8) roundf(    ((Float_C-*Float_F)    /    ((*Float_T-*Float_F)/(*Int_T-*Int_F)))    +   *Int_F    );
}

/*
IncDec - увеличиваем или уменьшаем значение
*/
void Menu_Float_s16_IncDec (u8 IncDec)
{
	Float_s16_IncDec (IncDec, ((menu_current+menu_pos)->int_curr), (s16*)&((menu_current+menu_pos)->int_from), (s16*)&((menu_current+menu_pos)->int_to), (float*)&((menu_current+menu_pos)->float_from), (float*)&((menu_current+menu_pos)->float_to), (float*)&((menu_current+menu_pos)->float_step));
}

/*
IncDec - увеличиваем или уменьшаем значение
*/
void Menu_Float_s8_IncDec (u8 IncDec)
{
	Float_s8_IncDec (IncDec, (s8*)((menu_current+menu_pos)->int_curr), (s16*)&((menu_current+menu_pos)->int_from), (s16*)&((menu_current+menu_pos)->int_to), (float*)&((menu_current+menu_pos)->float_from), (float*)&((menu_current+menu_pos)->float_to), (float*)&((menu_current+menu_pos)->float_step));
}

/*
IncDec - увеличиваем или уменьшаем значение
item - позиция "настроек сенсора", в которой изменяется значение
*/
void Spectrum_Float_s16_IncDec (u8 IncDec, u8 item)
{
	Float_s16_IncDec (IncDec, ((Settings_sensor+item)->int_curr), (s16*)&((Settings_sensor+item)->int_from), (s16*)&((Settings_sensor+item)->int_to), (float*)&((Settings_sensor+item)->float_from), (float*)&((Settings_sensor+item)->float_to), (float*)&((Settings_sensor+item)->float_step));
}

/*
IncDec - увеличиваем или уменьшаем значение
*/
void Calibration_Pos (u8 IncDec)
{
	if (IncDec) {if (E_Settings[E_Sel].Pos < (SPECTRA_SIZE-1)) E_Settings[E_Sel].Pos++; else E_Settings[E_Sel].Pos=SPECTRA_SIZE-1;}
	else {if (E_Settings[E_Sel].Pos > 1) E_Settings[E_Sel].Pos--; else E_Settings[E_Sel].Pos=1;}
}

/*
IncDec - увеличиваем или уменьшаем значение
*/
void Calibration_Itrv (u8 IncDec)
{
	if (IncDec) {if (E_Settings[E_Sel].Itrv < E_ITRV_MAX) E_Settings[E_Sel].Itrv++; else E_Settings[E_Sel].Itrv=E_ITRV_MAX;}
	else {if (E_Settings[E_Sel].Itrv > E_ITRV_MIN) E_Settings[E_Sel].Itrv--; else E_Settings[E_Sel].Itrv=E_ITRV_MIN;}
}


//==========================//
//       Главное меню       //
//==========================//
void Menu_Automat (void)
{
        
	switch (MenuState)
	{
/******/case MenuState_Idle:
		{
			DisplayMode = DisplayMode_Menu;
			menu_pos=0;
			menu_pos_parent=0;
			menu_current = MainMenu;	// Сбрасываю указатель на корень меню
			MenuState=MenuState_Working;
			MenuNeedUpdate=1;
		} break;
/******/case MenuState_Working:
		{
			// ENTER
			if ((SETPARAMS.iKey==KEY_ENT) && Keyb_SpeedDelay())
			{
				// Если у выделенного пункта меню есть привязанная задача, запускаем её
				if ((menu_current+menu_pos)->event_enter) Task_add((menu_current+menu_pos)->event_enter);
				
                                if (((menu_current+menu_pos)->secrity != 0))
                                {
                                  vSendSecurityMes((uint8_t*)&menu_pos, menu_pos, (uint32_t*)&menu_current, (uint32_t)menu_current);
                                  vSecurityEnter();
                                }
                                
                                /* передать в secyrity */
                                // menu_current
                                //menu_pos  
                                
				if (((menu_current+menu_pos)->submenu) != NULL)	// если переходить некуда, не переходим
				{
					menu_pos_parent=menu_pos;	// сохраняем пункт меню, чтобы при возврате выбрать этот же пункт
					menu_current=((menu_current+menu_pos)->submenu);	// вход в подменю
					menu_pos=0;		// когда входим в подменю, всегда встаём на 1-й пункт
					MenuNeedUpdate=1;
				}
			}
			// RETURN
			if ((SETPARAMS.iKey==KEY_RET) && Keyb_SpeedDelay())
			{
				if ((menu_current->parent) != NULL)	// если переходить некуда, не переходим
				{
					menu_current=(menu_current->parent);	// выход на уровень вверх
					menu_pos=menu_pos_parent;	// при возврате, восстанавливаем пункт меню
					MenuNeedUpdate=1;
				}
				else Task_add(TASK_MENU_STOP);	// Если находимся на верхнем уровне, и нажата кнопка "RETURN", выходим из меню
			}
			// DOWN, Переход к следующему пункту
			if ((SETPARAMS.iKey==KEY_D) && Keyb_SpeedDelay())
			{
				if (menu_pos == (menu_current->items_all-1)) menu_pos=0; else menu_pos++;
				MenuNeedUpdate=1;
			}
			// UP, Переход к предыдущему пункту
			if ((SETPARAMS.iKey==KEY_U) && Keyb_SpeedDelay())
			{
				if (menu_pos == 0) menu_pos=(menu_current->items_all-1); else menu_pos--;
				MenuNeedUpdate=1;
			}
			// LEFT, Уменьшение значения
			if ((SETPARAMS.iKey==KEY_L) && Keyb_SpeedDelay())	// Если нажата кнопка + не запущена задержка
			{
				if ((menu_current+menu_pos)->display_type)	// Если есть что изменять
				{
					// float/s16 type
					if (((menu_current+menu_pos)->display_type) == DISP_float_s16) Menu_Float_s16_IncDec(0);
					// float/s8 type
					if (((menu_current+menu_pos)->display_type) == DISP_float_s8) Menu_Float_s8_IncDec(0);
					// bool type
					if (((menu_current+menu_pos)->display_type) == DISP_bool)
					{
						*((menu_current+menu_pos)->int_curr) = (*((menu_current+menu_pos)->int_curr) ^ (1<<((menu_current+menu_pos)->int_from)));	// инвертирование бита
					}
				MenuNeedUpdate=1;
				}
			}
			// RIGHT, Увеличение значения
			if ((SETPARAMS.iKey==KEY_R) && Keyb_SpeedDelay())	// Если нажата кнопка + не запущена задержка
			{
				if ((menu_current+menu_pos)->display_type)	// Если есть что изменять
				{
					// float/s16 type
					if (((menu_current+menu_pos)->display_type) == DISP_float_s16) Menu_Float_s16_IncDec(1);
					// float/s8 type
					if (((menu_current+menu_pos)->display_type) == DISP_float_s8) Menu_Float_s8_IncDec(1);
					// bool type
					if (((menu_current+menu_pos)->display_type) == DISP_bool)
					{
						*((menu_current+menu_pos)->int_curr) = (*((menu_current+menu_pos)->int_curr) ^ (1<<((menu_current+menu_pos)->int_from)));	// инвертирование бита
					}
				MenuNeedUpdate=1;
				}
			}
			// Сохранение настроек, если нажать "MENU" находясь в меню
			if ((SETPARAMS.iKey==KEY_M) && Keyb_SpeedDelay()) Task_add(TASK_SETTINGS_WRITE);
		} break;
/******/case MenuState_Closing:
		{
			//LCDBUFFER_Clear();
			///Task_add(TASK_LCD_UPDATE);
			MenuState = MenuState_Idle;
			DisplayMode = DisplayMode_Spectrum;
		} break;
	}
	
	
	/*// Обновление дисплея
	if ((MenuNeedUpdate) && (LcdBufferNotUsed))
        {
          if(tasks & TASK_FB_WR_EN) 
                {
                  vMenuUpdate();
                  Task_del(TASK_FB_WR_EN);
                };                   
        }
        //Task_add(TASK_LCD_UPDATE);*/
}

void vMenuUpdate(void)
	{
          
                MenuNeedUpdate = 1;//0;
		
		char string_buf[10];	// Буфер для преобразования float -> string
		
                fMenuAlpha = (float)usMenuAlpha/100.f;
                
                if (fMenuAlpha > 0 ) 
                { 
                  
                  vInDisplayMode_Spectrum();
                  LCDBUFFER_Alfa_Rectangle (0, 0, 240, 320, BLACK, fMenuAlpha);
                 
                }
                else 
                {                    
                  LCDBUFFER_Clear();
                }
                
                fontTitle  = gdispOpenFont("MagistralCTT Regular 24");
		if ((menu_current->parent) == NULL) gdispFillStringBox ( 0, 0, gdispGetWidth()-1, gdispGetFontMetric( fontTitle, fontHeight)-1, MainMenu_title, fontTitle, SkyBlue, Black, justifyCenter);  //LCDBUFFER_Str_Shadow(MainMenu_title,5,0,YELLOW,2,2);	// Заголовок "Главное меню"
		else gdispFillStringBox( 0, 0, gdispGetWidth()-1, gdispGetFontMetric( fontTitle, fontHeight)-1, (menu_current->parent+menu_pos_parent)->title, fontTitle, Yellow, Black, justifyCenter);//LCDBUFFER_Str((menu_current->parent+menu_pos_parent)->title,5,0,YELLOW,BLACK,2,2);	// Заголовок подменю
		
                /* Select new font */
                fontTemp  = gdispOpenFont("MagistralCTT Regular 18");
                
                for (u8 i=0; i<(menu_current->items_all); i++)	// все пункты меню
		{
			//заменяяе LCDBUFFER_Str_Shadow((menu_current+i)->title,5,((i*20)+40),WHITE,2,2);	// название пункта меню
			gdispDrawStringBox( 1, ((i*20)+40), gdispGetWidth()-1, gdispGetFontMetric(fontTemp, fontLineSpacing), (menu_current+i)->title, fontTemp, White, justifyLeft);
                        if ((menu_current+i)->display_type)	// Если есть что отображать
			{
				// тип данных float/s16
				if (((menu_current+i)->display_type) == DISP_float_s16)
				{
					sprintf(string_buf, "%8.1f", Menu_Float_s16_read(i));	// Выравнивание вправо, 8 символов, после запятой 1 знак
					gdispDrawStringBox( 1, ((i*20)+40), gdispGetWidth()-1, gdispGetFontMetric(fontTemp, fontLineSpacing), string_buf, fontTemp, White, justifyRight);
                                        //LCDBUFFER_Str_Shadow(string_buf,140,((i*20)+40),WHITE,2,2);
				};
				// тип данных float/s8
				if (((menu_current+i)->display_type) == DISP_float_s8)
				{
					sprintf(string_buf, "%8.1f", Menu_Float_s8_read(i));	// Выравнивание вправо, 8 символов, после запятой 1 знак
					gdispDrawStringBox( 1, ((i*20)+40), gdispGetWidth()-1, gdispGetFontMetric(fontTemp, fontLineSpacing), string_buf, fontTemp, White, justifyRight);
                                        //LCDBUFFER_Str_Shadow(string_buf,140,((i*20)+40),WHITE,2,2);
				};
				// тип данных boolean
				if (((menu_current+i)->display_type) == DISP_bool)
				{
					if (*((menu_current+i)->int_curr) & (1<<((menu_current+i)->int_from))) 
                                          //LCDBUFFER_Char_Shadow('+',220,((i*20)+40),WHITE,2,2);
                                          gdispDrawStringBox( 1, ((i*20)+40), gdispGetWidth()-5, gdispGetFontMetric(fontTemp, fontLineSpacing), "+ ", fontTemp, White, justifyRight);
				};
				// тип данных s16
				if (((menu_current+i)->display_type) == DISP_int)
				{
					//LCDBUFFER_Num_Shadow(*((menu_current+i)->int_curr),140,((i*20)+40),WHITE,2,2);
                                        sprintf(string_buf, "%8d", *((menu_current+i)->int_curr));
                                        gdispDrawStringBox( 1, ((i*20)+40), gdispGetWidth()-1, gdispGetFontMetric(fontTemp, fontLineSpacing), string_buf, fontTemp, White, justifyRight);
				};
                                
			}
		}
               
               // LCDBUFFER_Rectangle(0,((menu_pos*20)+38),LCD_WIDTH-1,((menu_pos*20)+56),WHITE, 1);	// инверсия цвета выбранного пункта меню
		//LCDBUFFER_Str((menu_current+menu_pos)->title,5,((menu_pos*20)+40),BLACK,WHITE,2,2);	// название пункта меню
		
              //  gdispFillArea ( 0, ((menu_pos*20)+40), gdispGetWidth()-1, gdispGetFontMetric(font, fontLineSpacing)-1, White);
                gdispFillStringBox ( 1, ((menu_pos*20)+40), gdispGetWidth()-1, gdispGetFontMetric(fontTemp, fontLineSpacing)-1, (menu_current+menu_pos)->title, fontTemp, Black, White, justifyLeft);
			
                        if ((menu_current+menu_pos)->display_type)	// Если есть что отображать
			{
				// тип данных float/s16
				if (((menu_current+menu_pos)->display_type) == DISP_float_s16)
				{
					sprintf(string_buf, "%8.1f", Menu_Float_s16_read(menu_pos));	// Выравнивание вправо, 8 символов, после запятой 1 знак
					//LCDBUFFER_Str(string_buf,140,((menu_pos*20)+40),BLACK,WHITE,2,2);
                                        gdispDrawStringBox( 1, ((menu_pos*20)+40), gdispGetWidth()-1, gdispGetFontMetric(fontTemp, fontLineSpacing), string_buf, fontTemp, Black, justifyRight);
				};
				// тип данных float/s8
				if (((menu_current+menu_pos)->display_type) == DISP_float_s8)
				{
					sprintf(string_buf, "%8.1f", Menu_Float_s8_read(menu_pos));	// Выравнивание вправо, 8 символов, после запятой 1 знак
					//LCDBUFFER_Str(string_buf,140,((menu_pos*20)+40),BLACK,WHITE,2,2);
                                        gdispDrawStringBox( 1, ((menu_pos*20)+40), gdispGetWidth()-1, gdispGetFontMetric(fontTemp, fontLineSpacing), string_buf, fontTemp, Black, justifyRight);
				};
				// тип данных boolean
				if (((menu_current+menu_pos)->display_type) == DISP_bool)
				{
					if (*((menu_current+menu_pos)->int_curr) & (1<<((menu_current+menu_pos)->int_from))) 
                                          //LCDBUFFER_Char('+',220,((menu_pos*20)+40),BLACK,WHITE,2,2);
                                          gdispDrawStringBox( 1, ((menu_pos*20)+40), gdispGetWidth()-5, gdispGetFontMetric(fontTemp, fontLineSpacing), "+ ", fontTemp, Black, justifyRight);
				};
				// тип данных s16
				if (((menu_current+menu_pos)->display_type) == DISP_int)
				{
					//LCDBUFFER_Num(*((menu_current+menu_pos)->int_curr),140,((menu_pos*20)+40),BLACK,WHITE,2,2);
                                        sprintf(string_buf, "%8d", *((menu_current+menu_pos)->int_curr));
                                        gdispDrawStringBox( 1, ((menu_pos*20)+40), gdispGetWidth()-1, gdispGetFontMetric(fontTemp, fontLineSpacing), string_buf, fontTemp, Black, justifyRight);
                                };
			}
                
                if ((menu_current == Settings_explosive))
                {
                  
                  LCDBUFFER_Alfa_Rectangle (2, 290, 240-2, 320-2, WHITE, 0.5F);
                  /* Для системного шрифта кодировка Russian! Символы не исправлять */
                //  LCDBUFFER_Str_Shadow("ÍɌÍɅ! ū󠭠񲰮ꫨ 񲨵 ࡰ᭥򰮢",5,292,YELLOW,1,1);
                //  LCDBUFFER_Str_Shadow("𥪮񻔥򱿠鲯לּ询᳼ 𥦨좬5,300,YELLOW,1,1);
                //  LCDBUFFER_Str_Shadow("\"ˠ쩡𮢪ᜢ",5,308,YELLOW,1,1);

                }    
                        
                //инверсия меню
		//Task_add(TASK_LCD_UPDATE);
		Task_add(TASK_BATT_ICON);
	}

