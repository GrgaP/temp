#ifndef MAIN_H		// Блокируем повторное включение этого модуля
#define MAIN_H

#include "lib/extra/ex_types/ex_types.h"

#define FREQ_APB1 45000000
#define FREQ_APB2 90000000

//====================================================
//              Настраиваемые значения
//====================================================
#define SPECTRA_SIZE 512	// Размер данных спектра

#define DISORBER_TIME 30    // Время работы дисорбера, сек

#define E_COUNT (EXPLOSIV_COUNT)

/* Explosive defenition group ***************************************** START */
#define EXPLOSIV_COUNT 6			
#define EXPLOSIVE_NAME_LENGHT 5

#define EXP_NAME { {EXP0_NAME},         \
                   {EXP1_NAME},         \
                   {EXP2_NAME},         \
                   {EXP3_NAME},         \
                   {EXP4_NAME},         \
                   {EXP5_NAME} }

#define EXP0_NAME "TNT"
#define EXP1_NAME "RDX"
#define EXP2_NAME "HMX"
#define EXP3_NAME "PENT"
#define EXP4_NAME "SUB1"
#define EXP5_NAME "SUB2"

#define EXP_COLOR   { EXP0_COLOR, \
                      EXP1_COLOR, \
                      EXP2_COLOR, \
                      EXP3_COLOR, \
                      EXP4_COLOR, \
                      EXP5_COLOR }

#define EXP0_COLOR (LIGHTYELLOW)
#define EXP1_COLOR (RED)
#define EXP2_COLOR (GREEN)
#define EXP3_COLOR (ORANGE)
#define EXP4_COLOR (LIGHTYELLOW)
#define EXP5_COLOR (LIGHTGREEN)

/* Maximum of offset and wimdow for all explosive */
#define EXP_OFFSET_MAX          (EXP_OFFSET_MAX_PR*SPECTRA_SIZE/100) /* 50 procent */
#define EXP_WND_MAX             (EXP_WND_MAX_PR*SPECTRA_SIZE/100) /* 25 procent */
#define EXP_OFFSET_MAX_PR       (50.f)
#define EXP_WND_MAX_PR          (25.f)

/* Default of window value for all explosive */
#define EXP_WND_DEF             (10*SPECTRA_SIZE/100) /* 10 procent */
#define EXP_OFFSET_DEF          (0)     
#define EXP_LEVEL_ALRM_DEF          (40000)

/* Default offset value for other explosive */
#define EXP0_OFFSET (0)
#define EXP1_OFFSET (+10*SPECTRA_SIZE/100)
#define EXP2_OFFSET (-10*SPECTRA_SIZE/100)
#define EXP3_OFFSET (+20*SPECTRA_SIZE/100)
#define EXP4_OFFSET (-20*SPECTRA_SIZE/100)
#define EXP5_OFFSET (+30*SPECTRA_SIZE/100)


#define EXP0_WND        (0)
#define EXP1_WND        (8*SPECTRA_SIZE/100)
#define EXP2_WND        (8*SPECTRA_SIZE/100)
#define EXP3_WND        (10*SPECTRA_SIZE/100)
#define EXP4_WND        (5*SPECTRA_SIZE/100)
#define EXP5_WND        (30*SPECTRA_SIZE/100)


#define EXP0_LEVEL      (22000/2) /* Level of TNT */ 
#define EXP1_LEVEL      (EXP_LEVEL_ALRM_DEF/2)
#define EXP2_LEVEL      (EXP_LEVEL_ALRM_DEF/2)
#define EXP3_LEVEL      (EXP_LEVEL_ALRM_DEF/2)
#define EXP4_LEVEL      (EXP_LEVEL_ALRM_DEF/2)
#define EXP5_LEVEL      (EXP_LEVEL_ALRM_DEF/2)

#define EXP0_ONOFF      (ON)
#define EXP1_ONOFF      (ON)
#define EXP2_ONOFF      (ON)
#define EXP3_ONOFF      (ON)
#define EXP4_ONOFF      (ON)
#define EXP5_ONOFF      (ON)

/* End explosive defenition group ********END****/

/* Default value for fabrication ************************************** START */
/* Track default value in  */
#define TRACK_VAL_0_DEF (105)   /* Vrf                       */
#define TRACK_VAL_1_DEF (0)     /* Vpull                     */
#define TRACK_VAL_2_DEF (50) /* Level for peak detect min */
#define TRACK_VAL_3_DEF (1)     /* Asqusition                */
#define TRACK_VAL_4_DEF (679)   /* Rp find from 1.0 V        */
#define TRACK_VAL_5_DEF (3164)  /* Rp find to 4.5 V          */
#define TRACK_VAL_6_DEF (324)   /* Scan from 0.5 V           */
#define TRACK_VAL_7_DEF (3519)  /* Scan to 5.0 V             */

/* Default value of array track use in SETPARAMS (old version) */
#define TRACK_VAL_DEF { TRACK_VAL_0_DEF, TRACK_VAL_1_DEF, \
                        TRACK_VAL_2_DEF, TRACK_VAL_3_DEF, \
                        TRACK_VAL_4_DEF, TRACK_VAL_5_DEF, \
                        TRACK_VAL_6_DEF, TRACK_VAL_7_DEF }

#define E_PULL_DEF      (-20)   /* E pull default value : -2.0V */
#define GAIN_DEF        (5)     /* Default value for gain adc x16 
                                   now in not use */
#define FLAG_BITS_DEF   (19)    /* Flags default: pow_on, vrf_on, bluetoth_on */

#define RP_ID_WND_DEF   (8*SPECTRA_SIZE/100)  /* 8% of spectra */
#define RP_ID_OFFSET_DEF (0*SPECTRA_SIZE/100) /* 0% of spectra */


/* End of Default value for fabrication ***END***/


#define E_ITRV_MIN 0		// Диапазон перестройки доверительного интервала
#define E_ITRV_MAX 250

#define GAN_SIZE	64		// Размер данных ГАН
#define GAN_FILTER	4
#define GAN_MAX		3000	// Уровни для осциллограммы ГАН
#define GAN_MIN		-3000
#define GAN_LINES	5		// Количество линий сетки по вертикали

#define MAX_COUNT_MIN_PEAKS	16	// Предельное число искомых минимальных пиков
#define MAX_COUNT_MAX_PEAKS	16	// Предельное число искомых максимальных пиков

#define Grad_Stp_Up		200	// Шаг инкремента/декремента текущего уровня
#define Grad_Stp_Down	50

//#define SKIP_CRC_CHECKING		// Отключить все проверки CRC

#define LCD_WIDTH	240		// Разрешение дисплея
#define LCD_HEIGHT	320

#define LCD_W	(LCD_WIDTH-1)		// Чтобы постоянно не писать LCD_WIDTH-1
#define LCD_H	(LCD_HEIGHT-1)

#define GRID_LINES_WIDTH	8	// Количество линий сетки по горизонтали и вертикали (1...255)
#define GRID_LINES_HEIGHT	10

// Подписи к сетке по оси Y
#define label_Y_max		30.0F
#define label_Y_min		-30.0F

// Калибровка напряжений АКБ под нагрузкой
#define BATT_VMIN     8500  // разряд, 0% под нагрузкой
#define BATT_VMAX     12100  // разряд, 100% под нагрузкой
#define BATT_VCHARGED 12600  // заряд, 100%

// Координаты отображения символа АКБ
#define LCD_BATT_X	2
#define LCD_BATT_Y	2
// Интервалы измерения и показа/мигания символа АКБ
#define BATT_CHECK_INTERVAL	100	// 1 сек
#define BATT_SHOW_INTERVAL	25	// 0,5 сек

#define Keyb_MaxPause		200		// Максимальная пауза перебора (ms)
#define Keyb_MinPause		5		// Минимальная пауза перебора (ms)
#define Keyb_SpeedupTime	2500	// Время разгона до полной скорости перебора (ms)

#define KeyPowerOn              100

//====================================================
//               Подключаем хедеры
//====================================================

#include "stm32f7xx.h"		// StdPeriph Driver
#include "stm32f7xx_hal.h"
#include "cmsis_os.h"
#include "usb_device.h"

#include "FreeRTOS_CLI.h"

#include "us81632_def.h"		// Функции для работы с дисплеем  

#include "binary_iar.h"		// Макро для использования двоичного формата чисел (до 16 бит)
#include <stdio.h>			// Функции обработки строк
#include <math.h>			// Функции обработки строк
#include <string.h>
#include "delay.h"			// Программные задержки
#include "leds.h"			// Индикаторные светодиоды
#include "buzzer_pwm.h"		// ШИМ для пищалки TIM1_PA11, таймер TIM3 для отсчёта временных интервалов
#include "gpio_mco.h"		// Управляющий вывод для запроса данных (PB12), Выход тактовой частоты (MCO)
#include "crc_16.h"			// Функции расчёта контрольной суммы CRC16
#include "Symbols.h"		// Символы (шрифт дисплея)

#include "dims_dsp_sdram.h"
#include "lcd_ltdc.h"	
#include "dma_usart_f7_hal.h"
#include "DIMS_rtc.h"

#include "logotype.h"		// Логотип при включении девайса, версия ПО

#include "SpectrumMath.h"
#include "graph.h"			// Поиск пиков, обработка массива, усреднение точек, обработка данных для графика
#include "keyboard_tmr.h"	// Матричная клавиатура 3*3 клавиши, опрос по таймеру TIM6
#include "menu.h"			// Работа с меню
#include "flash_memory.h"	// Работа с флеш-памятью (чтение и сохранение настроек)
#include "adc_battery.h"	// Работа с АЦП (измерение уровня заряда аккумулятора)
#include "task_manager.h"	// Типа менеджер задач
#include "convert.h"		// Преобразование величин
#include "bug.h"
#include "backlight.h"
#include "power.h"
#include "disorber.h"
#include "DIMS_SystemSettings.h"

#include "dms_data.h"

#define USE_NEW_KEYBOARD
#define GPIO_HIGH(a, b) a -> BSRRL = b  // Теперь при помощи конструкции GPIO_HIGH(a, b) можно выдать лог.1 на порт "a" пин "b"
#define GPIO_LOW(a, b) a -> BSRRH = b   // Теперь при помощи конструкции GPIO_LOW(a, b) можно выдать лог.0 на порт "a" пин "b"
/*
------------------------------------------------------------
SETPARAMS 0x01 Передаём 6 байт заголовок + 10 слов данных
GETPARAMS 0x02 Принимаем 6 байт заголовок + 10 слов данных
---
byte   iMagicNumber;	// Маркер начала комманды = 0xAA
byte   iCmdId;			// Код комманды 0x01 или 0x02
word   iDataSize;		// Длина данных комманды в байтах (может меняться для 1й и той же комманды от версии к версии)
word   iCrc;			// CRC всех байтов заголовка комманды за исключением поля iCrc
word   iTrackValue[8];	// Значения 8-ми трекбаров
word   iReserved;		// Зарезервировано
word   iFlagBits;		// Биты-флаги
------------------------------------------------------------
GETDATA 0x03 Принимаем 6 байт заголовок + 1024 слова данных
---
byte   iMagicNumber;	// Маркер начала комманды = 0xAA
byte   iCmdId;			// Код комманды 0x03
word   iDataSize;		// Длина данных комманды в байтах (может меняться для 1й и той же комманды от версии к версии)
word   iCrc;			// CRC всех байтов заголовка комманды за исключением поля iCrc
word   usart_massiv[];	// Массив с принятыми данными
------------------------------------------------------------
SETKEYS 0x04 Передаём 6 байт заголовок + 2 слова данных
---
byte   iMagicNumber;	// Маркер начала комманды = 0xAA
byte   iCmdId;			// Код комманды 0x04
word   iDataSize;		// Длина данных комманды в байтах (может меняться для 1й и той же комманды от версии к версии)
word   iCrc;			// CRC всех байтов заголовка комманды за исключением поля iCrc
word   iKey;			// Данные о нажатых клавишах
word   iReserved;		// Зарезервировано
------------------------------------------------------------
*/
#pragma pack (push,1)
typedef struct	// Структуры SETPARAMS + SETKEYS
{
	const u8 iMagicNumber;	// AA
	const u8 iCmdId;		// 01
	const u16 iDataSize;	// 20
	u16 iCrc;
	s16 iTrackValue[8];	// Значения восьми трек-баров
	s8 ePull;
	s8 Gain;
	u16 iFlagBits;		// Биты-флаги
	const u8 iMagicNumber2;	// AA
	const u8 iCmdId2;		// 04
	const u16 iDataSize2;	// 4
	u16 iCrc2;
	u16 iKey;			// Данные о нажатых клавишах
	u16 iReserved2;		// Зарезервировано, 0
        int32_t reperAmpl;
        int32_t reperId;
        SystemSettingsTypeDef systemSettings;
} SETPARAMS_t;

typedef struct	// Структуры GETPARAMS + GETDATA
{
	u8  iMagicNumber;
	u8  iCmdId;
	u16 iDataSize;
	u16 iCrc;
	s16 iTrackValue[8];	// Значения восьми трек-баров
	u16 iReserved;		// Зарезервировано, 0
	u16 iFlagBits;		// Биты-флаги
	u8  iMagicNumber2;
	u8  iCmdId2;
	u16 iDataSize2;
	u16 iCrc2;
        u16 nSpctr;
	u16 Spectra[MULTI_SPCTR][SPECTRA_SIZE];
	float eSpectra[GAN_SIZE];
	float eFreq;
        float ganTemp;
        float ganCurrent;
        float ganHvVoltsFeedBack;
        float ganVrf1garm;
        float ganVrf2garm;
        float ganPh1garm;
        float ganPh2garm;
        float ganPhDelta;
        uint32_t ganPayLoad2;
        int32_t reperDac;
        int32_t cntSpctr;
        float mainBoardTemperature;
        float mainBoardRh;
        uint32_t reperIdOk;
        uint32_t reperAmplOk; 
        int32_t meanRpOk;
        int32_t meanAnOk;
        int32_t vrfRef;
        int32_t uVozdReper;
        float tempExt;
        float fHeaterTemp;     /* Температура нагревателя */
        uint32_t reserved1;
        uint32_t reserved2;
        uint32_t reserved3;
        uint32_t reserved4; 
} GETPARAMS_t;


typedef struct	// Структура каждого взрывчатого вещества
{
	char* const Name;	// Наименование вещества
	s16 Pos;			// Позиция пика (по напряжению)
	s16 Itrv;			// Доверительный интервал (по напряжению)
	u16 Level;			// Амплитуда максимального пика в зоне обнаружения. Если пики не найдены - обнулять!
} EXPLOSIVE_t;

typedef enum ECalcName
{
  OFF = 0,
  ON = (!OFF)
} EOnOff_TypeDef;

typedef struct	ExplosiveStructName     /* Тип для хранения данных о веществе */
{
	char       Name[EXPLOSIV_COUNT][EXPLOSIVE_NAME_LENGHT] ;	        /* Указатель на символьное имя вещества */
        uint16_t        usColor;        /* Цвет вещества */
	int16_t         lOffsetId;	/* Смещения пика в SPECTRA_SIZE */ 
        uint16_t        ulWndId;        /* Ширина зоны в SPECTRA_SIZE. */
	uint16_t        usLevelAlarm;	/* Амплитуда реагирования на пик 0-65535 */
        EOnOff_TypeDef  eOnOff;          /* Включен поиск или нет */ 
} Explosive_TypeDef;


#pragma pack(pop)
extern SETPARAMS_t SETPARAMS;
extern GETPARAMS_t GETPARAMS, UsartGETPARAMS;
extern EXPLOSIVE_t E_Settings[];
extern Explosive_TypeDef  Explosive_Set[];
extern u8 E_Sel;

#define RP_PEAK_SERCH_LEVEL_DEF 4000

extern  int16_t rpPeakSerchLevel;
extern  int16_t rpIdWnd;
extern  int16_t rpIdOffset;
extern  uint16_t pExplColor[E_COUNT];



//extern  int16_t rpPrWnd;
//extern  int16_t rpPrOffset;

extern  int16_t rpTest;


////////////////////////////////////////////////
//    Секция прототипов локальных функций     //
////////////////////////////////////////////////
void Reboot_system (void);

#include "gfx.h"



/*
// Названия пунктов меню, диапазоны изменения значений
#define track1_name			"GAIN"		// Название пункта меню
#define track1_from			0			// Integer-значение "От"
#define track1_to			6			// Integer-значение "До"
#define track1_default		0			// Integer-значение "По умолчанию"
#define track1_change_step	1			// Integer-значение "Шаг настройки"
#define track1_rangefrom	0.0F		// Float-значение "От"
#define track1_rangeto		6.0F		// Float-значение "До"

#define track2_name			"Vpull,V"
#define track2_from			-1900
#define track2_to			1900
#define track2_default		0
#define track2_change_step	10
#define track2_rangefrom	-45.0F
#define track2_rangeto		45.0F*/
/*
#define track3_name			"Уровень"	// Горизонтальная граница (порог обнаружения)
#define track3_from			-32768
#define track3_to			32767
#define track3_default		-5000
#define track3_change_step	50
#define track3_rangefrom	-32768.0F
#define track3_rangeto		32767.0F
*//*
#define track3_name			"Уровень"	// Горизонтальная граница (порог обнаружения)
#define track3_from			1500
#define track3_to			15000
#define track3_default		1500
#define track3_change_step	50
#define track3_rangefrom	1500.0F
#define track3_rangeto		15000.0F

#define track4_name			"Acq, ms"
#define track4_from			1
#define track4_to			100
#define track4_default		50
#define track4_change_step	1
#define track4_rangefrom	1.0F
#define track4_rangeto		100.0F

#define track5_name			"От, V"				// Левая вертикальная граница (порог обнаружения)
#define track5_from			-31768
#define track5_to			31767	// 2047
#define track5_default		-31768
#define track5_change_step	60
#define track5_rangefrom	-45.0F
#define track5_rangeto		45.0F

#define track6_name			"До, V"				// Правая вертикальная граница (порог обнаружения)
#define track6_from			-31768
#define track6_to			31767	// 2047
#define track6_default		31767
#define track6_change_step	60
#define track6_rangefrom	-45.0F
#define track6_rangeto		45.0F

#define track7_name			"From, V"
#define track7_from			-31768
#define track7_to			31767
#define track7_default		-31768
#define track7_change_step	80
#define track7_rangefrom	-45.0F
#define track7_rangeto		45.0F

#define track8_name			"To,   V"
#define track8_from			-31768
#define track8_to			31767
#define track8_default		31767
#define track8_change_step	80
#define track8_rangefrom	-45.0F
#define track8_rangeto		45.0F
*/

#endif