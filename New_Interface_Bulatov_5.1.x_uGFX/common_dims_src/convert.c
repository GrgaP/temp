#include "convert.h"

/*
ѕреобразование Voltage Int в индекс массива пиков PeakIdx
*/
u16 VInt_to_PeakIdx (s16 VInt)
{
	return (u16) ((VInt + (0-/*From_V*/(s16)GETPARAMS.iTrackValue[6]))\
	/((float)(/*To_V*/(s16)GETPARAMS.iTrackValue[7]-/*From_V*/(s16)GETPARAMS.iTrackValue[6]) / (SPECTRA_SIZE-1)));
}

/*
ѕреобразование Voltage Int в координату диспле€ по оси X
*/
u16 VInt_to_LcdX (s16 VInt)
{
	return (u16) ((VInt + (0-/*From_V*/(s16)GETPARAMS.iTrackValue[6]))\
	/((float)(/*To_V*/(s16)GETPARAMS.iTrackValue[7]-/*From_V*/(s16)GETPARAMS.iTrackValue[6]) / (LCD_WIDTH-1)));
}

/*
ѕреобразование индекса массива пиков PeakIdx в координату диспле€ по оси X
*/
u16 PeakIdx_to_LcdX (u16 PeakIdx)
{
	return (u16) (PeakIdx/((SPECTRA_SIZE-1)/(float)LCD_W));
}

float convertPeakIdFloat(u16 Id)
{
  int16_t peakInt;
  peakInt=Id;
  peakInt =(+(int16_t)GETPARAMS.iTrackValue[6]                 \
              + Id*(                                            \
                      (s16)GETPARAMS.iTrackValue[7]             \
                     -(s16)GETPARAMS.iTrackValue[6]             \
                    )                                           \
                /(SPECTRA_SIZE-1));                              \
  float peakFloat;                      
  peakFloat = VInt_to_VFloat(peakInt);
  return (float)(peakFloat);
}


