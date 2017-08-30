/* FreeRTOS includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "adc_battery.h"

/* Standard includes. */
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

/* FreeRTOS+CLI includes. */
#include "FreeRTOS_CLI.h"
/* Prototype */
static BaseType_t GetBattState( char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString );

static const CLI_Command_Definition_t GetBattStateCmdDefenition =
{
  ( const char * const ) "GetBattState", /* The command string to type. */
  ( const char * const ) "GetBattState: Used for getting info about current the battery charge level 'GetBattState x', there x - may be 'v'(that get current voltage value) or '%'(that get charge level in percentage) ",
  GetBattState,     /* The function to run. */
  1             /* Number of parameters are expected. */
};

void vRegisterCLICommands( void )
{
	/* Register all the command line commands defined immediately above. */
	FreeRTOS_CLIRegisterCommand( &GetBattStateCmdDefenition );
}

static BaseType_t GetBattState( char *pcWriteBuffer,
                            size_t xWriteBufferLen,
                      const char *pcCommandString )
{
  char  *pcParameter1;
  BaseType_t xParameter1StringLength;

    /* Obtain the name of the source file, and the length of its name, from
    the command string. The name of the source file is the first parameter. */
  pcParameter1 = ( char* )FreeRTOS_CLIGetParameter
                        (
                          /* The command string itself. */
                          pcCommandString,
                          /* Return the first parameter. */
                          1,
                          /* Store the parameter string length. */
                          &xParameter1StringLength
                        );

   /* Terminate both file names. */
   pcParameter1[xParameter1StringLength] = 0x00;
   

   
   const char * paramItem[2] = 
   { 
     "v",  
     "%",
   };
   
   uint8_t nameFind = 0;
   
   
   
   for ( uint32_t i = 0; i < 2; i++ )
   {
     if (strcmp(paramItem[i], pcParameter1) == 0)
     {
       char str[64];
		
       switch(i)
		{
			case(0):
			{
				//strcat(str, "BattVoltage: ");
				sprintf(str, "BattVoltage: = %u", ADC_BAT_get_voltage());
                                //strcat(str, paramItem[i]);
				strcat(str, "\r\n\r\n");				
			}break;
			case(1):
			{
				//strcat(str, "BattPrcLv: ");
                                sprintf(str, "BattPrcLv: = %u", ADC_BAT_get_prc());
				//strcat(str, paramItem[i]);
				strcat(str, "\r\n\r\n");	
			}			
		};

        strncpy(pcWriteBuffer, str, configCOMMAND_INT_MAX_OUTPUT_SIZE); 
        
        nameFind++;        
     };
   } 
   
   if (!nameFind) 
   {
      strncpy(pcWriteBuffer, "error", 
              configCOMMAND_INT_MAX_OUTPUT_SIZE);
   }
   return pdFALSE;
}