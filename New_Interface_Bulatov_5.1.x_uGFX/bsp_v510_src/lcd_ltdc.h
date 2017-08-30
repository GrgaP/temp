#ifndef LCD_LTDC_H		// Блокируем повторное включение этого модуля
#define LCD_LTDC_H

#include "main.h"



#define LcdBufferNotUsed 1//((DMA2_Stream0->CR & DMA_SxCR_EN)==0)

extern LTDC_HandleTypeDef hltdc;

extern uint16_t* LCDBUFFER;		// Дисплейный буфер в ОЗУ микроконтроллера, 320*240 пикселей = 76800 слов (16 бит/пиксель)

//============================================================================
//    Набор стандартных цветов RGB565, чтобы не оперировать HEX-значениями
//============================================================================
#define BORDER_COLOR	        0x0014	// Темно-синий для границ обнаружения TNT
#define GRIDTEXTS_COLOR	        LIGHTYELLOW /*0x07E0*/	// Зелёный для подписей к сетке
#define GRID_COLOR		0x2945/*0x3186*/	// Темно-серый для сетки
#define RED      		0xF800	// Красный
#define GREEN    		0x07E0	// Зелёный
#define LIGHTGREEN    		0xBFF7	// Светло Зелёный
#define BLUE     		0x001F	// Синий
#define LIGHTBLUE     		0x0418	// Светлосиний Синий
#define YELLOW   		0xFFE0	// Жёлтый
#define LIGHTYELLOW   		0xFFF0	// Светло Жёлтый
#define BLACK    		0x0000	// Чёрный
#define WHITE    		0xFFFF	// Белый
#define WHT_YELLOW              0xFFF6  // Светложелтый
#define WHT_RED                 0xFD96  // Светокрасный
#define DARK_RED                0x7000  // Темнокрасный
#define DARK_GRAY               0x5ACB //Темносерый
#define ORANGE                  0xFD6B 
#define DARK_GREEN 0x7CA8

enum YELLOW_COLOR   { 
                      YELLOW_1 = 0xFFE5,  \
                      YELLOW_2 = 0xEF40, \
                      YELLOW_3 = 0xCE60, \
                      YELLOW_4 = 0x94A0, \
                      YELLOW_5 = 0x6B60, \
                      YELLOW_6 = 0x5AE0, \
                      YELLOW_7 = 0x4200, \
                      YELLOW_8 = 0x10A0
                     };

#define Y_ZONE          (LCD_HEIGHT-22)  // use in LCDBUFFER_ShowZone
#define Y_CLB_ZONE_MAX  (LCD_HEIGHT-22-25) 
#define Y_ZONE_H        (6)

void LCD_init();

////////////////////////////////////////////////
//    Секция прототипов локальных функций     //
////////////////////////////////////////////////
void LCD_BMP (u16 x0, u16 y0, u16 x1, u16 y1, u16* data);
void LCDBUFFER_BMP (u16 x0, u16 y0, u16 x1, u16 y1, u16* data);
void LCDBUFFER_Clear (void);
void LCDBUFFER_Pixel (u16 x, u16 y, u16 color);
void LCDBUFFER_PixelAlpha (u16 x, u16 y, u16 color, float alph);
void LCDBUFFER_Line (u16 x1, u16 y1, u16 x2, u16 y2, u16 color);
void LCDBUFFER_LineAlpha (u16 x1, u16 y1, u16 x2, u16 y2, u16 color, float alph);
void LCDBUFFER_Rectangle (u16 x0, u16 y0, u16 x1, u16 y1, u16 color, u8 fill);
void LCDBUFFER_Alfa_Rectangle (u16 x0, u16 y0, u16 x1, u16 y1, u16 color, float alf);
void LCDBUFFER_Gradient_Rectangle (u16 x0, u16 y0, u16 x1, u16 y1, u16 color1_RGB, u16 color2_RGB);
void LCDBUFFER_Gradient_RectangleAlpha (u16 x0, u16 y0, u16 x1, u16 y1, u16 color1, u16 color2);
void LCDBUFFER_ProgressBarGradiend(u16 x, u16 y, u16 w, u16 h, u16 clr, float set);
void LCDBUFFER_Inverse_Rectangle (u16 x0, u16 y0, u16 x1, u16 y1);
void LCDBUFFER_Char (u8 symbol, u16 x, u16 y, u16 t_color, u16 b_color, u8 zoom_width, u8 zoom_height);
void LCDBUFFER_Char_Shadow (u8 symbol, u16 x, u16 y, u16 t_color, u8 zoom_width, u8 zoom_height);
void LCDBUFFER_Char_ShadowAlpha (u8 symbol, u16 x, u16 y, u16 t_color, u8 zoom_width, u8 zoom_height, float alph);
void LCDBUFFER_Str (char *str, u16 x, u16 y,  u16 t_color, u16 b_color, u8 zoom_width, u8 zoom_height);
void LCDBUFFER_Str_Shadow (char *str, u16 x, u16 y,  u16 t_color, u8 zoom_width, u8 zoom_height);
void LCDBUFFER_Str_ShadowAlpha (char *str, u16 x, u16 y,  u16 t_color, u8 zoom_width, u8 zoom_height, float alph);
void LCDBUFFER_Num (s32 number, u16 x, u16 y, u16 t_color, u16 b_color, u8 zoom_width, u8 zoom_height);
void LCDBUFFER_Num_Shadow (s32 number, u16 x, u16 y, u16 t_color, u8 zoom_width, u8 zoom_height);
void LCDBUFFER_Num_ShadowAlpha (s32 number, u16 x, u16 y, u16 t_color, u8 zoom_width, u8 zoom_height, float alph);
void LCDBUFFER_Dma_Init (void);
void LCDBUFFER_BAAPixelAlpha (u16 x, u16 y, u16 color);
void LCDBUFFER_BAALine (u16 x1, u16 y1, u16 x2, u16 y2, u16 color);
void LCDBUFFER_ShowZone (u16 x1, u16 x2, u16 color, float alph, uint8_t n);
void LCDBUFFER_ShowClbZone (u16 x1, u16 x2, u16 color, float alph, float lvl);
void LCD_Update (void);

void LCDBUFFER_HLine (u16 y, u16 color);
void LCDBUFFER_VLine (u16 x, u16 color);
void LCDBUFFER_VLineAlpha (u16 x, u16 color, float alph);
void LCDBUFFER_VLine_Bold (u16 x, u16 color);
void LCDBUFFER_VLine_H_Bold (u16 x, u16 color);
void LCDBUFFER_VLine_HP_Bold (u16 x, u16 color);
void LCDBUFFER_VLine_Flash (u16 x, u16 color);
void LCDBUFFER_VLine_FlashMini (u16 x, u16 color);
void LCDBUFFER_FloatV (s16 voltage, u16 x, u16 y, u16 t_color);



#endif