/**
  ******************************************************************************
  * @file    i2c_drv.c
  * @author  SvD
  * @version V1.0.0
  * @date    05-August-2016
  * @brief   Source file of i2c driver for DIMS project.
  ******************************************************************************
  */

#include "i2c_drv.h"
/*
osSemaphoreId semaphore_i2c_tx_complete;
osSemaphoreDef(semaphore_i2c_tx_complete);

osSemaphoreId semaphore_i2c_rx_complete;
osSemaphoreDef(semaphore_i2c_rx_complete);

osSemaphoreId semaphore_i2c_error;
osSemaphoreDef(semaphore_i2c_error);
*/

I2C_HandleTypeDef I2cHandle;

uint32_t I2c_Timeout = I2C_TIMEOUT_MAX; /*<! Value of Timeout when I2C communication fails */   

static uint8_t bus_init_state = 0;

/*---I2C bus states list---*/
///BUS_READY - The i2c bus is ready for doing any read/write operations
///BUS_BUSY - The i2c bus now is performs some operations
///TX_CPLT - Last operation of write was successfully completed
///RX_CPLT - Last operation of reading was successfully completed
///ACK_FAIL - A slave device don't send an acknolege
///ERR_IT - 
///ERR_OTHER - 

enum I2C_BUS_STATE_TD i2c_bus_state;

static void I2Cx_Error(void);

static void I2Cx_msp_init()
{
	GPIO_InitTypeDef GPIO_InitStruct;
	
	__GPIOA_CLK_ENABLE();
	__GPIOC_CLK_ENABLE();	
	
	/* Configure I2C Tx as alternate function  */
    GPIO_InitStruct.Pin       = GPIO_PIN_8;
    GPIO_InitStruct.Mode      = GPIO_MODE_AF_OD;
    GPIO_InitStruct.Pull      = GPIO_NOPULL;
    GPIO_InitStruct.Speed     = GPIO_SPEED_FAST;
    GPIO_InitStruct.Alternate = GPIO_AF4_I2C3;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
      
    /* Configure I2C Rx as alternate function  */
    GPIO_InitStruct.Pin = GPIO_PIN_9;
    HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);
	
    __I2C3_CLK_ENABLE();
	
	/* Force the I2C Peripheral Clock Reset */  
    __I2C3_FORCE_RESET();
      
    /* Release the I2C Peripheral Clock Reset */  
    __I2C3_RELEASE_RESET(); 
    
    /* Enable and set Discovery I2Cx Interrupt to the highest priority */
    HAL_NVIC_SetPriority(I2C3_EV_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(I2C3_EV_IRQn);
    
    /* Enable and set Discovery I2Cx Interrupt to the highest priority */
    HAL_NVIC_SetPriority(I2C3_ER_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(I2C3_ER_IRQn);
}
  
/**
  * @brief  I2Cx Bus initialization.
  */
void I2Cx_Init(void)
{
  if(!bus_init_state)
  {
    if(HAL_I2C_GetState(&I2cHandle) == HAL_I2C_STATE_RESET)
    {
      I2cHandle.Instance              = I2C3;
      I2cHandle.Init.Timing           = I2C_TIMING;
      //I2cHandle.Init.DutyCycle        = I2C_DUTYCYCLE_2;
      I2cHandle.Init.OwnAddress1      = 0;
      I2cHandle.Init.AddressingMode   = I2C_ADDRESSINGMODE_7BIT;
      I2cHandle.Init.DualAddressMode  = I2C_DUALADDRESS_DISABLED;
      I2cHandle.Init.OwnAddress2      = 0;
      I2cHandle.Init.GeneralCallMode  = I2C_GENERALCALL_DISABLED;
      I2cHandle.Init.NoStretchMode    = I2C_NOSTRETCH_DISABLED;  
    
      /* Init the I2C */
      I2Cx_msp_init();
      HAL_I2C_Init(&I2cHandle);
    
      i2c_bus_state = BUS_READY;
      
      bus_init_state = 1;
    }
    
   /* semaphore_i2c_rx_complete = osSemaphoreCreate(osSemaphore(semaphore_i2c_rx_complete), 2);
    semaphore_i2c_tx_complete = osSemaphoreCreate(osSemaphore(semaphore_i2c_tx_complete), 2);
    semaphore_i2c_error = osSemaphoreCreate(osSemaphore( semaphore_i2c_error), 2);*/
    
  }
}

enum I2C_BUS_STATE_TD I2Cx_get_bus_state(void)
{  
  return i2c_bus_state;
};

void I2Cx_reset_bus(void)
{
  if(i2c_bus_state != BUS_READY)
  {
    if(i2c_bus_state == ERR_OTHER)
      I2Cx_Error();
    else
      i2c_bus_state = BUS_READY;
  };
}

/**
* @brief This function handles I2C3 global interrupt.
*/
void I2C3_EV_IRQHandler(void)
{
  /* USER CODE BEGIN ADC_IRQn 0 */

  /* USER CODE END ADC_IRQn 0 */
  HAL_I2C_EV_IRQHandler(&I2cHandle);
  /* USER CODE BEGIN ADC_IRQn 1 */

  /* USER CODE END ADC_IRQn 1 */
}

/**
* @brief This function handles I2C3 global interrupt.
*/
void I2C3_ER_IRQHandler(void)
{
  /* USER CODE BEGIN ADC_IRQn 0 */

  /* USER CODE END ADC_IRQn 0 */
  HAL_I2C_ER_IRQHandler(&I2cHandle);
  /* USER CODE BEGIN ADC_IRQn 1 */

  /* USER CODE END ADC_IRQn 1 */
}

void HAL_I2C_MemTxCpltCallback(I2C_HandleTypeDef *hi2c)
{
  i2c_bus_state = TX_CPLT;
  //HAL_GPIO_WritePin(GPIOF, GPIO_PIN_8, !(HAL_GPIO_ReadPin(GPIOF,GPIO_PIN_8)));    
  //osSemaphoreRelease(semaphore_i2c_tx_complete);
}

void HAL_I2C_MemRxCpltCallback(I2C_HandleTypeDef *hi2c)
{
  i2c_bus_state = RX_CPLT;
  //HAL_GPIO_WritePin(GPIOF, GPIO_PIN_8, !(HAL_GPIO_ReadPin(GPIOF,GPIO_PIN_8)));    
  //osSemaphoreRelease(semaphore_i2c_rx_complete);
}


void HAL_I2C_MasterTxCpltCallback(I2C_HandleTypeDef *hi2c)
{
  i2c_bus_state = TX_CPLT;
  //HAL_GPIO_WritePin(GPIOF, GPIO_PIN_8, !(HAL_GPIO_ReadPin(GPIOF,GPIO_PIN_8)));    
  //osSemaphoreRelease(semaphore_i2c_tx_complete);//
}

void HAL_I2C_MasterRxCpltCallback(I2C_HandleTypeDef *hi2c)
{
  i2c_bus_state = RX_CPLT;
  //HAL_GPIO_WritePin(GPIOF, GPIO_PIN_8, !(HAL_GPIO_ReadPin(GPIOF,GPIO_PIN_8)));    
  //osSemaphoreRelease(semaphore_i2c_rx_complete);//
}
  
void HAL_I2C_ErrorCallback(I2C_HandleTypeDef *hi2c)
{
  i2c_bus_state = ERR_IT;
  //HAL_GPIO_WritePin(GPIOF, GPIO_PIN_9, !(HAL_GPIO_ReadPin(GPIOF,GPIO_PIN_9)));    
  //osSemaphoreRelease(semaphore_i2c_error);//
}

/**
  * @brief  Writes a value in a register of the device through BUS.
  * @param  Addr: Device address on BUS Bus.  
  * @param  Reg: The target register address to write
  * @param  Value: The target register value to be written 
  */
uint8_t I2Cx_Write(uint8_t Addr, uint8_t Reg, uint8_t Value)
{
  uint8_t retval = FALSE;
  
  if(i2c_bus_state == BUS_READY)
  {  
     if(HAL_I2C_Mem_Write_IT(&I2cHandle, Addr, (uint16_t)Reg, I2C_MEMADD_SIZE_8BIT, &Value, 1) != HAL_OK)
     {
      /* Error_Handler() function is called when Timeout error occurs.
       When Acknowledge failure occurs (Slave don't acknowledge it's address)
       Master restarts communication */
      if (HAL_I2C_GetError(&I2cHandle) != HAL_I2C_ERROR_AF)
      {
        i2c_bus_state = ERR_OTHER;
        I2Cx_Error();
      }
      else
        i2c_bus_state = ACK_FAIL;
      
     }
     else
     {
       i2c_bus_state = BUS_BUSY;
       retval = TRUE;
     }
  }
  return retval;
}

/**
  * @brief  Writes a value in a register of the device through BUS.
  * @param  Addr: Device address on BUS Bus.  
  * @param  Reg: The target register address to write
  * @param  pBuffer: The target register value to be written 
  * @param  Length: buffer size to be written
  */
uint8_t I2Cx_WriteBuffer(uint8_t Addr, uint8_t Reg,  uint8_t *pBuffer, uint16_t Length)
{  
  uint8_t retval = FALSE;
  
  if(i2c_bus_state == BUS_READY)
  {  
     if(HAL_I2C_Mem_Write_IT(&I2cHandle, Addr, (uint16_t)Reg, I2C_MEMADD_SIZE_8BIT, pBuffer, Length) != HAL_OK)
     {
      /* Error_Handler() function is called when Timeout error occurs.
       When Acknowledge failure occurs (Slave don't acknowledge it's address)
       Master restarts communication */
      if (HAL_I2C_GetError(&I2cHandle) != HAL_I2C_ERROR_AF)
      {
        i2c_bus_state = ERR_OTHER;
        I2Cx_Error();
      }
      else
        i2c_bus_state = ACK_FAIL;
      
     }
     else
     {
       i2c_bus_state = BUS_BUSY;
       retval = TRUE;
     }
  }
  return retval;  
}

/**
  * @brief  Reads a register of the device through BUS.
  * @param  Addr: Device address on BUS Bus.  
  * @param  Reg: The target register address to write
  * @retval Data read at register address
  */
uint8_t I2Cx_Read(uint8_t Addr, uint8_t Reg, uint8_t *pResult)
{
  
  uint8_t retval = FALSE;
  
  if(i2c_bus_state == BUS_READY)
  {  
     if(HAL_I2C_Mem_Read_IT(&I2cHandle, Addr, Reg, I2C_MEMADD_SIZE_8BIT, pResult, 1) != HAL_OK)
     {
      /* Error_Handler() function is called when Timeout error occurs.
       When Acknowledge failure occurs (Slave don't acknowledge it's address)
       Master restarts communication */
      if (HAL_I2C_GetError(&I2cHandle) != HAL_I2C_ERROR_AF)
      {
        i2c_bus_state = ERR_OTHER;
        I2Cx_Error();
      }
      else
        i2c_bus_state = ACK_FAIL;
      
     }
     else
     {
       i2c_bus_state = BUS_BUSY;
       retval = TRUE;
     }
  }
  return retval;  
}

/**
  * @brief  Reads multiple data on the BUS.
  * @param  Addr: I2C Address
  * @param  Reg: Reg Address 
  * @param  pBuffer: pointer to read data buffer
  * @param  Length: length of the data
  * @retval 0 if no problems to read multiple data
  */
uint8_t I2Cx_ReadBuffer(uint8_t Addr, uint8_t Reg, uint8_t *pBuffer, uint16_t Length)
{
  
  uint8_t retval = FALSE;
  
  if(i2c_bus_state == BUS_READY)
  {  
    if(HAL_I2C_Mem_Read_IT(&I2cHandle, Addr, Reg, I2C_MEMADD_SIZE_8BIT, pBuffer, Length) != HAL_OK)
    {
      /* Error_Handler() function is called when Timeout error occurs.
       When Acknowledge failure occurs (Slave don't acknowledge it's address)
       Master restarts communication */
      if (HAL_I2C_GetError(&I2cHandle) != HAL_I2C_ERROR_AF)
      {
        i2c_bus_state = ERR_OTHER;
        I2Cx_Error();
      }
      else
        i2c_bus_state = ACK_FAIL;
      
     }
     else
     {
       i2c_bus_state = BUS_BUSY;
       retval = TRUE;
     }
  }
  return retval; 
  
}

/**
  * @brief  I2Cx error treatment function
  */
static void I2Cx_Error(void)
{
        bus_init_state = 0;
	/* De-initialize the SPI communication BUS */
	HAL_I2C_DeInit(&I2cHandle);
  
	/* Re-Initialize the SPI communication BUS */
	I2Cx_Init();
}