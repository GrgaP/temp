#include "main.h"
#include "svd_gui.h"

#define MAX_VALUE 180
#define H_GPAPH_SCALE_SIZE 240
#define REFRESH_TMR_PERIOD 30

//static GRAPH_DATA_Handle  _ahData[10]; // Array of handles for the GRAPH_DATA objects
static GRAPH_SCALE_Handle _hScaleV;   // Handle of vertical scale
static GRAPH_SCALE_Handle _hScaleH;   // Handle of horizontal scale

static xQueueHandle svd_graph_data_queue;

typedef struct
{
  GRAPH_DATA_Handle ahData;//it is handle for the GRAPH_DATA objects
  int16_t* pData;//pointer to data for showing
  GUI_COLOR color;//Value of color for graphic curve
  uint32_t* UserData[4];//reserved
}SVD_Graph_Data_TypeDef;

//Указатели на массивы с данными для отображения;
//static int16_t* graph_data[10]={NULL};

//позиция моды
//левый репер
//правый репер

//static GUI_COLOR _aColor[10] = {GUI_WHITE}; // Array of colors for the GRAPH_DATA objects

static WM_HWIN hGraph_Dlg;

static const GUI_WIDGET_CREATE_INFO _aDialog[] = {
  { WINDOW_CreateIndirect, "",       ID_WINDOW_3,               0,  0,   240, 320, 0               },
  //{ TEXT_CreateIndirect,   "Graph screen", 0,               80, 5,   100, 20,  TEXT_CF_HCENTER },
  { GRAPH_CreateIndirect,   0, GUI_ID_GRAPH0, 0, 0, 240, 320, 0,  0},
};

WM_HTIMER       hRefreshTimer;

static void upd_graph(void);
void Graph_screen_exit(WM_HWIN hWin);

/*
 * @param pData - указатель на массив с данными на 240 элементов;
 * @param color - передает цвет графика (см. таблицу GUI_COLOR в STemWIN manual) 
 * @retval - 0x00 - ошибка/0x01 - данные добавлены
 */
uint8_t Graph_screen_add_data(int16_t* pData, GUI_COLOR color)
{
  //uint8_t i;
  uint8_t retval = 0x01;
  SVD_Graph_Data_TypeDef graph_data;
  WM_HWIN hItem;
  
  graph_data.pData = pData;
  graph_data.color = color;  
  graph_data.ahData = GRAPH_DATA_YT_Create(graph_data.color, 240, graph_data.pData, 240);
  
  hItem = WM_GetDialogItem(hGraph_Dlg, GUI_ID_GRAPH0);
  
  GRAPH_AttachData(hItem, graph_data.ahData);
  
  
  xQueueSend(svd_graph_data_queue,&graph_data,100);
  
  
  /*for(i=0; i<(sizeof(graph_data)/sizeof(graph_data[0])); i++)
  {
    if(graph_data[i] == NULL)//находим первую неиспользованную позицию;
    {
      graph_data[i] = pData;//сохраняем адрес данных нового графика
      _aColor[i] = color;//добавляем новый цвет
      retval = 0x01;
      break ;//выходим из цикла
    }
    //else
      //Если исчерпан предел количества отображаемых графиков - вернем ошибку;
      
  }  */
  return retval;
}

/*
 * Обнуляет указатели на данные для отображения;
 */
void Graph_screen_reset_data(void)
{
  //uint8_t i;
  
  /*for(i=0; i<(sizeof(graph_data)/sizeof(graph_data[0])); i++)
  {
    graph_data[i] = NULL;
  };*/
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
 //unsigned i;
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
    
    /*for (i = 0; i < (sizeof(graph_data)/sizeof(graph_data[0])); i++) {
      //_aValue[i] = rand() % 180;
      if(graph_data[i] != NULL)
      {_ahData[i] = GRAPH_DATA_YT_Create(_aColor[i], 240, graph_data[i], 240);
      GRAPH_AttachData(hItem, _ahData[i]);}
    };  */
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
  case WM_TIMER:
    upd_graph();  
    WM_RestartTimer(pMsg->Data.v, REFRESH_TMR_PERIOD);
    break;
   default:
    WM_DefaultProc(pMsg);
  }
}

static void upd_graph(void)
{
  unsigned i;
  //unsigned j;
  SVD_Graph_Data_TypeDef tmp_ptr;
  
  xQueuePeek(svd_graph_data_queue,&tmp_ptr,100);
  for(i=0;i<H_GPAPH_SCALE_SIZE;i++)
  {
    GRAPH_DATA_YT_AddValue(tmp_ptr.ahData, tmp_ptr.pData[i]);    
  };
  
  /*for(i=0;i<H_GPAPH_SCALE_SIZE;i++)
  {
    for(j=0;j<(sizeof(graph_data)/sizeof(graph_data[0]));j++)
    {   
      if(graph_data[j]!=NULL)
      {
        tmp_ptr = graph_data[j];//
        GRAPH_DATA_YT_AddValue(_ahData[j], tmp_ptr[i]);
      }
    }; 
          
  }; */ 

}

 
/*
 * Закрывает все объекты текущего окна и освобождает ресурсы;
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
  hGraph_Dlg = GUI_CreateDialogBox(_aDialog, GUI_COUNTOF(_aDialog), _cbDialog, hWin, xpos, ypos);
  
  hRefreshTimer = WM_CreateTimer(hGraph_Dlg, 0, REFRESH_TMR_PERIOD, 0);     
}

/*
 * Вызывается для инициализации ресурсов;
 * @retval 0x01 - выполнено успешно/0x00 - ошибка при инициализации
 */
uint8_t SVD_graph_module_init(void)
{
  uint8_t retval = 0x01;
  
  svd_graph_data_queue = xQueueCreate(10,sizeof(SVD_Graph_Data_TypeDef));//создаем очередь на 10 элементов типа SVD_Graph_Data_TypeDef
  if(svd_graph_data_queue == NULL)
    retval = 0x00;
  
  return retval;
};

/*
 * Вызывается для освобождения ресурсов;
 */
void SVD_graph_module_deinit(void)
{
  vQueueDelete(svd_graph_data_queue);//удаляем очередь на 10 элементов типа SVD_Graph_Data_TypeDef
};