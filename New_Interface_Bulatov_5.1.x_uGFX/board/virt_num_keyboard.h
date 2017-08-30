/**
 ******************************************************************************
 * @file    virt_num_keyboard.h
 * @author  a.bulatov
 * @version V1.1.0
 * @date    07.07.2016
 * @brief       Header file for virtual numeric keyboard used uGFX.
 *              $Id$
 ****************************************************************************** 
 */ 

#ifndef __VIRT_NUM_KEYBOARD_H___
#define __VIRT_NUM_KEYBOARD_H___

#include <stdint.h>
#include "gfx.h"
#include "keyboard_tmr.h"
/**
 * @defgroup    virt_num_keyboard Virtual numeric keyboard.
 * @brief       Module for virtual keyboard 
 * @{ 
 */

#define K_U     KEY_U
#define K_D     KEY_D
#define K_L     KEY_L
#define K_R     KEY_R
#define K_RET     KEY_RET
#define K_ENT     KEY_ENT
   
typedef enum ButStateEnum
{
  UNPRESSED = 0,
  PRESSED = 1,
  SELECT = 2
} ButStateEnum_TypeDef;

#define BUT_DEF(b) {{b},UNPRESSED}

typedef struct ButKeyStr
{
  const char name[4];
  uint8_t state;
} ButKey_TypeDef;

typedef struct VirtNumKeybObjStr
{
  uint16_t usColorBgr;
  uint16_t usColorBut;
  uint16_t usColorSel;
  uint16_t usColorPressed;
  uint16_t usColorText;
  float fColorBqrAlpha;
  float fColorButAlpha;
  uint16_t x;
  uint16_t y;
  uint16_t width;
  uint16_t height;
  uint8_t* pfontName;
  uint8_t ucSelBut;
  int8_t cPressedBut;
} VirtNumKeybObj_TypeDef;

void vVirtNumKeybCreate(uint16_t x, uint16_t y, uint16_t w, uint16_t h);
void vVirtNumKeybRedraw(void);
void vVirtNumKeybAvtomat(uint16_t key);
void vVirtNumKeybSetDef(void);
void vClearBufInput(void);

/**
 *@}
 */ 
 
#endif /* __VIRT_NUM_KEYBOARD_H_ */
 
