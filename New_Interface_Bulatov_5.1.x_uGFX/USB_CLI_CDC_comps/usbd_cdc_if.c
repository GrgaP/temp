/**
  ******************************************************************************
  * @file           : usbd_cdc_if.c
  * @author         : MCD Application Team
  * @version        : V1.1.0
  * @date           : 19-March-2012
  * @brief          :
  ******************************************************************************
  * COPYRIGHT(c) 2016 STMicroelectronics
  *
  * Redistribution and use in source and binary forms, with or without modification,
  * are permitted provided that the following conditions are met:
  * 1. Redistributions of source code must retain the above copyright notice,
  * this list of conditions and the following disclaimer.
  * 2. Redistributions in binary form must reproduce the above copyright notice,
  * this list of conditions and the following disclaimer in the documentation
  * and/or other materials provided with the distribution.
  * 3. Neither the name of STMicroelectronics nor the names of its contributors
  * may be used to endorse or promote products derived from this software
  * without specific prior written permission.
  *
  * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
  * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
*/

/* Includes ------------------------------------------------------------------*/
#include "usbd_cdc_if.h"

volatile static uint32_t debugVar[256];

/* Count of CDC_ControlTransfer if > 10 - file open */
static uint8_t cnt=0; /*  */

/** @addtogroup STM32_USB_OTG_DEVICE_LIBRARY
  * @{
  */

/** @defgroup USBD_CDC 
  * @brief usbd core module
  * @{
  */ 

/** @defgroup USBD_CDC_Private_TypesDefinitions
  * @{
  */ 
  /* USER CODE BEGIN 0 */ 
  /* USER CODE END 0 */ 
/**
  * @}
  */ 

/** @defgroup USBD_CDC_Private_Defines
  * @{
  */ 
  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */  
/**
  * @}
  */ 

/** @defgroup USBD_CDC_Private_Macros
  * @{
  */ 
  /* USER CODE BEGIN 2 */ 
  /* USER CODE END 2 */
/**
  * @}
  */ 
  
/** @defgroup USBD_CDC_Private_Variables
  * @{
  */
  /* USER CODE BEGIN 3 */
/* Create buffer for reception and transmission           */
/* It's up to user to redefine and/or remove those define */
/* Received Data over USB are stored in this buffer       */
uint8_t USB_UserRxBufferHS[USB_RX_DATA_SIZE];

/* Send Data over USB CDC are stored in this buffer       */
uint8_t USB_UserTxBufferHS[USB_TX_DATA_SIZE];


  /* USER CODE END 3 */

/* USB handler declaration */
/* Handle for USB Full Speed IP */
USBD_HandleTypeDef  *hUsbDevice_1;

extern USBD_HandleTypeDef hUsbDeviceHS;

/**
  * @}
  */ 
  
/** @defgroup USBD_CDC_Private_FunctionPrototypes
  * @{
  */
static int8_t CDC_Init_HS     (void);
static int8_t CDC_DeInit_HS   (void);
static int8_t CDC_Control_HS  (uint8_t cmd, uint8_t* pbuf, uint16_t length);
static int8_t CDC_Receive_HS  (uint8_t* pbuf, uint32_t *Len);

USBD_CDC_ItfTypeDef USBD_Interface_fops_HS = 
{
  CDC_Init_HS,
  CDC_DeInit_HS,
  CDC_Control_HS,  
  CDC_Receive_HS
};

/* Private functions ---------------------------------------------------------*/
/**
  * @brief  CDC_Init_HS
  *         Initializes the CDC media low layer over the HS USB IP
  * @param  None
  * @retval Result of the opeartion: USBD_OK if all operations are OK else USBD_FAIL
  */
static int8_t CDC_Init_HS(void)
{
  hUsbDevice_1 = &hUsbDeviceHS;
  /* USER CODE BEGIN 4 */ 
  /* Set Application Buffers */
  USBD_CDC_SetTxBuffer(hUsbDevice_1, USB_UserTxBufferHS, 0);
  USBD_CDC_SetRxBuffer(hUsbDevice_1, USB_UserRxBufferHS);
  return (USBD_OK);
  /* USER CODE END 4 */ 
}

/**
  * @brief  CDC_DeInit_HS
  *         DeInitializes the CDC media low layer
  * @param  None
  * @retval Result of the opeartion: USBD_OK if all operations are OK else USBD_FAIL
  */
static int8_t CDC_DeInit_HS(void)
{
  /* USER CODE BEGIN 5 */ 
  return (USBD_OK);
  /* USER CODE END 5 */ 
}

/**
  * @brief  CDC_Control_HS
  *         Manage the CDC class requests
  * @param  Cmd: Command code            
  * @param  Buf: Buffer containing command data (request parameters)
  * @param  Len: Number of data to be sent (in bytes)
  * @retval Result of the opeartion: USBD_OK if all operations are OK else USBD_FAIL
  */
static int8_t CDC_Control_HS  (uint8_t cmd, uint8_t* pbuf, uint16_t length)
{ 
  /* USER CODE BEGIN 6 */

  debugVar[cnt++] = cmd;
  switch (cmd)
  {
  
  case CDC_SEND_ENCAPSULATED_COMMAND:
 
    break;

  case CDC_GET_ENCAPSULATED_RESPONSE:
 
    break;

  case CDC_SET_COMM_FEATURE:
 
    break;

  case CDC_GET_COMM_FEATURE:

    break;

  case CDC_CLEAR_COMM_FEATURE:

    break;

  /*******************************************************************************/
  /* Line Coding Structure                                                       */
  /*-----------------------------------------------------------------------------*/
  /* Offset | Field       | Size | Value  | Description                          */
  /* 0      | dwDTERate   |   4  | Number |Data terminal rate, in bits per second*/
  /* 4      | bCharFormat |   1  | Number | Stop bits                            */
  /*                                        0 - 1 Stop bit                       */
  /*                                        1 - 1.5 Stop bits                    */
  /*                                        2 - 2 Stop bits                      */
  /* 5      | bParityType |  1   | Number | Parity                               */
  /*                                        0 - None                             */
  /*                                        1 - Odd                              */ 
  /*                                        2 - Even                             */
  /*                                        3 - Mark                             */
  /*                                        4 - Space                            */
  /* 6      | bDataBits  |   1   | Number Data bits (5, 6, 7, 8 or 16).          */
  /*******************************************************************************/
  case CDC_SET_LINE_CODING:   
	
    break;

  case CDC_GET_LINE_CODING:     

    break;

  case CDC_SET_CONTROL_LINE_STATE:

    break;

  case CDC_SEND_BREAK:
 
    break;    
    
  default:
    break;
  }

  return (USBD_OK);
  /* USER CODE END 6 */
}

uint32_t USB_GetFileOpenStatus(void)
{
  if(cnt > 10) return 1;
  else return 0;
}

/**
  * @brief  CDC_Receive_HS
  *         Data received over USB OUT endpoint are sent over CDC interface 
  *         through this function.
  *           
  *         @note
  *         This function will block any OUT packet reception on USB endpoint 
  *         untill exiting this function. If you exit this function before transfer
  *         is complete on CDC interface (ie. using DMA controller) it will result 
  *         in receiving more data while previous ones are still not sent.
  *                 
  * @param  Buf: Buffer of data to be received
  * @param  Len: Number of data received (in bytes)
  * @retval Result of the opeartion: USBD_OK if all operations are OK else USBD_FAIL
  */
static int8_t CDC_Receive_HS (uint8_t* Buf, uint32_t *Len)
{
  USB_CDC_ReciveCallback(Buf, *Len);
  /* Prepare to recive new packet */ 
  USBD_CDC_ReceivePacket(hUsbDevice_1);
  return (USBD_OK);
}

__weak void USB_CDC_ReciveCallback(uint8_t* Buf, uint32_t Len)
{
  for (uint32_t i = 0; i< Len; i++ )
  {
   USB_UserTxBufferHS[i] = Buf[i];
  }
  
  CDC_Transmit_HS(Buf, Len);
}
/**
  * @brief  CDC_Transmit_HS
  *         Data send over USB IN endpoint are sent over CDC interface 
  *         through this function.           
  *         @note
  *         
  *                 
  * @param  Buf: Buffer of data to be send
  * @param  Len: Number of data to be send (in bytes)
  * @retval Result of the opeartion: USBD_OK if all operations are OK else USBD_FAIL or USBD_BUSY
  */
uint8_t CDC_Transmit_HS(uint8_t* Buf, uint16_t Len)
{
  for (uint32_t i = 0; i < Len; i++)
  USB_UserTxBufferHS[i] = Buf[i];
  /* Try to send */  
  uint8_t status = USBD_OK;
  if(hUsbDevice_1->dev_state == USBD_STATE_CONFIGURED)
  {
    USBD_CDC_SetTxBuffer(hUsbDevice_1, USB_UserTxBufferHS, Len);
    status = USBD_CDC_TransmitPacket(hUsbDevice_1);
  }
  else
    status = USBD_BUSY;
  
  return status;
}

uint32_t  USBD_CheckDeviceStatus (USBD_HandleTypeDef *pdev)
{
  return (pdev->dev_state);
}

/**
  * @}
  */ 

/**
  * @}
  */ 

/**
  * @}
  */ 

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
