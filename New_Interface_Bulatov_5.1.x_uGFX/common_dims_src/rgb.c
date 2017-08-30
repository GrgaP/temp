
#include "rgb.h"
#include "stdint.h"

RgbColor HsvToRgb(HsvColor hsv)
{
    RgbColor rgb;
    unsigned char region, remainder, p, q, t;

    if (hsv.s == 0)
    {
        rgb.r = hsv.v;
        rgb.g = hsv.v;
        rgb.b = hsv.v;
        return rgb;
    }

    region = hsv.h / 43;
    remainder = (hsv.h - (region * 43)) * 6; 

    p = (hsv.v * (255 - hsv.s)) >> 8;
    q = (hsv.v * (255 - ((hsv.s * remainder) >> 8))) >> 8;
    t = (hsv.v * (255 - ((hsv.s * (255 - remainder)) >> 8))) >> 8;

    switch (region)
    {
        case 0:
            rgb.r = hsv.v; rgb.g = t; rgb.b = p;
            break;
        case 1:
            rgb.r = q; rgb.g = hsv.v; rgb.b = p;
            break;
        case 2:
            rgb.r = p; rgb.g = hsv.v; rgb.b = t;
            break;
        case 3:
            rgb.r = p; rgb.g = q; rgb.b = hsv.v;
            break;
        case 4:
            rgb.r = t; rgb.g = p; rgb.b = hsv.v;
            break;
        default:
            rgb.r = hsv.v; rgb.g = p; rgb.b = q;
            break;
    }

    return rgb;
}

HsvColor RgbToHsv(RgbColor rgb)
{
    HsvColor hsv;
    unsigned char rgbMin, rgbMax;

    rgbMin = rgb.r < rgb.g ? (rgb.r < rgb.b ? rgb.r : rgb.b) : (rgb.g < rgb.b ? rgb.g : rgb.b);
    rgbMax = rgb.r > rgb.g ? (rgb.r > rgb.b ? rgb.r : rgb.b) : (rgb.g > rgb.b ? rgb.g : rgb.b);

    hsv.v = rgbMax;
    if (hsv.v == 0)
    {
        hsv.h = 0;
        hsv.s = 0;
        return hsv;
    }

    hsv.s = (unsigned char)(255.f * (long)(rgbMax - rgbMin) / (float)hsv.v);
    if (hsv.s == 0)
    {
        hsv.h = 0;
        return hsv;
    }

    if (rgbMax == rgb.r)
        hsv.h = 0 + 43 * (rgb.g - rgb.b) / (rgbMax - rgbMin);
    else if (rgbMax == rgb.g)
        hsv.h = 85 + 43 * (rgb.b - rgb.r) / (rgbMax - rgbMin);
    else
        hsv.h = 171 + 43 * (rgb.r - rgb.g) / (rgbMax - rgbMin);

    return hsv;
}

RgbColor xRgb565ToRgb(uint16_t xColor565)
{
  RgbColor xColor = {0,0,0};
  
  xColor.r = (xColor565 >> 8) & ~0x7;
  xColor.g = (xColor565 >> 3) & ~0x3;
  xColor.b = (xColor565 << 3) & ~0x7;
  
  return xColor;
}

uint16_t xRgbToRgb565(RgbColor xColor)
{
  uint16_t sColor565 = 0;
  
   
  sColor565 |= ((uint16_t)(xColor.r) & ~0x7) << 8;
  sColor565 |= ((uint16_t)(xColor.g) & ~0x3) << 3;
  sColor565 |= (uint16_t)(xColor.b) >> 3 ; 

  return sColor565;
}

/* alpha = 0 trancparence = 0%, alpha = 1 trancparence = 100% */ 
uint16_t usAlphaBlending(uint16_t col565_1, uint16_t col565_2, float alph)
{
  RgbColor clr1, clr2, rClr;
  
  clr1 = xRgb565ToRgb(col565_1);
  clr2 = xRgb565ToRgb(col565_2);
  
  rClr.r = (uint8_t)((float)clr1.r * alph + (float)clr2.r * (1.0f - alph));
  rClr.g = (uint8_t)((float)clr1.g * alph + (float)clr2.g * (1.0f - alph));
  rClr.b = (uint8_t)((float)clr1.b * alph + (float)clr2.b * (1.0f - alph));

  return (xRgbToRgb565(rClr));
}







