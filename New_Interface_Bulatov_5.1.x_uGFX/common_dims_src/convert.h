#ifndef CONVERT_H
#define CONVERT_H

#include "main.h"

/*
Преобразование Voltage Int -> Float
Преобразование выполняется на основании значений VFloat_From и VFloat_To
VInt / ((VInt_To - VInt_From) / (VFloat_To - VFloat_From))
*/
#define VInt_to_VFloat(VInt) \
(VInt/((((Settings_sensor+7)->int_to)-(Settings_sensor+6)->int_from)\
/(((Settings_sensor+7)->float_to)-((Settings_sensor+6)->float_from))))

/*
Преобразование Voltage Int в индекс массива пиков PeakIdx
*/
//#define VInt_to_PeakIdx(VInt) \
//((u16)((VInt + (0-/*From_V*/(s16)GETPARAMS.iTrackValue[6]))\
///((float)(/*To_V*/(s16)GETPARAMS.iTrackValue[7]-/*From_V*/(s16)GETPARAMS.iTrackValue[6]) / (GETDATA_SIZE-1))))

/*
Преобразование Voltage Int в координату дисплея по оси X
*/
//#define VInt_to_LcdX(VInt) \
//((u16)((VInt + (0-/*From_V*/(s16)GETPARAMS.iTrackValue[6]))\
///((float)(/*To_V*/(s16)GETPARAMS.iTrackValue[7]-/*From_V*/(s16)GETPARAMS.iTrackValue[6]) / (LCD_WIDTH-1))))

/*
Преобразование индекса массива пиков PeakIdx в координату дисплея по оси X
*/
//#define PeakIdx_to_LcdX(PeakIdx) \
//((u16)(PeakIdx/((GETDATA_SIZE-1)/(LCD_WIDTH-1))))

////////////////////////////////////////////////
//    Секция прототипов локальных функций     //
////////////////////////////////////////////////
u16 VInt_to_PeakIdx (s16 VInt);
u16 VInt_to_LcdX (s16 VInt);
u16 PeakIdx_to_LcdX (u16 PeakIdx);
float convertPeakIdFloat(u16 Id);
#endif