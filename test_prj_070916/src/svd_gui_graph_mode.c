#include "main.h"
#include "svd_gui.h"

#define MAX_VALUE 180
#define H_GPAPH_SCALE_SIZE 240
#define REFRESH_TMR_PERIOD 10

static GRAPH_DATA_Handle  _ahData[10]; // Array of handles for the GRAPH_DATA objects
static GRAPH_SCALE_Handle _hScaleV;   // Handle of vertical scale
static GRAPH_SCALE_Handle _hScaleH;   // Handle of horizontal scale

//��������� �� ������� � ������� ��� �����������;
static int16_t* graph_data[10]={NULL};

//������� ����
//����� �����
//������ �����

static GUI_COLOR _aColor[10] = {GUI_WHITE}; // Array of colors for the GRAPH_DATA objects

static const GUI_WIDGET_CREATE_INFO _aDialog[] = {
  { WINDOW_CreateIndirect, "",       ID_WINDOW_3,               0,  0,   240, 320, 0               },
  //{ TEXT_CreateIndirect,   "Graph screen", 0,               80, 5,   100, 20,  TEXT_CF_HCENTER },
  { GRAPH_CreateIndirect,   0, GUI_ID_GRAPH0, 0, 0, 240, 320, 0,  0},
};

WM_HTIMER       hRefreshTimer;

static void upd_graph(GRAPH_DATA_Handle graph_hnd);
void Graph_screen_exit(WM_HWIN hWin);

/*
 * @param pData - ��������� �� ������ � ������� �� 240 ���������;
 * @param color - �������� ���� ������� (��. ������� GUI_COLOR � STemWIN manual) 
 * @retval - 0x00 - ������/0x01 - ������ ���������
 */
uint8_t Graph_screen_add_data(int16_t* pData, GUI_COLOR color)
{
  uint8_t i;
  uint8_t retval = 0x00;  
  
  for(i=0; i<(sizeof(graph_data)/sizeof(graph_data[0])); i++)
  {
    if(graph_data[i] == NULL)//������� ������ ���������������� �������;
    {
      graph_data[i] = pData;//��������� ����� ������ ������ �������
      _aColor[i] = color;//��������� ����� ����
      retval = 0x01;
      break ;//������� �� �����
    }
    //else
      //���� �������� ������ ���������� ������������ �������� - ������ ������;
      
  }
  return retval;
}

/*
 * @param pData - ��������� �� ������ � ������� �� 240 ���������;
 * @param color - �������� ���� ������� (��. ������� GUI_COLOR � STemWIN manual) 
 * @param index - ����� ������� 
 * @retval - 0x00 - ������/0x01 - ������ ���������
 */
uint8_t Graph_screen_add_data_sel(int16_t* pData, GUI_COLOR color, uint8_t index)
{
  uint8_t retval = 0x00;  
  
  if(0 < index < sizeof(graph_data)/sizeof(graph_data[0])){
      graph_data[index] = pData;//��������� ����� ������ ������ �������
      _aColor[index] = color;//��������� ����� ����
      retval = 0x01;
  };
  return retval;
}

/*
 * ������� ��������� �������� ��������b �� ��������� �������
 * @param uint16_t gph_mask - ������������ ��� ����� �����(0...9), ��������� 1 � ��������� ��� ������� �����+1 ������
 */
void Graph_screen_reset_sel_data(uint16_t gph_mask)
{
  uint8_t i;
  for(i=0; i<(sizeof(graph_data)/sizeof(graph_data[0])); i++)
  {
    gph_mask &= 0x03ff;
    if((gph_mask>>i)&0x01)  
      graph_data[i] = NULL;
  };

}

/*********************************************************************
*
*       _UserDraw
*
* Function description
*   This routine is called by the GRAPH object before anything is drawn
*   and after the last drawing operation.
*/
static void _UserDraw(WM_HWIN hWin, int Stage) {
  if (Stage == GRAPH_DRAW_LAST) {
    char acText[] = "Temperature";
    GUI_RECT Rect;
    GUI_RECT RectInvalid;
    int FontSizeY;

    GUI_SetFont(&GUI_Font13_ASCII);
    FontSizeY = GUI_GetFontSizeY();
    WM_GetInsideRect(&Rect);
    WM_GetInvalidRect(hWin, &RectInvalid);
    Rect.x1 = Rect.x0 + FontSizeY;
    GUI_SetColor(GUI_YELLOW);
    GUI_DispStringInRectEx(acText, &Rect, GUI_TA_HCENTER, strlen(acText), GUI_ROTATE_CCW);
  }
}

/*********************************************************************
*
*       _cbDialog
*
* Function description
*   Callback routine of the dialog
*/
static void _cbDialog(WM_MESSAGE *pMsg) {
 int user_data;
 unsigned i;
  //int      NCode;
  //int      Id;
  //int      Value;
 WM_HWIN  hDlg;
 WM_HWIN  hItem; 
  
 hDlg = pMsg->hWin;  
 user_data = pMsg->Data.v;
  
   switch(user_data){
    case MSG_ENTER://ignore;
    break;
    case MSG_ESC://go to start screen;
        Graph_screen_exit(pMsg->hWin);
        Main_screen_startup(WM_HBKWIN,16,0);
    break;
    case MSG_RIGHT://go to scale screen;
        Graph_screen_exit(pMsg->hWin);
        Scale_screen_startup(WM_HBKWIN,16,0);
    break;
    case MSG_LEFT:///go to scale screen;
        Graph_screen_exit(pMsg->hWin);
        Scale_screen_startup(WM_HBKWIN,16,0);
    break;
    case MSG_UP://ignore;
    break;
    case MSG_DOWN://ignore;
    break;
    default:
    break;  
    };
 
  
    
  switch (pMsg->MsgId) {
  case WM_INIT_DIALOG:
    hItem = WM_GetDialogItem(hDlg, GUI_ID_GRAPH0);
    //
    // Add graphs
    //
    //uxQueueMessageWaiting();
    
    for (i = 0; i < (sizeof(graph_data)/sizeof(graph_data[0])); i++) {
      //_aValue[i] = rand() % 180;
      if(graph_data[i] != NULL)
      {_ahData[i] = GRAPH_DATA_YT_Create(_aColor[i], 240, graph_data[i], 240);
      GRAPH_AttachData(hItem, _ahData[i]);}
    };  
    //
    // Set graph attributes
    //
    GRAPH_SetGridDistY(hItem, 25);
    GRAPH_SetGridVis(hItem, 1);
    GRAPH_SetGridFixedX(hItem, 1);
    //GRAPH_SetUserDraw(hItem, _UserDraw);
    //
    // Create and add vertical scale
    //
    _hScaleV = GRAPH_SCALE_Create( 20, GUI_TA_RIGHT, GRAPH_SCALE_CF_VERTICAL, 25);
    GRAPH_SCALE_SetTextColor(_hScaleV, GUI_WHITE);
    GRAPH_AttachScale(hItem, _hScaleV);
    //
    // Create and add horizontal scale
    //
    _hScaleH = GRAPH_SCALE_Create(310, GUI_TA_HCENTER, GRAPH_SCALE_CF_HORIZONTAL, 50);
    GRAPH_SCALE_SetTextColor(_hScaleH, GUI_GREEN);
    GRAPH_AttachScale(hItem, _hScaleH);
    break;
  case WM_TIMER://������� - ������������ ������� ����������
    hItem = WM_GetDialogItem(hDlg, GUI_ID_GRAPH0);
    upd_graph(hItem);//�������� ������� ������ ������ �� ������� �� �����
    WM_RestartTimer(pMsg->Data.v, REFRESH_TMR_PERIOD);//���������� �������
    break;
   default:
    WM_DefaultProc(pMsg);
  }
}

/**
  * 
  */
static void upd_graph(WM_HWIN hItem)
{
  uint8_t i,j;
  int16_t* tmp_ptr;
  
  for(i=0;i<H_GPAPH_SCALE_SIZE;i++)
  {
    for(j=0;j<(sizeof(graph_data)/sizeof(graph_data[0]));j++)
    {   
      if(graph_data[j]!=NULL)
      {
        if(_ahData[j] == 0)
          {_ahData[j] = GRAPH_DATA_YT_Create(_aColor[j], 240, graph_data[j], 240);
            GRAPH_AttachData(hItem, _ahData[j]);};
        tmp_ptr = graph_data[j];
        GRAPH_DATA_YT_AddValue(_ahData[j], tmp_ptr[i]);
      }
    }; 
          
  };

}

 
/*
 * ��������� ��� ������� �������� ���� � ����������� �������;
 */
void Graph_screen_exit(WM_HWIN hWin)
{
  WM_DeleteTimer(hRefreshTimer);
  GUI_EndDialog(hWin, 1);
}

/**
  * @brief  Graph screen window Starup
  * @param  hWin: pointer to the parent handle.
  * @param  xpos: X position 
  * @param  ypos: Y position
  * @retval None
  */
void Graph_screen_startup(WM_HWIN hWin, uint16_t xpos, uint16_t ypos)
{
  WM_HWIN hGraph_Dlg;
  
  hGraph_Dlg = GUI_CreateDialogBox(_aDialog, GUI_COUNTOF(_aDialog), _cbDialog, hWin, xpos, ypos);//������� �������� �������� �������
  
  hRefreshTimer = WM_CreateTimer(hGraph_Dlg, 0, REFRESH_TMR_PERIOD, 0);//������� ������������� ������ ��� ���������� ����
  //��� ������� ������ ���� �������� ��������� � ������� ������� �������
}