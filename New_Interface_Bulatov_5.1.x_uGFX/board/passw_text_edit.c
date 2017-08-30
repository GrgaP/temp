/**
 ******************************************************************************
 * @file    passw_text_edit.c
 * @author  a.bulatov
 * @version V1.1.0
 * @date    08.07.2016
 * @brief       Source code for passord text edit.
 *              $Id$
 ****************************************************************************** 
 */ 

/**
 * @addtogroup  passw_text_edit 
 * @{ 
 */

#include "string.h"
#include "passw_text_edit.h"

#define setBgClr(x,y,w,h)  gdispFillStringBox (x, y, 0, 0, " ", font, White, gdispGetPixelColor (x+w/2, y+h/2), justifyCenter)
#define retBgClr()         gdispFillStringBox (0, 0, 0, 0, " ", font, White, Black, justifyCenter)

#define INT_GAP_PX 2 /* internal clearence */
#define EXT_GAP_PX 4 /* external clearance */

static PswTxtEdObj_TypeDef xPswTxtEdObj = PSW_TXT_ED_OBJ_DEF;
static PswTxtEdObj_TypeDef xPswTxtEdObjDef = PSW_TXT_ED_OBJ_DEF;

font_t font;

/* Craeate object */
void vCreatePswTextObj(uint16_t x , uint16_t y, uint16_t w, uint16_t h, char* str)
{
  xPswTxtEdObj.x = x;
  xPswTxtEdObj.y = y;
  xPswTxtEdObj.w = w;
  xPswTxtEdObj.h = h;
  xPswTxtEdObj.pString = str;
  xPswTxtEdObj.usRandomNumber = rand(); 
  
  font = gdispOpenFont((char*)xPswTxtEdObj.pFontName);
  memset(xPswTxtEdObj.lastPassword, 0, 32);
}

/* Set string to show */
void vPswTxtEdSetString( char *str)
{
  xPswTxtEdObj.pString = str;
}

void vPswTxtEdRender(void)
{
  uint16_t lsx = xPswTxtEdObj.w-2*EXT_GAP_PX;
  uint16_t lsy = (xPswTxtEdObj.h-2*EXT_GAP_PX-2*INT_GAP_PX)/3;
  
  /* Calculate second string */
  char srtKey[64]; 
  sprintf(srtKey, xPswTxtEdObj.pKeyLabel, xPswTxtEdObj.usRandomNumber);
  
  uint16_t tx, ty;
  /* calc x, y */
  tx = xPswTxtEdObj.x;
  ty = xPswTxtEdObj.y;
  
  /* Show window */
  LCDBUFFER_Alfa_Rectangle (tx, ty, tx+xPswTxtEdObj.w, ty+2*EXT_GAP_PX+2*INT_GAP_PX+3*lsy, xPswTxtEdObj.usClrBgr, xPswTxtEdObj.fAlphaWindow);
  //LCDBUFFER_Alfa_Rectangle (tx+EXT_GAP_PX, ty+EXT_GAP_PX, tx+xPswTxtEdObj.w-EXT_GAP_PX, ty+2*EXT_GAP_PX+2*INT_GAP_PX+3*lsy-EXT_GAP_PX, xPswTxtEdObj.usClrBgr, xPswTxtEdObj.fAlphaWindow);

  /* Show passwort label */
  
  /* calc x, y */
  tx = xPswTxtEdObj.x+EXT_GAP_PX;
  ty = xPswTxtEdObj.y+EXT_GAP_PX;
  
  LCDBUFFER_Alfa_Rectangle (tx, ty, tx+lsx, ty+lsy, xPswTxtEdObj.usClrBgr, xPswTxtEdObj.fAlphaWindow);
  setBgClr(tx, ty, lsx, lsy);
  gdispDrawStringBox( tx, ty, lsx, lsy, (char*)xPswTxtEdObj.pPasswLabel, font, xPswTxtEdObj.usCltTextLabel, justifyCenter);
  retBgClr();
  
  /* Show Key Label */
  tx = xPswTxtEdObj.x+EXT_GAP_PX; /* don't change */
  ty = ty + lsy + INT_GAP_PX;
  
  LCDBUFFER_Alfa_Rectangle (tx, ty, tx+lsx, ty+lsy, xPswTxtEdObj.usClrBgr, xPswTxtEdObj.fAlphaWindow);
  setBgClr(tx, ty, lsx, lsy);
  gdispDrawStringBox( tx, ty, lsx, lsy, srtKey, font, xPswTxtEdObj.usCltTextLabel, justifyCenter);
  retBgClr();

  /* Show Key Label */
  tx = xPswTxtEdObj.x+EXT_GAP_PX; /* don't change */
  ty = ty + lsy + INT_GAP_PX;
  
  #define CR_DELAY_MS 250 
  static uint32_t ulLastTick;
  static uint8_t showCur = 0;
  char str[32];
  
   if ( (gfxSystemTicks() - ulLastTick) > CR_DELAY_MS )
  {
    ulLastTick = gfxSystemTicks(); 
    showCur = ~showCur;
  }
  
  /* Find end */
  char* ch = strchr(xPswTxtEdObj.pString, '\n');
  
  if (ch) 
  {
    /*copy password */
    *ch = 0;
    memset(xPswTxtEdObj.lastPassword, 0, 32);
    strncpy(xPswTxtEdObj.lastPassword, xPswTxtEdObj.pString, 32);
  }
  
  strncpy(str, xPswTxtEdObj.pString, 32);
  
  if (showCur == 0) 
  {
    strcat(str, "|");
  }
  else
  {
    strcat(str, ".");
  }
  
 
  LCDBUFFER_Alfa_Rectangle (tx, ty, tx+lsx, ty+lsy, xPswTxtEdObj.usClrTextEdit, xPswTxtEdObj.fAlphaTextEdit);
  setBgClr(tx, ty, lsx, lsy);
  gdispDrawStringBox( tx, ty, lsx, lsy, str, font, xPswTxtEdObj.usClrTextInput, justifyCenter);
  retBgClr();
  //xPswTxtEdObj.usRandomNumber = rand(); 
  
}

void vPswTextObjSetDef()
{
  xPswTxtEdObj = xPswTxtEdObjDef;
}

uint16_t usPswTextObjGetPassw()
{

int32_t passw;
passw = atol(xPswTxtEdObj.lastPassword);

return ((uint16_t)(passw)); 
}

void vPswTextEdClearPasswordString(void)
{
  memset(xPswTxtEdObj.lastPassword, 0, 32);
}


void vPswTextEdRefreshKey(void)
{
   xPswTxtEdObj.usRandomNumber = rand();
}

uint16_t vPswTextEdGetKey(void)
{
   return xPswTxtEdObj.usRandomNumber;
}






/**
 *@}
 */
 

