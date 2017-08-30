#include "ili9341_bsp_cfg.h"

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

void ILI9341_init(void)
{
        gpio_init();
        ILI9341_WRX_SET;
        ILI9341_RST_HI;
        
        spi_init();
        
        HAL_Delay(100);
        
        ILI9341_RST_HI;
  
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

