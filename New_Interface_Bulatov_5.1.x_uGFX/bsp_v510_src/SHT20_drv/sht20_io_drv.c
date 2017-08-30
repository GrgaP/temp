#include "i2c_drv.h"
#include "sht20_io_drv.h"

uint8_t sht20_io_init(void)
{
  I2Cx_Init();
	return 1;
}

uint8_t sht20_io_write(uint8_t Reg, uint8_t Value)
{
  uint8_t retval = I2Cx_Write(SHT20_I2C_ADDRESS, Reg, Value);
   
    return (retval)?TRUE:FALSE;  
}

uint8_t sht20_io_read(uint8_t Reg, uint8_t* rVal)
{
  
  uint8_t retval = I2Cx_ReadBuffer(SHT20_I2C_ADDRESS, Reg, uint8_t *rVal, 2);//I2Cx_Read(SHT20_I2C_ADDRESS, Reg, rVal);
    
  return (retval)?TRUE:FALSE;  
}