/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __SVD_GUI_H
#define __SVD_GUI_H

#include "GUI.h"
#include "WM.h"
/* GUI modules */
#include "button.h"
#include "DIALOG.h"

#include "svd_gui_start_screen.h"
#include "svd_gui_scale_mode.h"
#include "svd_gui_graph_mode.h"
#include "svd_gui_menu_mode.h"

#define MSG_ESC (WM_USER + 0)
#define MSG_LEFT (WM_USER + 1)
#define MSG_UP (WM_USER + 2)
#define MSG_DOWN (WM_USER + 3)
#define MSG_ENTER (WM_USER + 4)
#define MSG_RIGHT (WM_USER + 5)

/*Defines for LCD config---------------------------------------------------------*/
#define SCR_PORTRAIT 0
#define SCR_LANDSCAPE 1

#ifdef USE_STM32746G_DISCOVERY
  #define XSIZE_PHYS_ 272
  #define YSIZE_PHYS_ 480
  #define SCREEN_ORIENT SCR_LANDSCAPE
#endif
#ifdef DIMS
  //#define XSIZE_PHYS 240
  //#define YSIZE_PHYS 320
  #define SCREEN_ORIENT SCR_PORTRAIT  
#endif

/*Defines for MainFrame & ctrl Buttoms---------------------------------------------------------*/

#define MAIN_FRAME_Y_POS 0
#define MAIN_FRAME_X_POS 16

#define MAIN_FRAME_Y_SIZE 320
#define MAIN_FRAME_X_SIZE 240

/*----------------*/

#define BTN_Y_SIZE 79
#define BTN_X_SIZE 90

/*----------------*/

#define BTN0_Y_POS 322
#define BTN0_X_POS 182

/*----------------*/

#define BTN1_Y_POS 401
#define BTN1_X_POS 182

/*----------------*/

#define BTN2_Y_POS 322
#define BTN2_X_POS 91

/*----------------*/

#define BTN3_Y_POS 401
#define BTN3_X_POS 91

/*----------------*/

#define BTN4_Y_POS 322
#define BTN4_X_POS 0

/*----------------*/

#define BTN5_Y_POS 401
#define BTN5_X_POS 0

/*----------------*/

#define ID_WINDOW_0               (GUI_ID_USER + 0x00)
#define ID_WINDOW_1               (GUI_ID_USER + 0x01)
#define ID_WINDOW_2               (GUI_ID_USER + 0x02)
#define ID_WINDOW_3               (GUI_ID_USER + 0x03)
#define ID_WINDOW_4               (GUI_ID_USER + 0x04)
#define ID_GRAPH_0                (GUI_ID_USER + 0x05)     

/*Defines for SDRAM config-------------------------------------------------------*/
/*#ifdef USE_STM32746G_DISCOVERY
  #define SDRAM_START_ADDR 0xC0000000
#endif
#ifdef DIMS
  #define SDRAM_START_ADDR 0xD0000000
#endif*/

typedef enum 
{
  SVD_GUI_INIT_OK      = 0x00, /*!<  Ok */
  SVD_GUI_INIT_ERROR   = 0x01, /*!< Error */
  
}SVD_GUI_init_status_TypeDef;

/**
  * @brief init for used HW and STemWIN GUI
  * @param  None
  * @retval OK/ERROR
  */
uint8_t SVD_GUI_Init(void);

/**
  * @brief 
  * @param  None
  * @retval None
  */
void SVD_GUI_Exec(void);

/**
  * @brief  Read the coordinate of the point touched and assign their
  *         value to the variables u32_TSXCoordinate and u32_TSYCoordinate
  * @param  None
  * @retval None
  */
void SVD_GUI_TouchUpdate(void);

void SVD_GUI_back_ctrl_startup(void);

#ifdef __cplusplus
}
#endif

#endif /*__SVD_GUI_H */