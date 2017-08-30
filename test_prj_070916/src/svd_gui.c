#include "main.h"
#include "svd_gui.h"

//
// Recommended memory to run the sample with adequate performance
//
#define RECOMMENDED_MEMORY (1024L * 30)

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
  { WINDOW_CreateIndirect, "", ID_WINDOW_0,                    0,    0,  272, 160, 0, 0x64, 0 }, 
  { BUTTON_CreateIndirect, "Esc", GUI_KEY_ESCAPE, BTN4_X_POS, 0, BTN_X_SIZE, BTN_Y_SIZE,  0, WM_CF_SHOW,  0 },
  { BUTTON_CreateIndirect, "<-", GUI_KEY_RIGHT, BTN5_X_POS, BTN_Y_SIZE+1, BTN_X_SIZE, BTN_Y_SIZE,  0, WM_CF_SHOW,  0 },
  { BUTTON_CreateIndirect, "UP", GUI_KEY_UP, BTN2_X_POS, 0, BTN_X_SIZE, BTN_Y_SIZE,  0, WM_CF_SHOW,  0 },
  { BUTTON_CreateIndirect, "DN", GUI_KEY_DOWN, BTN3_X_POS, BTN_Y_SIZE+1, BTN_X_SIZE, BTN_Y_SIZE,  0, WM_CF_SHOW,  0 },
  { BUTTON_CreateIndirect, "Enter", GUI_KEY_ENTER, BTN0_X_POS, 0, BTN_X_SIZE, BTN_Y_SIZE,  0, WM_CF_SHOW,  0 },
  { BUTTON_CreateIndirect, "->", GUI_KEY_LEFT, BTN1_X_POS, BTN_Y_SIZE+1, BTN_X_SIZE, BTN_Y_SIZE,  0, WM_CF_SHOW,  0 },  
};

void _crate_ctrl_btn(void);
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
  
  //GUI_SetBkColor(GUI_WHITE);
  //GUI_Clear();
  
  //ctrl_btns_create();
    
  return SVD_GUI_INIT_OK;
};

/*
  * @brief  Callback routine of desktop window.
  * @param  pMsg: pointer to data structure of type WM_MESSAGE
  * @retval None
  */
static void _cbCBh(WM_MESSAGE * pMsg) 
{ 
  int NCode;
  int Id;
  switch (pMsg->MsgId) 
  {
  case WM_PAINT:
    GUI_SetBkColor(GUI_BLACK);
    GUI_Clear();
    GUI_DispString("Background window");
  break;
  case WM_NOTIFY_PARENT:
    NCode = pMsg->Data.v;
    switch (NCode)
    { case WM_NOTIFICATION_RELEASED:
      Id    = WM_GetId(pMsg->hWinSrc);   // Id of widget
    switch (Id)
      {
      case GUI_KEY_ESCAPE:
        pMsg->Data.v = MSG_ESC;
        WM_BroadcastMessage(pMsg);
        break;
      case GUI_KEY_RIGHT:
        pMsg->Data.v = MSG_RIGHT;
        WM_BroadcastMessage(pMsg);
        break;
      case GUI_KEY_UP:
        pMsg->Data.v = MSG_UP;
        WM_BroadcastMessage(pMsg);
        break;
      case GUI_KEY_DOWN:
        pMsg->Data.v = MSG_DOWN;
        WM_BroadcastMessage(pMsg);
        break;
      case GUI_KEY_ENTER:
        pMsg->Data.v = MSG_ENTER;
        WM_BroadcastMessage(pMsg);
        break;
      case GUI_KEY_LEFT:
        pMsg->Data.v = MSG_LEFT;
        WM_BroadcastMessage(pMsg);
        break;     
      };
      break;
    };
    //NCode = pMsg->Data.v;               // Notification code
  break;
  default:
    WM_DefaultProc(pMsg);
  break;  
  };  
}

/*
  * @brief  Callback routine of desktop window.
  * @param  pMsg: pointer to data structure of type WM_MESSAGE
  * @retval None
  */
static void _cbBk(WM_MESSAGE * pMsg)
{

  switch (pMsg->MsgId) 
  {
  case WM_PAINT:
    GUI_SetBkColor(GUI_BLACK);
    GUI_Clear();
    GUI_DispString("Device is off");
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
  WM_SetCallback(WM_HBKWIN, &_cbBk);
  
  GUI_CreateDialogBox(_aDialog, GUI_COUNTOF(_aDialog), &_cbCBh, WM_HBKWIN, 0, 321);
  Main_screen_startup(WM_HBKWIN,16,0);//main_scr_startup
  
  //при запуске экрана надо изменить сообщение в очереди текущих режимов
}