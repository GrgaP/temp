#ifndef __SVD_GUI_GRAPH_SCREEN_H
#define __SVD_GUI_GRAPH_SCREEN_H


/**
  * @brief  Main screen window Starup
  * @param  hWin: pointer to the parent handle.
  * @param  xpos: X position 
  * @param  ypos: Y position
  * @retval None
  */
void Graph_screen_startup(WM_HWIN hWin, uint16_t xpos, uint16_t ypos);

/*
 * @param pData - указатель на массив с данными на 240 элементов;
 * @param color - передает цвет графика (см. таблицу GUI_COLOR в STemWIN manual) 
 * @retval - 0x00 - ошибка/0x01 - данные добавлены
 */
uint8_t Graph_screen_add_data(int16_t* pData, GUI_COLOR color);

/*
 * @param pData - указатель на массив с данными на 240 элементов;
 * @param color - передает цвет графика (см. таблицу GUI_COLOR в STemWIN manual) 
 * @param index - номер графика 
 * @retval - 0x00 - ошибка/0x01 - данные добавлены
 */
uint8_t Graph_screen_add_data_sel(int16_t* pData, GUI_COLOR color, uint8_t index);

/*
 * Функция позволяет сбросить указателb на выбранные графики
 * @param uint16_t gph_mask - используется как набор битов(0...9), установка 1 в указанный бит обнулит соотв+1 график
 */
void Graph_screen_reset_sel_data(uint16_t gph_mask);

#ifdef __cplusplus
}
#endif

#endif /*__SVD_GUI_GRAPH_SCREEN_H */