/**
  ******************************************************************************
  * @file    dims_rtc.h
  * @author  SvD
  * @version V1.0.0
  * @date    15-August-2016
  * @brief   This file provides the rtc functions 
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "dims_rtc.h"

/* External variables --------------------------------------------------------*/
RTC_HandleTypeDef RtcHandle;
/* Private typedef -----------------------------------------------------------*/
/* Private defines -----------------------------------------------------------*/
#define RTC_ASYNCH_PREDIV  0x7F   /* LSE as RTC clock */
#define RTC_SYNCH_PREDIV   0x00FF /* LSE as RTC clock */
/* Private macros ------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/

/**
  * @brief  Configure the current time and date.
  * @param  None
  * @retval None
  */
uint8_t DIMS_RTC_CalendarBkupInit(void)
{
  RTC_DateTypeDef sdatestructure;
  RTC_TimeTypeDef stimestructure;
  
  /*##-1- Configure the RTC peripheral #######################################*/
  /* Configure RTC prescaler and RTC data registers */
  /* RTC configured as follow:
  - Hour Format    = Format 24
  - Asynch Prediv  = Value according to source clock
  - Synch Prediv   = Value according to source clock
  - OutPut         = Output Disable
  - OutPutPolarity = High Polarity
  - OutPutType     = Open Drain */ 
  RtcHandle.Instance = RTC;
  RtcHandle.Init.HourFormat = RTC_HOURFORMAT_24;
  RtcHandle.Init.AsynchPrediv = RTC_ASYNCH_PREDIV;
  RtcHandle.Init.SynchPrediv = RTC_SYNCH_PREDIV;
  RtcHandle.Init.OutPut = RTC_OUTPUT_DISABLE;
  RtcHandle.Init.OutPutPolarity = RTC_OUTPUT_POLARITY_HIGH;
  RtcHandle.Init.OutPutType = RTC_OUTPUT_TYPE_OPENDRAIN;
  
  if(HAL_RTC_Init(&RtcHandle) != HAL_OK)
    return 0;
  
  /*##-2- Check if Data stored in BackUp register0: No Need to reconfigure RTC#*/
  /* Read the Back Up Register 0 Data */
  /*if (HAL_RTCEx_BKUPRead(&RtcHandle, RTC_BKP_DR0) != 0x32F2)
  {
    
    //RTC_CalendarConfig();
		
		stimestructure.Hours          = 0x07;
		stimestructure.Minutes        = 0x07;
		stimestructure.Seconds        = 0x07;
		stimestructure.SubSeconds     = 0x07;
		stimestructure.TimeFormat     = RTC_HOURFORMAT12_AM;
		stimestructure.DayLightSaving = RTC_DAYLIGHTSAVING_NONE ;
		stimestructure.StoreOperation = RTC_STOREOPERATION_RESET;
  
		if(HAL_RTC_SetTime(&RtcHandle,&stimestructure, RTC_FORMAT_BCD) != HAL_OK)
			return 0;;
		
		sdatestructure.Year    = 0x16;
		sdatestructure.Month   = RTC_MONTH_AUGUST;
		sdatestructure.Date    = 0x16;
		sdatestructure.WeekDay = RTC_WEEKDAY_TUESDAY;
		
		if(HAL_RTC_SetDate(&RtcHandle,&sdatestructure, RTC_FORMAT_BCD) != HAL_OK)
			return 0;
                
                DIMS_RTC_BkupSaveParameter(RTC_BKP_DR0,);
                DIMS_RTC_BkupSaveParameter(RTC_BKP_DR1,);

  };*/
  
  return 1;
}

/**
  * @brief RTC MSP Initialization 
  *        This function configures the hardware resources used in this example: 
  *           - Peripheral's clock enable
  * @param  hrtc: RTC handle pointer
  * @retval None
  */
void HAL_RTC_MspInit(RTC_HandleTypeDef *hrtc)
{
  RCC_OscInitTypeDef        RCC_OscInitStruct;
  RCC_PeriphCLKInitTypeDef  PeriphClkInitStruct;
  
  /*##-1- Configure LSE as RTC clock source ##################################*/ 
  RCC_OscInitStruct.OscillatorType =  RCC_OSCILLATORTYPE_LSI | RCC_OSCILLATORTYPE_LSE;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  RCC_OscInitStruct.LSEState = RCC_LSE_ON;
  RCC_OscInitStruct.LSIState = RCC_LSI_OFF;
  if(HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  { 
    return;
  }
  
  PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_RTC;
  PeriphClkInitStruct.RTCClockSelection = RCC_RTCCLKSOURCE_LSE;
  if(HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK)
  { 
    return;
  }
  
  /*##-2- Enable RTC peripheral Clocks #######################################*/ 
  /* Enable RTC Clock */ 
  __HAL_RCC_RTC_ENABLE(); 
}

/**
  * @brief RTC MSP De-Initialization 
  *        This function frees the hardware resources used in this example:
  *          - Disable the Peripheral's clock
  * @param  hrtc: RTC handle pointer
  * @retval None
  */
void HAL_RTC_MspDeInit(RTC_HandleTypeDef *hrtc)
{
  /*##-1- Reset peripherals ##################################################*/
   __HAL_RCC_RTC_DISABLE();
}


/**
  * @brief  Backup save parameter 
  * @param  address: RTC Backup data Register number.
  *                  This parameter can be: RTC_BKP_DRx where x can be from 0 to 19 to 
  *                                         specify the register.
  * @param  Data:    Data to be written in the specified RTC Backup data register.
  * @retval None
  */
void DIMS_RTC_BkupSaveParameter(uint32_t address, uint32_t data)
{
  HAL_RTCEx_BKUPWrite(&RtcHandle,address,data);  
}

/**
  * @brief  Backup restore parameter. 
  * @param  address: RTC Backup data Register number.
  *                  This parameter can be: RTC_BKP_DRx where x can be from 0 to 19 to 
  *                                         specify the register. 
  * @retval None
  */
uint32_t DIMS_RTC_BkupRestoreParameter(uint32_t address)
{
   return HAL_RTCEx_BKUPRead(&RtcHandle,address);  
}

/**
  * @brief  RTC Get time. 
  * @param  Time: Pointer to Time structure
  * @retval None
  */
void DIMS_RTC_GetTime(  RTC_TimeTypeDef *Time)
{
   HAL_RTC_GetTime(&RtcHandle, Time, FORMAT_BIN);
}

/**
  * @brief  RTC Set time. 
  * @param  Time: Pointer to Time structure
  * @retval None
  */
void DIMS_RTC_SetTime(  RTC_TimeTypeDef *Time)
{
   Time->StoreOperation = 0;
   Time->SubSeconds = 0;
   Time->DayLightSaving = 0;
   HAL_RTC_SetTime(&RtcHandle, Time, FORMAT_BIN);
}

/**
  * @brief  RTC Get date
  * @param  Date: Pointer to Date structure
  * @retval None
  */
void DIMS_RTC_GetDate(  RTC_DateTypeDef *Date)
{
   HAL_RTC_GetDate(&RtcHandle, Date, FORMAT_BIN);
   
   if((Date->Date == 0) || (Date->Month == 0))
   {
     Date->Date = Date->Month = 1;
   }    
}

/**
  * @brief  RTC Set date
  * @param  Date: Pointer to Date structure
  * @retval None
  */
void DIMS_RTC_SetDate(  RTC_DateTypeDef *Date)
{
   HAL_RTC_SetDate(&RtcHandle, Date, FORMAT_BIN);
}

/** 
  * Function write value of time to user string
  * @param timestr : pointer to user string for save the result
  */ 
void DIMS_RTC_module_getStrT(char *timestr)
{
	RTC_TimeTypeDef stimestructureget;
	
	HAL_RTC_GetTime(&RtcHandle, &stimestructureget, RTC_FORMAT_BIN);
	
	sprintf(timestr, "%02d:%02d:%02d", stimestructureget.Hours, stimestructureget.Minutes, stimestructureget.Seconds);	
}

/** 
  * Function write value of date to user string
  * @param datestr : pointer to user string for save the result
  */ 
void DIMS_RTC_module_getStrD(char *datestr)
{
	RTC_DateTypeDef sdatestructureget;
	
	HAL_RTC_GetDate(&RtcHandle, &sdatestructureget, RTC_FORMAT_BIN);
	
	sprintf(datestr, "%02d/%02d/%02d", sdatestructureget.Date, sdatestructureget.Month, 2000 + sdatestructureget.Year);	
}