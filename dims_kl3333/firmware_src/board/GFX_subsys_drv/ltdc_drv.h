#ifndef __LTDC_DRV_H		
#define __LTDC_DRV_H

#include "stm32f7xx_hal.h"

//extern osSemaphoreId sem_ltdc_fb_chd_id;

uint8_t LTDC_drv_init(void);

void LTDC_drv_fb_update(void);

uint32_t LTDC_drv_get_free_fb_addr(void);

#endif /* __LTDC_DRV_H */


