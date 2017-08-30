/**
 ******************************************************************************
 * @file    fb_dma2d_test.h
 * @author  SvD
 * @version V1.0.0
 * @date    30.08.2016
 * @brief       Brief description.
 * @details     Detailed description.
 *              $Id$
 ****************************************************************************** 
 */ 
 
#ifndef __FB_DMA2DTEST_H_		
#define __FB_DMA2DTEST_H_

#define LCD_WIDTH 240
#define LCD_HIGH 320
#define LCD_BYTE_P_PIXEL 2
#define LCD_FB_SIZE (LCD_HIGH*LCD_WIDTH*LCD_BYTE_P_PIXEL)

#define LTDC_USE_LAYER 1

void test_module_init();

#endif /* __FB_DMA2DTEST_H_ */