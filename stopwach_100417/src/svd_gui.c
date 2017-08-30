#include "main.h"
#include "svd_gui.h"

//
// Recommended memory to run the sample with adequate performance
//
#define RECOMMENDED_MEMORY (1024L * 30)

#define SW_REFRESH_TMR_PERIOD 1//ms

/* custom message */

//Структура текущего сотояния интерфейса:  
/*typedef struct
{
  SCREEN_LIST_TypeDef  current_screen;//Текущий отображаемый экран      
  LAYER_NUM_TypeDef current_layer;//Слой текущего экрана
  KEY_ID_TypeDef last_key_id;//ID последней нажатой кнопки  
} SVD_GUI_StateTypeDef;*/

/* Ext variables */
//WM_HTIMER       hRefreshTimer;
/* Private variables ---------------------------------------------------------*/
static const GUI_WIDGET_CREATE_INFO _aDialog[] = 
{
  { WINDOW_CreateIndirect, "XXX", ID_WINDOW_0, 0, 192, 480, 80, 0, 0, 0 }, 
  { BUTTON_CreateIndirect, "Start", GUI_KEY_ENTER, BTN0_X_POS, BTN0_Y_POS, BTN_X_SIZE, BTN_Y_SIZE,  0, WM_CF_SHOW,  0 },
  { BUTTON_CreateIndirect, "Reset", GUI_KEY_ESCAPE, BTN1_X_POS, BTN0_Y_POS, BTN_X_SIZE, BTN_Y_SIZE,  0, WM_CF_SHOW,  0 },
  { BUTTON_CreateIndirect, "Stop", GUI_KEY_BACKSPACE, BTN2_X_POS, BTN0_Y_POS, BTN_X_SIZE, BTN_Y_SIZE,  0, WM_CF_SHOW,  0 },
  //{ BUTTON_CreateIndirect, "DN", GUI_KEY_DOWN, BTN3_X_POS, BTN_Y_SIZE+1, BTN_X_SIZE, BTN_Y_SIZE,  0, WM_CF_SHOW,  0 },
  //{ BUTTON_CreateIndirect, "Enter", GUI_KEY_ENTER, BTN0_X_POS, 0, BTN_X_SIZE, BTN_Y_SIZE,  0, WM_CF_SHOW,  0 },
  //{ BUTTON_CreateIndirect, "->", GUI_KEY_LEFT, BTN1_X_POS, BTN_Y_SIZE+1, BTN_X_SIZE, BTN_Y_SIZE,  0, WM_CF_SHOW,  0 },  
};

WM_HTIMER       hRefreshTimer;

void SVD_GUI_Exec(void);

uint8_t SVD_GUI_Init(void)
{
  /* Initialize the SDRAM */
  if(BSP_SDRAM_Init()!= SDRAM_OK){
    /* Return  error */
     return SVD_GUI_INIT_ERROR;
  };
  
 /* Initialize LEDs */
  BSP_LED_Init(LED1);

  /* Initialize the Touch screen */
  if(BSP_TS_Init(XSIZE_PHYS_, YSIZE_PHYS_)!=TS_OK) {
     /* Return  error */
     return SVD_GUI_INIT_ERROR;
	}  
 
  /* Enable CRC to Unlock GUI */
 __HAL_RCC_CRC_CLK_ENABLE();
  
 /* Enable Back up SRAM */
__HAL_RCC_BKPSRAM_CLK_ENABLE();  

/* Init the STemWin GUI Library */

  /* Initialize GUI */
  if (GUI_Init() != 0) {
     /* Return  error */
     return SVD_GUI_INIT_ERROR;
	}
  
  if (GUI_ALLOC_GetNumFreeBytes() < RECOMMENDED_MEMORY) {
    GUI_ErrorOut("Not enough memory available."); 
    return SVD_GUI_INIT_ERROR;
  } 
    
  WM_MULTIBUF_Enable(1);
  
  WM_SetDesktopColor(GUI_BLACK);
  WM_SetCreateFlags(WM_CF_MEMDEV);
  
  GUI_SetLayerVisEx (1, 0);
  GUI_SelectLayer(0);
  
  return SVD_GUI_INIT_OK;
};

/*
  * @brief  Callback routine of desktop window.
  * @param  pMsg: pointer to data structure of type WM_MESSAGE
  * @retval None
  */
static void _cbCBh(WM_MESSAGE * pMsg) 
{ 
  char* acText;
  
  int NCode;
  int Id;
  
 switch (pMsg->MsgId) 
  {
  case WM_PAINT:
    GUI_SetBkColor(GUI_BLACK);
    GUI_Clear();
    
    acText = sw_get_tim_fmt();
  break;
  case WM_NOTIFY_PARENT:
    NCode = pMsg->Data.v;
    switch (NCode)
    { case WM_NOTIFICATION_RELEASED:
      Id    = WM_GetId(pMsg->hWinSrc);   // Id of widget
      switch (Id)
      {
      case GUI_KEY_ENTER:
          sw_start();
        break;
      case GUI_KEY_BACKSPACE:
          sw_stop();
        break;
      case GUI_KEY_ESCAPE:
          sw_reset();
        break;    
      };
      break;
    };
    //NCode = pMsg->Data.v;               // Notification code
  break;
  case WM_TIMER://событие - сробатывание таймера обновления
    GUI_DispStringHCenterAt( acText, 240, 50);
    WM_RestartTimer(pMsg->Data.v, SW_REFRESH_TMR_PERIOD);//перезапуск таймера
  break;
  default:
    WM_DefaultProc(pMsg);
  break;  
  };  
}

/**
  * @brief 
  * @param  None
  * @retval None
  */   
void SVD_GUI_menu_create(void)
{
  
}

/**
  * @brief 
  * @param  None
  * @retval None
  */
void SVD_GUI_Exec(void)
{
  GUI_Exec();
};

/*static void SVD_GUI_screen0(void)
{ 
  
  //Рисуем белый прямоугольник
 /GUI_SetColor(GUI_WHITE);
  GUI_FillRect(MAIN_FRAME_X_POS, MAIN_FRAME_Y_POS, (MAIN_FRAME_X_SIZE+MAIN_FRAME_X_POS), (MAIN_FRAME_Y_SIZE+MAIN_FRAME_Y_POS));
  GUI_SetFont(&GUI_Font8x16);
  GUI_SetColor(GUI_BLACK);
  GUI_SetTextMode(GUI_TM_TRANS);
  GUI_DispStringHCenterAt("Screen 0" , 136, 160);
  GUI_DispStringHCenterAt("Start screen" , 136, 178);
};*/

/**
  * @brief  Read the coordinate of the point touched and assign their
  *         value to the variables u32_TSXCoordinate and u32_TSYCoordinate
  * @param  None
  * @retval None
  */
void SVD_GUI_TouchUpdate(void)
{
  static GUI_PID_STATE TS_State = {0, 0, 0, 0};
  __IO TS_StateTypeDef  ts;
  uint16_t xDiff, yDiff;  

  BSP_TS_GetState((TS_StateTypeDef *)&ts);

  if((ts.touchX[0] >= LCD_GetXSize()) ||(ts.touchY[0] >= LCD_GetYSize()) ) 
  {
    ts.touchX[0] = 0;
    ts.touchY[0] = 0;
    ts.touchDetected =0;
  }

  xDiff = (TS_State.x > ts.touchX[0]) ? (TS_State.x - ts.touchX[0]) : (ts.touchX[0] - TS_State.x);
  yDiff = (TS_State.y > ts.touchY[0]) ? (TS_State.y - ts.touchY[0]) : (ts.touchY[0] - TS_State.y);
  
  
  if((TS_State.Pressed != ts.touchDetected ) ||
     (xDiff > 30 )||
      (yDiff > 30))
  {
    TS_State.Pressed = ts.touchDetected;
    TS_State.Layer = 0;
    if(ts.touchDetected) 
    {
      
      TS_State.x = ts.touchX[0];
      TS_State.y = ts.touchY[0];
      GUI_TOUCH_StoreStateEx(&TS_State);
    }
    else
    {
      GUI_TOUCH_StoreStateEx(&TS_State);
      TS_State.x = 0;
      TS_State.y = 0;
    }
  }
}



void SVD_GUI_back_ctrl_startup(void)
{
  //WM_HWIN hSW_Dlg;
  
  sw_module_init();
  
  WM_SetCallback(WM_HBKWIN, &_cbBk);
  
  //hSW_Dlg = 
  GUI_CreateDialogBox(_aDialog, GUI_COUNTOF(_aDialog), &_cbCBh, WM_HBKWIN, 0, 0);
  
  //Main_screen_startup(WM_HBKWIN,16,0);//main_scr_startup
  
  hRefreshTimer = WM_CreateTimer(WM_HBKWIN, 0, SW_REFRESH_TMR_PERIOD, 0);//заводим периодический таймер для обновления окна
  //при запуске экрана надо изменить сообщение в очереди текущих режимов
}