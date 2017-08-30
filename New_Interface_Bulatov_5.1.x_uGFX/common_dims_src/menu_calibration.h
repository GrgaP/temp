#ifndef __MENU_CALIBRATION_H		// Блокируем повторное включение этого модуля
#define __MENU_CALIBRATION_H

#include "stdint.h"

typedef enum eVAR_TYPE_EN
{
  eVT_ON_OFF,
  eVT_PR_OFFSET,
  eVT_PR_WND, 
} eVarType_TypeDef;

typedef enum eClbZone
{
  eCLB_ZONE_TNT  = 0,
  eCLB_ZONE_EXP1 = 1,
  eCLB_ZONE_EXP2 = 2,
  eCLB_ZONE_EXP3 = 3,
  eCLB_ZONE_EXP4 = 4,
  eCLB_ZONE_EXP5 = 5,
  eCLB_ZONE_NONE = -1
}
eClbZone_TypeDef;

typedef enum eMENU_CLB_TYPE
{
  eMCLBT_MAIN_MENU,
  eMCLBT_TNT_MENU,
  eMCLBT_EXP_MENU 
} eMCLBT_TypeDef;

typedef struct MenuClbStrctName
{
  char*                    pcTitle;
  struct MenuClbStrctName* pxMenuParent;
  struct MenuClbStrctName* pxMenuChild;
  uint16_t*                pusMenuColor;
  eVarType_TypeDef         eVarType;
  void*                    pVarCng;
} xMenuClb;





void vMenuClbInit(void);
void vMenuClbAutomat(void);
void vMenuClbShow(uint16_t x, uint16_t y);
eClbZone_TypeDef eGetClbZone(void);
#endif /* __MENU_CALIBRATION_H */