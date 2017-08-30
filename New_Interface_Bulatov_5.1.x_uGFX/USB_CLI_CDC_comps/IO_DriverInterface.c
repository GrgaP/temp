/**
  ******************************************************************************
  * @file    
  * @author  
  * @version 
  * @date    
  * @brief   
  ******************************************************************************
*/ 

#include "BSP_IO_DIMS_DSP_V5.1.0.h"
#include "IO_DriverInterface.h"
#include <stdint.h>
#include <string.h>
#include <stdio.h>

/* List of device in board*/
static const AvailablePeripherals_t AvailablePeripherals[] = boardAVAILABLE_DEVICES_LIST;

/* Number device in system */
const uint32_t IO_DeviceNumber = sizeof (AvailablePeripherals) / sizeof (AvailablePeripherals_t);


/* Global static table for link path handel and device driver.  
   Static file system */
volatile static IO_DeviceHandleNode_t IO_DeviceHandleTable[IO_DEVICE_MAX_OPEN] = {0};


/** @defgroup IO_DeviceDriver list tith static link
  * @{    
  */ 
IO_DeviceDriver_t IO_USB_DeviceDriver =  {__IO_USB_LL_Open, __IO_USB_LL_Write, __IO_USB_LL_Read};
IO_DeviceDriver_t IO_I2C_DeviceDriver =  {__IO_I2C_LL_Open, __IO_I2C_LL_Write, __IO_I2C_LL_Read};
IO_DeviceDriver_t IO_SPI_DeviceDriver =  {__IO_SPI_LL_Open, __IO_SPI_LL_Write, __IO_SPI_LL_Read};
IO_DeviceDriver_t IO_UART_DeviceDriver = {__IO_UART_LL_Open,__IO_UART_LL_Write, __IO_UART_LL_Read};
/**
  * @}
  */

/**
  * @brief This function selekt IO_DeviceDriver depending of his
  *        PeripheralType value    
  * @param PeripheralType - type of device 
  * @retval Pointer of IO_DeviceDriver 
  */
IO_DeviceDriver_t* IO_DeviceDriverLink(PeripheralTypes_t PeripheralType)
{
  switch (PeripheralType)
  {
  case PT_USB_HS_CDC_TYPE:
    return (&IO_USB_DeviceDriver);
    break;
  case PT_I2C_TYPE:
    return (&IO_I2C_DeviceDriver);
    break;
  case PT_SPI_TYPE:
    return (&IO_SPI_DeviceDriver);
    break;
  case PT_UART_TYPE:
    return (&IO_UART_DeviceDriver);
    break;
  default:
    return (NULL); /* no driver found*/
  }
}

/**
  * @brief Callbak for system __open function. Set fild of IO_DeviceHandleTable.
  * @param *filename: pointer to constant string file name.
  * @param mode: from fopen function param.
  * @retval returnValue: if stream create return handle of file, else return -1.  
  */
int IO_OpenCallBack(const char * filename, int mode, int handle)
{
   int returnValue = handle;
   
   
   /* Search avalable device for filename */
   for (uint32_t i = 0; i < IO_DeviceNumber; i++)
   {
      if (!strcmp(( const char * const ) filename, ( const char * const ) AvailablePeripherals[ i ].Path )) 
      {
        /* Write fild of  IO_DeviceHandleTabl */
        IO_DeviceHandleTable[handle].Handle = handle; 
        IO_DeviceHandleTable[handle].pIO_DeviceDriver = IO_DeviceDriverLink( AvailablePeripherals[ i ].PeripheralType);
        IO_DeviceHandleTable[handle].Path = (int8_t*)AvailablePeripherals[ i ].Path;
        returnValue = handle;
        
        for (uint32_t a = 0 ; a < 3; a++)
        {
          /* redirect stderr stdout */
          IO_DeviceHandleTable[a].Handle = a; 
          IO_DeviceHandleTable[a].pIO_DeviceDriver = IO_DeviceDriverLink( AvailablePeripherals[ i ].PeripheralType);
          IO_DeviceHandleTable[a].Path = (int8_t*)AvailablePeripherals[ i ].Path;
        }
        
        break;
      }
      else 
      {
        /* No device found */
        returnValue = -1;
      }
   }
   if ( /* Check driver instal and open install */
       (IO_DeviceHandleTable[handle].pIO_DeviceDriver->IO_Open == NULL) \
     ||(IO_DeviceHandleTable[handle].pIO_DeviceDriver == NULL)      
      )
   {    
     return (-1);
   }
   returnValue = IO_DeviceHandleTable[handle].pIO_DeviceDriver->IO_Open(handle);
   return(returnValue);
}

/**
  * @brief Callbak from system __write function. Select write function
  *        from separated IO_DeviceDriver register in IO_DeviceHandleTable. 
  * @param handle: sream of file.
  * @param *buffer: pointer to a buffer data.
  * @param size: size of data to write.
  * @retval returnValue: if stream create return handle of file, else return -1.  
  */
int IO_WriteCallBack(int handle, unsigned char * buffer, size_t size)
{
   /* Check of present metod */
   if (IO_DeviceHandleTable[handle].pIO_DeviceDriver->IO_Write == NULL)
   {
     /* Error driver not present */
     return (-1);
   }
   else
   {
     /* Run driver function and return result */
     return (IO_DeviceHandleTable[handle].pIO_DeviceDriver->IO_Write(handle, buffer, size));
   }
}


int IO_ReadCallBack(int handle, char * buffer, size_t size)
{
   /* Check of present metod */
   if (IO_DeviceHandleTable[handle].pIO_DeviceDriver->IO_Read == NULL)
   {
     /* Error driver not present */
     return (-1);
   }
   else
   {
     /* Run driver function and return result */
     return (IO_DeviceHandleTable[handle].pIO_DeviceDriver->IO_Read(handle, buffer, size));
   }
}










