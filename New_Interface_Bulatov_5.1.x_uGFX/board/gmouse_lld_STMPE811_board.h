/*
 * This file is subject to the terms of the GFX License. If a copy of
 * the license was not distributed with this file, you can obtain one at:
 *
 *              http://ugfx.org/license.html
 */

#ifndef _GINPUT_LLD_MOUSE_BOARD_H
#define _GINPUT_LLD_MOUSE_BOARD_H

#include "i2c_drv.h"
#include "dims_rtc.h"

#define GMOUSE_AX_CAL_BKUP_DR   RTC_BKP_DR26
#define GMOUSE_BX_CAL_BKUP_DR   RTC_BKP_DR27
#define GMOUSE_CX_CAL_BKUP_DR   RTC_BKP_DR28
#define GMOUSE_AY_CAL_BKUP_DR   RTC_BKP_DR29
#define GMOUSE_BY_CAL_BKUP_DR   RTC_BKP_DR30
#define GMOUSE_CY_CAL_BKUP_DR   RTC_BKP_DR31

// Resolution and Accuracy Settings
#define GMOUSE_STMPE811_PEN_CALIBRATE_ERROR		8
#define GMOUSE_STMPE811_PEN_CLICK_ERROR			6
#define GMOUSE_STMPE811_PEN_MOVE_ERROR			4
#define GMOUSE_STMPE811_FINGER_CALIBRATE_ERROR	        14
#define GMOUSE_STMPE811_FINGER_CLICK_ERROR		18
#define GMOUSE_STMPE811_FINGER_MOVE_ERROR		14

// How much extra data to allocate at the end of the GMouse structure for the board's use
#define GMOUSE_STMPE811_BOARD_DATA_SIZE			0

// Options - Leave these commented to make it user configurable in the gfxconf.h
#define GMOUSE_STMPE811_READ_PRESSURE		TRUE
//#define GMOUSE_STMPE811_SELF_CALIBRATE	        FALSE
//#define GMOUSE_STMPE811_TEST_MODE		FALSE

// Set to FALSE because it does not work properly on this board even though the pin exists.
#define GMOUSE_STMPE811_GPIO_IRQPIN				TRUE

// If TRUE this is a really slow CPU and we should always clear the FIFO between reads.
#define GMOUSE_STMPE811_SLOW_CPU				FALSE

// Slave address
#define STMPE811_ADDR				0x82

// Maximum timeout
//#define STMPE811_TIMEOUT			0x3000

/*static const I2CConfig i2ccfg = {
	OPMODE_I2C,
	400000,
	FAST_DUTY_CYCLE_2,
};*/

static bool_t init_board(GMouse* m, unsigned driverinstance) {
	(void)		m;

	// This board only supports one touch panel
	if (driverinstance)
		return FALSE;

	// Set pin modes
	//palSetPadMode(GPIOA, 15, PAL_MODE_INPUT | PAL_STM32_PUDR_FLOATING);			/* TP IRQ */
	//palSetPadMode(GPIOA, 8, PAL_MODE_ALTERNATE(4) | PAL_STM32_OTYPE_OPENDRAIN);	/* SCL */
	//palSetPadMode(GPIOC, 9, PAL_MODE_ALTERNATE(4) | PAL_STM32_OTYPE_OPENDRAIN);	/* SDA */

	// Start the I2C
	//i2cStart(&I2CD3, &i2ccfg);

        GPIO_InitTypeDef GPIO_InitStruct;
        
        GPIO_InitStruct.Pin       = GPIO_PIN_7;
        GPIO_InitStruct.Mode      = GPIO_MODE_IT_FALLING;
        GPIO_InitStruct.Pull      = GPIO_NOPULL;;
        HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
        
        /* Enable and set EXTI lines 9 to 5 Interrupt to the lowest priority */
        HAL_NVIC_SetPriority(EXTI9_5_IRQn, 5, 0);
        HAL_NVIC_EnableIRQ(EXTI9_5_IRQn);        
        
        
        /* Initialize IO BUS layer */
        I2Cx_Init();
	
        return TRUE;
}

void EXTI9_5_IRQHandler (void)
{
  HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_7);
}

/**
  * @brief EXTI line detection callbacks
  * @param GPIO_Pin: Specifies the pins connected EXTI line
  * @retval None
  */
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
  if (GPIO_Pin == GPIO_PIN_7)
  {

   GMouse *m;
  // Find the instance
  if (!(m = (GMouse *)gdriverGetInstance(GDRIVER_TYPE_MOUSE, 0)))
    return;

  //chSysLockFromISR();
  _gmouseWakeupI(m);
  //chSysUnlockFromISR();    
  }
}

#if GMOUSE_STMPE811_GPIO_IRQPIN
	static bool_t getpin_irq(GMouse* m) {
		(void)		m;

                return !(HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_7));//!palReadPad(GPIOA, 15);
	}
#endif

static GFXINLINE void aquire_bus(GMouse* m) {
	(void)		m;

}

static GFXINLINE void release_bus(GMouse* m) {
	(void)		m;

}

static void write_reg(GMouse* m, uint8_t reg, uint8_t val) {
        
       if(I2Cx_Write(STMPE811_ADDR, reg, val))
       {
         while(I2Cx_get_bus_state() == BUS_BUSY);
         if(I2Cx_get_bus_state() == TX_CPLT)
            I2Cx_reset_bus();
         
       }
}

static uint8_t read_byte(GMouse* m, uint8_t reg) {
       static uint8_t touch_data8;
       uint8_t	rxbuf;

       if(I2Cx_Read(STMPE811_ADDR, reg, &rxbuf))
       {
         while(I2Cx_get_bus_state() == BUS_BUSY);
         
         if(I2Cx_get_bus_state() == RX_CPLT)
            touch_data8 = rxbuf;
         
       I2Cx_reset_bus();
       }  
             
       return touch_data8;
}

static uint16_t read_word(GMouse* m, uint8_t reg) {
	static uint16_t touch_data16;  
        uint8_t	rxbuf[2];
  
        if(I2Cx_ReadBuffer(STMPE811_ADDR, reg, rxbuf, 2))
        { 
          while(I2Cx_get_bus_state() == BUS_BUSY);
          
          if(I2Cx_get_bus_state() == RX_CPLT)            
            touch_data16 = (((uint16_t)rxbuf[0]) << 8) | rxbuf[1];
          
          I2Cx_reset_bus();
        };
        
        return touch_data16;
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
