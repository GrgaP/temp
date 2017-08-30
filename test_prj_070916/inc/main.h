#ifndef __MAIN_H
#define __MAIN_H

/* Includes ---------------------------------------------------------------------*/
#include "stm32f7xx_hal.h"
#include "stm32f7xx_it.h"

#include "cmsis_os.h"

#include "gui.h"
//#include "gf.h"

#include "uart_data_test.h"
#include "data_processing.h"

/* utilities and standard includes components */   

#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

/* HW Includes ------------------------------------------------------------------*/
#ifdef USE_STM32746G_DISCOVERY
  #include "stm32746g_discovery.h"
  #include "stm32746g_discovery_sdram.h"
  //#include "stm32746g_discovery_ts.h"


  
 // #include "k_bsp.h"
#endif
#ifdef DIMS
/*---------------------------------*/
#endif

#define COUNTOF(X) (sizeof(X)/sizeof(X[0]))

#define SCREEN_SIZE_X 240
#define GRAPH_WIDTH SCREEN_SIZE_X
#define SCREEN_SIZE_Y 320
#define GRAPH_HIGH SCREEN_SIZE_Y
#define USART_DATA_SIZE 1024
#define GRAPH_DATA_SIZE USART_DATA_SIZE

extern xQueueHandle graph_data_ptr_queue;

#endif /* __MAIN_H */