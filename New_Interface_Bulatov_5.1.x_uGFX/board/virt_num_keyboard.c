 /**
 ******************************************************************************
 * @file    virt_num_keyboard.c
 * @author  a.bulatov
 * @version V1.1.0
 * @date    07.07.2016
 * @brief       Sorce code for virtual numeric keyboard used uGFX.
 *              $Id$
 ****************************************************************************** 
 */ 
 
/**
 * @addtogroup  virt_num_keyboard Virtual numeric keyboard. 
 * @brief       Module for virtual keyboard.
 * @{ 
 */

#include "virt_num_keyboard.h"

   
#define FONT_DEF "MagistralCTT Regular 16"
#define CN 3 /* column number */
#define RN 4 /* row number */
#define INT_GAP_PX 2 /* internal clearence */
#define EXT_GAP_PX 4 /* external clearance */

#define VIRT_NUM_KEYB_DEF  {                                             \
                              LIGHTYELLOW,                                   \
                              LIGHTYELLOW,                                   \
                              LIGHTBLUE,                                     \
                              DARK_RED,                                      \
                              White,           \
                              0.8,                                      \
                              0.8,                                      \
                              (120-50),                                        \
                              175,                                        \
                              100,                                      \
                              100,                                      \
                              FONT_DEF,                                 \
                              10 ,                                       \
                              -1                                       \
                          }



static font_t pxButFont = 0;

#define bs 10
static struct buf
{
 char ucData[bs+1] ;
 int8_t cPnt ;
} xBufInput;

void vClearBufInput(void)
{
  memset( xBufInput.ucData, 0, bs+1);
  xBufInput.cPnt = 0;
}


void vParsSimb(uint8_t sel);

/* Create state obvject */
static VirtNumKeybObj_TypeDef xVirtNumKeybObj = VIRT_NUM_KEYB_DEF;
static VirtNumKeybObj_TypeDef xVirtNumKeybObjDef = VIRT_NUM_KEYB_DEF;

ButKey_TypeDef xButKey[12] = {
   BUT_DEF("0"),
   BUT_DEF("1"),
   BUT_DEF("2"),
   BUT_DEF("3"),
   BUT_DEF("4"),
   BUT_DEF("5"),
   BUT_DEF("6"),
   BUT_DEF("7"),
   BUT_DEF("8"),
   BUT_DEF("9"),
   BUT_DEF("Del"),
   BUT_DEF("Ent"),
};

void vVirtNumKeybSetFont(char* font )
{
  pxButFont = gdispOpenFont(font);
}

void vVirtNumKeybRender(void)
{
  #define sx xVirtNumKeybObj.x
  #define sy xVirtNumKeybObj.y
  #define sw xVirtNumKeybObj.width
  #define sh xVirtNumKeybObj.height
  
  /* Button size calculate */
  uint16_t bxs = (xVirtNumKeybObj.width-(CN-1)*INT_GAP_PX-(CN-1)*EXT_GAP_PX)/CN;
  uint16_t bys = (xVirtNumKeybObj.height-(RN-1)*INT_GAP_PX-(RN-1)*EXT_GAP_PX)/RN;
  
  /* Place fill Keyboard region no correct div */
  //LCDBUFFER_Alfa_Rectangle (sx, sy, sx+sw, sx+sh, xVirtNumKeybObj.usColorBgr, xVirtNumKeybObj.fColorBqrAlpha);
  
  /* Place fill keyboard regionthis correct div */
  LCDBUFFER_Alfa_Rectangle (sx, sy, sx+(2*EXT_GAP_PX+CN*bxs+(CN-1)*INT_GAP_PX), sy+(2*EXT_GAP_PX+RN*bys+(RN-1)*INT_GAP_PX), xVirtNumKeybObj.usColorBgr, xVirtNumKeybObj.fColorBqrAlpha);
  
  uint8_t by = sy+EXT_GAP_PX;
  uint8_t bx = sx+EXT_GAP_PX;
  
  for (uint8_t r = 0; r < RN; r++)
  {  
    by = sy+EXT_GAP_PX + ((bys+INT_GAP_PX)*r) ;
    bx = sx+EXT_GAP_PX;
    for (uint8_t c = 0; c < CN ;c++)
    {
       bx = sx+EXT_GAP_PX+((bxs+INT_GAP_PX)*c);
       
       /* Draw button */
       uint16_t clr;
       if       ((r*CN+c) == xVirtNumKeybObj.cPressedBut)  clr = xVirtNumKeybObj.usColorPressed; 
       else if  ((r*CN+c) == xVirtNumKeybObj.ucSelBut)     clr = xVirtNumKeybObj.usColorSel; 
       else                                             clr = xVirtNumKeybObj.usColorBut; 
       LCDBUFFER_Alfa_Rectangle (bx, by, bx+bxs, by+bys, clr, xVirtNumKeybObj.fColorButAlpha);
       
       /* Draw text */
       /* For write anti anal. */
       gdispFillStringBox (bx, by, 0, 0, " ", pxButFont, xVirtNumKeybObj.usColorText, gdispGetPixelColor (bx+bxs/2, by+bys/2), justifyCenter);
       gdispDrawStringBox( bx, by, bxs, bys, xButKey[r*CN+c].name, pxButFont, xVirtNumKeybObj.usColorText, justifyCenter);
       gdispFillStringBox (bx, by, 0, 0, " ", pxButFont, xVirtNumKeybObj.usColorText, Black, justifyCenter);
    }
  }

  #undef sx 
  #undef sy 
  #undef sw 
  #undef sh 
}

void vVirtNumKeybCreate(uint16_t x, uint16_t y, uint16_t w, uint16_t h)
{
  xVirtNumKeybObj.x = x;
  xVirtNumKeybObj.y = y;
  xVirtNumKeybObj.width = w;
  xVirtNumKeybObj.height = h;
  
  pxButFont = gdispOpenFont((char*)xVirtNumKeybObj.pfontName);
  xVirtNumKeybObj.ucSelBut = 10;
}

void vVirtNumKeybRedraw(void)
{
  vVirtNumKeybRender();
}

void vVirtNumKeybAvtomat(uint16_t key)
{
  int8_t r,c, sel;
  
  sel = xVirtNumKeybObj.ucSelBut;
  
  r = sel/CN;
  c = sel % CN;
    
  static uint16_t oldKeyStat = 0;
  
  if ((key != oldKeyStat)) 
  {
    /* NEW State */
    uint16_t kesCh = key ^ oldKeyStat;
    uint16_t nState = kesCh & key; //
    
    oldKeyStat = key;
    if (nState) /* if press */
    {
      switch (kesCh)
      {        
        case K_ENT:
          /* send char to buf or delete */
          xVirtNumKeybObj.cPressedBut = sel;
          vParsSimb(sel);
        break;
        case K_RET:
          break;
        case K_U:
          r--;
          break;
        case K_D:
          r++;
        break;
        case K_L:
          c--;
        break;
        case K_R:
          c++;
        break;
        default:;
      }
    }
    else /* if realise */
    {
      switch (kesCh)
      {        
        case K_ENT:
          /* send char to buf or delete */
          xVirtNumKeybObj.cPressedBut = -1;
        break;
        case K_RET:
        break;
        case K_U:
        break;
        case K_D:
        break;
        case K_L:
        break;
        case K_R:
        break;
        default:;
      }
    }
  }
  if (r < 0) r = RN-1;
  if (c < 0) c = CN-1;
  
  if ( c > (CN-1)) c = 0;
  if ( r > (RN-1)) r = 0;
  
  sel = r*CN+c;
  xVirtNumKeybObj.ucSelBut = sel;
}

void vParsSimb(uint8_t sel)
{
  if (sel < 10)
  {
    xBufInput.ucData[xBufInput.cPnt++] = xButKey[sel].name[0];
  }
  else if (sel == 10 )
  {
    /* del */
    xBufInput.ucData[xBufInput.cPnt] = 0;
    xBufInput.cPnt--;
    xBufInput.ucData[xBufInput.cPnt] = 0;
  }
  else if (sel == 11)
  {
    xBufInput.ucData[xBufInput.cPnt] = '\n';
  }
  
  if (xBufInput.cPnt >= bs) {xBufInput.cPnt = bs-1;}
  if (xBufInput.cPnt < 0)   {xBufInput.cPnt = 0;}

}

void vVirtNumKeybSetDef(void)
{
  xVirtNumKeybObj = xVirtNumKeybObjDef;
}

char* vVirtNumKeyGetBuf(void)
{
  return xBufInput.ucData;
}
 


/**
 *@}
 */
 
