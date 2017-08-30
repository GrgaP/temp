#include "main.h"
#include "uart_data_test.h"
#include "spectra_data.h"
#include <string.h>

/* Private variables ---------------------------------------------------------*/
extern UART_HandleTypeDef huart6;

osSemaphoreId semSerialCommReceivedHandle;
osSemaphoreId semSerialCommTxCompletedHandle;
osThreadId USART6_ThreadId;
osThreadId SPECTRA_DATA_ThreadId;

xQueueHandle uart_data_ptr_queue;

uint8_t const test_data_tx[4] = {0x55,0xAA,0x5A,0xA5}; 
uint16_t const addr_inc = sizeof(spectra_data_clear[0]);
uint16_t const addr1_end = (SPECTRA_DATA_CLEAR_NUMBER-1) * sizeof(spectra_data_clear[0]);
uint8_t index = 0;
uint8_t stage = 0;


/* Private function prototypes -----------------------------------------------*/
static void USART6_DMA_Thread(void const *argument);
static void SPECTRA_DATA_Thread(void const *argument);

void svd_uart_module_cfg(void){

   int16_t* uart_rx_buf_ptr1;//адрес приемного буфера
   int16_t* uart_rx_buf_ptr2;//адрес приемного буфера
   
   uart_rx_buf_ptr1 = (int16_t*) pvPortMalloc(1024*sizeof(int16_t));//выделяем память под принимаемые по UART данные (пока 2048 байт);
   uart_rx_buf_ptr2 = (int16_t*) pvPortMalloc(1024*sizeof(int16_t));//выделяем память под принимаемые по UART данные (пока 2048 байт);
   
  
   semSerialCommReceivedHandle = xSemaphoreCreateCounting(3,0);//определяем семафор завершения приема фрейма данных;
   semSerialCommTxCompletedHandle = xSemaphoreCreateCounting(3,0);//определяем семафор завершения передачи фрейма данных;
  
   uart_data_ptr_queue = xQueueCreate(2,sizeof(int16_t*));//создаем очередь для ВЕЧНОГО хранения указателя на приемный буфер
  
   if(uart_data_ptr_queue == NULL)
     asm("NOP");//добавить вызов обработчика ошибок - очередь не создана
   else
    xQueueSend(uart_data_ptr_queue,&uart_rx_buf_ptr1,osWaitForever);//регистрируем в очереди указатель на приемный буфер
    xQueueSend(uart_data_ptr_queue,&uart_rx_buf_ptr2,osWaitForever);//регистрируем в очереди указатель на приемный буфер
  
  //Инициализация основной задачи: 
  //osThreadDef(USART6, USART6_DMA_Thread, osPriorityHigh, 0, 128);
  //USART6_ThreadId = osThreadCreate (osThread(USART6), NULL);
  
  //Инициализация тестовой задачи: 
  osThreadDef(SPECTRA_DATA, SPECTRA_DATA_Thread, osPriorityLow, 0, 128);
  SPECTRA_DATA_ThreadId = osThreadCreate (osThread(SPECTRA_DATA), NULL);  
  
}			

/**
  * Отладочная задача выдает вместо данных с USART, ранее записанные данные из памяти.
  */
static void SPECTRA_DATA_Thread(void const *argument)
{
  portTickType xLastWakeTime;
  xLastWakeTime = xTaskGetTickCount();  
  
  int16_t* tmp_ptr1 = NULL;
  int16_t* tmp_ptr2 = NULL; 
  uint8_t max_index;

  for(;;)
  {
    
      
    if(uxQueueMessagesWaiting(uart_data_ptr_queue) == 0)
      asm("NOP");//добавить вызов обработчика ошибок - очередь не должна быть пуста!
    max_index = SPECTRA_DATA_TNT_NUMBER;
    xQueueReceive(uart_data_ptr_queue,&tmp_ptr1,0);//взяли указатель на приемный буфер только чтобы удалить его из очереди!!!(только в автономном режиме)
    xQueueReceive(uart_data_ptr_queue,&tmp_ptr2,0);//взяли указатель на приемный буфер только чтобы удалить его из очереди!!!(только в автономном режиме)
    switch(stage){
    case 0:
      max_index = SPECTRA_DATA_CLEAR_NUMBER;
      tmp_ptr1 = (int16_t*) spectra_data_clear[index];
      tmp_ptr2 = (int16_t*) spectra_data_clear[max_index-index];
      break;
    case 1:
      tmp_ptr1 = (int16_t*) spectra_data_TNTlevel1[index];
      tmp_ptr2 = (int16_t*) spectra_data_TNTlevel2[index];
      break;
    case 2:
      tmp_ptr1 = (int16_t*) spectra_data_TNTlevel2[index];
      tmp_ptr2 = (int16_t*) spectra_data_TNTlevel3[index];
      break;
    case 3:
      tmp_ptr1 = (int16_t*) spectra_data_TNTlevel3[index];
      tmp_ptr2 = (int16_t*) spectra_data_TNTlevel1[index];
      break;
    default:
        break;
    };
        
    xQueueSend(uart_data_ptr_queue,&tmp_ptr1,osWaitForever);//сохраняем вместо него адрес буфера с ранее сохраненными данными! 
    xQueueSend(uart_data_ptr_queue,&tmp_ptr2,osWaitForever);
    index++;//это для 
    xSemaphoreGive(semSerialCommReceivedHandle);//поднимаем семафор о обновлении данных UART
    if(index == max_index){index = 0; stage++;};      
    if(stage == 4)
      stage = 0;
       
    vTaskDelayUntil(&xLastWakeTime,500/portTICK_RATE_MS);
  }  
}

/**
  * Задача выполняет обработку принятых по UART, через DMA, данных, а так же инициирует передачу ответов.
  */
static void USART6_DMA_Thread(void const *argument)
 {
   int16_t* uart_rx_buf_tmp_ptr;
   /* USER CODE BEGIN TaskSerialCommStart */  
   
  /* Set semaphores to default state */  
  //osSemaphoreWait(semSerialCommReceivedHandle, osWaitForever);
  //osSemaphoreWait(semSerialCommTxCompletedHandle, osWaitForever);
   
  xQueuePeek(uart_data_ptr_queue,&uart_rx_buf_tmp_ptr,0);//посмотрели указатель на приемный буфер
  
  HAL_UART_Receive_DMA(&huart6, (uint8_t *)uart_rx_buf_tmp_ptr, USART_DATA_SIZE);//запускаем процесс приема
  /* Infinite loop */
  for(;;)
  {
    osDelay(50); 
    
    osSemaphoreWait(semSerialCommReceivedHandle, osWaitForever);
    HAL_UART_DMAStop(&huart6);
    //передаем в очередь указатель на буфер с данными
    HAL_UART_Transmit_DMA(&huart6, (uint8_t *)test_data_tx, sizeof(test_data_tx));
    
    osSemaphoreWait(semSerialCommTxCompletedHandle, osWaitForever);
    HAL_UART_DMAStop(&huart6);
    HAL_UART_Receive_DMA(&huart6, (uint8_t *)uart_rx_buf_tmp_ptr, USART_DATA_SIZE);
  };

 }
 
void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
 {
   portBASE_TYPE xTaskWoken;   
   xSemaphoreGiveFromISR(semSerialCommTxCompletedHandle, &xTaskWoken);
 }

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
 {
   portBASE_TYPE xTaskWoken;
   xSemaphoreGiveFromISR(semSerialCommReceivedHandle, &xTaskWoken);
 }


