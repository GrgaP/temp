#include "main.h"
#include "svd_gui.h"

static const GUI_WIDGET_CREATE_INFO _aDialog[] = {
  { WINDOW_CreateIndirect, "",       ID_WINDOW_4,               0,  0,   240, 320, 0               },
  { TEXT_CreateIndirect,   "Menu screen", 0,               80, 5,   100, 20,  TEXT_CF_HCENTER },
};

/*********************************************************************
*
*       _cbDialog
*
* Function description
*   Callback routine of the dialog
*/
static void _cbDialog(WM_MESSAGE *pMsg) {

int user_data;
    user_data = pMsg->Data.v;
    switch(user_data){
    case MSG_ENTER://enter to current menu item;
    break;
    case MSG_ESC://go to start screen;
        GUI_EndDialog(pMsg->hWin, 1);
        Main_screen_startup(WM_HBKWIN,16,0);
    break;
    case MSG_RIGHT://ignore
    break;
    case MSG_LEFT:///ignore;
    break;
    case MSG_UP://Move_cursor_to_up;
    break;
    case MSG_DOWN://Move_cursor_to_down;
    break;
    default:
    break;  
    };
  
  
  switch (pMsg->MsgId) {
  case WM_PAINT:
    GUI_SetBkColor(GUI_BROWN);
    GUI_Clear();
	GUI_DispString("Menu screen");
    break;
  default:
    WM_DefaultProc(pMsg);
  }
}

/**
  * @brief  Menu screen window Starup
  * @param  hWin: pointer to the parent handle.
  * @param  xpos: X position 
  * @param  ypos: Y position
  * @retval None
  */
void Menu_screen_startup(WM_HWIN hWin, uint16_t xpos, uint16_t ypos)
{  
  GUI_CreateDialogBox(_aDialog, GUI_COUNTOF(_aDialog), _cbDialog, hWin, xpos, ypos);
  
  //при запуске экрана надо изменить сообщение в очереди текущих режимов
}