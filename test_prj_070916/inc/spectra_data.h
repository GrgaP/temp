#ifndef SPECTRA_DATA_H		// Блокируем повторное включение этого модуля
#define SPECTRA_DATA_H

#include "main.h"

#define SPECTRA_DATA_CLEAR_NUMBER 32    // кратно двойке
#define SPECTRA_DATA_TNT_NUMBER 8       //  кратно двойке
#define SPECTRA_DATA_TNT_LEVEL_NUMBER 3 // количество отснятых уровней спектров ТНТ 


extern const int16_t spectra_data_clear[SPECTRA_DATA_CLEAR_NUMBER][1024];

extern const int16_t spectra_data_TNTlevel1[SPECTRA_DATA_TNT_NUMBER][1024];
extern const int16_t spectra_data_TNTlevel2[SPECTRA_DATA_TNT_NUMBER][1024]; 
extern const int16_t spectra_data_TNTlevel3[SPECTRA_DATA_TNT_NUMBER][1024];

#endif // SPECTRA_DATA_H