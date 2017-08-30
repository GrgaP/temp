#include "lcd_ltdc.h"
#include "dims_dsp_sdram.h"
#include "rgb.h"

//#include "gdisp_lld_framebuffer.h"

// дисплей DisplayTech DT24
// разрешение 320 x 240
// контроллер дисплея ILI9341 (ilitek)
// дисплей используется в вертикальном режиме

/* Private defines */
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

static LTDC_HandleTypeDef hltdc;
static DMA2D_HandleTypeDef hdma2d;
static SPI_HandleTypeDef hspi1;

#define _SRAM1

#pragma location="SRAM1"
uint16_t LCDBUFFER0[LCD_WIDTH*LCD_HEIGHT], LCDBUFFER1[LCD_WIDTH*LCD_HEIGHT], LCDBUFFER2[LCD_WIDTH*LCD_HEIGHT];//LCDBUFFER0/LCDBUFFER1 uses for refresh layer1, LCDBUFFER2 use for layer2 with blending
uint16_t* LCDBUFFER = NULL;//pointer to current layer which receive new graphics data from application

void LCDBUFFER_Clear(void);
static void DMA2D_FillBuffer(void * pDst, uint32_t xSize, uint32_t ySize, uint32_t OffLine, uint32_t ColorIndex);

void LCDrefresh_Task(void const * argument);
void Disp_SM_Task(void const * argument);

/*Init section begin---------------------------------------*/

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

/* DMA2D init function */
void LCD_DMA2D_Init(void)
{
  __DMA2D_CLK_ENABLE();
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

static void LCD_ltdc_init(void)
{  
  __LTDC_CLK_ENABLE();

  hltdc.Instance = LTDC;
  hltdc.Init.HSPolarity = LTDC_HSPOLARITY_AL;
  hltdc.Init.VSPolarity = LTDC_VSPOLARITY_AL;
  hltdc.Init.DEPolarity = LTDC_DEPOLARITY_AL;
  hltdc.Init.PCPolarity = LTDC_PCPOLARITY_IPC;
  
  hltdc.Init.Backcolor.Reserved = 0;
  hltdc.Init.Backcolor.Reserved = 0;
  hltdc.Init.Backcolor.Reserved = 0;
  
  hltdc.Init.HorizontalSync = LCD_HSYNC;
  hltdc.Init.VerticalSync = LCD_VSYNC;
  hltdc.Init.AccumulatedHBP = AHBP;
  hltdc.Init.AccumulatedVBP = AVBP;
  hltdc.Init.AccumulatedActiveW = AAW;
  hltdc.Init.AccumulatedActiveH = AAH;
  hltdc.Init.TotalWidth = TOTALW;
  hltdc.Init.TotalHeigh = TOTALH;  
  
  HAL_LTDC_Init(&hltdc);
}
  
static void LCD_layers_init(void)
{
  LTDC_LayerCfgTypeDef pLayerCfg;
  
  pLayerCfg.WindowX0 = 0;
  pLayerCfg.WindowX1 = 240;
  pLayerCfg.WindowY0 = 0;
  pLayerCfg.WindowY1 = 320;
  
  pLayerCfg.PixelFormat = LTDC_PIXEL_FORMAT_RGB565;
  pLayerCfg.Alpha = 0xff;//fully visible
  
  pLayerCfg.BlendingFactor1 = LTDC_BLENDING_FACTOR1_PAxCA;
  pLayerCfg.BlendingFactor2 = LTDC_BLENDING_FACTOR2_PAxCA;
  pLayerCfg.FBStartAdress = (uint32_t)LCDBUFFER0;
      
  pLayerCfg.ImageWidth = 240;
  pLayerCfg.ImageHeight = 320;
  
  pLayerCfg.Backcolor.Reserved = 0;
  pLayerCfg.Backcolor.Reserved = 0;
  pLayerCfg.Backcolor.Reserved = 0;
  pLayerCfg.Alpha0 = 0x00;
  
  HAL_LTDC_ConfigLayer(&hltdc, &pLayerCfg, 0);
  
  HAL_LTDC_EnableDither(&hltdc);
  
  DMA2D_FillBuffer(LCDBUFFER0, 320,240,0, BLACK);
  DMA2D_FillBuffer(LCDBUFFER1, 320,240,0, BLACK);
  
  LCDBUFFER = LCDBUFFER1;
  
  HAL_LTDC_ConfigColorKeying(&hltdc,0x0000,0);
  
  HAL_LTDC_EnableColorKeying(&hltdc, 0);
}

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

void LCD_init()
{
  LCD_DMA2D_Init();//DMA2D config
  
  LCD_LTDC_GPIO_init();//config pins for LTDC bus
  
  LCD_spi_init();//config SPI for link to LCD controller ili9341
  
  BSP_SDRAM_Init();//External SDRAM init for frame buffer allocatin
  
  LCDBUFFER_Clear();
  
  LCD_lli9341_init();//Config LCD/ILI9341 with using SPI link
    
  LCD_ltdc_init();//common LTDC init
  
  LCD_layers_init();//Layer 0 LTDC init
  
  Task_add(TASK_FB_WR_EN);//give the resolution to the frame buffer write operation 
}

/*Init section end---------------------------------------*/


/*Run section begin--------------------------------------*/

/**
  * This function called from user application and generate signal about beginning of frame buffer change
  */
void LCD_Update(void)
{
  HAL_LTDC_ProgramLineEvent(&hltdc, 0);//initiate event at passing of screen edge
}

/**
* @brief This function handles LTDC global interrupt.
*/
void LTDC_IRQHandler(void)
{
  /* USER CODE BEGIN LTDC_IRQn 0 */

  /* USER CODE END LTDC_IRQn 0 */
  HAL_LTDC_IRQHandler(&hltdc);
  /* USER CODE BEGIN LTDC_IRQn 1 */

  /* USER CODE END LTDC_IRQn 1 */
}

/**
  * @brief  Line Event callback.
  * @param  hltdc: pointer to a LTDC_HandleTypeDef structure that contains
  *                the configuration information for the specified LTDC.
  * @retval None
  */
void HAL_LTDC_LineEvenCallback(LTDC_HandleTypeDef *hltdc)
{

  __HAL_LTDC_LAYER(hltdc, 0)->CFBAR = (uint32_t) LCDBUFFER;///Set new framebuffer adress in shadow register for next displaying
  LCDBUFFER = (uint16_t*) __HAL_LTDC_LAYER(hltdc, 0)->CFBAR;///Reading another framebuffer adress from current layer configuration  
 
  //gdisp_lld_fb_refresh(GDISP);///call special function from "uGFX", for refreshing the adress of framebuffer   
  
  __HAL_LTDC_RELOAD_CONFIG(hltdc);
  
  Task_del(TASK_LCD_UPDATE);
  Task_add(TASK_FB_WR_EN);
}
/*--Run section end--*/


/*--drawing functions section begin--*/

void LCDBUFFER_Clear(void)
{
  __HAL_LTDC_DISABLE_IT(&hltdc,LTDC_IT_LI);
    DMA2D_FillBuffer(LCDBUFFER, 320,240,0, BLACK);
  __HAL_LTDC_DISABLE_IT(&hltdc,LTDC_IT_LI);
}  


void LCDBUFFER_ShowZone (uint16_t x1, uint16_t x2, uint16_t color, float alph, uint8_t n)
{
  LCDBUFFER_LineAlpha(x1, Y_ZONE - Y_ZONE_H*n - 3, x1, Y_ZONE - Y_ZONE_H*n+10-2 +0, color, alph);
  LCDBUFFER_LineAlpha(x2, Y_ZONE - Y_ZONE_H*n - 3, x2, Y_ZONE - Y_ZONE_H*n+10-2, color, alph);
  
  if (x2-x1 > 1)
  {
    LCDBUFFER_LineAlpha(x1+1, Y_ZONE - Y_ZONE_H*n +0, x2-1, Y_ZONE - Y_ZONE_H*n +0, color, (1-0.25*(1-alph)));
    LCDBUFFER_LineAlpha(x1+1, Y_ZONE - Y_ZONE_H*n +1, x2-1, Y_ZONE - Y_ZONE_H*n +1, color, (1-0.50*(1-alph)));
    LCDBUFFER_LineAlpha(x1+1, Y_ZONE - Y_ZONE_H*n +2, x2-1, Y_ZONE - Y_ZONE_H*n +2, color, (1-0.75*(1-alph)));
    LCDBUFFER_LineAlpha(x1+1, Y_ZONE - Y_ZONE_H*n +3, x2-1, Y_ZONE - Y_ZONE_H*n +3, color, alph);
    LCDBUFFER_LineAlpha(x1+1, Y_ZONE - Y_ZONE_H*n +4, x2-1, Y_ZONE - Y_ZONE_H*n +4, color, alph);
    LCDBUFFER_LineAlpha(x1+1, Y_ZONE - Y_ZONE_H*n +5, x2-1, Y_ZONE - Y_ZONE_H*n +5, color, alph);
    LCDBUFFER_LineAlpha(x1+1, Y_ZONE - Y_ZONE_H*n +6, x2-1, Y_ZONE - Y_ZONE_H*n +6, color, alph);
    LCDBUFFER_LineAlpha(x1+1, Y_ZONE - Y_ZONE_H*n +7, x2-1, Y_ZONE - Y_ZONE_H*n +7, color,(1-0.75*(1-alph)));
    LCDBUFFER_LineAlpha(x1+1, Y_ZONE - Y_ZONE_H*n +8, x2-1, Y_ZONE - Y_ZONE_H*n +8, color,(1-0.50*(1-alph)));
    LCDBUFFER_LineAlpha(x1+1, Y_ZONE - Y_ZONE_H*n +9, x2-1, Y_ZONE - Y_ZONE_H*n +9, color,(1-0.25*(1-alph)));
  }
}

void LCDBUFFER_ShowClbZone (u16 x1, u16 x2, u16 color, float alph, float lvl)
{
  
  if (lvl > 1) lvl = 1.f;
  
  u16 h = (u16)(Y_CLB_ZONE_MAX * lvl);
  
  LCDBUFFER_LineAlpha(x1, Y_ZONE - 3, x1, Y_ZONE - h, color, alph);
  LCDBUFFER_LineAlpha(x2, Y_ZONE - 3, x2, Y_ZONE - h, color, alph);
   
  if (x2-x1 > 4) 
  { 
    LCDBUFFER_LineAlpha(x1+(x2-x1)/2, Y_ZONE - 3, x1+(x2-x1)/2, Y_ZONE - Y_CLB_ZONE_MAX, color, alph);
    //LCDBUFFER_LineAlpha(x1+2,  Y_ZONE - h, x2-2, Y_ZONE - h, color, alph);
  }
  if (x2-x1 > 1)
  {
    for (uint32_t i = 1; i < (x2 - x1); i++) /* Не заполняем крайние зоны */
    {
      LCDBUFFER_LineAlpha(x1+i, Y_ZONE, x1+i, Y_ZONE-Y_CLB_ZONE_MAX, color, (1-0.5*(1-alph)));
      LCDBUFFER_LineAlpha(x1+i, Y_ZONE, x1+i, Y_ZONE-h, color, (1-0.5*(1-alph)));
    }

  }
}

void LCDBUFFER_BAALine (uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t color)
{ 
	int16_t  x, y, d, dx, dy, i, i1, i2, kx, ky;
	int8_t flag;
	
	dx = x2 - x1;
	dy = y2 - y1;
	if (dx == 0 && dy == 0) LCDBUFFER_BAAPixelAlpha(x1, y1, color);  //Точка
	else      //Линия
	{
		kx = 1;
		ky = 1;
		if( dx < 0 )
		{ 
			dx = -dx;
			kx = -1;
		}
		else
		if(dx == 0) kx = 0;
		if(dy < 0)
		{ 
			dy = -dy;
			ky = -1;
		}
		if(dx < dy)
		{ 
			flag = 0;
			d = dx;
			dx = dy;
			dy = d;
		}
		else flag = 1;
		i1 = dy + dy;
		d = i1 - dx;
		i2 = d - dx;
		x = x1;
		y = y1;
		
		for(i=0; i < dx; i++)
		{
			LCDBUFFER_BAAPixelAlpha(x, y, color);
			if(flag) x += kx;
			else y += ky;
			if( d < 0 ) d += i1;
			else
			{
				d += i2;
				if(flag) y += ky;
				else x += kx;
			}
		}
		LCDBUFFER_BAAPixelAlpha(x, y, color);
	}
}

//=========================================
// LCDBUFFER - работа с дисплейным буфером
// Нарисовать линию по двум точкам
//=========================================
void LCDBUFFER_Line (uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t color)
{ 
	int16_t  x, y, d, dx, dy, i, i1, i2, kx, ky;
	int8_t flag;
	
	dx = x2 - x1;
	dy = y2 - y1;
	if (dx == 0 && dy == 0) LCDBUFFER_Pixel(x1, y1, color);  //Точка
	else      //Линия
	{
		kx = 1;
		ky = 1;
		if( dx < 0 )
		{ 
			dx = -dx;
			kx = -1;
		}
		else
		if(dx == 0) kx = 0;
		if(dy < 0)
		{ 
			dy = -dy;
			ky = -1;
		}
		if(dx < dy)
		{ 
			flag = 0;
			d = dx;
			dx = dy;
			dy = d;
		}
		else flag = 1;
		i1 = dy + dy;
		d = i1 - dx;
		i2 = d - dx;
		x = x1;
		y = y1;
		
		for(i=0; i < dx; i++)
		{
			LCDBUFFER_Pixel(x, y, color);
			if(flag) x += kx;
			else y += ky;
			if( d < 0 ) d += i1;
			else
			{
				d += i2;
				if(flag) y += ky;
				else x += kx;
			}
		}
		LCDBUFFER_Pixel(x, y, color);
	}
}

//==========================================================
// LCDBUFFER - работа с дисплейным буфером
// fill==0 нарисовать рамку цветом color
// fill==1 нарисовать прямоугольник с заливкой цветом color
//==========================================================
void LCDBUFFER_Rectangle (uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint16_t color, uint8_t fill)
{
	if(fill==0)
	{
		LCDBUFFER_Line(x0,y0,x1,y0,color);
		LCDBUFFER_Line(x0,y1,x1,y1,color);
		LCDBUFFER_Line(x0,y0,x0,y1,color);
		LCDBUFFER_Line(x1,y0,x1,y1,color);
	}
	else
	{
		for (uint16_t y=y0; y<y1; y++)		// Проходимся по координатам оси Y
		{
			for (uint16_t x=x0; x<x1; x++)	// Проходимся по координатам оси X
			{
				LCDBUFFER[(LCD_WIDTH*y)+x]=color;	// Делаем заливку по пикселям
			}
		}
	}
}

void LCDBUFFER_ProgressBarGradiend3colors(u16 x, u16 y, u16 w, u16 h, u16 clr1, u16 clr2, u16 clr3, float gr, float set)
{
  /* Calculaite hight gradient in pixel*/
  uint16_t usGrH =  (uint16_t)(h*gr/2);

  /* Paint top of set */
  for (int32_t i = (int32_t) (h*set); i >= 0; i--)
  {
    float alph = (float)(h*set-i) / (float)usGrH;
    if (alph > 1) alph = 1;
    uint16_t clr =  usAlphaBlending(clr1, clr2, alph);
    LCDBUFFER_Line (x, i+y, x+w, i+y, clr);
  }
  /* Paint bot of set  */
  
  /* Paint top of set */
  for (int32_t i = (int32_t)(h*set+1); i < h; i++)
  {
    float alph = (float)(i-h*set) / (float)usGrH;
    if (alph > 1) alph = 1;
    uint16_t clr =  usAlphaBlending(clr3, clr2, alph);
    LCDBUFFER_Line (x, i+y, x+w, i+y, clr);
  }
}

void LCDBUFFER_ProgressBarGradiend(u16 x, u16 y, u16 w, u16 h, u16 clr, float set)
{
  
  if (2*set > 1.f)
  {
    float pr = 2*set - 1;
    
    clr = usAlphaBlending(RED, YELLOW, pr);

    
    for (int16_t i = 0; i < h; i=i+1 )
    {
      float alph = 1 - (float)(i) / ((float)(h-h*pr));
      if (alph < 0) alph = 0;
      LCDBUFFER_LineAlpha (x, i + y , x+w,  i + y , clr,alph);
    }
  }
  else
  {
    clr = usAlphaBlending(YELLOW, GREEN, 2*set);
    for (int16_t i = 0; i < h*2*set; i=i+1 )
    {
    float alph = ((h*2*set - i) / (float)(h*2*set));
    if (alph < 0) alph = 0;
    LCDBUFFER_LineAlpha (x, i + y + (h - h*2*set), x+w,  i + y + ( h - h*2*set), clr,alph);
  
    }
  }
  #define GC 5
  for (uint32_t i = 0; i < GC; i++)
  {
    LCDBUFFER_LineAlpha (x+i, y , x+i,  (h)+y , BLACK, i/5.f);
    LCDBUFFER_LineAlpha (x+(w)-i, y , x+(w)-i,  (h)+y , BLACK, i/5.f);
  }
  
  LCDBUFFER_LineAlpha (x-1, y+(h-h*set) , x-3,  y+(h-h*set) , RED, 0.25);
  LCDBUFFER_LineAlpha (x+w+1, y+(h-h*set) , x+w+1+3,  y+(h-h*set) , RED, 0.25);

}

void LCDBUFFER_Gradient_RectangleAlpha (u16 x0, u16 y0, u16 x1, u16 y1, u16 color1, u16 color2)
{
	// Цвета начала градиента
        LCDBUFFER_Rectangle (x0, y0, x1, y1, color1, 1);
	
        for (u16 y=y0; y<y1; y++)	// Проходимся по координатам оси Y
	{
		// Вычисляю цвета
                float alph;
                
                alph = (float)(y-y0)/(float)(y1-y0);
		
                LCDBUFFER_LineAlpha (x0, y, x1, y, color2, alph);


	}
}


//=======================================================================
// LCDBUFFER - работа с дисплейным буфером
// Нарисовать прямоугольник двухцветным градиентом
// Начальный/конечный цвета градиента привязываются к низу/верху дисплея
// x0, y0, x1, y1 - отображаемая на дисплей часть градиента
//=======================================================================
void LCDBUFFER_Gradient_Rectangle (uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint16_t color1_RGB, uint16_t color2_RGB)
{
	// Цвета начала градиента
	uint8_t color1_R = color1_RGB>>11;
	uint8_t color1_G = (color1_RGB&0x7E0)>>5;
	uint8_t color1_B = color1_RGB&0x1F;
	// Цвета конца градиента
	uint8_t color2_R = color2_RGB>>11;
	uint8_t color2_G = (color2_RGB&0x7E0)>>5;
	uint8_t color2_B = color2_RGB&0x1F;
	// Для расчёта цветов R, G, B
	uint8_t color_R;
	uint8_t color_G;
	uint8_t color_B;
	// Для расчёта цвета RGB565
	uint16_t color_RGB;
	for (uint16_t y=y0; y<y1; y++)	// Проходимся по координатам оси Y
	{
		// Вычисляю цвета
		// Цвет изменяется только по оси Y
		color_R = (uint8_t)(color2_R + (((float)(color1_R-color2_R)/(LCD_HEIGHT-1)) * y));
		color_G = (uint8_t)(color2_G + (((float)(color1_G-color2_G)/(LCD_HEIGHT-1)) * y));
		color_B = (uint8_t)(color2_B + (((float)(color1_B-color2_B)/(LCD_HEIGHT-1)) * y));
		color_RGB = (color_R<<11) | (color_G<<5) | (color_B);
		
		for (uint16_t x=x0; x<x1; x++)	// Проходимся по координатам оси X
		{
			LCDBUFFER[(LCD_WIDTH*y)+x]=color_RGB;	// Делаем заливку по пикселям
		}
	}
}


//============================================
// LCDBUFFER - работа с дисплейным буфером
// Инвертирует цвета заданной области дисплея
//============================================
void LCDBUFFER_Inverse_Rectangle (uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1)
{
	for (uint16_t y=y0; y<y1; y++)			// Проходимся по координатам оси Y
	{
		for (uint16_t x=x0; x<x1; x++)		// Проходимся по координатам оси X
		{
			LCDBUFFER[(LCD_WIDTH*y)+x] = LCDBUFFER[(LCD_WIDTH*y)+x] ^ 0xFFFF;	// Инверсия пикселя
		}
	}
}

//============================================
// LCDBUFFER - работа с дисплейным буфером
// создает прямоугольник с заданным цветом и 
// прозрачностью. 1 - прозрачность 100%
// 0 - прозрачность 0%
//============================================
void LCDBUFFER_Alfa_Rectangle (uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint16_t color, float alf)
{
	for (uint16_t y=y0; y<y1; y++)			// Проходимся по координатам оси Y
	{
		for (uint16_t x=x0; x<x1; x++)		// Проходимся по координатам оси X
		{
                        uint16_t usColorRgb565wp = LCDBUFFER[(LCD_WIDTH*y)+x];
			LCDBUFFER[(LCD_WIDTH*y)+x] = usAlphaBlending(usColorRgb565wp, color, alf);
		}
	}
}                


//=========================================================================================
// LCDBUFFER - работа с дисплейным буфером
// Отобразить символ
// symbol – выводимый на экран символ в ASCII-кодировке; 
// x и y – начальная координата вывода символа; 
// t_color – цвет пикселя; 
// b_color – цвет фона; 
// переменные zoom_width и zoom_height увеличивают символ на экране в указанное число раз;
//=========================================================================================
void LCDBUFFER_Char (uint8_t symbol, uint16_t x, uint16_t y, uint16_t t_color, uint16_t b_color, uint8_t zoom_width, uint8_t zoom_height)
{
	uint16_t temp_symbol, a, b, zw, zh, mask; 
	int16_t m, n;
	m=x;
	n=y;
	if (symbol>127) symbol-=64;    //Убираем отсутствующую часть таблицы ASCII
	for ( a = 0; a < 5; a++)       //Перебираю 5 байт, составляющих символ
	{
		temp_symbol = font_5x8[symbol-32][a];
		zw = 0;
		while(zw != zoom_width)       //Вывод байта выполняется zw раз 
		{
			n=y;
			mask=0x01;
			for ( b = 0; b < 8; b++ )    //Цикл перебирания 8 бит байта
			{
				zh = 0; //в zoom_height раз увеличится высота символа
				while(zh != zoom_height)    //Вывод пикселя выполняется z раз
				{
					if (temp_symbol&mask)
					{
						LCDBUFFER_Pixel (m+zw, n+zh, t_color);
					}
					else
					{
						LCDBUFFER_Pixel (m+zw, n+zh, b_color);
					}
					zh++;
				}
				mask<<=1; //Смещаю содержимое mask на 1 бит влево;
				n=n+zoom_height;
			}
			zw++;
		}
		m=m+zoom_width;
	}
	/*
	// Закрашивание пробела между символами [Zlodey 21-04-2014]
	for ( b = y; b < 8+y; b++ ) LCDBUFFER_Pixel (x+5, b, RED);*/
}


//=========================================================================================
// LCDBUFFER - работа с дисплейным буфером
// Отобразить сиимвол (без цвета фона)
// symbol – выводимый на экран символ в ASCII-кодировке; 
// x и y – начальная координата вывода символа; 
// t_color – цвет пикселя; 
// переменные zoom_width и zoom_height увеличивают символ на экране в указанное число раз;
//=========================================================================================
void LCDBUFFER_Char_Shadow (uint8_t symbol, uint16_t x, uint16_t y, uint16_t t_color, uint8_t zoom_width, uint8_t zoom_height)
{
	uint16_t temp_symbol, a, b, zw, zh, mask;
	int16_t m, n;
	m=x;
	n=y;
	if (symbol>127) symbol-=64;    //Убираем отсутствующую часть таблицы ASCII
	for ( a = 0; a < 5; a++)       //Перебираю 5 байт, составляющих символ
	{  
		temp_symbol = font_5x8[symbol-32][a];
		zw = 0;
		while(zw != zoom_width)       //Вывод байта выполняется zw раз 
		{    
			n=y;
			mask=0x01;     
			for ( b = 0; b < 8; b++ )    //Цикл перебирания 8 бит байта
			{         
				zh = 0; //в zoom_height раз увеличится высота символа
				while(zh != zoom_height)    //Вывод пикселя выполняется z раз
				{
					if (temp_symbol&mask)
					{
						LCDBUFFER_Pixel (m+zw, n+zh, t_color);
					}
					zh++;
				}
				mask<<=1; //Смещаю содержимое mask на 1 бит влево;
				n=n+zoom_height;
			}
			zw++;
		}
		m=m+zoom_width;
	}
}

//=========================================================================================
// LCDBUFFER - работа с дисплейным буфером
// Отобразить сиимвол (без цвета фона)
// symbol – выводимый на экран символ в ASCII-кодировке; 
// x и y – начальная координата вывода символа; 
// t_color – цвет пикселя; 
// переменные zoom_width и zoom_height увеличивают символ на экране в указанное число раз;
//=========================================================================================
static void LCDBUFFER_Char_ShadowAlpha (uint8_t symbol, uint16_t x, uint16_t y, uint16_t t_color, uint8_t zoom_width, uint8_t zoom_height, float alph)//inner
{
	uint16_t temp_symbol, a, b, zw, zh, mask;
	int16_t m, n;
	m=x;
	n=y;
	if (symbol>127) symbol-=64;    //Убираем отсутствующую часть таблицы ASCII
	for ( a = 0; a < 5; a++)       //Перебираю 5 байт, составляющих символ
	{  
		temp_symbol = font_5x8[symbol-32][a];
		zw = 0;
		while(zw != zoom_width)       //Вывод байта выполняется zw раз 
		{    
			n=y;
			mask=0x01;     
			for ( b = 0; b < 8; b++ )    //Цикл перебирания 8 бит байта
			{         
				zh = 0; //в zoom_height раз увеличится высота символа
				while(zh != zoom_height)    //Вывод пикселя выполняется z раз
				{
					if (temp_symbol&mask)
					{
						LCDBUFFER_PixelAlpha (m+zw, n+zh, t_color, alph);
					}
					zh++;
				}
				mask<<=1; //Смещаю содержимое mask на 1 бит влево;
				n=n+zoom_height;
			}
			zw++;
		}
		m=m+zoom_width;
	}
}


//==========================================================================================
// LCDBUFFER - работа с дисплейным буфером
// Отобразить строку
// str – строка, которую хотим вывести на дисплей; 
// x и y – начальная координата вывода первого символа; 
// t_color – цвет пикселя; 
// b_color – цвет фона; 
// переменные zoom_width и zoom_height увеличивают символы на экране в указанное число раз;
//==========================================================================================
void LCDBUFFER_Str (char *str, uint16_t x, uint16_t y,  uint16_t t_color, uint16_t b_color, uint8_t zoom_width, uint8_t zoom_height)
{
	uint8_t i=0;
	
	if(zoom_width == 0)   zoom_width = 1;
	if(zoom_height == 0)  zoom_height = 1;
	
	while (str[i]) // x и y - адрес пикселя начальной позиции; с увеличением переменной i адрес вывода очередного символа смещается на i*6 (чем организуются столбцы дисплея)  
	{      
		LCDBUFFER_Char(str[i], x+(i*6*zoom_width), y, t_color, b_color, zoom_width, zoom_height);
		i++;
	}  
}


//==========================================================================================
// LCDBUFFER - работа с дисплейным буфером
// Отобразить строку (без цвета фона)
// str – строка, которую хотим вывести на дисплей; 
// x и y – начальная координата вывода первого символа; 
// t_color – цвет пикселя; 
// переменные zoom_width и zoom_height увеличивают символы на экране в указанное число раз;
//==========================================================================================
void LCDBUFFER_Str_Shadow (char *str, uint16_t x, uint16_t y,  uint16_t t_color, uint8_t zoom_width, uint8_t zoom_height)
{
	uint8_t i=0;
	
	if(zoom_width == 0)   zoom_width = 1;
	if(zoom_height == 0)  zoom_height = 1;
	
	while (str[i]) // x и y - адрес пикселя начальной позиции; с увеличением переменной i адрес вывода очередного символа смещается на i*6 (чем организуются столбцы дисплея)  
	{      
		LCDBUFFER_Char_Shadow(str[i], x+(i*6*zoom_width), y, t_color, zoom_width, zoom_height);
		i++;
	}  
}

void LCDBUFFER_Str_ShadowAlpha (char *str, uint16_t x, uint16_t y,  uint16_t t_color, uint8_t zoom_width, uint8_t zoom_height, float alph)
{
	uint8_t i=0;
	
	if(zoom_width == 0)   zoom_width = 1;
	if(zoom_height == 0)  zoom_height = 1;
	
	while (str[i]) // x и y - адрес пикселя начальной позиции; с увеличением переменной i адрес вывода очередного символа смещается на i*6 (чем организуются столбцы дисплея)  
	{      
		LCDBUFFER_Char_ShadowAlpha(str[i], x+(i*6*zoom_width), y, t_color, zoom_width, zoom_height, alph);
		i++;
	}  
}

void LCDBUFFER_BMP (uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint16_t* data)
{
        uint32_t i = 0;
	
	for (uint16_t y=y0; y<y1; y++)		// Проходимся по координатам оси Y
	{
          for (uint16_t x=x0; x<x1; x++)	// Проходимся по координатам оси X
          {
	     LCDBUFFER[(LCD_WIDTH*y)+x]=data[i++];	// Цвет RRRRRGGGGGGBBBBB
	  }
	}
}

//==========================================================================================
// LCDBUFFER - работа с дисплейным буфером
// Отобразить число
// number – число, которое хотим вывести на дисплей; 
// x и y – начальная координата вывода первого символа; 
// t_color – цвет пикселя; 
// b_color – цвет фона; 
// переменные zoom_width и zoom_height увеличивают символы на экране в указанное число раз;
//==========================================================================================
void LCDBUFFER_Num(int32_t number, uint16_t x, uint16_t y, uint16_t t_color, uint16_t b_color, uint8_t zoom_width, uint8_t zoom_height)
{
	char buffer[10];
	//sprintf(buffer, "%lu", number);	// беззнаковое
	sprintf(buffer, "%ld", number);		// знаковое
	LCDBUFFER_Str(buffer, x, y, t_color, b_color, zoom_width, zoom_height);
}


//===============================================================
// LCDBUFFER - работа с дисплейным буфером
// Отобразить число (без цвета фона)
// number – число, которое хотим вывести на дисплей;
// x и y – начальная координата вывода первого символа; 
// t_color – цвет пикселя; 
// переменные zoom_width и zoom_height увеличивают символы на экране в указанное число раз;
//===============================================================
void LCDBUFFER_Num_Shadow (int32_t number, uint16_t x, uint16_t y, uint16_t t_color, uint8_t zoom_width, uint8_t zoom_height)
{
	char buffer[10];
	//sprintf(buffer, "%lu", number);	// беззнаковое
	sprintf(buffer, "%ld", number);		// знаковое
	LCDBUFFER_Str_Shadow(buffer, x, y, t_color, zoom_width, zoom_height);
}

void LCDBUFFER_Num_ShadowAlpha (s32 number, u16 x, u16 y, u16 t_color, u8 zoom_width, u8 zoom_height, float alph)
{
	char buffer[10];
	//sprintf(buffer, "%lu", number);	// беззнаковое
	sprintf(buffer, "%ld", number);		// знаковое
	LCDBUFFER_Str_ShadowAlpha(buffer, x, y, t_color, zoom_width, zoom_height,alph);
}

//===================================================
// LCDBUFFER - работа с дисплейным буфером
// Нарисовать горизонтальную линию шириной 1 пиксель
//===================================================
void LCDBUFFER_HLine (uint16_t y, uint16_t color)
{
	LCDBUFFER_Line (0, y, LCD_W, y, color);
}




//=================================================
// LCDBUFFER - работа с дисплейным буфером
// Нарисовать вертикальную линию шириной 1 пиксель
//=================================================
void LCDBUFFER_VLine (uint16_t x, uint16_t color)
{ 
	LCDBUFFER_Line (x, 0, x, LCD_H, color);
}

void LCDBUFFER_VLineAlpha (uint16_t x, uint16_t color, float alph)
{ 
	LCDBUFFER_LineAlpha (x, 0, x, LCD_H, color, alph);
}




//========================================================
// LCDBUFFER - работа с дисплейным буфером
// Нарисовать жирную вертикальную линию шириной 3 пикселя
//========================================================
void LCDBUFFER_VLine_Bold (uint16_t x, uint16_t color)
{ 
	if (x > 0) LCDBUFFER_VLine (x-1, color);	// Дополнительная вертикальная линия слева
	LCDBUFFER_VLine (x, color);
	if (x < (LCD_W-1)) LCDBUFFER_VLine (x+1, color);	// Дополнительная вертикальная линия справа
}

//========================================================
// LCDBUFFER - работа с дисплейным буфером
// Нарисовать жирную вертикальную линию шириной 6 пикселя
//========================================================
void LCDBUFFER_VLine_H_Bold (uint16_t x, uint16_t color)
{ 
        if (x > 1) LCDBUFFER_VLine (x-2, color);
	if (x > 0) LCDBUFFER_VLine (x-1, color);	
	LCDBUFFER_VLine (x, color);
	if (x < (LCD_W-1)) LCDBUFFER_VLine (x+1, color);
        if (x < (LCD_W-2)) LCDBUFFER_VLine (x+2, color);	
}

//========================================================
// LCDBUFFER - работа с дисплейным буфером
// Нарисовать жирную вертикальную линию шириной 6 пикселя
//========================================================
void LCDBUFFER_VLine_Flash (uint16_t x, uint16_t color)
{ 
        if (x > 3) LCDBUFFER_VLineAlpha (x-4, color, 0.8);
        if (x > 2) LCDBUFFER_VLineAlpha (x-3, color, 0.6);
        if (x > 1) LCDBUFFER_VLineAlpha (x-2, color, 0.4);
	if (x > 0) LCDBUFFER_VLineAlpha (x-1, color, 0.2);	
	LCDBUFFER_VLineAlpha (x, color, 0.);
	if (x < (LCD_W-1)) LCDBUFFER_VLineAlpha (x+1, color, 0.2);
        if (x < (LCD_W-2)) LCDBUFFER_VLineAlpha (x+2, color, 0.4);
        if (x < (LCD_W-3)) LCDBUFFER_VLineAlpha (x+3, color, 0.6);
        if (x < (LCD_W-4)) LCDBUFFER_VLineAlpha (x+4, color, 0.8);
}

//========================================================
// LCDBUFFER - работа с дисплейным буфером
// Нарисовать жирную вертикальную линию шириной 6 пикселя
//========================================================
void LCDBUFFER_VLine_FlashMini (uint16_t x, uint16_t color)
{ 
        
        if (x > 1) LCDBUFFER_VLineAlpha (x-2, color, 0.66);
	if (x > 0) LCDBUFFER_VLineAlpha (x-1, color, 0.33);	
	LCDBUFFER_VLineAlpha (x, color, 0.);
	if (x < (LCD_W-1)) LCDBUFFER_VLineAlpha (x+1, color, 0.33);
        if (x < (LCD_W-2)) LCDBUFFER_VLineAlpha (x+2, color, 0.66);
       
}
//========================================================
// LCDBUFFER - работа с дисплейным буфером
// Нарисовать жирную вертикальную линию шириной 6 пикселя
//========================================================
void LCDBUFFER_VLine_HP_Bold (uint16_t x, uint16_t color)
{ 
        if (x > 1) LCDBUFFER_VLine (x-2, color);
	//if (x > 0) LCDBUFFER_VLine (x-1, color);	
	LCDBUFFER_VLine (x, color);
	//if (x < (LCD_W-1)) LCDBUFFER_VLine (x+1, color);
        if (x < (LCD_W-2)) LCDBUFFER_VLine (x+2, color);	
}


// Преобразовать VInt в VFloat и вывести по координатам x,y цветом Color
void LCDBUFFER_FloatV (int16_t VInt, uint16_t x, uint16_t y, uint16_t t_color)
{
	char string_buff[5];
	// Перевод из "float" в "string", выравнивание влево, всего 5 символов, после запятой 1 символ
	//sprintf(string_buff, "%-5.1f", VInt_to_VFloat(VInt));
	LCDBUFFER_Str (string_buff, x, y, t_color, BLACK, 1, 1);
}

/*--Callback drawing functions section end--*/

/*--Elementary inner drawing functions section begin--*/

/**
  * @brief  Fill Buffer
  * @param  pDst:        pointer to destination
  * @param  xSize:       X size
  * @param  ySize:       Y size
  * @param  OffLine:     offset after each line
  * @param  ColorIndex:  color to be used.           
  * @retval None.
  */
static void DMA2D_FillBuffer(void * pDst, uint32_t xSize, uint32_t ySize, uint32_t OffLine, uint32_t ColorIndex)//inner
{

  /* Set up mode */
  DMA2D->CR      = 0x00030000UL | (1 << 9);        
  DMA2D->OCOLR   = ColorIndex;                     

  /* Set up pointers */
  DMA2D->OMAR    = (uint32_t)pDst;                      

  /* Set up offsets */
  DMA2D->OOR     = OffLine;                        

  /* Set up pixel format */
  DMA2D->OPFCCR  = LTDC_PIXEL_FORMAT_RGB565;                    

  /*  Set up size */
  DMA2D->NLR     = (uint32_t)(xSize << 16) | (uint16_t)ySize;
  
  if(DMA2D->OMAR != (uint32_t)LCDBUFFER)
    asm("NOP");
    
  DMA2D->CR     |= DMA2D_CR_START; 
  
  /* Wait until transfer is done */
  while (DMA2D->CR & DMA2D_CR_START) 
  {
  }
}

/**
  * Function drawing pixel with specific parameters of color
  */
static void LCDBUFFER_Pixel (uint16_t x, uint16_t y, uint16_t color)//inner
{
	LCDBUFFER[(LCD_WIDTH*y)+x]=color;	// рисуем пиксель цветом "color"
}

void LCDBUFFER_BAAPixelAlpha (uint16_t x, uint16_t y, uint16_t color)
{
       
    float alph = 0.85f;
        
        if (!((x == 0)|(y==0)|(x==(LCD_WIDTH-1))| (y==(LCD_HEIGHT-1))))
        {
        
        
        LCDBUFFER[(LCD_WIDTH*(y-1))+(x-1)] = usAlphaBlending(LCDBUFFER[(LCD_WIDTH*(y-1))+(x-1)], color, alph);
        LCDBUFFER[(LCD_WIDTH*(y+0))+(x-1)] = usAlphaBlending(LCDBUFFER[(LCD_WIDTH*(y+0))+(x-1)], color, alph);
        LCDBUFFER[(LCD_WIDTH*(y+1))+(x-1)] = usAlphaBlending(LCDBUFFER[(LCD_WIDTH*(y+1))+(x-1)], color, alph);
        
        LCDBUFFER[(LCD_WIDTH*(y-1))+(x+0)] = usAlphaBlending(LCDBUFFER[(LCD_WIDTH*(y-1))+(x+0)], color, alph);
        LCDBUFFER[(LCD_WIDTH*(y+0))+(x+0)] = usAlphaBlending(LCDBUFFER[(LCD_WIDTH*(y+0))+(x+0)], color, 0.5);
        LCDBUFFER[(LCD_WIDTH*(y+1))+(x+0)] = usAlphaBlending(LCDBUFFER[(LCD_WIDTH*(y+1))+(x+0)], color, alph);
        
        LCDBUFFER[(LCD_WIDTH*(y-1))+(x+1)] = usAlphaBlending(LCDBUFFER[(LCD_WIDTH*(y-1))+(x+1)], color, alph);
        LCDBUFFER[(LCD_WIDTH*(y+0))+(x+1)] = usAlphaBlending(LCDBUFFER[(LCD_WIDTH*(y+0))+(x+1)], color, alph);
        LCDBUFFER[(LCD_WIDTH*(y+1))+(x+1)] = usAlphaBlending(LCDBUFFER[(LCD_WIDTH*(y+1))+(x+1)], color, alph);
        }
}

/**
  * Function drawing pixel with specific parameters of color and transparency
  */
static void LCDBUFFER_PixelAlpha (uint16_t x, uint16_t y, uint16_t color, float alph)//inner
{
        
	LCDBUFFER[(LCD_WIDTH*y)+x] = usAlphaBlending(LCDBUFFER[(LCD_WIDTH*y)+x], color, alph);	// рисуем пиксель цветом "color"
}

/**
  * Function drawing line with specific parameters of color and transparency
  */
static void LCDBUFFER_LineAlpha (uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t color, float alph)//inner
{ 
	int16_t  x, y, d, dx, dy, i, i1, i2, kx, ky;
	int8_t flag;
	
	dx = x2 - x1;
	dy = y2 - y1;
	if (dx == 0 && dy == 0) LCDBUFFER_PixelAlpha(x1, y1, color, alph);  //Точка
	else      //Линия
	{
		kx = 1;
		ky = 1;
		if( dx < 0 )
		{ 
			dx = -dx;
			kx = -1;
		}
		else
		if(dx == 0) kx = 0;
		if(dy < 0)
		{ 
			dy = -dy;
			ky = -1;
		}
		if(dx < dy)
		{ 
			flag = 0;
			d = dx;
			dx = dy;
			dy = d;
		}
		else flag = 1;
		i1 = dy + dy;
		d = i1 - dx;
		i2 = d - dx;
		x = x1;
		y = y1;
		
		for(i=0; i < dx; i++)
		{
			LCDBUFFER_PixelAlpha(x, y, color, alph);
			if(flag) x += kx;
			else y += ky;
			if( d < 0 ) d += i1;
			else
			{
				d += i2;
				if(flag) y += ky;
				else x += kx;
			}
		}
		LCDBUFFER_PixelAlpha(x, y, color, alph);
	}
}

/*---Elementary inner drawing functions section end---*/