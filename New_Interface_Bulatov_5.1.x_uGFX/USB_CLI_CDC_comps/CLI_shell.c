#include "CLI_shell.h"

static void StartStandartIOTest(void);
static void StandartIOTestCode (void *pvPrams);
static void shellGetLine(FILE* streamIO, char *line, size_t size);

FILE* usbStream = NULL;

void CLI_IO_init(void)
{
	StartStandartIOTest();
}

/**
  * @brief  Lanch OS monitoring application
  * @param  None
  * @retval None
  */
static void StartStandartIOTest(void)
{
  xTaskCreate(  StandartIOTestCode, 
                "StandartIOTest", 
                configMINIMAL_STACK_SIZE, 
                NULL, 
                tskIDLE_PRIORITY+1, 
                NULL
              );
}

/**
  * @brief  ShellCode
  * @param  None
  * @retval None
  */
static void StandartIOTestCode (void *pvPrams)
{
  char *pcOutputString = NULL; 
  extern void vRegisterCLICommands(void);
  vRegisterCLICommands();
  pcOutputString = FreeRTOS_CLIGetOutputBuffer();
  
  usbStream = fopen("//USB_HS_CDC/", "w+");
  if (usbStream == NULL)
  {
    asm("NOP");/* Error fix */
  }
  /* Set buffer type line for transsmit */
  setvbuf(usbStream, NULL, _IONBF, 1);
  
  while( USB_GetFileOpenStatus() == 0 ) { /* waiting */ }
  fprintf(usbStream, "/******* Wellcom to DIMS Shell! *******/\r\n");
  
  #define STRING_TEST_DELAY  100
  
  for(;;)
  {
    vTaskDelay(STRING_TEST_DELAY);
    
    fprintf(usbStream, "DIMS_Shell << : ");
    
    #define MAX_CMD_LENGH 64
    static char strCmd[MAX_CMD_LENGH];
        
    shellGetLine(usbStream, strCmd, MAX_CMD_LENGH);
    
    BaseType_t xReturn;
    
    if (*strCmd != 0)
    {
      do
      {
        xReturn = FreeRTOS_CLIProcessCommand( strCmd, \
                                    pcOutputString, \
                                    configCOMMAND_INT_MAX_OUTPUT_SIZE );
        fprintf(usbStream, pcOutputString);
      } while (xReturn != pdFALSE);
    }
  }
}

/**
 * @brief   Reads line thith echo on stream.
 * @param steramIO      pointer to a FILE
 * @param line          pointer to the line buffer
 * @param size      buffer maximum length
 */
static void shellGetLine(FILE* streamIO, char *line, size_t size)
{
  char *p = line;

  memset(p, 0, size);
  while (1) 
  {
    int c;
    
    /* Read input char */
    clearerr(streamIO);
    c = fgetc(streamIO);
    /* For set EOF and unblock fputc */
    fgetc(streamIO);
    
    /* No Data */
    if (c == -1)
    {
      continue;
    }
    /* backsace process */
    if (c == '\b') 
    {
      if (p != line) 
      {
        fputc(c, streamIO);
        fputc(' ', streamIO);
        fputc(c, streamIO);
        p--;
      }
      continue;
    }
    /* del proces */
    if (c == 0x7F) 
    {
      if (p != line) 
      {
        fputc(c, streamIO);
        fputc(' ', streamIO);
        fputc(c, streamIO);
        p--;
      }
      continue;
      
    }
    /* Command enter */
    if (c == '\r') 
    {
      fputc('\a', streamIO);
      fputc('\n', streamIO);
      fputc('\r', streamIO);
      *p = 0;
      return;
    }
    /* Write char */
    if (p < line + size - 1) 
    {
      fputc(c, streamIO);
      *p++ = (char)c;
    }
  }
}