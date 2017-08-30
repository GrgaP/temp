/**
  ******************************************************************************
  * @file    
  * @author  
  * @version 
  * @date    
  * @brief   
  ******************************************************************************
*/ 

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __IO_DRIVER_INTERFACE_H
#define __IO_DRIVER_INTERFACE_H

#include <stdint.h>
#include <stddef.h>
#include <stdio.h>
#include "IO_USB_LL_Driver.h"

/* Link low level  */
#define IO_DEVICE_MAX_OPEN (FOPEN_MAX)

/* The peripherals the IO library can interface to. */
typedef enum PeripheralTypesEnum
{
        PT_NO_TYPE              = 0,
	PT_UART_TYPE            = 1,
	PT_SPI_TYPE             = 2,
	PT_I2C_TYPE             = 3,
        PT_USB_HS_CDC_TYPE      = 4
} PeripheralTypes_t;

/* The structure that defines the peripherals that are available for use on
any particular supported board. */
typedef struct AvailablePeripheralsEnum
{
	const int8_t * const Path;		/* Text name of the peripheral. 
                                                   For example, "/UART0/", or 
                                                   "/SPI2/". */
	const PeripheralTypes_t PeripheralType;	/* The type of the peripheral, 
                                                   as defined by the 
                                                   Peripheral_Types_t enum. */
	const void *BaseAddress;		/* The base address of the 
                                                   peripheral in the 
                                                   microcontroller memory map */
} AvailablePeripherals_t;


typedef int (*IO_Open_t)(int handle);
typedef int (*IO_Write_t)(int handle, unsigned char * buffer, size_t size);
typedef int (*IO_Read_t)(int handle, char * buffer, size_t size);


typedef struct IO_DeviceDriverStructName
{
  IO_Open_t  IO_Open;      /* Function Hrdaware init */
  IO_Write_t IO_Write;
  IO_Read_t IO_Read;
} IO_DeviceDriver_t;



typedef struct IO_DeviceHandleNodeStructName
{
    int Handle;
    int8_t* Path;
    IO_DeviceDriver_t* pIO_DeviceDriver;    
} IO_DeviceHandleNode_t ;



/* USB summaru draiver LL link */
#define __IO_USB_LL_Open IO_USB_LL_Open
#define __IO_USB_LL_Write IO_USB_LL_Write   /* Link to IO_USB_Write_Board */ 
#define __IO_USB_LL_Read  IO_USB_LL_Read    /* Link to IO_USB_Read_Board */

#define __IO_I2C_LL_Open NULL
#define __IO_I2C_LL_Write NULL    /* Link to IO_USB_Write_Board */ 
#define __IO_I2C_LL_Read  NULL    /* Link to IO_USB_Read_Board */

#define __IO_SPI_LL_Open NULL 
#define __IO_SPI_LL_Write NULL    /* Link to IO_USB_Write_Board */ 
#define __IO_SPI_LL_Read  NULL    /* Link to IO_USB_Read_Board */

#define __IO_UART_LL_Open NULL
#define __IO_UART_LL_Write NULL   /* Link to IO_USB_Write_Board */ 
#define __IO_UART_LL_Read  NULL   /* Link to IO_USB_Read_Board */

int IO_OpenCallBack(const char * filename, int mode, int handle);
int IO_WriteCallBack(int handle, unsigned char * buffer, size_t size);
int IO_ReadCallBack(int handle, char * buffer, size_t size);

#endif /* __IO_DRIVER_INTERFACE_H */