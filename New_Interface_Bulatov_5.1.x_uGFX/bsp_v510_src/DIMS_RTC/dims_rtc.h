/**
  ******************************************************************************
  * @file    dims_rtc.h
  * @author  SvD
  * @version V1.0.0
  * @date    15-August-2016
  * @brief   Header for dims_rtc.c file
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __K_RTC_H
#define __K_RTC_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Exported types ------------------------------------------------------------*/
extern RTC_HandleTypeDef RtcHandle;
/* Exported constants --------------------------------------------------------*/
/* Exported macros -----------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */ 
uint8_t     DIMS_RTC_CalendarBkupInit(void);
void     DIMS_RTC_BkupSaveParameter(uint32_t address, uint32_t data);
uint32_t DIMS_RTC_BkupRestoreParameter(uint32_t address);

void DIMS_RTC_SetTime  (RTC_TimeTypeDef *Time);
void DIMS_RTC_GetTime  (RTC_TimeTypeDef *Time);
void DIMS_RTC_SetDate  (RTC_DateTypeDef *Date);
void DIMS_RTC_GetDate  (RTC_DateTypeDef *Date);

/** 
  * Function return pointer to string with current time values 
  * @retval : ptr to strig in "HH:MM:SS" format
  */ 
void DIMS_RTC_getStrT(char *timestr);

/** 
  * Function return pointer to string with current data values 
  * @retval : ptr to strig in "DD/MM/YY" format
  */ 
void TDIMS_RTC_getStrD(char *datestr);

#ifdef __cplusplus
}
#endif

#endif /*__K_RTC_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
