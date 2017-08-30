#include "menu_calibration.h"
#include "main.h"

#include "menu.h"

TMenuItem* pxSwitchMenuItem(uint8_t ucExplSel);
void vMenuFloatS16IncDec (TMenuItem* item, u8 IncDec);

const char pcOn[]  = " ON";
const char pcOff[] = "OFF";

xMenuClb pxTntMenuClb[1];
xMenuClb pxExpMenuClb[EXPLOSIV_COUNT];

static uint8_t ucMenuClbPos = 0;
static eMCLBT_TypeDef eMenuType = eMCLBT_MAIN_MENU;
static uint8_t ucExplSel = 0;

xMenuClb pxMainMenuClb[EXPLOSIV_COUNT] = 
{
  {(void*)Explosive_Set[0].Name,   NULL, pxTntMenuClb,  NULL,                       eVT_ON_OFF, NULL                    },
  {(void*)Explosive_Set[1].Name,   NULL, &pxExpMenuClb[1], &Explosive_Set[1].usColor,  eVT_ON_OFF, &Explosive_Set[1].eOnOff },
  {(void*)Explosive_Set[2].Name,   NULL, &pxExpMenuClb[2], &Explosive_Set[2].usColor,  eVT_ON_OFF, &Explosive_Set[2].eOnOff },
  {(void*)Explosive_Set[3].Name,   NULL, &pxExpMenuClb[3], &Explosive_Set[3].usColor,  eVT_ON_OFF, &Explosive_Set[3].eOnOff },
  {(void*)Explosive_Set[4].Name,   NULL, &pxExpMenuClb[4], &Explosive_Set[4].usColor,  eVT_ON_OFF, &Explosive_Set[4].eOnOff },
  {(void*)Explosive_Set[5].Name,   NULL, &pxExpMenuClb[5], &Explosive_Set[5].usColor,  eVT_ON_OFF, &Explosive_Set[5].eOnOff },
};



#define EXPL_MENU_CNT 3
#define ALPHA_TR_MENU (0.75f)
#define ALPHA_TR_FONT (0.5f)
#define ALPHA_TR_MENUSEL (0.75f)

eClbZone_TypeDef eGetClbZone(void)
{
  switch (eMenuType)
  {
    case eMCLBT_MAIN_MENU:
      return  eCLB_ZONE_NONE;
      break;
    case eMCLBT_TNT_MENU:
      return eCLB_ZONE_TNT;
      break;
    case eMCLBT_EXP_MENU:
      return (eClbZone_TypeDef)ucExplSel;
      break;
  default: return eCLB_ZONE_NONE;
  }
}

void vMenuClbAutomat(void)
{
  switch (eMenuType)
  {
    case  eMCLBT_MAIN_MENU:
      /* ENTER */
      if ((SETPARAMS.iKey==KEY_ENT) && Keyb_SpeedDelay())
      {
        if ((Explosive_Set[ucMenuClbPos].eOnOff != OFF) || (ucMenuClbPos==0))
        {
          eMenuType = eMCLBT_EXP_MENU;
          ucExplSel = ucMenuClbPos;
          ucMenuClbPos = 0;
        }
      }
      /* RETURN */
      if ((SETPARAMS.iKey==KEY_RET) && Keyb_SpeedDelay())
      {
        MenuState = MenuState_Idle;
	DisplayMode = DisplayMode_Spectrum;
      }
      /* DOWN */
      if ((SETPARAMS.iKey==KEY_D) && Keyb_SpeedDelay())
      {
      if (++ucMenuClbPos >= EXPLOSIV_COUNT) ucMenuClbPos = 0;
      }
      /* UP */
      if ((SETPARAMS.iKey==KEY_U) && Keyb_SpeedDelay())
      {
        if (ucMenuClbPos-- == 0) ucMenuClbPos = EXPLOSIV_COUNT-1;
      }
      /* LEFT */
      if ((SETPARAMS.iKey==KEY_L) && Keyb_SpeedDelay())	
      {
        /* Заплатка для TNT */
        Explosive_Set[ucMenuClbPos].eOnOff = OFF;
      }
      /* RIGHT */
      if ((SETPARAMS.iKey==KEY_R) && Keyb_SpeedDelay())	
      {
        Explosive_Set[ucMenuClbPos].eOnOff = ON;
      }
    break;
    case eMCLBT_TNT_MENU:
      /* ENTER */
      if ((SETPARAMS.iKey==KEY_ENT) && Keyb_SpeedDelay())
      {
      }
      /* RETURN */
      if ((SETPARAMS.iKey==KEY_RET) && Keyb_SpeedDelay())
      {
        eMenuType = eMCLBT_MAIN_MENU;
        ucMenuClbPos = ucExplSel;
        ucExplSel = 0;
      }
      /* DOWN */
      if ((SETPARAMS.iKey==KEY_D) && Keyb_SpeedDelay())
      {
        if (++ucMenuClbPos >= EXPL_MENU_CNT) ucMenuClbPos = 0;
      }
      /* UP */
      if ((SETPARAMS.iKey==KEY_U) && Keyb_SpeedDelay())
      {
        if (ucMenuClbPos-- == 0) ucMenuClbPos = EXPL_MENU_CNT-1;
      }
      /* LEFT */
      if ((SETPARAMS.iKey==KEY_L) && Keyb_SpeedDelay())	
      {
      }
      /* RIGHT */
      if ((SETPARAMS.iKey==KEY_R) && Keyb_SpeedDelay())	
      {
      }
    break;
    case eMCLBT_EXP_MENU:
      /* ENTER */
      if ((SETPARAMS.iKey==KEY_ENT) && Keyb_SpeedDelay())
      {
      }
      /* RETURN */
      if ((SETPARAMS.iKey==KEY_RET) && Keyb_SpeedDelay())
      {
        eMenuType = eMCLBT_MAIN_MENU;
        ucMenuClbPos = ucExplSel;
        ucExplSel = 0;
      }
      /* DOWN */
      if ((SETPARAMS.iKey==KEY_D) && Keyb_SpeedDelay())
      {
        if (++ucMenuClbPos >= EXPL_MENU_CNT) ucMenuClbPos = 0;
      }
      /* UP */
      if ((SETPARAMS.iKey==KEY_U) && Keyb_SpeedDelay())
      {
        if (ucMenuClbPos-- == 0) ucMenuClbPos = EXPL_MENU_CNT-1;
      }
      /* LEFT */
      if ((SETPARAMS.iKey==KEY_L) && Keyb_SpeedDelay())	
      {
       TMenuItem* pxExpl = pxSwitchMenuItem(ucExplSel);
       vMenuFloatS16IncDec (pxExpl+ucMenuClbPos, 0);
      }
      /* RIGHT */
      if ((SETPARAMS.iKey==KEY_R) && Keyb_SpeedDelay())	
      {
       TMenuItem* pxExpl = pxSwitchMenuItem(ucExplSel);
       vMenuFloatS16IncDec (pxExpl+ucMenuClbPos, 1);
      }
    break;
    default:;
  };
};


void vMenuFloatS16IncDec (TMenuItem* item, u8 IncDec)
{
	Float_s16_IncDec (IncDec, ((item)->int_curr), (s16*)&((item)->int_from), (s16*)&((item)->int_to), (float*)&((item)->float_from), (float*)&((item)->float_to), (float*)&((item)->float_step));
}

float fMenuReadS16 (TMenuItem* item)
{
	float f_curr = (((((item)->float_to)-((item)->float_from))/(((item)->int_to)-((item)->int_from)))   *    (*((item)->int_curr)-((item)->int_from)))   +   ((item)->float_from);
	return f_curr;
}

TMenuItem* pxSwitchMenuItem(uint8_t ucExplSel)
{
    TMenuItem*  pvExplMenu = NULL;
    switch (ucExplSel)
    {
       case 0:
        pvExplMenu  = pExplMenuTnt;
        break;
      case 1:
        pvExplMenu  = pExplMenu1;
        break;
      case 2:
        pvExplMenu  = pExplMenu2;
        break;
      case 3:
        pvExplMenu  = pExplMenu3;
        break;
      case 4:
        pvExplMenu  = pExplMenu4;
        break;
      case 5:
        pvExplMenu  = pExplMenu5;
        break;
      default:
      ;
    }  
    return pvExplMenu;
}  
void vMenuClbShow(uint16_t x, uint16_t y)
{
  /* Local defienition for font 5x8 1pix 2piy */
  #undef W
  #undef H
  #define W (13*(6)+2*5)
  #define H (10*(EXPLOSIV_COUNT+3)+2*5) 
  
  #define line(num) (5+(num)*10+y)
  #define pos(num)  (5+(num)*6+x)
  
  if (eMenuType == eMCLBT_MAIN_MENU)
  {
 
    //LCDBUFFER_Rectangle (x+2, y+2, x+(W-1)-3, y+(H-1)-3, WHITE, 0);
    LCDBUFFER_Alfa_Rectangle (x, y, x+(W-1), y+(H-1), WHITE,  ALPHA_TR_MENU);
 
    LCDBUFFER_Alfa_Rectangle (x, line(ucMenuClbPos+3)-1, (x+(W-1)), (line(ucMenuClbPos+3)+8), WHITE,  ALPHA_TR_MENUSEL);
  
    LCDBUFFER_Str_ShadowAlpha ("CALIBRATION", pos(1), line(0),  WHITE, 1, 1, ALPHA_TR_FONT);
    LCDBUFFER_Str_ShadowAlpha ("MENU", pos(4), line(1),  WHITE, 1, 1, ALPHA_TR_FONT);
    LCDBUFFER_Str_ShadowAlpha ("-------------", pos(0), line(2),  WHITE, 1, 1, ALPHA_TR_FONT);

    LCDBUFFER_Str_ShadowAlpha(pxMainMenuClb[0].pcTitle, pos(0), line(3), *(pxMainMenuClb[0].pusMenuColor), 1, 1, ALPHA_TR_FONT); 
    LCDBUFFER_Str_ShadowAlpha((void*)pcOn, pos(10), line(3), *(pxMainMenuClb[0].pusMenuColor), 1, 1, ALPHA_TR_FONT); 

    for ( uint32_t i = 1; i < EXPLOSIV_COUNT; i++ )
    {
      LCDBUFFER_Str_ShadowAlpha(pxMainMenuClb[i].pcTitle, pos(0), line(3+i), *(pxMainMenuClb[i].pusMenuColor), 1, 1, ALPHA_TR_FONT); 
      if ( Explosive_Set[i].eOnOff == OFF ) 
        LCDBUFFER_Str_ShadowAlpha((void*)pcOff, pos(10), line(3+i), *(pxMainMenuClb[i].pusMenuColor), 1, 1, ALPHA_TR_FONT);
      else
        LCDBUFFER_Str_ShadowAlpha((void*)pcOn, pos(10), line(3+i), *(pxMainMenuClb[i].pusMenuColor), 1, 1, ALPHA_TR_FONT);
    }
  }
  if (eMenuType == eMCLBT_EXP_MENU)
  {
    LCDBUFFER_Alfa_Rectangle (x, y, x+(W-1), y+(H-1), Explosive_Set[ucExplSel].usColor,  ALPHA_TR_MENU);
    LCDBUFFER_Alfa_Rectangle (x, line(ucMenuClbPos+3)-1, (x+(W-1)), (line(ucMenuClbPos+3)+8), Explosive_Set[ucExplSel].usColor,  ALPHA_TR_MENUSEL);

    LCDBUFFER_Str_ShadowAlpha ("CALIBRATION", pos(1), line(0),  Explosive_Set[ucExplSel].usColor, 1, 1, ALPHA_TR_FONT);
    LCDBUFFER_Str_ShadowAlpha ((void*)Explosive_Set[ucExplSel].Name, pos(4), line(1),  Explosive_Set[ucExplSel].usColor, 1, 1, ALPHA_TR_FONT);
    LCDBUFFER_Str_ShadowAlpha ("-------------", pos(0), line(2),  Explosive_Set[ucExplSel].usColor, 1, 1, ALPHA_TR_FONT);
    
    LCDBUFFER_Str_ShadowAlpha ("WND", pos(0), line(3),Explosive_Set[ucExplSel].usColor, 1, 1, ALPHA_TR_FONT);
    LCDBUFFER_Str_ShadowAlpha ("OFFSET", pos(0), line(4),Explosive_Set[ucExplSel].usColor, 1, 1, ALPHA_TR_FONT);
    LCDBUFFER_Str_ShadowAlpha ("LEVEL", pos(0), line(5),Explosive_Set[ucExplSel].usColor, 1, 1, ALPHA_TR_FONT);
    
    
    TMenuItem* pvExplMenu = pxSwitchMenuItem(ucExplSel);
    
    /*wnd*/
    char string_buf[10];
    sprintf(string_buf, "%5.1f", fMenuReadS16 ( pvExplMenu+0)); 
    sprintf(string_buf, "%s%s", string_buf, "%");                   
    LCDBUFFER_Str_ShadowAlpha (string_buf, pos(7), line(3),Explosive_Set[ucExplSel].usColor, 1, 1, ALPHA_TR_FONT);
    
    /*offset*/
    sprintf(string_buf, "%5.1f", fMenuReadS16 (pvExplMenu+1)); 
    sprintf(string_buf, "%s%s", string_buf, "%");
    LCDBUFFER_Str_ShadowAlpha (string_buf, pos(7), line(4),Explosive_Set[ucExplSel].usColor, 1, 1, ALPHA_TR_FONT);
    
    /*level*/
    sprintf(string_buf, "%5.1f", (100.f * Explosive_Set[ucExplSel].usLevelAlarm/32767.f));
    sprintf(string_buf, "%s%s", string_buf, "%");
    LCDBUFFER_Str_ShadowAlpha (string_buf, pos(7), line(5),Explosive_Set[ucExplSel].usColor, 1, 1, ALPHA_TR_FONT);
  }
    

  /* undef local defenition */
  #undef W
  #undef H  
}


