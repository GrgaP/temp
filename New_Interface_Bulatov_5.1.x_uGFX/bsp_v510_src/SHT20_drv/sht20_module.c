#include "main.h"
#include "i2c_drv.h"
#include "sht20_module.h"

extern I2C_HandleTypeDef I2cHandle;  

uint8_t result_flag;

//osThreadId sht20TaskHandle;

void wait_timer_cb(void const *arg);

osTimerDef(sht20_wait_timer, wait_timer_cb);

const uint16_t POLYNOMIAL = 0x131; //P(x)=x^8+x^5+x^4+1 = 100110001

void sht20_Task(void const * argument);//this task execute a state machine routine

typedef enum{
  S_READY,
  S_CHK_IT_STATE,
  S_MEASURE_WAITING,
  S_BUS_ERR
}sht20_mod_state;

uint16_t wait_timer_counter;

typedef enum{
  START,
  STOP_RESET,
}wait_timer_state;

uint8_t SHT2x_CheckCrc(uint8_t data[], uint8_t nbrOfBytes, uint8_t checksum)
{ 
  uint8_t crc = 0;
  uint8_t byteCtr;
  
  //calculates 8-Bit checksum with given polynomial
  for (byteCtr = 0; byteCtr < nbrOfBytes; ++byteCtr){ 
      crc ^= (data[byteCtr]);
      for (uint8_t bit = 8; bit > 0; --bit){ 
          if (crc & 0x80) crc = (crc << 1) ^ POLYNOMIAL;
          else crc = (crc << 1);
      }
 }
 
 if (crc != checksum) 
   return 0xff;
 else 
   return 0;
}

uint8_t SHT2x_ReadUserRegister(uint8_t *pRegisterValue)
{
  uint8_t error = 0;
  
  uint8_t checksum; //variable for checksum bytee
  uint8_t rBuf[2];  
 
  I2Cx_ReadBuffer(SHT20_I2C_ADDRESS, SHT20_READ_USER_REG, rBuf, 2);
  
  pRegisterValue[0] = rBuf[0];
  
  checksum = rBuf[1];
  
  error |= SHT2x_CheckCrc (pRegisterValue,1,checksum);
 
  return error;
}

uint8_t SHT2x_WriteUserRegister(uint8_t *pRegisterValue)
{
  uint8_t error = 0; //variable for error code

  if(HAL_I2C_Mem_Write(&I2cHandle, SHT20_I2C_ADDRESS, (uint16_t)SHT20_WRITE_USER_REG, 1, pRegisterValue, 1, 0) != HAL_OK)
     error = 0xff; 
  
  return error;
}

uint8_t SHT2x_ReadData(etSHT2xMeasureType eSHT2xMeasureType, int16_t *pMeasurand)
{
  uint8_t checksum; //checksum
  uint8_t data[3]; //data array for checksum verification
  uint8_t error=0; //error variable
  //uint16_t i; //counting variable
  
  //-- write I2C sensor address and command --
 
  /*I2c_StartCondition();
 
  error |= I2c_WriteByte (I2C_ADR_W); // I2C Adr*/
  
  switch(eSHT2xMeasureType)
  { 
  case HUMIDITY: 
    //error |= I2c_WriteByte (TRIG_RH_MEASUREMENT_HM);
     I2Cx_ReadBuffer(SHT20_I2C_ADDRESS, TRIG_T_MEASUREMENT_HM, data, 3);
    break;
  case TEMP: 
    //error |= I2c_WriteByte (TRIG_T_MEASUREMENT_HM);
     I2Cx_ReadBuffer(SHT20_I2C_ADDRESS, TRIG_RH_MEASUREMENT_HM, data, 3);
    break;
  default: 
    break;
  }
  
  
    
  //-- read two data bytes and one checksum byte --
  
  
  
  
       I2Cx_reset_bus();
      
      pMeasurand[0] = data[0];
      pMeasurand[1] = data[1];
  
      checksum=data[2];
  
  //-- verify checksum --
 
      error |= SHT2x_CheckCrc (data,2,checksum);

  
  //I2c_StopCondition();
 
  return error;
}

uint8_t SHT2x_SendCMD(etSHT2xMeasureType eSHT2xMeasureType)
{
  uint8_t 
  
  switch(eSHT2xMeasureType)
  { 
  case HUMIDITY: 
    HAL_I2C_Master_Transmit(&I2cHandle, SHT20_I2C_ADDRESS, &tmp, 1, 1000);
    break;
  case TEMP: 
    HAL_I2C_Master_Transmit(&I2cHandle, SHT20_I2C_ADDRESS, &tmp, 1, 1000);
    break;
  default: 
    break;
 
    //osTimerStart(sht20_wait_timer, 100);//start timer for interrupt waiting
}

/*
void wait_timer_cb(void const *arg)
{
  switch(I2Cx_get_bus_state())
  case RX_CPLT:
    
  break;
  case TX_CPLT:
    result_flag = 1;
  break;
  case ERR_IT:
    
  break;
  default
  break; 
    
       
}*/

void sht20_module_init(void)
{
  //osThreadDef(sht20Task, sht20_Task, osPriorityLow, 0, 128);
  //sht20TaskHandle = osThreadCreate(osThread(sht20Task), NULL);
 
  osTimerCreate(osTimer(sht20_wait_timer), osTimerOnce, NULL);

}
  //osTimerS

/*
void sht20_Task(void const * argument);
{
  
  
  
} */

void sht20_module_statemashine(void);
{
  switch(sht20_module_get_state());
  case READY:
     sht20_mod_state = CMD_SEND;
  case CMD_SEND:    
    
    if(I2Cx_get_bus_state() == TX_CPLT)
    {
      sht20_mod_state = MEASURE_WAITING;
      //start timer till ending of an measuring
    }    
    else
      sht20_mod_state = ERR;
    
  case DATA_REQ:    
    if(I2Cx_get_bus_state() == RX_CPLT)
      sht20_mod_state = MEASURE_COMPLETE;
    else
      sht20_mod_state = ERR;
    
  case MEASURE_WAITING:
    //check timer end flag
    sht20_mod_state = DATA_REQ;//go to DATA_REQ stage
  
  case MEASURE_COMPLETE:
    sht20_mod_state = RESULT_UPD;    
  
  case RESULT_UPD:
  
  case ERR:
}

int16_t sht20readTemp(void)
{
  //Write CMD
  //Check result flag
  //Read data
}

int16_t sht20readRh(void)
{
  
}


void sht20_reset(void)
{    
  //HAL_I2C_Master_Transmit(&I2cHandle, SHT20_I2C_ADDRESS, &tmp, 1, 1000);  
}

int16_t sht20test_read(void)
{
  uint8_t tmp;
  
  SHT2x_ReadUserRegister(&tmp);
  
  return (int16_t)tmp;
}
