/**
  ******************************************************************************
  * @file    
  * @author  
  * @version 
  * @date    
  * @brief   
  ******************************************************************************
*/ 

#include "usbd_cdc_if.h"
#include "usb_device.h"
#include "IO_USB_LL_Driver.h"
#include <stdbool.h>

/**
  * @brief  IO_USB_Write to CDC Transmit
  * ....
  * @retval Number of sending byte or error -1.  
  */
int IO_USB_LL_Write_(int handle,  unsigned char * buffer, size_t size)
{
  return (CDC_Transmit_HS((uint8_t*)buffer, size));
}

#define DELAY_BUSY_TICK 2
int IO_USB_LL_Write(int handle,  unsigned char * buffer, size_t size)
{
  size_t trChar = 0;
  uint8_t status;
  while (size)
  {
    if (size > USB_TX_DATA_SIZE) /* Transmit full packet */
    {
       
      do /* Wait transmit */
      {
        status = CDC_Transmit_HS(buffer+trChar, USB_TX_DATA_SIZE);
        if (status ==  USBD_FAIL) return(-1);
        if (status ==  USBD_BUSY) HAL_Delay(DELAY_BUSY_TICK);
      } while (status == USBD_BUSY);
      size -= USB_TX_DATA_SIZE;
      trChar += USB_TX_DATA_SIZE;
    }
    else /* transmit reduce packet */
    {
      do /* Wait transmit */
      {
        status = CDC_Transmit_HS(buffer+trChar, size);
        if (status ==  USBD_FAIL) return(-1);
        if (status ==  USBD_BUSY) HAL_Delay(DELAY_BUSY_TICK);
      } while (status == USBD_BUSY);
      trChar += size;
      size = 0;
    }
  }
  return (trChar);
}

int IO_USB_LL_Open(int handle)
{
  MX_USB_DEVICE_Init();
  return handle;  
}

/* IO USB Low Level section defenition group ---------------------------------*/
static struct IO_USB_LL_RingBufSructName
{
  uint8_t Buf[IO_USB_LL_RAED_BUF_SIZE];
  uint32_t BufIn;  /* USB_CDC_ReciveCallback only write */
  uint32_t BufOut; /* IO_USB_LL_Read only write */
} IO_USB_LL_RingBuf = {{0},0,0};

#define BUFF_MASK (IO_USB_LL_RAED_BUF_SIZE-1)

/* This callback run in recive interrapt  */
void USB_CDC_ReciveCallback(uint8_t* Buf, uint32_t Len)
{
  volatile uint32_t saveBufOut = IO_USB_LL_RingBuf.BufOut;
  for (uint32_t i = 0; i < Len; i++) 
  {
      uint32_t temp = (IO_USB_LL_RingBuf.BufIn+1)&BUFF_MASK;
      
      if (temp != IO_USB_LL_RingBuf.BufOut)
      {
        IO_USB_LL_RingBuf.Buf[IO_USB_LL_RingBuf.BufIn] = Buf[i];
        IO_USB_LL_RingBuf.BufIn = temp;
      }
      else
      {
      /* Not space in buffer */
        return;
      }
  }
}

int IO_USB_LL_Read(int handle, char * buffer, size_t size)
{
  volatile uint32_t saveBufIn;
  int readChar = 0;
  
  for (uint32_t i = 0; i < size; )
  {
    saveBufIn =  IO_USB_LL_RingBuf.BufIn;
    if (IO_USB_LL_RingBuf.BufOut != IO_USB_LL_RingBuf.BufIn)
    {
      buffer[i++] = IO_USB_LL_RingBuf.Buf[IO_USB_LL_RingBuf.BufOut];
      /* must bee atom */
      IO_USB_LL_RingBuf.BufOut = ((IO_USB_LL_RingBuf.BufOut+1)&BUFF_MASK);
      readChar+=1;
    }
    else
    {
      break;  
      /* not data, waiting */
    }
  }
  return readChar;
}





