/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __CLI_SHELL_H
#define __CLI_SHELL_H

#include "main.h"
#include "FreeRTOS_CLI.h"
#include "usb_device.h"
#include "stdio.h"
#include "IO_USB_LL_Driver.h"
#include "IO_USB_LL_Driver.h"

extern FILE* usbStream;

void CLI_IO_init(void);

#ifdef __cplusplus
}
#endif
#endif /*__CLI_SHELL_H */