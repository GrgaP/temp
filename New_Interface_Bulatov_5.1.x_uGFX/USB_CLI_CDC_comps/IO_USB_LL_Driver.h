/**
  ******************************************************************************
  * @file    
  * @author  
  * @version 
  * @date    
  * @brief   
  ******************************************************************************
*/
#ifndef __IO_USB_LL_DRIVER_H
#define __IO_USB_LL_DRIVER_H

//int IO_USB_LL_Write_(int handle,  unsigned char * buffer, size_t size);

#define IO_USB_LL_RAED_BUF_SIZE (1<<8) //256


int IO_USB_LL_Open(int handle);
int IO_USB_LL_Write(int handle, unsigned char * buffer, size_t size);
int IO_USB_LL_Read(int handle, char * buffer, size_t size);

#endif /* __IO_DRIVER_INTERFACE_H */
