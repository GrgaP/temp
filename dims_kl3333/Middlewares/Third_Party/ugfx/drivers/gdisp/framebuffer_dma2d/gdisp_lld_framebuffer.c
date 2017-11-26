/*
 * This file is subject to the terms of the GFX License. If a copy of
 * the license was not distributed with this file, you can obtain one at:
 *
 *              http://ugfx.org/license.html
 */

#include "gfx.h"
#include "stm32_dma2d.h"

#if GFX_USE_GDISP

typedef struct fbInfo {
	void *			pixels;			// The pixel buffer
	coord_t			linelen;		// The number of bytes per display line
	} fbInfo;

#define GDISP_DRIVER_VMT			GDISPVMT_framebuffer
#include "gdisp_lld_config.h"
#include "../../../src/gdisp/gdisp_driver.h"
#include "board_framebuffer.h"

//#define DMA2D_ALPHA_PIXSZ 2
//#define DMA2D_ALPHA_BUFF_H 320
//#define DMA2D_ALPHA_BUFF_W 240
//#define DMA2D_ALPHA_BG_BUFFER 0xC00FF000
//#define DMA2D_ALPHA_FG_BUFFER(X) (0xC00FF000 + X*DMA2D_ALPHA_PIXSZ*480*272)

typedef struct fbPriv {
	fbInfo			fbi;			// Display information
	} fbPriv;

/*===========================================================================*/
/* Driver local routines    .                                                */
/*===========================================================================*/

#define PIXIL_POS(g, x, y)		((y) * ((fbPriv *)(g)->priv)->fbi.linelen + (x) * 2)//sizeof(LLDCOLOR_TYPE))
#define PIXEL_ADDR(g, pos)		((LLDCOLOR_TYPE *)(((char *)((fbPriv *)(g)->priv)->fbi.pixels)+pos))

/*===========================================================================*/
/* Driver exported functions.                                                */
/*===========================================================================*/

LLDSPEC bool_t gdisp_lld_init(GDisplay *g) {

	// Initialize the private structure
	if (!(g->priv = gfxAlloc(sizeof(fbPriv))))
		gfxHalt("GDISP Framebuffer: Failed to allocate private memory");
	((fbPriv *)g->priv)->fbi.pixels = 0;
	((fbPriv *)g->priv)->fbi.linelen = 0;

        #if USE_DMA2D
		dma2d_common_init();
	#endif
        
	// Initialize the GDISP structure
	g->g.Orientation = GDISP_ROTATE_0;
	g->g.Powermode = powerOn;
	g->board = 0;							// preinitialize
	board_init(g, &((fbPriv *)g->priv)->fbi);

	return TRUE;
}

#if GDISP_HARDWARE_FLUSH
	LLDSPEC void gdisp_lld_flush(GDisplay *g) {
            board_flush(&((fbPriv *)g->priv)->fbi);//In original file was: "board_flush(g);"
	}
#endif

LLDSPEC void gdisp_lld_draw_pixel(GDisplay *g) {
	unsigned	pos;

	#if GDISP_NEED_CONTROL
		switch(g->g.Orientation) {
		case GDISP_ROTATE_0:
		default:
			pos = PIXIL_POS(g, g->p.x, g->p.y);
			break;
		case GDISP_ROTATE_90:
			pos = PIXIL_POS(g, g->p.y, g->g.Width-g->p.x-1);
			break;
		case GDISP_ROTATE_180:
			pos = PIXIL_POS(g, g->g.Width-g->p.x-1, g->g.Height-g->p.y-1);
			break;
		case GDISP_ROTATE_270:
			pos = PIXIL_POS(g, g->g.Height-g->p.y-1, g->p.x);
			break;
		}
	#else
		pos = PIXIL_POS(g, g->p.x, g->p.y);
	#endif

		PIXEL_ADDR(g, pos)[0] = gdispColor2Native(g->p.color);
}

LLDSPEC	color_t gdisp_lld_get_pixel_color(GDisplay *g) {
	unsigned		pos;
	LLDCOLOR_TYPE	color;

	#if GDISP_NEED_CONTROL
		switch(g->g.Orientation) {
		case GDISP_ROTATE_0:
		default:
			pos = PIXIL_POS(g, g->p.x, g->p.y);
			break;
		case GDISP_ROTATE_90:
			pos = PIXIL_POS(g, g->p.y, g->g.Width-g->p.x-1);
			break;
		case GDISP_ROTATE_180:
			pos = PIXIL_POS(g, g->g.Width-g->p.x-1, g->g.Height-g->p.y-1);
			break;
		case GDISP_ROTATE_270:
			pos = PIXIL_POS(g, g->g.Height-g->p.y-1, g->p.x);
			break;
		}
	#else
		pos = PIXIL_POS(g, g->p.x, g->p.y);
	#endif

	color = PIXEL_ADDR(g, pos)[0];
	return gdispNative2Color(color);
}

#if USE_DMA2D

        static void dma2d_common_init(void) {
          
            // Enable DMA2D clock
            RCC->AHB1ENR |= RCC_AHB1ENR_DMA2DEN;
            
            // Output color format
            DMA2D->OPFCCR = DMA2D_RGB565;
            
            
            // Foreground color format
            DMA2D->FGPFCCR = DMA2D_RGB565;
            DMA2D->FGPFCCR |= (2<<16)|(0xff<<24);
            
            
            // Background color format
            DMA2D->BGPFCCR = DMA2D_RGB565;
            DMA2D->BGPFCCR |= (2<<16)|(0xff<<24);
            
        }
/*
	static void dma2d_draw_reinit(void) {
	
                // Output color format
                DMA2D->OPFCCR = DMA2D_ARGB8888;
	}

        static void dma2d_blend_reinit(void) {

		// Output color format
		DMA2D->OPFCCR = OPFCCR_RGB565;
	}
*/
        /**
          * @brief  Fill Buffer
          * @param  pDst:        pointer to destination
          * @param  xSize:       X size
          * @param  ySize:       Y size
          * @param  OffLine:     offset after each line
          * @param  ColorIndex:  color to be used.           
          * @retval None.
          */
        static void DMA2D_FillBuffer(void * pDst, uint32_t xSize, uint32_t ySize, uint32_t OffLine, uint32_t ColorIndex)
        {

                /* Set up mode */
                DMA2D->CR      = 0x00030000UL | (1 << 9);        
                DMA2D->OCOLR   = ColorIndex;                     

                /* Set up pointers */
                DMA2D->OMAR    = (uint32_t)pDst;                      

                /* Set up offsets */
                DMA2D->OOR     = OffLine;                        

                /*  Set up size */
                DMA2D->NLR     = (uint32_t)(xSize << 16) | (uint16_t)ySize;
  
                DMA2D->CR     |= DMA2D_CR_START; 
  
                /* Wait until transfer is done */
                while (DMA2D->CR & DMA2D_CR_START){}
          }

	// Uses p.x,p.y  p.cx,p.cy  p.color
	LLDSPEC void gdisp_lld_fill_area(GDisplay* g)
	{	
		uint32_t pos;
		uint32_t lineadd;
		uint32_t shape;

		// Wait until DMA2D is ready
		while(DMA2D->CR & DMA2D_CR_START);

		#if GDISP_NEED_CONTROL
			switch(g->g.Orientation) {
			case GDISP_ROTATE_0:
			default:
				pos = PIXIL_POS(g, g->p.x, g->p.y);
				lineadd = g->g.Width - g->p.cx;
				shape = (g->p.cx << 16) | (g->p.cy);
				break;
			case GDISP_ROTATE_90:
				pos = PIXIL_POS(g, g->p.y, g->g.Width-g->p.x-g->p.cx);
				lineadd = g->g.Height - g->p.cy;
				shape = (g->p.cy << 16) | (g->p.cx);
				break;
			case GDISP_ROTATE_180:
				pos = PIXIL_POS(g, g->g.Width-g->p.x-g->p.cx, g->g.Height-g->p.y-g->p.cy);
				lineadd = g->g.Width - g->p.cx;
				shape = (g->p.cx << 16) | (g->p.cy);
				break;
			case GDISP_ROTATE_270:
				pos = PIXIL_POS(g, g->g.Height-g->p.y-g->p.cy, g->p.x);
				lineadd = g->g.Height - g->p.cy;
				shape = (g->p.cy << 16) | (g->p.cx);
				break;
			}
		#else
			pos = PIXIL_POS(g, g->p.x, g->p.y);
			lineadd = g->g.Width - g->p.cx;
			shape = (g->p.cx << 16) | (g->p.cy);
		#endif
		
		// Start the DMA2D
		DMA2D->OMAR = (uint32_t)PIXEL_ADDR(g, pos);
		DMA2D->OOR = lineadd;
		DMA2D->NLR = shape;
		DMA2D->OCOLR = (uint32_t)(gdispColor2Native(g->p.color));
		//DMA2D->FGPFCCR |= (1<<5); 
                //DMA2D->BGPFCCR |= (1<<5);
                DMA2D->CR = DMA2D_CR_MODE_R2M | DMA2D_CR_START;	
	}

	/* Oops - the DMA2D only supports GDISP_ROTATE_0.
	 *
	 * Where the width is 1 we can trick it for other orientations.
	 * That is worthwhile as a width of 1 is common. For other
	 * situations we need to fall back to pixel pushing.
	 *
	 * Additionally, although DMA2D can translate color formats
	 * it can only do it for a small range of formats. For any
	 * other formats we also need to fall back to pixel pushing.
	 *
	 * As the code to actually do all that for other than the
	 * simplest case (orientation == GDISP_ROTATE_0 and
	 * GDISP_PIXELFORMAT == GDISP_LLD_PIXELFORMAT) is very complex
	 * we will always pixel push for now. In practice that is OK as
	 * access to the framebuffer is fast - probably faster than DMA2D.
	 * It just uses more CPU.
	 */
	#if GDISP_HARDWARE_BITFILLS 
		// Uses p.x,p.y  p.cx,p.cy  p.x1,p.y1 (=srcx,srcy)  p.x2 (=srccx), p.ptr (=buffer)
		LLDSPEC void gdisp_lld_blit_area(GDisplay* g) {
			// Wait until DMA2D is ready
			while(DMA2D->CR & DMA2D_CR_START);

			// Source setup
			DMA2D->FGMAR = LTDC_PIXELBYTES * (g->p.y1 * g->p.x2 + g->p.x1) + (uint32_t)g->p.ptr;
			DMA2D->FGOR = g->p.x2 - g->p.cx;
		
			// Output setup
			DMA2D->OMAR = (uint32_t)PIXEL_ADDR(g, PIXIL_POS(g, g->p.x, g->p.y));
			DMA2D->OOR = g->g.Width - g->p.cx;
			DMA2D->NLR = (g->p.cx << 16) | (g->p.cy);

			// Set MODE to M2M and Start the process
			DMA2D->CR = DMA2D_CR_MODE_M2M | DMA2D_CR_START;
		}
	#endif

	#if GDISP_HARDWARE_CLEARS
	void gdisp_lld_clear(GDisplay *g) {

		DMA2D_FillBuffer(PIXEL_ADDR(g, 0), g->g.Width, g->g.Height, 0, gdispColor2Native(g->p.color));
                
	}
        #endif
	
#endif /* USE_DMA2D */

#if GDISP_NEED_CONTROL
	LLDSPEC void gdisp_lld_control(GDisplay *g) {
		switch(g->p.x) {
		case GDISP_CONTROL_POWER:
			if (g->g.Powermode == (powermode_t)g->p.ptr)
				return;
			switch((powermode_t)g->p.ptr) {
			case powerOff: case powerOn: case powerSleep: case powerDeepSleep:
				board_power(g, (powermode_t)g->p.ptr);
				break;
			default:
				return;
			}
			g->g.Powermode = (powermode_t)g->p.ptr;
			return;

		case GDISP_CONTROL_ORIENTATION:
			if (g->g.Orientation == (orientation_t)g->p.ptr)
				return;
			switch((orientation_t)g->p.ptr) {
				case GDISP_ROTATE_0:
				case GDISP_ROTATE_180:
					if (g->g.Orientation == GDISP_ROTATE_90 || g->g.Orientation == GDISP_ROTATE_270) {
						coord_t		tmp;

						tmp = g->g.Width;
						g->g.Width = g->g.Height;
						g->g.Height = tmp;
					}
					break;
				case GDISP_ROTATE_90:
				case GDISP_ROTATE_270:
					if (g->g.Orientation == GDISP_ROTATE_0 || g->g.Orientation == GDISP_ROTATE_180) {
						coord_t		tmp;

						tmp = g->g.Width;
						g->g.Width = g->g.Height;
						g->g.Height = tmp;
					}
					break;
				default:
					return;
			}
			g->g.Orientation = (orientation_t)g->p.ptr;
			return;

		case GDISP_CONTROL_BACKLIGHT:
			if ((unsigned)g->p.ptr > 100) g->p.ptr = (void *)100;
			board_backlight(g, (unsigned)g->p.ptr);
			g->g.Backlight = (unsigned)g->p.ptr;
			return;

		case GDISP_CONTROL_CONTRAST:
			if ((unsigned)g->p.ptr > 100) g->p.ptr = (void *)100;
			board_contrast(g, (unsigned)g->p.ptr);
			g->g.Contrast = (unsigned)g->p.ptr;
			return;
		}
	}
#endif



#endif /* GFX_USE_GDISP */
