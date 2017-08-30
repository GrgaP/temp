#ifndef GRAPH_H		// Блокируем повторное включение этого модуля
#define GRAPH_H



#include "main.h"

//extern u32 num_emi_peaks;
//extern u32 emi_peaks[];
//extern u32 num_absop_peaks;
//extern u32 absop_peaks[];

enum DisplayMode_Enum	// Список режимов отображения
{
	DisplayMode_Logo        = 0,
	DisplayMode_PowerOff    = 1,
	DisplayMode_Calibration = 2,
	DisplayMode_Spectrum    = 3,
	DisplayMode_Gradient    = 4,
	DisplayMode_Gan         = 5,
	DisplayMode_Menu        = 6,
        DisplayMode_Starting    = 7,
        DisplayMode_Security    = 8
};
extern volatile u8 DisplayMode;
extern volatile u16 Grad_Lvl_Curr;
extern volatile u16 Grad_Lvl_Trgt;
extern volatile u16 Grad_Lvl_Trgt_Spctr;



////////////////////////////////////////////////
//    Секция прототипов локальных функций     //
////////////////////////////////////////////////
//void rp_peaks_search (void*);
void rp_peaks_show (void);
//void peaks_search (void*);
void peaks_show (void);
void show_grid_texts (void);
void Show_Spectrum (s16* spctr, uint32_t color);
void Show_Specrtum_Borders (void);
//void Check_Specrtum_Borders (void); для запуска пищалки
void Show_Calibration_Borders (void);
//void gradient_show (void); 
void Display_Automat (void);
void vInDisplayMode_Spectrum();
void Show_BAASpectrum (s16* spctr, uint32_t color);
void vShowPeakInfo(uint32_t ulExplNum, uint32_t x, uint32_t y);
#endif
