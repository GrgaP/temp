#include "main.h"
#include "svd_gui.h"

static const GUI_WIDGET_CREATE_INFO _aDialog[] = {
  { WINDOW_CreateIndirect, "start",       ID_WINDOW_1,               0,  0,   240, 320, 0               },
 // { TEXT_CreateIndirect,   "Main screen", 0,               80, 5,   100, 20,  TEXT_CF_HCENTER },
  { BUTTON_CreateIndirect, "Scale",  GUI_ID_BUTTON1,  10, 10, 100, 70,  0               },
  { BUTTON_CreateIndirect, "Graph",  GUI_ID_BUTTON2,  10, 85, 100, 70,  0               },
  { BUTTON_CreateIndirect, "Menu",  GUI_ID_BUTTON3,  10, 160, 100, 70,  0               },
  { BUTTON_CreateIndirect, "Close",  GUI_ID_BUTTON0,  10, 235, 100, 70,  0               }
};

/*********************************************************************
*
*       _cbDialog
*
* Function description
*   Callback routine of the dialog
*/

static void _cbDialog(WM_MESSAGE *pMsg) {
  int NCode;
  int Id;
  
  int user_data;
  user_data = pMsg->Data.v;
    switch(user_data){
    case MSG_ENTER://go to scale screen;
        GUI_EndDialog(pMsg->hWin, 1);
        Scale_screen_startup(WM_HBKWIN,16,0);
    break;
    case MSG_ESC://ignore;
    break;
    case MSG_RIGHT://ignore;
    break;
    case MSG_LEFT://ignore;
    break;
    case MSG_UP://move focus to up item
    break;
    case MSG_DOWN://move focus to down item
    break;
    default:
    break;  
    };
  switch (pMsg->MsgId) {
  case WM_PAINT:
    //На этапе прорисовки сообщаем фоновому окну ID создаваемого;
    GUI_SetBkColor(GUI_RED);
    GUI_Clear();
    GUI_DispString("Main screen");
    break;
  case WM_NOTIFY_PARENT:
    Id    = WM_GetId(pMsg->hWinSrc);    // Id of widget
    NCode = pMsg->Data.v;               // Notification code
    switch (NCode) {
    case WM_KEY:
      GUI_EndDialog(pMsg->hWin, 1);
    break;   
    case WM_NOTIFICATION_RELEASED:      // React only if released
      switch (Id) {
      case GUI_ID_BUTTON1:
        GUI_EndDialog(pMsg->hWin, 1);
        Scale_screen_startup(WM_HBKWIN,16,0);
        break;   
      case GUI_ID_BUTTON2:
        GUI_EndDialog(pMsg->hWin, 1);
        Graph_screen_startup(WM_HBKWIN,16,0);
        break;
      case GUI_ID_BUTTON3:
        GUI_EndDialog(pMsg->hWin, 1);
        Menu_screen_startup(WM_HBKWIN,16,0);
        break;   
      case GUI_ID_BUTTON0:        
        GUI_EndDialog(pMsg->hWin, 1);
        break;
        default:
        break;
        };
     break;
      }
  default:
    WM_DefaultProc(pMsg);
  break;
  }
}

/**
  * @brief  Main screen window Starup
  * @param  hWin: pointer to the parent handle.
  * @param  xpos: X position 
  * @param  ypos: Y position
  * @retval None
  */
void Main_screen_startup(WM_HWIN hWin, uint16_t xpos, uint16_t ypos)
{  
  GUI_CreateDialogBox(_aDialog, GUI_COUNTOF(_aDialog), _cbDialog, hWin, xpos, ypos);
  
  //при запуске экрана надо изменить сообщение в очереди текущих режимов
}