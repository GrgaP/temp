/**
  ******************************************************************************
  * @file    i2c_drv.h
  * @author  SvD
  * @version V1.0.0
  * @date    05-August-2016
  * @brief   Header file of i2c driver for DIMS project.
  ******************************************************************************
  */

#ifndef __I2C_DRV_H
#define __I2C_DRV_H 
  
#include "main.h"

#define I2C_BUS_SPEED 50000

#define I2C_TIMING   0x204064ff

#define I2C_TIMEOUT_MAX   0x3000 /*<! The value of the maximal timeout for I2C waiting loops */

/*extern osSemaphoreId semaphore_i2c_tx_complete;

extern osSemaphoreId semaphore_i2c_rx_complete;

extern osSemaphoreId semaphore_i2c_error;*/

extern I2C_HandleTypeDef I2cHandle;  
    
extern enum I2C_BUS_STATE_TD {
  BUS_READY,
  BUS_BUSY,
  TX_CPLT,
  RX_CPLT,
  ACK_FAIL,
  ERR_IT,
  ERR_OTHER
} i2c_bus_state;
  
void I2Cx_Init(void);

enum I2C_BUS_STATE_TD I2Cx_get_bus_state(void);

void I2Cx_reset_bus(void);
  
/**
  * @brief  Writes a value in a register of the device through BUS.
  * @param  Addr: Device address on BUS Bus.  
  * @param  Reg: The target register address to write
  * @param  Value: The target register value to be written 
  */
uint8_t I2Cx_Write(uint8_t Addr, uint8_t Reg, uint8_t Value);

/**
  * @brief  Writes a value in a register of the device through BUS.
  * @param  Addr: Device address on BUS Bus.  
  * @param  Reg: The target register address to write
  * @param  pBuffer: The target register value to be written 
  * @param  Length: buffer size to be written
  */
uint8_t I2Cx_WriteBuffer(uint8_t Addr, uint8_t Reg,  uint8_t *pBuffer, uint16_t Length);

/**
  * @brief  Reads a register of the device through BUS.
  * @param  Addr: Device address on BUS Bus.  
  * @param  Reg: The target register address to write
  * @retval Data read at register address
  */
uint8_t I2Cx_Read(uint8_t Addr, uint8_t Reg, uint8_t *pResult);

/**
  * @brief  Reads multiple data on the BUS.
  * @param  Addr: I2C Address
  * @param  Reg: Reg Address 
  * @param  pBuffer: pointer to read data buffer
  * @param  Length: length of the data
  * @retval 0 if no problems to read multiple data
  */
uint8_t I2Cx_ReadBuffer(uint8_t Addr, uint8_t Reg, uint8_t *pBuffer, uint16_t Length);

#ifdef __cplusplus
}
#endif
#endif /* __I2C_DRV_H */