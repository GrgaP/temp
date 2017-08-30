/**
 ******************************************************************************
 * @file    security.c
 * @author  a.bulatov
 * @version V1.1.0
 * @date    08.07.2016
 * @brief       Sorce secyriti file
 *              $Id$
 ****************************************************************************** 
 */ 
 
#include "security.h"
#include "virt_num_keyboard.h"
#include "passw_text_edit.h"

#if defined(STM32F429_439xx)
  #include "lcd_fmc.h"
#elif defined(STM32F746xx)
  #include "lcd_ltdc.h"
#endif

#include "graph.h"
#include "gfx.h"
#include "delay.h"
   
#define SEC_FONT_DEF "MagistralCTT Regular 14"
static font_t font;

#define INT_GAP_PX 2 /* internal clearence */
#define EXT_GAP_PX 4 /* external clearance */

#define setBgClr(x,y,w,h)  gdispFillStringBox (x, y, 0, 0, " ", font, White, gdispGetPixelColor (x+w/2, y+h/2), justifyCenter)
#define retBgClr()         gdispFillStringBox (0, 0, 0, 0, " ", font, White, Black, justifyCenter)

static uint32_t dwnd = 0;

/* Set default font*/
void vSetInfoWindowFontDef(void)
{
  font = gdispOpenFont(SEC_FONT_DEF);
}

/* Show window thith label */
void vShowWindowLb(uint16_t x, uint16_t y, uint16_t w, char* str , uint16_t h,  uint16_t clrTxt, uint16_t clrWnd, float fAlpha)
{
  LCDBUFFER_Alfa_Rectangle (x, y, x+w, y+h, clrWnd, fAlpha);
  LCDBUFFER_Alfa_Rectangle (x+EXT_GAP_PX, y+EXT_GAP_PX, x+w-EXT_GAP_PX, y+h-EXT_GAP_PX, clrWnd, fAlpha);
  setBgClr(x,y,w,h);
  gdispDrawStringBox( x+EXT_GAP_PX, y+EXT_GAP_PX, w-EXT_GAP_PX, h-EXT_GAP_PX, str, font, clrTxt, justifyCenter);
  retBgClr();
}

void vSecurityInit(void)
{
  vSetInfoWindowFontDef();
  
  vVirtNumKeybCreate(10,10,100,100);
  vVirtNumKeybSetDef();

  vCreatePswTextObj(10, 150,150,80,0);
  vPswTextObjSetDef();

  /* Link text edit and keyboard */
  vPswTxtEdSetString(vVirtNumKeyGetBuf());
}

void vSecurityEnter(void)
{
  vSecurityInit();
  vPswTextEdRefreshKey();
  dwnd = 0;
  vClearBufInput();
  DisplayMode=DisplayMode_Security;
}

void vSecurityDisplay(void)
{
  LCDBUFFER_Clear();
  //vMenuUpdate();
  
  vVirtNumKeybAvtomat(SETPARAMS.iKey);
  vVirtNumKeybRedraw();
  vPswTxtEdRender();
  
  static uint16_t pass = 0;
  
  pass = usPswTextObjGetPassw();
  if (pass != 0)
    vSecurityExit();
  
}

static uint8_t ucMenuPos = 0;
static uint8_t* pucMenuPos = 0;
static uint32_t* pMenuCur = 0;
static uint32_t MenuCur = 0;

void vSendSecurityMes(uint8_t* AddrMenuPos, uint8_t ValMenuPos, uint32_t* AddrMenuCur, uint32_t ValMenuCur)
{
  pucMenuPos = AddrMenuPos;
  ucMenuPos = ValMenuPos;
  pMenuCur = AddrMenuCur;
  MenuCur = ValMenuCur;
}

void vSecurityExit(void)
{
    uint16_t pass = usPswTextObjGetPassw();
    
    if (dwnd == 0) dwnd = GetTickCount();
    
    if (pass == vPswTextEdGetKey())
    {
      vShowWindowLb((120-90),  120, 180, "Password: OK" , 40,  White, GREEN, 0.5);
    }
    else
    {
      vShowWindowLb((120-90),  120, 180, "Password: FAIL" , 40,  White, RED, 0.5);
      ReturnMenu();
    }
    
    if (WaitUntil((TickCountType*)&dwnd, 1000) == READY)
    {
      DisplayMode=DisplayMode_Menu;
    }

}

void ReturnMenu(void)
{
  *pucMenuPos = ucMenuPos;
  *pMenuCur = (uint32_t)MenuCur;
}




