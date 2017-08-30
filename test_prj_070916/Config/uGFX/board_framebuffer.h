/*
 * This file is subject to the terms of the GFX License. If a copy of
 * the license was not distributed with this file, you can obtain one at:
 *
 *              http://ugfx.org/license.html
 */

#include "ltdc_drv.h"
 
// Set this to your frame buffer pixel format.
#ifndef GDISP_LLD_PIXELFORMAT
	#define GDISP_LLD_PIXELFORMAT		GDISP_PIXELFORMAT_RGB888
#endif

#define USE_DMA2D TRUE

// Uncomment this if your frame buffer device requires flushing
#define GDISP_HARDWARE_FLUSH		TRUE

#ifdef GDISP_DRIVER_VMT



	static void board_init(GDisplay *g, fbInfo *fbi) {
		// TODO: Initialize your frame buffer device here
                
                BSP_SDRAM_Init();
          
                LTDC_drv_init();

                // TODO: Set the details of the frame buffer
		g->g.Width = 480;
		g->g.Height = 272;
		g->g.Backlight = 100;
		g->g.Contrast = 50;
		fbi->linelen = g->g.Width * 4;//sizeof(LLDCOLOR_TYPE);				// bytes per row
		fbi->pixels = (void*)LTDC_drv_get_free_fb_addr(); //LTDC_drv_fb_update();                // pointer to the memory frame buffer
               
                //gdispGControl(g,1,NULL);
	}

	#if GDISP_HARDWARE_FLUSH
		static void board_flush(fbInfo* fbi) {
			// TODO: Can be an empty function if your hardware doesn't support this
			//(void) g;
			
			//If aphamode was enabled
			
			//Make DMA2D blending
                        HAL_GPIO_WritePin(GPIOI, GPIO_PIN_1, !HAL_GPIO_ReadPin(GPIOI, GPIO_PIN_1));

			LTDC_drv_fb_update();
                         
                        fbi->pixels = (void*)LTDC_drv_get_free_fb_addr();
		}
        
	#endif

	#if GDISP_NEED_CONTROL
		static void board_backlight(GDisplay *g, uint8_t percent) {
			// TODO: Can be an empty function if your hardware doesn't support this
			(void) g;
			(void) percent;
                        
                        // ST was stupid enought not to hook this up to a pin that
                        // is able to act as PWM output...
                        if (percent <= 0) {
                            GPIOK->ODR &=~ (1 << 3);	// Backlight off
                        } else {
                            GPIOK->ODR |= (1 << 3);		// Backlight on
                        }
		}

		static void board_contrast(GDisplay *g, uint8_t percent) {
			// TODO: Can be an empty function if your hardware doesn't support this
			(void) g;
			(void) percent;
		}

		static void board_power(GDisplay *g, powermode_t pwr) {
			// TODO: Can be an empty function if your hardware doesn't support this
			(void) g;
			(void) pwr;
		}
	#endif

#endif /* GDISP_LLD_BOARD_IMPLEMENTATION */
