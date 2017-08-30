/*
 * This file is subject to the terms of the GFX License. If a copy of
 * the license was not distributed with this file, you can obtain one at:
 *
 *              http://ugfx.org/license.html
 */

#ifndef _GINPUT_LLD_MOUSE_BOARD_H
#define _GINPUT_LLD_MOUSE_BOARD_H

#include "stm32f7xx.h"
#include "stm32f7_i2c.h"
#include "dims_rtc.h"

#define GMOUSE_AX_CAL_BKUP_DR   RTC_BKP_DR26
#define GMOUSE_BX_CAL_BKUP_DR   RTC_BKP_DR27
#define GMOUSE_CX_CAL_BKUP_DR   RTC_BKP_DR28
#define GMOUSE_AY_CAL_BKUP_DR   RTC_BKP_DR29
#define GMOUSE_BY_CAL_BKUP_DR   RTC_BKP_DR30
#define GMOUSE_CY_CAL_BKUP_DR   RTC_BKP_DR31

// Resolution and Accuracy Settings
#define GMOUSE_FT5336_PEN_CALIBRATE_ERROR		8
#define GMOUSE_FT5336_PEN_CLICK_ERROR			6
#define GMOUSE_FT5336_PEN_MOVE_ERROR			4
#define GMOUSE_FT5336_FINGER_CALIBRATE_ERROR	14
#define GMOUSE_FT5336_FINGER_CLICK_ERROR		18
#define GMOUSE_FT5336_FINGER_MOVE_ERROR			14

// How much extra data to allocate at the end of the GMouse structure for the board's use
#define GMOUSE_FT5336_BOARD_DATA_SIZE			0

// The FT5336 I2C slave address (including the R/W bit)
#define FT5336_SLAVE_ADDR 0x70

#if !GFX_USE_OS_CHIBIOS
	#define AFRL	AFR[0]
	#define AFRH	AFR[1]
#endif

static bool_t init_board(GMouse* m, unsigned instance)
{
	(void)m;
	(void)instance;

	// I2C3_SCL    GPIOH7, alternate, opendrain, highspeed
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOHEN;					// Enable clock for
	GPIOH->MODER |= GPIO_MODER_MODER7_1;					// Alternate function
	GPIOH->OTYPER |= GPIO_OTYPER_OT_7;						// OpenDrain
	GPIOH->OSPEEDR &= ~GPIO_OSPEEDER_OSPEEDR7;				// LowSpeed
	GPIOH->AFRL |= ((uint32_t)0x04 << 4*7);							// AF4

	// I2C3_SDA    GPIOH8, alternate, opendrain, highspeed
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOHEN;					// Enable clock
	GPIOH->MODER |= GPIO_MODER_MODER8_1;					// Alternate function
	GPIOH->OTYPER |= GPIO_OTYPER_OT_8;						// OpenDrain
	GPIOH->OSPEEDR &= ~GPIO_OSPEEDER_OSPEEDR8;				// LowSpeed
	GPIOH->AFRH |= ((uint32_t)0x04 << 4*0);							// AF4

	// Initialize the I2C3 peripheral
	if (!(i2cInit(I2C3))) {
		return FALSE;
	}

	return TRUE;
}

static void write_reg(GMouse* m, uint8_t reg, uint8_t val)
{
	(void)m;

	i2cWriteReg(I2C3, FT5336_SLAVE_ADDR, reg, val);
}

static uint8_t read_byte(GMouse* m, uint8_t reg)
{
	(void)m;

	return i2cReadByte(I2C3, FT5336_SLAVE_ADDR, reg);
}

static uint16_t read_word(GMouse* m, uint8_t reg)
{
	(void)m;

	return i2cReadWord(I2C3, FT5336_SLAVE_ADDR, reg);
}

/**
  * Function for loading calibration data with starting of the Gmouse module 
  */
bool_t LoadMouseCalibration(unsigned instance, void *buf, size_t sz)
{
  uint32_t* cal_data_ptr;
  
  if(DIMS_RTC_BkupRestoreParameter(GMOUSE_AX_CAL_BKUP_DR))
  {
      cal_data_ptr = (uint32_t*) buf;
    
      cal_data_ptr[0] = DIMS_RTC_BkupRestoreParameter(GMOUSE_AX_CAL_BKUP_DR);
      cal_data_ptr[1] = DIMS_RTC_BkupRestoreParameter(GMOUSE_BX_CAL_BKUP_DR);
      cal_data_ptr[2] = DIMS_RTC_BkupRestoreParameter(GMOUSE_CX_CAL_BKUP_DR);
                                                                           
      cal_data_ptr[3] = DIMS_RTC_BkupRestoreParameter(GMOUSE_AY_CAL_BKUP_DR);
      cal_data_ptr[4] = DIMS_RTC_BkupRestoreParameter(GMOUSE_BY_CAL_BKUP_DR);
      cal_data_ptr[5] = DIMS_RTC_BkupRestoreParameter(GMOUSE_CY_CAL_BKUP_DR);   
    
    return 1;
  }  
  else
    return 0;
};

/**
  * Function for saving calibration data, after a calibration process was completed
  */
bool_t SaveMouseCalibration(unsigned instance, const void *buf, size_t sz)
{
  uint32_t* cal_data_ptr;
  
  cal_data_ptr = (uint32_t*) buf;
  
  DIMS_RTC_BkupSaveParameter(GMOUSE_AX_CAL_BKUP_DR, cal_data_ptr[0]);
  DIMS_RTC_BkupSaveParameter(GMOUSE_BX_CAL_BKUP_DR, cal_data_ptr[1]);
  DIMS_RTC_BkupSaveParameter(GMOUSE_CX_CAL_BKUP_DR, cal_data_ptr[2]);
                                                    
  DIMS_RTC_BkupSaveParameter(GMOUSE_AY_CAL_BKUP_DR, cal_data_ptr[3]);
  DIMS_RTC_BkupSaveParameter(GMOUSE_BY_CAL_BKUP_DR, cal_data_ptr[4]);
  DIMS_RTC_BkupSaveParameter(GMOUSE_CY_CAL_BKUP_DR, cal_data_ptr[5]);
  
  return 1;
    
};    

#endif /* _GINPUT_LLD_MOUSE_BOARD_H */
