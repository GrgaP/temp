/**
 ******************************************************************************
 * @file    ltdc_drv_cfg.h
 * @author  SvD
 * @version V1.0.0
 * @date    30.08.2016
 * @brief       Brief description.
 * @details     Detailed description.
 *              $Id$
 ****************************************************************************** 
 */ 
 
#ifndef __LTDC_DRV_CFG_H_		
#define __LTDC_DRV_CFG_H_

/* Screen size */ 
#define LCD_WIDTH 240
#define LCD_HIGH 320

/* Screen sync parameters*/
#define LCD_HSYNC	10	// 10
#define LCD_HBP		20	// 20
#define LCD_HAREA	LCD_WIDTH	// 320
#define LCD_HFP		10	// 10
#define LCD_VSYNC	2	// 2
#define LCD_VBP		2	// 2
#define LCD_VAREA	LCD_HIGH 	// 240
#define LCD_VFP		4	// 4

/* Values for LTDC config registers */
#define HSW		(LCD_HSYNC - 1)
#define AHBP		(LCD_HSYNC + LCD_HBP - 1)
#define AAW		(LCD_HSYNC + LCD_HBP + LCD_HAREA - 1)
#define TOTALW		(LCD_HSYNC + LCD_HBP + LCD_HAREA + LCD_HFP - 1)
#define VSH		(LCD_VSYNC - 1)
#define AVBP		(LCD_VSYNC + LCD_VBP - 1)
#define AAH		(LCD_VSYNC + LCD_VBP + LCD_VAREA - 1)
#define TOTALH		(LCD_VSYNC + LCD_VBP + LCD_VAREA + LCD_VFP - 1)

//#define LTDC_COLKEY_USE FALSE
//#define LTDC_DITH_USE FALSE

/* Layers congiuration */
#define LTDC_USE_LAYER 2

#define LAYER_WIN_OFX 0
#define LAYER_WIN_OFY 0

#define LAYER_WIN_SZX 240
#define LAYER_WIN_SZY 320

#define LAYER_PF LTDC_PIXEL_FORMAT_RGB565
#define LAYER_ALPHA 0xff

#if ( LTDC_USE_LAYER > 1)
	
	#define LAYER1_WIN_OFX 0
	#define LAYER1_WIN_OFY 0

	#define LAYER1_WIN_SZX 240
	#define LAYER1_WIN_SZY 320

	#define LAYER1_ALPHA 0xff
	
#endif

/* Frame buffer parameters */
#define LCD_BYTE_P_PIXEL 2
#define LCD_FB_SIZE (LCD_HIGH*LCD_WIDTH*LCD_BYTE_P_PIXEL)

/* ILI9341 used pins */

/* ILI9341 used spi */


#endif /* __LTDC_DRV_CFG_H_ */


