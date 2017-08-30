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
 * @param pData - ��������� �� ������ � ������� �� 240 ���������;
 * @param color - �������� ���� ������� (��. ������� GUI_COLOR � STemWIN manual) 
 * @retval - 0x00 - ������/0x01 - ������ ���������
 */
uint8_t Graph_screen_add_data(int16_t* pData, GUI_COLOR color);

/*
 * @param pData - ��������� �� ������ � ������� �� 240 ���������;
 * @param color - �������� ���� ������� (��. ������� GUI_COLOR � STemWIN manual) 
 * @param index - ����� ������� 
 * @retval - 0x00 - ������/0x01 - ������ ���������
 */
uint8_t Graph_screen_add_data_sel(int16_t* pData, GUI_COLOR color, uint8_t index);

/*
 * ������� ��������� �������� ��������b �� ��������� �������
 * @param uint16_t gph_mask - ������������ ��� ����� �����(0...9), ��������� 1 � ��������� ��� ������� �����+1 ������
 */
void Graph_screen_reset_sel_data(uint16_t gph_mask);

#ifdef __cplusplus
}
#endif

#endif /*__SVD_GUI_GRAPH_SCREEN_H */