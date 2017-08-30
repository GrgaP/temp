/**
  ******************************************************************************
  * @file    DIMS_SystemSettings.h
  * @author  a.bulatov
  * @version V1.1.0
  * @date    25.01.2016
  * @brief   Heder files for DIMS system settings.
  ******************************************************************************
*/ 

/* Define to prevent recursive inclusion ------------------------------------ */
#ifndef __DIMS_SYSTEM_SETTINGS_H
#define __DIMS_SYSTEM_SETTINGS_H

#pragma pack(push, 1)	
typedef struct SystemSettingsStructName
{
  u8    gainReper;
  u8    gainAnalitic;
  u8    ganFan;
  u8    reperHeatOn;
  u8    ucHeaterOn;
  s8    cHeaterSetTemp;
  s8    cHeaterHistTemp;
} SystemSettingsTypeDef;
#pragma pack(pop)	

#define SYSTEM_SETTINGS_DEF {5,5,1,0, 45, 5}

#endif /* __GAN_SC_KERNEL_H */