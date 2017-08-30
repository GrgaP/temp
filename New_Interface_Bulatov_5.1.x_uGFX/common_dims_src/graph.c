#include "graph.h"
#include "arm_math.h" 
#include "main.h"
#include "convert.h"
#include "SpectrumMath.h"
#include "rgb.h"
#include "menu_calibration.h"
#include "menu.h"
#include "security.h"

// Пики
//u32 num_emi_peaks=0;			// Кол-во найденных минимальных пиков
//u32 emi_peaks[max_emi_peakss];	// Массив с найденными минимальными пиками

//u32 num_absop_peaks=0;				// Кол-во найденных максимальных пиков
//u32 absop_peaks[max_absop_peakss];	// Массив с найденными максимальными пиками

// Пики Репера
//u32 rp_num_emi_peaks=0;			// Кол-во найденных минимальных пиков
//u32 rp_emi_peaks[max_emi_peakss];	// Массив с найденными минимальными пиками

//u32 rp_num_absop_peaks=0;				// Кол-во найденных максимальных пиков
//u32 rp_absop_peaks[max_absop_peakss];	// Массив с найденными максимальными пиками



static int32_t rpId = -1; 

volatile u8 DisplayMode = DisplayMode_PowerOff;
volatile u16 Grad_Lvl_Curr=0;	// Текущий уровень градиента
volatile u16 Grad_Lvl_Trgt=0;	// Целевой уровень градиента
volatile u16 Grad_Lvl_Trgt_Spctr; // Уровень для эмитации спектра
// Уравнивание уровней выполняется в обработчике опроса клавиатуры (100 Гц)

void vInDisplayMode_Spectrum(void);
void vShowRpZone(void);
void vShowExpZone(uint32_t num);
void vShowLevelInfo(uint32_t ulExplNum, uint32_t x, uint32_t y);

int32_t rpPrToId(int32_t pr)
{
  return (int32_t)((float)pr * SPECTRA_SIZE / 100.f); 
}

void filterSimple(int16_t* data, int32_t n, uint32_t size)
{
  if (!n%2) n++;
  for (uint32_t i = 0; i < (size-(n-1)); i++)
  {
    s32 sum = 0;
    for (uint32_t j=0; j<n; j++ ) sum += data[i+j];
    data[i+n/2+1] = sum/n;
  }
}

void AlarmCalculateForGradient(void)
{
  static u32 alarm[EXPLOSIV_COUNT] = {0} ;
  
  if ((xTntPeakInfo.eKeep == KEEP) && (xTntPeakInfo.eType == PEAK_T_AN_MIN_IN_TNT_ZONE))
	{  
	
             /* Отрабатываем писк */
             alarm[0]++;
             if (alarm[0]>2)		// Если возведён флаг обнаружения TNT
             {
                  Buzzer_Tnt_On();  // Включаю прерывистый писк
                 // LCDBUFFER_Alfa_Rectangle (10-5, 270-5, 10+60+5, 270+16+5, WHITE, 0.5f);
                 // LCDBUFFER_Str_Shadow("ALARM",10,270,RED,2,2);  // Вывожу предупреждение на дисплей
                  char stringBuf[20];
                  LCDBUFFER_Alfa_Rectangle (180-5, 230-5-5, 230, 260-5, LIGHTYELLOW, 0.75f);
                LCDBUFFER_Num_Shadow(xTntPeakInfo.sScanId,180,230-5,YELLOW,1,1);
                sprintf(stringBuf, "%2.1f", xTntPeakInfo.fScanV);
                LCDBUFFER_Str_Shadow(stringBuf,180,240-5,LIGHTYELLOW,1,1);
                LCDBUFFER_Str_Shadow("В",205,240-5,LIGHTYELLOW,1,1);
                  LCDBUFFER_Num_Shadow(xTntPeakInfo.sAmpId, 180,250-5,YELLOW,1,1);
                  
                  LCDBUFFER_Str_ShadowAlpha("TNT",120,230,LIGHTYELLOW,2,2, 0.2f);  // Вывожу предупреждение на дисплей
             }
          
        }
  else 
  { 
     
     alarm[0] = 0;
     // Buzzer_Tnt_Off();
  }
  #define isAlarm() 
  
  for (uint32_t num = 1; num < EXPLOSIV_COUNT; num++)
  {
    if ((xExplPeakInfo[num].eKeep == KEEP) && \
        (xExplPeakInfo[num].eType == (1<<(num*2+1))) && \
        (Explosive_Set[num].eOnOff != OFF))
    {
      
    
    /* Отрабатываем писк */
             alarm[num]++;
             if (alarm[num]>2)		// Если возведён флаг обнаружения TNT
             {
                  Buzzer_Tnt_On();  // Включаю прерывистый писк
                  //LCDBUFFER_Str_ShadowAlpha("ALARM",10,270,RED,2,2, 0.2f);  // Вывожу предупреждение на дисплей
                  vShowPeakInfo(num, 175, 40*(EXPLOSIV_COUNT-num) - 20);
                  LCDBUFFER_Str_ShadowAlpha(*Explosive_Set[num].Name,120,40*(EXPLOSIV_COUNT-num) - 10,Explosive_Set[num].usColor,2,2, 0.2f);  // Вывожу предупреждение на дисплей
             }
          
    }
    else 
    { 
     alarm[num] = 0;
    // Buzzer_Tnt_Off();
    }
     
  }
  
  u32 alarmSum;
  alarmSum = 0;
  for ( uint32_t i = 0; i<EXPLOSIV_COUNT; i++)
  {
    alarmSum += alarm[i];
  }
  if (alarmSum != 0)
  {
   // LCDBUFFER_Alfa_Rectangle (10-5, 270-5, 10+60+5, 270+16+5, WHITE, 0.8f);
    //LCDBUFFER_Str_Shadow("ALARM",10,270,RED,2,2);  // Вывожу предупреждение на дисплей
    LCDBUFFER_Alfa_Rectangle (115, 10, 235, 313, RED, 0.8f);
  }
  else
  {
    LCDBUFFER_Rectangle (120, 260, 239, 319, BLACK, 1);
    Buzzer_Tnt_Off();
  }  
  
}


void AlarmCalculate(void)
{
  static u32 alarm[EXPLOSIV_COUNT] = {0} ;
  
  if ((xTntPeakInfo.eKeep == KEEP) && (xTntPeakInfo.eType == PEAK_T_AN_MIN_IN_TNT_ZONE))
	{  
	
             /* Отрабатываем писк */
             alarm[0]++;
             if (alarm[0]>2)		// Если возведён флаг обнаружения TNT
             {
                  Buzzer_Tnt_On();  // Включаю прерывистый писк
                 // LCDBUFFER_Alfa_Rectangle (10-5, 270-5, 10+60+5, 270+16+5, WHITE, 0.5f);
                 // LCDBUFFER_Str_Shadow("ALARM",10,270,RED,2,2);  // Вывожу предупреждение на дисплей
                  LCDBUFFER_Str_ShadowAlpha("TNT",185,230,WHITE,2,2, 0.2f);  // Вывожу предупреждение на дисплей
             }
          
        }
  else 
  { 
     
     alarm[0] = 0;
     // Buzzer_Tnt_Off();
  }
  #define isAlarm() 
  
  for (uint32_t num = 1; num < EXPLOSIV_COUNT; num++)
  {
    if ((xExplPeakInfo[num].eKeep == KEEP) && \
        (xExplPeakInfo[num].eType == (1<<(num*2+1))) && \
        (Explosive_Set[num].eOnOff != OFF))
    {
      
    
    /* Отрабатываем писк */
             alarm[num]++;
             if (alarm[num]>2)		// Если возведён флаг обнаружения TNT
             {
                  Buzzer_Tnt_On();  // Включаю прерывистый писк
                  //LCDBUFFER_Str_ShadowAlpha("ALARM",10,270,RED,2,2, 0.2f);  // Вывожу предупреждение на дисплей
                  LCDBUFFER_Str_ShadowAlpha(*Explosive_Set[num].Name,180,40*(EXPLOSIV_COUNT-num) - 10,WHITE,2,2, 0.2f);  // Вывожу предупреждение на дисплей
                 
             }
          
    }
    else 
    { 
     alarm[num] = 0;
    // Buzzer_Tnt_Off();
    }
     
  }
  
  u32 alarmSum;
  alarmSum = 0;
  for ( uint32_t i = 0; i<EXPLOSIV_COUNT; i++)
  {
    alarmSum += alarm[i];
  }
  if (alarmSum != 0)
  {
    LCDBUFFER_Alfa_Rectangle (10-5, 270-5, 10+60+5, 270+16+5, WHITE, 0.8f);
    LCDBUFFER_Str_Shadow("ALARM",10,270,RED,2,2);  // Вывожу предупреждение на дисплей
  }
  else
  {
    Buzzer_Tnt_Off();
  }  
  
}
void Show_Spectrum (s16* spctr, uint32_t color)
{
	s32 result=0;		// Результат усреднения (текущая точка)
	u16 old_result=0;	// Значение предыдущей точки, для того, чтобы нарисовать линию между предыдущей и текущей точкой
	u16 start;			// Начальный элемент USART-массива (для усреднения)
	u16 stop;			// Конечный элемент USART-массива (для усреднения)
	for (u16 i=0; i<LCD_WIDTH; i++)	// Для всех пикселей по ширине дисплея
	{
		start = (u16) (i*((float)SPECTRA_SIZE/LCD_WIDTH));		// 1024/320=4.3 ячейки USART-массива приходится на 1 пиксель дисплея по ширине
		stop = (u16) ((i+1)*((float)SPECTRA_SIZE/LCD_WIDTH));
		for (u16 j=start; j<stop; j++) {result += spctr[j];}	// Цикл усреднения точек (сложение)
		result = result / (stop-start);	// Деление
		// Преобразую точку (от -32768 до +32767) в высоту дисплея (от 0 до 319) пикселей
		result = (u32) ((result+32768)/((float)65535/(LCD_HEIGHT-1)));
		// Отрисовку первой линии пропускаю, т.к. для первой линии нет координат предыдущей точки
		// Линия должна рисоваться между предыдущей и текущей точками
		if (i>0) LCDBUFFER_Line(i, old_result, i, result, color);	// Рисую линию от предыдущей точки к текущей точке (белым цветом)
		old_result=result;	// Текущую точку сохраняю как предыдущую точку (нужно для прорисовки следующей линии)
	}
}

void Show_BAASpectrum (s16* spctr, uint32_t color)
{
	s32 result=0;		// Результат усреднения (текущая точка)
	u16 old_result=0;	// Значение предыдущей точки, для того, чтобы нарисовать линию между предыдущей и текущей точкой
	u16 start;			// Начальный элемент USART-массива (для усреднения)
	u16 stop;			// Конечный элемент USART-массива (для усреднения)
	for (u16 i=0; i<LCD_WIDTH; i++)	// Для всех пикселей по ширине дисплея
	{
		start = (u16) (i*((float)SPECTRA_SIZE/LCD_WIDTH));		// 1024/320=4.3 ячейки USART-массива приходится на 1 пиксель дисплея по ширине
		stop = (u16) ((i+1)*((float)SPECTRA_SIZE/LCD_WIDTH));
		for (u16 j=start; j<stop; j++) {result += spctr[j];}	// Цикл усреднения точек (сложение)
		result = result / (stop-start);	// Деление
		// Преобразую точку (от -32768 до +32767) в высоту дисплея (от 0 до 319) пикселей
		result = (u32) ((result+32768)/((float)65535/(LCD_HEIGHT-1)));
		// Отрисовку первой линии пропускаю, т.к. для первой линии нет координат предыдущей точки
		// Линия должна рисоваться между предыдущей и текущей точками
		if (i>0) LCDBUFFER_BAALine(i, old_result, i, result, color);	// Рисую линию от предыдущей точки к текущей точке (белым цветом)
		old_result=result;	// Текущую точку сохраняю как предыдущую точку (нужно для прорисовки следующей линии)
	}
}

//===================================================
// Отобразить границы обнаружения для режима спектра
//===================================================
void Show_Specrtum_Borders (void)
{
	if (/*От*/GETPARAMS.iTrackValue[4] < /*From_V*/GETPARAMS.iTrackValue[6]) /*От*/GETPARAMS.iTrackValue[4]=/*From_V*/GETPARAMS.iTrackValue[6];	// Защита от понижения "От"
	if (/*От*/GETPARAMS.iTrackValue[4] > /*To_V*/GETPARAMS.iTrackValue[7])   /*От*/GETPARAMS.iTrackValue[4]=/*To_V*/GETPARAMS.iTrackValue[7];	// Защита от превышения "От"
	if (/*До*/GETPARAMS.iTrackValue[5] > /*To_V*/GETPARAMS.iTrackValue[7])   /*До*/GETPARAMS.iTrackValue[5]=/*To_V*/GETPARAMS.iTrackValue[7];	// Защита от превышения "До"
	if (/*До*/GETPARAMS.iTrackValue[5] < /*From_V*/GETPARAMS.iTrackValue[6]) /*До*/GETPARAMS.iTrackValue[5]=/*From_V*/GETPARAMS.iTrackValue[6];	// Защита от понижения "До"
	if (/*От*/GETPARAMS.iTrackValue[4] > /*До*/GETPARAMS.iTrackValue[5])     /*До*/GETPARAMS.iTrackValue[5]=/*От*/GETPARAMS.iTrackValue[4];		// Защита от нарушения порядка границ
	// Преобразую в координаты дисплея и вывожу границы
        if (rpId < 0)
        {
          LCDBUFFER_VLine (VInt_to_LcdX (/*От*/GETPARAMS.iTrackValue[4]), BORDER_COLOR);
          LCDBUFFER_VLine (VInt_to_LcdX (/*До*/GETPARAMS.iTrackValue[5]), BORDER_COLOR);
	}
        else
        {
          LCDBUFFER_VLine (VInt_to_LcdX (/*От*/GETPARAMS.iTrackValue[4]), DARK_GRAY);
          LCDBUFFER_VLine (VInt_to_LcdX (/*До*/GETPARAMS.iTrackValue[5]), DARK_GRAY);
          
          u32 Zone_L = 0; 
          u32 Zone_R = 0;
          int32_t temp;
         
          temp =  (int32_t)rpId-(int32_t)rpIdWnd+(int32_t)rpIdOffset;
          Zone_L = (temp < 0) ? 0 : temp; 	// вольтаж
	  temp =  (int32_t)rpId+(int32_t)rpIdWnd+(int32_t)rpIdOffset;
          Zone_R = (temp >=  SPECTRA_SIZE) ? ( SPECTRA_SIZE-1) : temp; 
          
          
          LCDBUFFER_VLineAlpha (PeakIdx_to_LcdX (/*От*/Zone_L), BORDER_COLOR, 0.75);
          LCDBUFFER_VLineAlpha (PeakIdx_to_LcdX (/*До*/ Zone_R), BORDER_COLOR, 0.75);

	}
	
}
/*
//===========================================
// Проверка наличия пиков в зоне обнаружения
//===========================================
void Check_Specrtum_Borders (void)
{
  
	// Вычисление зоны обнаружения
        u16 Zone_L = VInt_to_PeakIdx((s16)GETPARAMS.iTrackValue[4]);;
        u16 Zone_R = VInt_to_PeakIdx((s16)GETPARAMS.iTrackValue[5]);
        if ((rpId < Zone_L)||(rpId > Zone_R)||(rpId<0))
        {
          rpId = -1;
        }
        else 
        {
          int32_t temp;
          temp =  (int32_t)rpId-(int32_t)rpIdWnd+(int32_t)rpIdOffset;
          Zone_L = (temp < 0) ? 0 : temp; 	// вольтаж
          temp =  (int32_t)rpId+(int32_t)rpIdWnd+(int32_t)rpIdOffset;
          Zone_R = (temp >=  SPECTRA_SIZE) ? ( SPECTRA_SIZE-1) : temp; 
        }

        
        
          
	u16 flag_warning;
        flag_warning=0;
        static u16 alarm = 0 ;      // Сброс флага обнаружения TNT
	for (u32 i=0; i<num_absop_peaks; i++)		// Прохожусь по всем пикам, проверяя условие попадания в границы обнаружения
	{
		if ((absop_peaks[i]>Zone_L) && (absop_peaks[i]<Zone_R))	flag_warning++;
                
	};
	if (flag_warning)
        { 
          alarm++;
          if (alarm>11)		// Если возведён флаг обнаружения TNT
          {
                  Buzzer_Tnt_On();  // Включаю прерывистый писк
                  LCDBUFFER_Str("TNT",185,285,RED,BLACK,2,2);  // Вывожу предупреждение на дисплей
          }
          
        }
        else 
        { 
          alarm = 0;
          Buzzer_Tnt_Off();
        }
}*/

//======================================================
// Отобразить границы обнаружения для режима калибровки
//======================================================
void Show_Calibration_Borders (void)
{
	// Вычисление границ
	s16 Frr = E_Settings[E_Sel].Pos-E_Settings[E_Sel].Itrv;
	u16 Now = E_Settings[E_Sel].Pos;
	s16 Too = E_Settings[E_Sel].Pos+E_Settings[E_Sel].Itrv;
	// Защита от переполнения
	if (Frr<0) Frr=0;
	if (Too>(SPECTRA_SIZE-1)) Too=SPECTRA_SIZE-1;
	// Отображение на дисплей
	//if (E_Settings[E_Sel].Enable)
	//{
		LCDBUFFER_VLine (PeakIdx_to_LcdX (Frr), BLUE);
		LCDBUFFER_VLine (PeakIdx_to_LcdX (Now), RED);
		LCDBUFFER_VLine (PeakIdx_to_LcdX (Too), BLUE);
		LCDBUFFER_Num(E_Settings[E_Sel].Pos,50,275,RED,BLACK,1,1);
		LCDBUFFER_Num(E_Settings[E_Sel].Itrv,150,275,BLUE,BLACK,1,1);
	//}
}

//==================================
// Отобразить сетку и подписи к ней
//==================================
void show_grid_texts (void)
{
	// Расчёт расстояний между линиями сетки
	u8 step_W = LCD_WIDTH / (GRID_LINES_WIDTH+1);
	u8 step_H = LCD_HEIGHT / (GRID_LINES_HEIGHT+1);
	// Подписи по оси X (из USART-буфера)
	float float_from_V = (((s16)GETPARAMS.iTrackValue[6]-((Settings_sensor+6)->int_from))/((((Settings_sensor+6)->int_to)-((Settings_sensor+6)->int_from))/(((Settings_sensor+6)->float_to)-((Settings_sensor+6)->float_from))))+((Settings_sensor+6)->float_from);	// Переводим "from_V" из "integer" во "float"
	float float_to_V = (((s16)GETPARAMS.iTrackValue[7]-((Settings_sensor+7)->int_from))/((((Settings_sensor+7)->int_to)-((Settings_sensor+7)->int_from))/(((Settings_sensor+7)->float_to)-((Settings_sensor+7)->float_from))))+((Settings_sensor+7)->float_from);	// Переводим "to_V" из "integer" во "float"
	// Строковый буфер для вывода на дисплей (на 5 символов)
	char string_buff[5];
	// Отрисовка линий по горизонтальной оси
	for (u8 i=1; i<(GRID_LINES_WIDTH+1); i++)
	{
		LCDBUFFER_VLine (step_W*i, GRID_COLOR);
		// Подпись к вертикальной линии (по оси X, значения из USART-буфера)
		sprintf(string_buff, "%-5.1f", (float_from_V+(i*(((float_to_V)-float_from_V)/(GRID_LINES_WIDTH+1)))));	// Перевод из "float" в "string", выравнивание влево, всего 5 символов, после запятой 1 символ
		LCDBUFFER_Str_ShadowAlpha (string_buff,((step_W*i)-(step_W/3)),(LCD_HEIGHT-10),GRIDTEXTS_COLOR,1,1,0.5);		// Подпись по оси X
	}
	// Отрисовка линий по вертикальной оси
	for (u8 i=1; i<(GRID_LINES_HEIGHT+1); i++)
	{
		LCDBUFFER_HLine (step_H*i, GRID_COLOR);
		// Подпись к горизонтальной линии (по оси Y, значения рассчитанные из дефайнов)
		sprintf(string_buff, "%+3.0f", (label_Y_max - (((label_Y_max-label_Y_min) / (GRID_LINES_HEIGHT+1)) * i)));	// Перевод из "float" в "string", выравнивание влево, всего 5 символов
		sprintf(string_buff, "%s%s", string_buff,"pA");										// Добавляю единицы измерения "pA"
		LCDBUFFER_Str_ShadowAlpha (string_buff,0,((step_H*i)-8),GRIDTEXTS_COLOR,1,1,0.5);	// Подпись по оси Y	
	}
}

void vShoTntLevelInfo(void)
{
            /* Show no alrm line */
            LCDBUFFER_VLineAlpha(PeakIdx_to_LcdX(xTntPeakInfo.sScanId), LIGHTYELLOW, 0.1f);
            /* Show level window */
            char stringBuf[11];
            LCDBUFFER_Alfa_Rectangle (10, 230-5-5, 65, 260-5, LIGHTYELLOW, 0.75f);
            sprintf(stringBuf, "S:%5.1f%%", 100.f * Explosive_Set[0].usLevelAlarm/32767.f);
            LCDBUFFER_Str_ShadowAlpha(stringBuf,15,230-5,LIGHTYELLOW,1,1, 0.25);
            sprintf(stringBuf, "%7.1f", xTntPeakInfo.fScanV);
            sprintf(stringBuf, "%s%s", stringBuf, "В");
            LCDBUFFER_Str_ShadowAlpha(stringBuf,15,240-5,LIGHTYELLOW,1,1,0.25);
            sprintf(stringBuf, "%7.1f", 100.f * xTntPeakInfo.sAmpId/65535.f);
            sprintf(stringBuf, "%s%s", stringBuf, "%" );
            LCDBUFFER_Str_ShadowAlpha(stringBuf,15,250-5,LIGHTYELLOW,1,1,0.25);
}

//=====================================================
// Отобразить пики тонкими линиями
// Пики в зоне обнаружения отобразить жирными линиями
// Функция используется в режимах спектра и калибровки
//=====================================================
#define NO_PEAK (-100000.f) /* non reachable value */
#define PEAK_FIND(a) (a >= (NO_PEAK+10000)) /* return TRUE if peak is find */


void peaks_show (void)
{
          /* Show explosive peaks */
          for ( uint32_t num = 1; num < EXPLOSIV_COUNT; num++)
          {
            
            if (Explosive_Set[num].eOnOff == ON) /* Check on or off zone */
            { 
              if (xExplPeakInfo[num].eKeep == KEEP) /* Check have a peak */
              {
                if (xExplPeakInfo[num].eType == (1<<(num*2+1))) /* Check Alarm */
                {
                  /* Show alarm line and peak info*/
                  LCDBUFFER_VLine_Flash(PeakIdx_to_LcdX(xExplPeakInfo[num].sScanId), Explosive_Set[num].usColor);
                  vShowPeakInfo(num, 175, 40*(EXPLOSIV_COUNT-num) - 20);
                  vShowLevelInfo(num, 10, 40*(EXPLOSIV_COUNT-num) - 20);
                }
                else if ((xExplPeakInfo[num].eType == (1<<(num*2+0)))) /* Check piks not alarm, not nes... */
                {
                  if (xExplPeakInfo[num].sAmpId/65535.f > 0.1)
                  {
                    if (usMenuFlags.bits.bit1 == 1)
                    {
                      LCDBUFFER_VLineAlpha(PeakIdx_to_LcdX(xExplPeakInfo[num].sScanId), Explosive_Set[num].usColor, 0.95f);
                      vShowLevelInfo(num, 10, 40*(EXPLOSIV_COUNT-num) - 20);
                    }
                  }
                }
              }
            }
          }
          
          
          /* Show TNT peaks. NEW! */ 
          if (xTntPeakInfo.eKeep == KEEP) /* Check have a peak */
          {
            if (xTntPeakInfo.eType ==  PEAK_T_AN_MIN_IN_TNT_ZONE) /* Ia alarm? */
            {
             /* Show alarm line */
             LCDBUFFER_VLine_Flash(PeakIdx_to_LcdX(xTntPeakInfo.sScanId), LIGHTYELLOW);
              /* Show info window */
             char stringBuf[20];
             LCDBUFFER_Alfa_Rectangle (180-5, 230-5-5, 230, 260-5, LIGHTYELLOW, 0.75f);
             LCDBUFFER_Num_Shadow(xTntPeakInfo.sScanId,180,230-5,YELLOW,1,1);
             sprintf(stringBuf, "%2.1f", xTntPeakInfo.fScanV);
             LCDBUFFER_Str_Shadow(stringBuf,180,240-5,LIGHTYELLOW,1,1);
             LCDBUFFER_Str_Shadow("В",205,240-5,LIGHTYELLOW,1,1);
             LCDBUFFER_Num_Shadow(xTntPeakInfo.sAmpId, 180,250-5,YELLOW,1,1);
             
             vShoTntLevelInfo();
            }
           
          else if (xTntPeakInfo.eType == PEAK_T_AN_IN_TNT_ZONE) /* is not alarm */
          {
            if (usMenuFlags.bits.bit1 == 1)
            {                      
              vShoTntLevelInfo();
            }
          
          }
          }
          
          
          
          /* Show TNT peaks old*/ /*
          for (uint32_t i = 0; i < xAnPeaksData.ulNumMin; i++)
          {
           if (xAnPeaksData.pePeakTypeMin[i] &  PEAK_T_AN_MIN_IN_TNT_ZONE) LCDBUFFER_VLine_Flash(PeakIdx_to_LcdX(xAnPeaksData.pulIndexMin[i]), LIGHTYELLOW);  
           else if (xAnPeaksData.pePeakTypeMin[i] &  PEAK_T_AN_IN_TNT_ZONE) LCDBUFFER_VLineAlpha(PeakIdx_to_LcdX(xAnPeaksData.pulIndexMin[i]), LIGHTYELLOW, 0.5f);;
          } */
          
          /* Show other peaks or all */
          /* if (xAnPeaksData.pePeakTypeMin[i] == PEAK_T_AN_NO_ZONE) 
             LCDBUFFER_VLineAlpha(PeakIdx_to_LcdX(xAnPeaksData.pulIndexMin[i]), LIGHTYELLOW, 0.7f); */
	

  
}

void vShowPeakInfo(uint32_t ulExplNum, uint32_t x, uint32_t y)
{
  #define WIDTH 55
  #define HIGHT 35
  char stringBuf[20];
  if (xExplPeakInfo[ulExplNum].eKeep == KEEP)
  {
    LCDBUFFER_Alfa_Rectangle (x, y, x+WIDTH, y+HIGHT, Explosive_Set[ulExplNum].usColor, 0.75f);
    
    LCDBUFFER_Num_ShadowAlpha(xExplPeakInfo[ulExplNum].sScanId,x+5,y+5,Explosive_Set[ulExplNum].usColor,1,1,0.25);
                
    sprintf(stringBuf, "%2.1f", xExplPeakInfo[ulExplNum].fScanV);
    LCDBUFFER_Str_ShadowAlpha(stringBuf,x+5,y+5+10,Explosive_Set[ulExplNum].usColor,1,1,0.25f);
    LCDBUFFER_Str_ShadowAlpha("В",x+5+6*4+2,y+5+10,Explosive_Set[ulExplNum].usColor,1,1,0.25f);
                
    LCDBUFFER_Num_ShadowAlpha(xExplPeakInfo[ulExplNum].sAmpId, x+5,y+5+2*10,Explosive_Set[ulExplNum].usColor,1,1, 0.25f);
    
  }
}

void vShowLevelInfo(uint32_t ulExplNum, uint32_t x, uint32_t y)
{
  #define WIDTH 55
  #define HIGHT 35
 
  LCDBUFFER_Alfa_Rectangle (x, y, x+WIDTH, y+HIGHT, Explosive_Set[ulExplNum].usColor, 0.75f);
           
            /* Show level window */
  char stringBuf[11];
            //LCDBUFFER_Alfa_Rectangle (10, 230-5-5, 65, 260-5, LIGHTYELLOW, 0.75f);
  sprintf(stringBuf, "S:%5.1f%%", 100.f * Explosive_Set[ulExplNum].usLevelAlarm/32767.f);
  LCDBUFFER_Str_ShadowAlpha(stringBuf,x+5,y+5,Explosive_Set[ulExplNum].usColor,1,1, 0.25);
  sprintf(stringBuf, "%7.1f", xTntPeakInfo.fScanV);
  sprintf(stringBuf, "%s%s", stringBuf, "В");
  LCDBUFFER_Str_ShadowAlpha(stringBuf,x+5,y+5+10,Explosive_Set[ulExplNum].usColor,1,1,0.25);
  sprintf(stringBuf, "%7.1f", 100.f * xExplPeakInfo[ulExplNum].sAmpId/65535.f);
  sprintf(stringBuf, "%s%s", stringBuf, "%" );
  LCDBUFFER_Str_ShadowAlpha(stringBuf,x+5,y+5+20,Explosive_Set[ulExplNum].usColor,1,1,0.25);
  
  
}


/* Показывает пики Repera 
*************************
*************************/
void rp_peaks_show(void)
{       
  
        /* Dtaw reper peaks line*/
	for (uint32_t i = 0; i < xRpPeaksData.ulNumMin; i++)
	{
          switch (xRpPeaksData.pePeakTypeMin[i])
          {
            case PEAK_T_RP_IN_ZONE:
              LCDBUFFER_VLine_Bold(PeakIdx_to_LcdX(xRpPeaksData.pulIndexMin[i]), LIGHTBLUE);
            break;
            case PEAK_T_RP_MIN_IN_ZONE:
              LCDBUFFER_VLine_FlashMini(PeakIdx_to_LcdX(xRpPeaksData.pulIndexMin[i]), LIGHTBLUE);
            break;
            case PEAK_T_RP_OUT_ZONE:
              LCDBUFFER_VLine(PeakIdx_to_LcdX(xRpPeaksData.pulIndexMin[i]), LIGHTBLUE);
            break;
            default:;
          }
	}

        if (xRpPeakInfo.eKeep == KEEP)
	{
           char stringBuf[5];
           LCDBUFFER_Alfa_Rectangle (180-5, 270-5-5, 230, 300-5, LIGHTBLUE, 0.75f);
 	   
           /* Write scan voltage in index of spectra array */
           LCDBUFFER_Num_Shadow(xRpPeakInfo.sScanId,180,270-5,BLUE,1,1);
           
           /* Пишем напряжение скана пика реапера в вольтах */
           sprintf(stringBuf, "%2.1f", xRpPeakInfo.fScanV);
           LCDBUFFER_Str_Shadow(stringBuf,180,280-5,LIGHTBLUE,1,1);
           LCDBUFFER_Str_Shadow("В",205,280-5,LIGHTBLUE,1,1);
           
           LCDBUFFER_Num_Shadow(xRpPeakInfo.sAmpId,180,290-5,LIGHTBLUE,1,1);
        } 
}

//==================================================
// Нахожу пики в зонах обнаружения каждого вещества
// Нахожу пик с максимальным уровнем
// Отображаю градиент на дисплее
//==================================================
void gradient_show (void)
{
  
 float pr;
 static float nowPr = 00.1f;
  
  if (xRpPeakInfo.eKeep == KEEP) 
    pr = (float)xTntPeakInfo.sAmpId/ (2.f*32765);
  else
    pr =  0.015;
  
  float err = (pr-nowPr);
  
  if (err > 0) 
  {
    nowPr += 0.1*err;
  }
  else 
  {
    nowPr += 0.01*err;
  }
  if (nowPr > 1.0f) nowPr = 1.0f;
  if (nowPr < 0.01f) nowPr = 0.01f;
  
  
  LCDBUFFER_ProgressBarGradiend(33, 74, 51, 216, GREEN , nowPr);


 
  //LCDBUFFER_Gradient_RectangleAlpha (10, 10, 100, 310, GREEN,RED);
  
  AlarmCalculateForGradient();
  
         /*
	if ((SETPARAMS.iFlagBits & (1<<6)) == 0)
	{
	
	
	
	// Нахождение уровня для каждого вещества
	for (u8 j=0; j<E_COUNT; j++)	// Для каждого вещества
	{
		// Обнуление амплитуды максимального пика
		E_Settings[j].Level=0;
		// Если доверительный интервал = 0
		if (E_Settings[j].Itrv==0) continue;
		// Защита от некорректно заданных значений
		if ((E_Settings[j].Itrv < E_ITRV_MIN) || (E_Settings[j].Itrv > E_ITRV_MAX)) E_Settings[j].Itrv=E_ITRV_MIN;
		if ((E_Settings[j].Pos - E_Settings[j].Itrv) < 0) E_Settings[j].Pos=E_Settings[j].Itrv;
		if ((E_Settings[j].Pos + E_Settings[j].Itrv) > (SPECTRA_SIZE-1)) E_Settings[j].Pos=E_Settings[j].Itrv;
		// Зона обнаружения
		u16 Zone_L = E_Settings[j].Pos - E_Settings[j].Itrv;
		u16 Zone_R = E_Settings[j].Pos + E_Settings[j].Itrv;
		// Обрабатываю найденные пики
		for (u16 i=0; i<num_absop_peaks; i++)
		{
			if ((absop_peaks[i]>Zone_L) && (absop_peaks[i]<Zone_R))	// Пик находится в зоне обнаружения
			{
				// Из всех найденных пиков нахожу пик с максимальной амплитудой
				if (((-((s16)GETPARAMS.Spectra[0][absop_peaks[i]] - (s16)GETPARAMS.Spectra[0][emi_peaks[i]]))) > (E_Settings[j].Level)) E_Settings[j].Level=-((s16)GETPARAMS.Spectra[0][absop_peaks[i]] - (s16)GETPARAMS.Spectra[0][emi_peaks[i]]);
			};
		};
		// Отображаю название вещества на дисплей
		if (E_Settings[j].Level!=0) LCDBUFFER_Str(E_Settings[j].Name,150,40+(20*j),RED,BLACK,2,2);
	};
	// Нахождение максимального уровня среди всех веществ
	u16 MaxLevel=0;
	for (u8 j=0; j<E_COUNT; j++)
	{
		if (E_Settings[j].Level > MaxLevel) MaxLevel=E_Settings[j].Level;
	}
	// Вкл пищалки
	if (MaxLevel!=0) Buzzer_Tnt_On(); else Buzzer_Tnt_Off();
	// Защита от переполнения, т.к. амплитуда может быть > 16000
	if (MaxLevel>16000) MaxLevel=16000;
	// Обновляю целевой уровень градиента
	Grad_Lvl_Trgt = MaxLevel;
	
	
	
	}
	else
	{
	
	
	
	if (bug_actual)
	{
		u8 bug_command = bug_get_cmd();
		switch (bug_command)
		{
			case 20:
			{
				//Buzzer_tnt_on();
				if (Grad_Lvl_Trgt<15900) Grad_Lvl_Trgt+=100;
				u8 bug_name=0;
				LCDBUFFER_Str(E_Settings[bug_name].Name,150,40+(20*bug_name),RED,BLACK,2,2);
			} break;
			case 10:
			{
				//Buzzer_tnt_on();
				if (Grad_Lvl_Trgt<15900) Grad_Lvl_Trgt+=100;
				u8 bug_name=1;
				LCDBUFFER_Str(E_Settings[bug_name].Name,150,40+(20*bug_name),RED,BLACK,2,2);
			} break;
			case 30:
			{
				//Buzzer_tnt_on();
				if (Grad_Lvl_Trgt<15900) Grad_Lvl_Trgt+=100;
				u8 bug_name=2;
				LCDBUFFER_Str(E_Settings[bug_name].Name,150,40+(20*bug_name),RED,BLACK,2,2);
			} break;
			case 40:
			{
				//Buzzer_tnt_on();
				if (Grad_Lvl_Trgt<15900) Grad_Lvl_Trgt+=100;
				u8 bug_name=3;
				LCDBUFFER_Str(E_Settings[bug_name].Name,150,40+(20*bug_name),RED,BLACK,2,2);
			} break;
			case 50:
			{
				//Buzzer_tnt_on();
				if (Grad_Lvl_Trgt<15900) Grad_Lvl_Trgt+=100;
				u8 bug_name=0;
				LCDBUFFER_Str(E_Settings[bug_name].Name,150,40+(20*bug_name),RED,BLACK,2,2);
			} break;
			default:
			{
				//Buzzer_off();
				Grad_Lvl_Trgt=0;
			} break;
		}
	}
	else
	{
		//Buzzer_off();
		Grad_Lvl_Trgt=0;
	}
	
	
	
	}
	
	
	
	
	// Отображаю на дисплей текущий уровень градиента
	LCDBUFFER_Gradient_Rectangle (0, (u16)((LCD_HEIGHT-10)-(Grad_Lvl_Curr/(16000.0F/(LCD_HEIGHT-10)))), LCD_WIDTH/3, LCD_HEIGHT-1, 0x380F, RED);
	LCDBUFFER_Rectangle (0, 0, LCD_WIDTH/3, LCD_HEIGHT-1, WHITE, 0);
	// Отображение Уровней всех веществ
	//for (u8 j=0; j<EXPLOSIVES_COUNT; j++) LCDBUFFER_PutNum(EXPLOSIVE_Settings[j].Level,5,40+(20*j),YELLOW,BLACK,2,2);
*/
}




//=============================
// Фильтр - скользящее среднее
//=============================

void Filter_GAN (void)
{
	float Sum;
	float Buf[GAN_FILTER];
	float Out[GAN_SIZE+GAN_FILTER];
	// Добавляю точки в начало массива для корректного усреднения
	for (u8 j=0; j<GAN_FILTER; j++) Buf[j]=GETPARAMS.eSpectra[(GAN_SIZE-1-GAN_FILTER)+j];
	for (u8 i=0; i<(GAN_SIZE+GAN_FILTER); i++)
	{
		// Помещаю данные из массива во временный буфер
		if (i<GAN_SIZE) Buf[i&(GAN_FILTER-1)] = GETPARAMS.eSpectra[i];
		// Добавляю точки в конец массива для корректного усреднения
		else Buf[i&(GAN_FILTER-1)] = GETPARAMS.eSpectra[i-GAN_SIZE];
		// Усреднение в пределах заданного окна
		Sum=0;
		for (u8 j=0; j<GAN_FILTER; j++) Sum+=Buf[j];
		Sum=Sum/GAN_FILTER;
		Out[i]=Sum;
	}
	// Возвращаю массив усреднённых значений
	for (u8 i=0; i<GAN_SIZE; i++) GETPARAMS.eSpectra[i]=Out[i+(GAN_FILTER/2)];	// сдвиг массива на 1/2 буфера
}


//===========================================================
// Отображает на дисплей осциллограмму ГАН + сетку + подписи
//===========================================================
void Show_Gan (void)
{
	// Сетка
	for (u8 j=0; j<GAN_LINES; j++)
	{
		u16 Coordinate = (LCD_H/(GAN_LINES+1))*(j+1);
		s16 Voltage = GAN_MAX-(((GAN_MAX-GAN_MIN)/(GAN_LINES+1))*(j+1));
		LCDBUFFER_Line(0, Coordinate, LCD_W, Coordinate, GRID_COLOR);
		LCDBUFFER_Num_Shadow(Voltage, 0, Coordinate, GRIDTEXTS_COLOR, 1, 1);
	}
	// Фильтрация
	Filter_GAN();
	// Осциллограмма
	u16 Y_start=0;
	u16 Y_end=0;
	for (u8 j=0; j<4; j++)
	{
		for (u8 k=0; k<GAN_SIZE; k++)
		{
			if ((k+(j*GAN_SIZE))>LCD_W) break;	// Чтобы не выйти за границу дисплея по ширине
			Y_end = (u16)(LCD_H - ((GETPARAMS.eSpectra[k]-GAN_MIN) / (float) ((GAN_MAX-GAN_MIN)/(float)LCD_H)));
			if (Y_end>LCD_H) Y_end=LCD_H;	// Чтобы не выйти за границу дисплея по высоте
			if (!((j==0)&&(k==0)))	// Первую линию пропускаю
			LCDBUFFER_Line(k+(j*GAN_SIZE), Y_start, k+(j*GAN_SIZE), Y_end, WHITE);
			Y_start = Y_end;
		}
	}
}


void Show_Info_Window(uint32_t color)
{
  char stringBuf[20];
  /* Parameter list set */
  LCDBUFFER_Str_Shadow("Частота Vrf, Гц =",20,20,color,1,1);
  sprintf(stringBuf, "%6.1f", GETPARAMS.eFreq);
  LCDBUFFER_Str_Shadow(stringBuf,180,20,color,1,1);
                        
 LCDBUFFER_Str_Shadow("Температ. ГАН, гр. С =",20,30,color,1,1);
  sprintf(stringBuf, "%3.1f", GETPARAMS.ganTemp);
  LCDBUFFER_Str_Shadow(stringBuf,180,30,color,1,1);
                        
  sprintf(stringBuf, "%3.1f", GETPARAMS.ganCurrent);
  LCDBUFFER_Str_Shadow("Ток ГАН, А =",20,40,color,1,1);
  LCDBUFFER_Str_Shadow(stringBuf,180,40,color,1,1);
                        
  sprintf(stringBuf, "%3.1f", GETPARAMS.ganHvVoltsFeedBack);
  LCDBUFFER_Str_Shadow("Напрядение HV, В =",20,50,color,1,1);
  LCDBUFFER_Str_Shadow(stringBuf,180,50,color,1,1);
                        
  sprintf(stringBuf, "%3.1f", GETPARAMS.ganVrf1garm);
  LCDBUFFER_Str_Shadow("1-я гарм, В =",20,60,color,1,1);
  LCDBUFFER_Str_Shadow(stringBuf,180,60,color,1,1);
                        
  sprintf(stringBuf, "%3.1f", GETPARAMS.ganVrf2garm);
  LCDBUFFER_Str_Shadow("2-я гарм, В =",20,70,color,1,1);
  LCDBUFFER_Str_Shadow(stringBuf,180,70,color,1,1);
                        
  sprintf(stringBuf, "%3.1f", GETPARAMS.ganPh1garm);
  LCDBUFFER_Str_Shadow("Фаза 1-я гарм, гр.=",20,80,color,1,1);
  LCDBUFFER_Str_Shadow(stringBuf,180,80,color,1,1);
                        
  sprintf(stringBuf, "%3.1f", GETPARAMS.ganPh2garm);
  LCDBUFFER_Str_Shadow("Фаза 2-я гарм, гр.=",20,90,color,1,1);
  LCDBUFFER_Str_Shadow(stringBuf,180,90,color,1,1);
                        
  sprintf(stringBuf, "%3.1f", GETPARAMS.ganPhDelta);
  LCDBUFFER_Str_Shadow("Фаза дельта, гр.=",20,100,color,1,1);
  LCDBUFFER_Str_Shadow(stringBuf,180,100,color,1,1);
  
  sprintf(stringBuf, "%4.1f", GETPARAMS.ganVrf1garm+GETPARAMS.ganVrf2garm);
  LCDBUFFER_Str_Shadow("Сумма амплитуд, В.=",20,110,color,1,1);
  LCDBUFFER_Str_Shadow(stringBuf,180,110,color,1,1);
  
  sprintf(stringBuf, "%2.1f", GETPARAMS.ganVrf1garm/GETPARAMS.ganVrf2garm);
  LCDBUFFER_Str_Shadow("Соотнош. гарм. =",20,120,color,1,1);
  LCDBUFFER_Str_Shadow(stringBuf,180,120,color,1,1);
  
  sprintf(stringBuf, "%d", GETPARAMS.reperDac);
  LCDBUFFER_Str_Shadow("Напряжение репера =",20,130,color,1,1);
  LCDBUFFER_Str_Shadow(stringBuf,180,130,color,1,1);
  
  sprintf(stringBuf, "%d", 0x3FF&GETPARAMS.cntSpctr);
  LCDBUFFER_Str_Shadow("№ спектра =",20,140,color,1,1);
  LCDBUFFER_Str_Shadow(stringBuf,180,140,color,1,1);
  
  LCDBUFFER_Str_Shadow("Температ. приб С =",20,150,color,1,1);
  sprintf(stringBuf, "%3.1f", GETPARAMS.mainBoardTemperature);
  LCDBUFFER_Str_Shadow(stringBuf,180,150,color,1,1);

  LCDBUFFER_Str_Shadow("Температ. нар. =",20,160,color,1,1);
  sprintf(stringBuf, "%3.1f", GETPARAMS.tempExt);
  LCDBUFFER_Str_Shadow(stringBuf,180,160,color,1,1); 
  
  LCDBUFFER_Str_Shadow("Задан. знач Vrf. =",20,170,color,1,1);
  sprintf(stringBuf, "%3.d", GETPARAMS.vrfRef);
  LCDBUFFER_Str_Shadow(stringBuf,180,170,color,1,1);  
  
  LCDBUFFER_Str_Shadow("Reper wiper. =",20,180,color,1,1);
  sprintf(stringBuf, "%3.d", GETPARAMS.uVozdReper);
  LCDBUFFER_Str_Shadow(stringBuf,180,180,color,1,1); 
  
  LCDBUFFER_Str_Shadow("Влажность PH =",20,190,color,1,1);
  sprintf(stringBuf, "%3.1f", GETPARAMS.mainBoardRh);
  LCDBUFFER_Str_Shadow(stringBuf,180,190,color,1,1); 
  
  LCDBUFFER_Str_Shadow("Напряжение Ак. mV =",20,200,color,1,1);
  sprintf(stringBuf, "%6d", batt_voltage_mV);
  LCDBUFFER_Str_Shadow(stringBuf,180,200,color,1,1); 
  
  LCDBUFFER_Str_Shadow("Темпер. нагреваетля =",20,210,color,1,1);
  //sprintf(stringBuf, "%3.1f", GETPARAMS.fHeaterTemp);
  LCDBUFFER_Str_Shadow(stringBuf,180,210,color,1,1); 
  
  if (SETPARAMS.systemSettings.reperHeatOn != 0)
  {
    LCDBUFFER_Str_Shadow("HEAT ON!!!",20,10,RED,1,1);
  }
  asm("NOP");
  
  LCDBUFFER_Alfa_Rectangle (10, 10, 240-10, 220, WHITE, 0.9);
}

/* Show line of reper zone */
void vShowRpBorders (void)
{
         LCDBUFFER_VLine (PeakIdx_to_LcdX(ulGetScanId(Z_LEFT)), DARK_GRAY);
         LCDBUFFER_VLine (PeakIdx_to_LcdX(ulGetScanId(Z_RIGHT)), DARK_GRAY);

         LCDBUFFER_VLineAlpha (PeakIdx_to_LcdX(ulGetRpZoneId(Z_LEFT)), BLUE, 0.75);
         LCDBUFFER_VLineAlpha (PeakIdx_to_LcdX(ulGetRpZoneId(Z_RIGHT)), BLUE, 0.75);
}

/* Show reper zone in down rectangular */
void vShowRpZone(void)
{
  int16_t usColor = DARK_GRAY;
  /* Chek reper is find or not, for color select */
  if (eRpIsKeep() == KEEP)
  {
    usColor = LIGHTBLUE;
  }
  
  uint32_t ulZoneLeftLcdX = PeakIdx_to_LcdX(ulGetRpZoneId(Z_LEFT));
  uint32_t ulZoneRightLcdX = PeakIdx_to_LcdX(ulGetRpZoneId(Z_RIGHT));  
  
  LCDBUFFER_ShowZone (ulZoneLeftLcdX, ulZoneRightLcdX, usColor, 0.5, 0);
  
}

/* Show explozive zone in down rectangular */
void vSnowExpZone(uint32_t num)
{
   
   LCDBUFFER_ShowZone (PeakIdx_to_LcdX(xExplZone[num].ulLeftId), PeakIdx_to_LcdX(xExplZone[num].ulRightId), Explosive_Set[num].usColor, 0.7, 0);
  
};

void vSnowExpClbZone(uint32_t num, float lvl)
{
   
   LCDBUFFER_ShowClbZone (PeakIdx_to_LcdX(xExplZone[num].ulLeftId), PeakIdx_to_LcdX(xExplZone[num].ulRightId),Explosive_Set[num].usColor, 0.7, lvl);
  
};

void vSnowTntClbZone(uint32_t num, float lvl)
{ 
  if (!(eRpIsKeep() == KEEP))
  {
    LCDBUFFER_ShowZone (PeakIdx_to_LcdX(ulGetRpZoneClbId(Z_LEFT)), PeakIdx_to_LcdX(ulGetRpZoneClbId(Z_RIGHT)), Explosive_Set[0].usColor, 0.5, 0);
  }
   
   LCDBUFFER_ShowClbZone (PeakIdx_to_LcdX(ulGetRpZoneClbId(Z_LEFT)), PeakIdx_to_LcdX(ulGetRpZoneClbId(Z_RIGHT)),Explosive_Set[0].usColor, 0.7, lvl);
};


//============================================
// Выполняет ряд задач:
// -очистка фрейм-буфера
// -поиск пиков
// -отображение пиков
// -отображение "границ обнаружения"
// -отображение графика
// -проверка наличия пиков в зоне обнаружения
// -отображение подписей к сетке
// -отображение индикатора батарейки
// -вывод фрейм-буфера на дисплей
//============================================


void Display_Automat (void)
{
  //rpIdWnd = rpPrToId(rpPrWnd);
  //rpIdOffset = rpPrToId(rpPrOffset); 
        /* Off buzzer alarm in all mode exclusive of DisplayMode_Spectrum */
        if (DisplayMode!=DisplayMode_Spectrum)
        {
          Buzzer_Tnt_Off();
	}
        
        if ((LcdBufferNotUsed) )
	{
                if (DisplayMode==DisplayMode_Security)
                {
                  vSecurityDisplay();                  
                }
		// Обработка принятых данных
		if (DisplayMode==DisplayMode_Spectrum)
		{
                        //Buzzer_Tnt_Off();
                        //GPIO_ResetBits(GPIOC, GPIO_Pin_6);  
                        vInDisplayMode_Spectrum(); 
                        battery_icon();
                        AlarmCalculate(); //пищать или нет
		}
		if (DisplayMode==DisplayMode_Calibration)
		{
			vInDisplayMode_Spectrum();
                        vMenuClbShow(125,30);
                        eClbZone_TypeDef eZone = eGetClbZone();
                        if ((eZone != eCLB_ZONE_NONE) && (eZone != eCLB_ZONE_TNT))
                        {
                          vSnowExpClbZone(eZone, Explosive_Set[eZone].usLevelAlarm/32767.f);
                        }
                        if (eZone == eCLB_ZONE_TNT) 
                        {
                          vSnowTntClbZone(0, Explosive_Set[0].usLevelAlarm/32767.f);
                        }
                            //LCDBUFFER_Clear();
			//peaks_search(&GETPARAMS.Spectra[0][0]);
			/*show_grid_texts();
			peaks_show();
			Show_Spectrum(0,WHT_YELLOW);
			Show_Calibration_Borders();
			LCDBUFFER_Str("Режим калибровки",5,0,RED,BLACK,2,2);
			LCDBUFFER_Str(E_Settings[E_Sel].Name,5,20,RED,BLACK,2,2);	// Название вещества
			if (E_Settings[E_Sel].Itrv) LCDBUFFER_Str("Вкл",150,20,RED,BLACK,2,2);	// Флаг активации
			else LCDBUFFER_Str("Откл",150,20,RED,BLACK,2,2);
			LCDBUFFER_Str("MENU - вещество",130,60,YELLOW,BLACK,1,1);
			LCDBUFFER_Str("OK - вкл/откл",130,70,YELLOW,BLACK,1,1);
			LCDBUFFER_Str("ESC - выход",130,80,YELLOW,BLACK,1,1);
			LCDBUFFER_Num(bug_get_cmd(),0,0,YELLOW,BLACK,1,1);	// bug cmd
			LCDBUFFER_Num(bug[0],30,0,YELLOW,BLACK,1,1);	// bug1
			LCDBUFFER_Num(bug[1],30,10,YELLOW,BLACK,1,1);	// bug2
			LCDBUFFER_Num(bug[2],30,20,YELLOW,BLACK,1,1);	// bug3
			LCDBUFFER_Num(bug[3],30,30,YELLOW,BLACK,1,1);	// bug4
			LCDBUFFER_Num(bug[4],30,40,YELLOW,BLACK,1,1);	// bug5
			//LCDBUFFER_Num(Grad_Lvl_Curr,60,0,YELLOW,BLACK,1,1);	// bug actual
			//LCDBUFFER_Num(Grad_Lvl_Trgt,60,10,YELLOW,BLACK,1,1);	// bug actual*/
		}
		if (DisplayMode==DisplayMode_Gradient)
		{
			gradientLogoShw();
			//peaks_search(&GETPARAMS.Spectra[0][0]);
			gradient_show();
			battery_icon();
                        LCDBUFFER_Num (GETPARAMS.iTrackValue[2],150,5,WHITE,BLACK,1,1);  // Уровень
			if (Disorber_sec)
			{
				LCDBUFFER_Str ("Десорбер",100,20,RED,BLACK,1,1);
				LCDBUFFER_Num (Disorber_sec,150,20,RED,BLACK,1,1);
			}
		}
		if (DisplayMode==DisplayMode_Gan)
		{
                        LCDBUFFER_Clear();
			Show_Gan();
                        LCDBUFFER_Str("Осциллограмма ГАН",5,0,YELLOW,BLACK,2,2);
                        Show_Info_Window(LIGHTYELLOW);
                        asm("NOP");
		}
		if (DisplayMode==DisplayMode_PowerOff)
		{
                  //About_logo();
		//	LCDBUFFER_Clear();
		//	LCDBUFFER_Str("Для включения",40,20,WHITE,BLACK,2,2);
		//	LCDBUFFER_Str("питания нажмите",25,40,WHITE,BLACK,2,2);
		//	LCDBUFFER_Str("и удерживайте",35,60,WHITE,BLACK,2,2);
		//	LCDBUFFER_Str("клавишу ESC",50,80,WHITE,BLACK,2,2);
		//	if ((batt_voltage_mV) && (batt_voltage_mV<BATT_VMIN))
		//	{
		//		LCDBUFFER_Str("АКБ разряжена!",5,150,RED,BLACK,2,2);
		//	}
		//	LCDBUFFER_Str("SW 20150901-1627",20,300,BLUE,BLACK,2,2);
                        //LCDBUFFER_Clear();
                        Task_add(TASK_BATT_CHECK);      
                        About_logo();
                }
                if (DisplayMode==DisplayMode_Starting)
                {
                  LCDBUFFER_Clear();
                  startingLogo();
                }
		//Task_add(TASK_BATT_ICON);
                
                
                if (DisplayMode==DisplayMode_Menu)
                {
                      vMenuUpdate();
                }               
                
                Task_add(TASK_LCD_UPDATE);
	}
}

void vInDisplayMode_Spectrum(void)
{
  LCDBUFFER_Clear();
                        
  show_grid_texts();
  rp_peaks_show();
	
  peaks_show();
			
                        
  //Show_Spectrum(&psSpectrHistory[(ulPntSpctrHstr-0)&(HISTORY_DEEP-1)][0],YELLOW_2);
  Show_Spectrum(&psSpectrHistory[(ulPntSpctrHstr-1)&(HISTORY_DEEP-1)][0],YELLOW_5);
  Show_Spectrum(&psSpectrHistory[(ulPntSpctrHstr-2)&(HISTORY_DEEP-1)][0],YELLOW_6);
  Show_Spectrum(&psSpectrHistory[(ulPntSpctrHstr-3)&(HISTORY_DEEP-1)][0],YELLOW_7);
                        
                        
  // filterSimple(spectrZoomAn, 25, SPECTRA_SIZE);
                        
  Show_Spectrum(psSpectrAn,YELLOW_1);
  Show_Spectrum(psSpectrRp, LIGHTBLUE);
  
  
  vShowRpBorders();                
  vShowRpZone();                        
  /* Show explosive zone */
  for (uint32_t i = 1; i < EXPLOSIV_COUNT; i++ )
  {
    if (Explosive_Set[i].eOnOff == ON)
    {
      vSnowExpZone(i);
    }
  }

  
  LCDBUFFER_Num (GETPARAMS.iTrackValue[2],150,5,WHITE,BLACK,1,1);  // Уровень
  LCDBUFFER_FloatV (GETPARAMS.iTrackValue[4],100,5,BLUE);  // От
  LCDBUFFER_FloatV (GETPARAMS.iTrackValue[5],200,5,BLUE);  // До
  if (Disorber_sec)
  {
    LCDBUFFER_Str ("Десорбер",100,20,RED,BLACK,1,1);
    LCDBUFFER_Num (Disorber_sec,150,20,RED,BLACK,1,1);
  }
  
  if (usMenuFlags.bits.bit0 == 1)
  {
    Show_Info_Window(DARK_GRAY);
    //LCDBUFFER_Alfa_Rectangle (10, 10, 240-10, 220, WHITE, 0.9);
  }
  else
  {
    LCDBUFFER_Alfa_Rectangle (10, 10, 240-10, 220, WHITE, 1.f);
  }
  // Подмена спектра
  // LCDBUFFER_Num(Grad_Lvl_Trgt_Spctr,50,5,YELLOW,BLACK,1,1);
  if (bug_actual)
  {
    if (Grad_Lvl_Trgt_Spctr<15900) Grad_Lvl_Trgt_Spctr+=100;
  }
  else
  {
    if (Grad_Lvl_Trgt_Spctr>210) Grad_Lvl_Trgt_Spctr-=100;
    else Grad_Lvl_Trgt_Spctr=0;
  }
}


