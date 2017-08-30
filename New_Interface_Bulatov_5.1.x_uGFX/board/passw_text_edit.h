/**
 ******************************************************************************
 * @file    passw_text_edit.h
 * @author  a.bulatov
 * @version V1.1.0
 * @date    08.07.2016
 * @brief       Heder file for password text edit.
 *              $Id$
 ****************************************************************************** 
 */ 
 
#ifndef __PASSWORD_TEXT_EDIT_H_		
#define __PASSWORD_TEXT_EDIT_H_

/**
 * @defgroup    passw_text_edit Password text edit.
 * @brief       Realise text edit for input password.
 * @{ 
 */

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
   
#include "main.h"  

/**
 * @brief       Type of object password edit
 */
typedef struct PswTxtEdObjStruct
{
  uint16_t x;                   //!< position x
  uint16_t y;                   //!< position y
  uint16_t w;                   //!< window widh  
  uint16_t h;                   //!< window hight 
  uint8_t pPasswLabel[32];      //!< password label
  char pKeyLabel[32];           //!< key label
  uint16_t usClrBgr;            //!< window background color
  uint16_t usClrTextEdit;       //!< text input color
  uint16_t usClrTextInput;      
  uint16_t usCltTextLabel;
  float fAlphaWindow;
  float fAlphaTextEdit;
  char* pFontName;             //!< pointer to str font name
  char* pString;               //!< pointer string to view
  uint16_t usRandomNumber;     //!< for random number genberate
  char lastPassword[32];
} PswTxtEdObj_TypeDef;

 
#define PSW_FONT_DEF "MagistralCTT Regular 14"

#define PSW_TXT_ED_OBJ_DEF      \
{                               \
  (120-75),                            \
  25,                            \
  150,                          \
  80,                           \
  "Password",                   \
  "Key: %6d",                   \
  LIGHTBLUE,                    \
  WHITE,                        \
  BLACK,                        \
  WHITE,                        \
  0.6,                          \
  0.3,                          \
  {PSW_FONT_DEF},               \
  (char*)0,                     \
  RAND_MAX,                     \
  {0}                           \
}

void vCreatePswTextObj(uint16_t x , uint16_t y, uint16_t w, uint16_t h, char* str);
void vPswTextObjSetDef();        
void vPswTxtEdRender(void);
void vPswTextEdClearPasswordString(void);
void vPswTextEdRefreshKey(void);
uint16_t usPswTextObjGetPassw(void);
uint16_t vPswTextEdGetKey(void);



/**
 *@}
 */ 
#endif /* __PASSWORD_TEXT_EDIT_H_ */




