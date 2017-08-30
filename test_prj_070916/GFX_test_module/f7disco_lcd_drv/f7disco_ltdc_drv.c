/**
 ******************************************************************************
 * @file    ltdc_drv.c
 * @author  SvD
 * @version V1.0.0
 * @date    30.08.2016
 * @brief       Brief description.
 * @details     Detailed description.
 *              $Id$
 ****************************************************************************** 
 */ 
 
typedef struct ltdc_layer_cfg_td{
  // Frame buffer address
  // Frame size in pixels
  // Line pitch, in bytes
  // Pixel format in LTDC format
  
  
  // Start pixel position of the virtual layer
  // Size of the virtual layer
  
  // Default color, ARGB8888
  // Color key, RGB888
  // Blending factors
  // The palette, RGB888 (can be NULL)
  // Palette length
  // Constant alpha factor
  // Layer configuration  
}ltdc_layer_cfg;

typedef struct ltdc_cfg_td{
  //screen size
  //hsync, vsync
  //hbp, vbp
  //hfp, vfp
  //bgcolor
  
  ///background layer config 
  ///foreground layer config

  ///framebuffer refresh callback
}ltdc_cfg;

ltdc_layers_init()
{
  
}

void ltdc_init()
{
  ;
}

/**
  * @brief  Line Event callback.
  * @param  hltdc: pointer to a LTDC_HandleTypeDef structure that contains
  *                the configuration information for the specified LTDC.
  * @retval None
  */
void HAL_LTDC_LineEvenCallback(LTDC_HandleTypeDef *hltdc)
{
  ///go to framebuffer refresh callback  
}

