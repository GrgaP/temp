#ifndef FLASH_MEMORY_H		// Блокируем повторное включение этого модуля
#define FLASH_MEMORY_H


#include "main.h"

extern volatile u16 Flash_ReWrites;	// Количество перезаписей памяти при сохранении настроек

#define FLASH_ADRESS_11SECT 0x080E0000

////////////////////////////////////////////////
//    Секция прототипов локальных функций     //
////////////////////////////////////////////////
void Flash_unlock (void);
void Flash_lock (void);
void Flash_erase_sector (u8 sector);
u16 Flash_read_16bit (u32 address);
void Flash_write_16bit (u32 Address, u16 data);
void Flash_read_settings (void);
void Flash_write_settings (void);
void Flash_check_settings (void);
void Flash_set_default_settings (void);
void vSetFabricationSettings(void);

extern const char pFabrExplName[EXPLOSIV_COUNT][EXPLOSIVE_NAME_LENGHT]; 

#endif
