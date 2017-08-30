#ifndef __RGB_H		// Блокируем повторное включение этого модуля
#define __RGB_H

#include "stdint.h"

typedef struct RgbColor
{
    unsigned char r;
    unsigned char g;
    unsigned char b;
} RgbColor;

typedef struct HsvColor
{
    unsigned char h;
    unsigned char s;
    unsigned char v;
} HsvColor;

RgbColor HsvToRgb(HsvColor hsv);
HsvColor RgbToHsv(RgbColor rgb);
uint16_t xRgbToRgb565(RgbColor xColor);
RgbColor xRgb565ToRgb(uint16_t xColor565);
/* alpha = 0 trancparence = 0%, alpha = 1 trancparence = 100% */ 
uint16_t usAlphaBlending(uint16_t col565_1, uint16_t col565_2, float alph);

#endif