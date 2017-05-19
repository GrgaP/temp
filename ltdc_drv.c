#include "ltdc_drv.h"
#include "ltdc_bsp_cfg.h"

static LTDC_HandleTypeDef hltdc;

/* Adress of the framebuffer that is visible now */
#define LTDC_FB_ADDR(X) (FB_BEGIN_ADDR + X*(LCD_PIX_SZ*LCD_PIX_HIGHT*LCD_PIX_WIDTH))//instead of "x" need put "fb_index"

uint8_t fb_index = 0;//A framebuffer switching index. It can have only two values: 0 or 1.
//static uint8_t fb_ch_flag = 0;//This variable is signal that indicate about need change a framebuffer

osSemaphoreId sem_ltdc_fb_chd_id;
osSemaphoreDef(sem_ltdc_fb_chd_id);
               
/*Init section begin---------------------------------------*/

#define HSW		(LCD_HSYNC - 1)
#define AHBP		(LCD_HSYNC + LCD_HBP - 1)
#define AAW		(LCD_HSYNC + LCD_HBP + LCD_HAREA - 1)
#define TOTALW		(LCD_HSYNC + LCD_HBP + LCD_HAREA + LCD_HFP - 1)
#define VSH		(LCD_VSYNC - 1)
#define AVBP		(LCD_VSYNC + LCD_VBP - 1)
#define AAH		(LCD_VSYNC + LCD_VBP + LCD_VAREA - 1)
#define TOTALH		(LCD_VSYNC + LCD_VBP + LCD_VAREA + LCD_VFP - 1)

static void LTDC_ltdc_init(void)
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

  HAL_LTDC_MspInit(&hltdc);
  
  HAL_LTDC_Init(&hltdc);
}
  
static void LTDC_layers_init(void)
{
  LTDC_LayerCfgTypeDef pLayerCfg;
  
  pLayerCfg.WindowX0 = 0;
  pLayerCfg.WindowX1 = LCD_PIX_WIDTH;
  pLayerCfg.WindowY0 = 0;
  pLayerCfg.WindowY1 = LCD_PIX_HIGHT;
  
  pLayerCfg.PixelFormat = LTDC_PIXEL_FORMAT_RGB565;
  pLayerCfg.Alpha = 0xff;//fully visible
  
  pLayerCfg.BlendingFactor1 = LTDC_BLENDING_FACTOR1_CA;
  pLayerCfg.BlendingFactor2 = LTDC_BLENDING_FACTOR2_CA;
  pLayerCfg.FBStartAdress = LTDC_FB_ADDR(fb_index);
      
  pLayerCfg.ImageWidth = LCD_PIX_WIDTH;
  pLayerCfg.ImageHeight = LCD_PIX_HIGHT;
  
  pLayerCfg.Backcolor.Reserved = 0;
  pLayerCfg.Backcolor.Reserved = 0;
  pLayerCfg.Backcolor.Reserved = 0;
  pLayerCfg.Alpha0 = 0xff;
  
  HAL_LTDC_ConfigLayer(&hltdc, &pLayerCfg, 0);
  
  /*--*/
  
  //HAL_LTDC_EnableDither(&hltdc);
  
  HAL_LTDC_ConfigColorKeying(&hltdc,0x000000,0);
  
  HAL_LTDC_EnableColorKeying(&hltdc, 0);
}

uint8_t LTDC_drv_init(void)
{
  LTDC_ltdc_init();//common LTDC init
  
  LTDC_layers_init();//Layer 0 LTDC init
  
  sem_ltdc_fb_chd_id = osSemaphoreCreate(osSemaphore(sem_ltdc_fb_chd_id), 1);
  
  return 0x01;
}

/*Init section end---------------------------------------*/


/*Run section begin--------------------------------------*/

/**
  * This function called from user application and initiate changing of framebuffer. Address of new free frame buffer will be returned
  */
void LTDC_drv_fb_update(void)
{
	fb_index = !fb_index;
        
        __HAL_LTDC_LAYER(&hltdc, 0)->CFBAR = LTDC_FB_ADDR(fb_index);///Set new framebuffer adress in shadow register for next displaying
  
        //__HAL_LTDC_RELOAD_CONFIG(&hltdc);
        
        HAL_LTDC_ProgramLineEvent(&hltdc, 0);//initiate event at passing of screen edge
}

uint32_t LTDC_drv_get_free_fb_addr(void)
{
	osSemaphoreWait (sem_ltdc_fb_chd_id, osWaitForever);
        return (LTDC_FB_ADDR(!fb_index));//return adress of free framebuffer
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
        
        __HAL_LTDC_RELOAD_CONFIG(hltdc);        
        
        //HAL_LTDC_ProgramLineEvent(hltdc, 0);//initiate event at passing of screen edge
        
        osSemaphoreRelease (sem_ltdc_fb_chd_id);
}

/* 
BOOL LTDC_drv_ctrl(ltdc_drv_msg_td cnrl_cmd)
{
	
}
	

*/


/*--Run section end--*/