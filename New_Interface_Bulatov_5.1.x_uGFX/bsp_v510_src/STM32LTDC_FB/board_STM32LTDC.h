/*
 * This file is subject to the terms of the GFX License. If a copy of
 * the license was not distributed with this file, you can obtain one at:
 *
 *              http://ugfx.org/license.html
 */

#ifndef _BOARD_STM32LTDC_H
#define _BOARD_STM32LTDC_H

#include "main.h"

//#include "stm32f4xx_fmc.h"

#define SPI_PORT	&SPID5
#define DC_PORT		GPIOD
#define DC_PIN		GPIOD_LCD_WRX

#define LTDC_PIX_SIZE 16
#define DMA2D_PIX_SIZE 24

#define FB_ADDR_OFFSET (LTDC_PIX_SIZE*LCD_HEIGHT*LCD_WIDTH)

#define BG_FB_ADDR SDRAM_DEVICE_ADDR
#define FG_FB_ADDR (SDRAM_DEVICE_ADDR+FB_ADDR_OFFSET)

#if FGL_DBL_FB
	#define FG_FB2_ADDR (SDRAM_DEVICE_ADDR+(2*FB_ADDR_OFFSET))
#else
	#define FG_FB2_ADDR NULL
#endif

#if BGL_DBL_FB
	#define BG_FB2_ADDR (SDRAM_DEVICE_ADDR+(3*FB_ADDR_OFFSET))
#else
	#define BG_FB2_ADDR NULL
#endif

#define FG_FB2_ADDR (SDRAM_DEVICE_ADDR+FB_ADDR_OFFSET)

//static LTDC_HandleTypeDef hltdc;
//static DMA2D_HandleTypeDef hdma2d;
static SPI_HandleTypeDef hspi1;

/*static const SPIConfig spi_cfg = {
	NULL,
	GPIOC,
	GPIOC_SPI5_LCD_CS,
	((1 << 3) & SPI_CR1_BR) | SPI_CR1_SSM | SPI_CR1_SSI | SPI_CR1_MSTR
};*/

static const ltdcConfig driverCfg = {
	240, 320,
	10, 2,
	20, 2,
	10, 4,
	0,
	0x00ff00,
	{
		(LLDCOLOR_TYPE *)BG_FB_ADDR,	// frame
		LCD_WIDTH, LCD_HEIGHT,							// width, height
		240 * LTDC_PIXELBYTES,				// pitch
		LTDC_PIXELFORMAT,					// fmt
		0, 0,								// x, y
		240, 320,							// cx, cy
		0xFF00FF00,					// defcolor
		0x00ff00,						// keycolor
		LTDC_BLEND_MOD1_MOD2,				// blending
		0,									// palette
		0,									// palettelen
		0xFF,								// alpha
		LTDC_LEF_ENABLE//|LTDC_LEF_KEYING						// flags
	},
	LTDC_UNUSED_LAYER_CONFIG        
        /*{
		(LLDCOLOR_TYPE *)FG_FB_ADDR,	// frame
		LCD_WIDTH, LCD_HEIGHT,							// width, height
		240 * LTDC_PIXELBYTES,				// pitch
		LTDC_PIXELFORMAT,					// fmt
		0, 0,								// x, y
		240, 100,							// cx, cy
		LTDC_COLOR_YELLOW,					// defcolor
		BLACK,						// keycolor
		LTDC_BLEND_MOD1_MOD2,				// blending
		0,									// palette
		0,									// palettelen
		0x7F,								// alpha
		LTDC_LEF_ENABLE//|LTDC_LEF_KEYING						// flags
	}*/
};

/* Pin functions */
#define ILI9341_CS_SET		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_15, GPIO_PIN_SET);
#define ILI9341_CS_RESET	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_15, GPIO_PIN_RESET);
#define ILI9341_WRX_SET		HAL_GPIO_WritePin(GPIOD, GPIO_PIN_13, GPIO_PIN_SET);
#define ILI9341_WRX_RESET	HAL_GPIO_WritePin(GPIOD, GPIO_PIN_13, GPIO_PIN_RESET);
#define ILI9341_RST_HI		HAL_GPIO_WritePin(GPIOC, GPIO_PIN_1, GPIO_PIN_SET);
#define ILI9341_RST_LOW		HAL_GPIO_WritePin(GPIOC, GPIO_PIN_1, GPIO_PIN_RESET);

/* LCD settings */
#define ILI9341_WIDTH 				240
#define ILI9341_HEIGHT				320
#define ILI9341_PIXEL				76800

/* Commands */
#define ILI9341_RESET				0x01
#define ILI9341_SLEEP_OUT			0x11
#define ILI9341_GAMMA				0x26
#define ILI9341_DISPLAY_OFF			0x28
#define ILI9341_DISPLAY_ON			0x29
#define ILI9341_COLUMN_ADDR			0x2A
#define ILI9341_PAGE_ADDR			0x2B
#define ILI9341_GRAM				0x2C
#define ILI9341_MAC				0x36
#define ILI9341_PIXEL_FORMAT		        0x3A
#define ILI9341_WDB				0x51
#define ILI9341_WCD				0x53
#define ILI9341_RGB_INTERFACE		        0xB0
#define ILI9341_FRC				0xB1
#define ILI9341_BPC				0xB5
#define ILI9341_DFC				0xB6
#define ILI9341_POWER1				0xC0
#define ILI9341_POWER2				0xC1
#define ILI9341_VCOM1				0xC5
#define ILI9341_VCOM2				0xC7
#define ILI9341_POWERA				0xCB
#define ILI9341_POWERB				0xCF
#define ILI9341_PGAMMA				0xE0
#define ILI9341_NGAMMA				0xE1
#define ILI9341_DTCA				0xE8
#define ILI9341_DTCB				0xEA
#define ILI9341_POWER_SEQ			0xED
#define ILI9341_3GAMMA_EN			0xF2
#define ILI9341_INTERFACE			0xF6
#define ILI9341_PRC				0xF7

static void LCD_LTDC_GPIO_init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct;
  
    /* Peripheral clock enable */
     
    /**LTDC GPIO Configuration    
    PF10     ------> LTDC_DE
    PA3     ------> LTDC_B5
    PA4     ------> LTDC_VSYNC
    PA6     ------> LTDC_G2
    PB0     ------> LTDC_R3
    PB1     ------> LTDC_R6
    PB10     ------> LTDC_G4
    PB11     ------> LTDC_G5
    PG6     ------> LTDC_R7
    PG7     ------> LTDC_CLK
    PC6     ------> LTDC_HSYNC
    PC7     ------> LTDC_G6
    PA11     ------> LTDC_R4
    PA12     ------> LTDC_R5
    PD3     ------> LTDC_G7
    PG10     ------> LTDC_G3
    PG11     ------> LTDC_B3
    PG12     ------> LTDC_B4
    PB8     ------> LTDC_B6
    PB9     ------> LTDC_B7 
    */
    
    /*--LCD_RESET--*/
    GPIO_InitStruct.Pin = GPIO_PIN_1;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_LOW;
    HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);
    
    ILI9341_RST_LOW;
  
    GPIO_InitStruct.Pin = GPIO_PIN_10;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FAST;
    GPIO_InitStruct.Alternate = GPIO_AF14_LTDC;
    HAL_GPIO_Init(GPIOF, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = GPIO_PIN_3|GPIO_PIN_4|GPIO_PIN_6|GPIO_PIN_11 
                          |GPIO_PIN_12;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FAST;
    GPIO_InitStruct.Alternate = GPIO_AF14_LTDC;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = GPIO_PIN_0|GPIO_PIN_1;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FAST;
    GPIO_InitStruct.Alternate = GPIO_AF9_LTDC;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = GPIO_PIN_10|GPIO_PIN_11|GPIO_PIN_8|GPIO_PIN_9;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FAST;
    GPIO_InitStruct.Alternate = GPIO_AF14_LTDC;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = GPIO_PIN_6|GPIO_PIN_7|GPIO_PIN_11;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FAST;
    GPIO_InitStruct.Alternate = GPIO_AF14_LTDC;
    HAL_GPIO_Init(GPIOG, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = GPIO_PIN_6|GPIO_PIN_7;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FAST;
    GPIO_InitStruct.Alternate = GPIO_AF14_LTDC;
    HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = GPIO_PIN_3;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FAST;
    GPIO_InitStruct.Alternate = GPIO_AF14_LTDC;
    HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = GPIO_PIN_10|GPIO_PIN_12;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FAST;
    GPIO_InitStruct.Alternate = GPIO_AF9_LTDC;
    HAL_GPIO_Init(GPIOG, &GPIO_InitStruct);
    
    /*-RD/FMARK/WRX--*/
    GPIO_InitStruct.Pin = GPIO_PIN_4|GPIO_PIN_7|GPIO_PIN_13;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;
    HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);
    
    HAL_GPIO_WritePin(GPIOD, GPIO_PIN_4|GPIO_PIN_7, GPIO_PIN_RESET);    
    ILI9341_WRX_SET;
    
    ILI9341_RST_HI//LCD_RESET_INIT

    /* Peripheral interrupt init*/
    HAL_NVIC_SetPriority(LTDC_IRQn, 0xD, 0);
    HAL_NVIC_EnableIRQ(LTDC_IRQn);
}

static void LCD_spi_init(void)
{
  __SPI1_CLK_ENABLE();
  
  hspi1.Instance = SPI1;
  hspi1.Init.Mode = SPI_MODE_MASTER;
  hspi1.Init.Direction = SPI_DIRECTION_2LINES;
  hspi1.Init.DataSize = SPI_DATASIZE_8BIT;
  hspi1.Init.CLKPolarity = SPI_POLARITY_LOW;
  hspi1.Init.CLKPhase = SPI_PHASE_1EDGE;
  hspi1.Init.NSS = SPI_NSS_HARD_OUTPUT;
  hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_16;
  hspi1.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi1.Init.TIMode = SPI_TIMODE_DISABLED;
  hspi1.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLED;
  hspi1.Init.CRCPolynomial = 7;
  //hspi1.Init.CRCLength = SPI_CRC_LENGTH_DATASIZE;
  //hspi1.Init.NSSPMode = SPI_NSS_PULSE_ENABLED;
  HAL_SPI_Init(&hspi1);
      
  GPIO_InitTypeDef GPIO_InitStruct;
  /*--MOSI SCK--*/
  GPIO_InitStruct.Pin = GPIO_PIN_5|GPIO_PIN_7;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;
  GPIO_InitStruct.Alternate = GPIO_AF5_SPI1;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
  /*--CS--*/
  GPIO_InitStruct.Pin = GPIO_PIN_15;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
  
  ILI9341_CS_SET;
  
  /*--MISO--*/
  GPIO_InitStruct.Pin = GPIO_PIN_4;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;
  GPIO_InitStruct.Alternate = GPIO_AF5_SPI1;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
}

#define LCD_HSYNC	10	// 10
#define LCD_HBP		20	// 20
#define LCD_HAREA	240	// 320
#define LCD_HFP		10	// 10
#define LCD_VSYNC	2	// 2
#define LCD_VBP		2	// 2
#define LCD_VAREA	320 	// 240
#define LCD_VFP		4	// 4

#define HSW		(LCD_HSYNC - 1)
#define AHBP		(LCD_HSYNC + LCD_HBP - 1)
#define AAW		(LCD_HSYNC + LCD_HBP + LCD_HAREA - 1)
#define TOTALW		(LCD_HSYNC + LCD_HBP + LCD_HAREA + LCD_HFP - 1)
#define VSH		(LCD_VSYNC - 1)
#define AVBP		(LCD_VSYNC + LCD_VBP - 1)
#define AAH		(LCD_VSYNC + LCD_VBP + LCD_VAREA - 1)
#define TOTALH		(LCD_VSYNC + LCD_VBP + LCD_VAREA + LCD_VFP - 1)

static void ILI9341_SendCommand(uint8_t data) {
	ILI9341_WRX_RESET;
	ILI9341_CS_RESET;
	HAL_SPI_Transmit(&hspi1,&data,1,0);
	ILI9341_CS_SET;
}

static void ILI9341_SendData(uint8_t data) {
	ILI9341_WRX_SET;
	ILI9341_CS_RESET;
	HAL_SPI_Transmit(&hspi1,&data,1,0);
	ILI9341_CS_SET;
}

static void LCD_lli9341_init(void)
{
  	ILI9341_SendCommand(0xCA);
	ILI9341_SendData(0xC3);
	ILI9341_SendData(0x08);
	ILI9341_SendData(0x50);
	ILI9341_SendCommand(ILI9341_POWERB);
	ILI9341_SendData(0x00);
	ILI9341_SendData(0xC1);
	ILI9341_SendData(0x30);
	ILI9341_SendCommand(ILI9341_POWER_SEQ);
	ILI9341_SendData(0x64);
	ILI9341_SendData(0x03);
	ILI9341_SendData(0x12);
	ILI9341_SendData(0x81);
	ILI9341_SendCommand(ILI9341_DTCA);
	ILI9341_SendData(0x85);
	ILI9341_SendData(0x00);
	ILI9341_SendData(0x78);
	ILI9341_SendCommand(ILI9341_POWERA);
	ILI9341_SendData(0x39);
	ILI9341_SendData(0x2C);
	ILI9341_SendData(0x00);
	ILI9341_SendData(0x34);
	ILI9341_SendData(0x02);
	ILI9341_SendCommand(ILI9341_PRC);
	ILI9341_SendData(0x20);
	ILI9341_SendCommand(ILI9341_DTCB);
	ILI9341_SendData(0x00);
	ILI9341_SendData(0x00);
	ILI9341_SendCommand(ILI9341_FRC);
	ILI9341_SendData(0x00);
	ILI9341_SendData(0x1B);
	ILI9341_SendCommand(ILI9341_DFC);
	ILI9341_SendData(0x0A);
	ILI9341_SendData(0xA2);
	ILI9341_SendCommand(ILI9341_POWER1);
	ILI9341_SendData(0x10);
	ILI9341_SendCommand(ILI9341_POWER2);
	ILI9341_SendData(0x10);
	ILI9341_SendCommand(ILI9341_VCOM1);
	ILI9341_SendData(0x45);
	ILI9341_SendData(0x15);
	ILI9341_SendCommand(ILI9341_VCOM2);
	ILI9341_SendData(0x90);
	ILI9341_SendCommand(ILI9341_MAC);
	ILI9341_SendData(0x08);//Set screen orientation
	ILI9341_SendCommand(ILI9341_3GAMMA_EN);
	ILI9341_SendData(0x00);
	ILI9341_SendCommand(ILI9341_RGB_INTERFACE);
	ILI9341_SendData(0xC2);
	ILI9341_SendCommand(ILI9341_DFC);
	ILI9341_SendData(0x0A);
	ILI9341_SendData(0xA7);
	ILI9341_SendData(0x27);
	ILI9341_SendData(0x04);

	ILI9341_SendCommand(ILI9341_COLUMN_ADDR);
	ILI9341_SendData(0x00);
	ILI9341_SendData(0x00);
	ILI9341_SendData(0x00);
	ILI9341_SendData(0xEF);

	ILI9341_SendCommand(ILI9341_PAGE_ADDR);
	ILI9341_SendData(0x00);
	ILI9341_SendData(0x00);
	ILI9341_SendData(0x01);
	ILI9341_SendData(0x3F);
	ILI9341_SendCommand(ILI9341_INTERFACE);
	ILI9341_SendData(0x01);
	ILI9341_SendData(0x00);
	ILI9341_SendData(0x06);

	ILI9341_SendCommand(ILI9341_GRAM);
	HAL_Delay(100);

	ILI9341_SendCommand(ILI9341_GAMMA);
	ILI9341_SendData(0x01);

	ILI9341_SendCommand(ILI9341_PGAMMA);
	ILI9341_SendData(0x0F);
	ILI9341_SendData(0x29);
	ILI9341_SendData(0x24);
	ILI9341_SendData(0x0C);
	ILI9341_SendData(0x0E);
	ILI9341_SendData(0x09);
	ILI9341_SendData(0x4E);
	ILI9341_SendData(0x78);
	ILI9341_SendData(0x3C);
	ILI9341_SendData(0x09);
	ILI9341_SendData(0x13);
	ILI9341_SendData(0x05);
	ILI9341_SendData(0x17);
	ILI9341_SendData(0x11);
	ILI9341_SendData(0x00);
	ILI9341_SendCommand(ILI9341_NGAMMA);
	ILI9341_SendData(0x00);
	ILI9341_SendData(0x16);
	ILI9341_SendData(0x1B);
	ILI9341_SendData(0x04);
	ILI9341_SendData(0x11);
	ILI9341_SendData(0x07);
	ILI9341_SendData(0x31);
	ILI9341_SendData(0x33);
	ILI9341_SendData(0x42);
	ILI9341_SendData(0x05);
	ILI9341_SendData(0x0C);
	ILI9341_SendData(0x0A);
	ILI9341_SendData(0x28);
	ILI9341_SendData(0x2F);
	ILI9341_SendData(0x0F);

	ILI9341_SendCommand(ILI9341_SLEEP_OUT);
	HAL_Delay(100);
	ILI9341_SendCommand(ILI9341_DISPLAY_ON);

	ILI9341_SendCommand(ILI9341_GRAM);
}

static void init_board(GDisplay *g) {

	// As we are not using multiple displays we set g->board to NULL as we don't use it.
	g->board = 0;

	switch(g->controllerdisplay) {
	case 0:											// Set up for Display 0
		//palSetPadMode(GPIOA, 9, PAL_MODE_ALTERNATE(7));		// UART_TX
		//palSetPadMode(GPIOA, 10, PAL_MODE_ALTERNATE(7));	// UART_RX
		//palSetPadMode(GPIOF, GPIOF_LCD_DCX, PAL_MODE_ALTERNATE(5));
		//palSetPadMode(GPIOF, GPIOF_LCD_DE, PAL_MODE_ALTERNATE(14));

		//#define STM32_SAISRC_NOCLOCK    (0 << 23)   /**< No clock.                  */
		//#define STM32_SAISRC_PLL        (1 << 23)   /**< SAI_CKIN is PLL.           */
		//#define STM32_SAIR_DIV2         (0 << 16)   /**< R divided by 2.            */
		//#define STM32_SAIR_DIV4         (1 << 16)   /**< R divided by 4.            */
		//#define STM32_SAIR_DIV8         (2 << 16)   /**< R divided by 8.            */
		//#define STM32_SAIR_DIV16        (3 << 16)   /**< R divided by 16.           */

		//#define STM32_PLLSAIN_VALUE                 192
		//#define STM32_PLLSAIQ_VALUE                 7
		//#define STM32_PLLSAIR_VALUE                 4
		//#define STM32_PLLSAIR_POST                  STM32_SAIR_DIV4

		/* PLLSAI activation.*/
		//RCC->PLLSAICFGR = (STM32_PLLSAIN_VALUE << 6) | (STM32_PLLSAIR_VALUE << 28) | (STM32_PLLSAIQ_VALUE << 24);
		//RCC->DCKCFGR = (RCC->DCKCFGR & ~RCC_DCKCFGR_PLLSAIDIVR) | STM32_PLLSAIR_POST;
		//RCC->CR |= RCC_CR_PLLSAION;

                LCD_LTDC_GPIO_init();
          
                LCD_spi_init();//config SPI for link to LCD controller ili9341
  
                BSP_SDRAM_Init();//External SDRAM init for frame buffer allocatin
  
                LCDBUFFER_Clear();

		// Clear the SDRAM
		memset((void *)SDRAM_DEVICE_ADDR, 0, 0x12C000);

		//spiStart(SPI_PORT, &spi_cfg);

		LCD_lli9341_init();//Config LCD/ILI9341 with using SPI link//Init9341(g);
		break;
	}
}

static GFXINLINE void post_init_board(GDisplay *g) {
	(void) g;
}

static GFXINLINE void set_backlight(GDisplay *g, uint8_t percent) {
	(void) g;
	(void) percent;
}

LLDSPEC	void gdisp_lld_flush(GDisplay *g)
{
  HAL_GPIO_WritePin(GPIOD, GPIO_PIN_5, !(HAL_GPIO_ReadPin(GPIOD,GPIO_PIN_5)));
}

#endif /* _BOARD_STM32LTDC_H */
