/* Default bus configuration */
#include "i2c_drv.h"



#define I2C_DEFAULT_INSTANCE I2C3
#define I2C_DEFAULT_TIMING 0x204064ff
#define I2C_DEFAULT_TIMEOUT 0x3000 /*<! The value of the maximal timeout for I2C waiting loops */

static const I2C_HandleTypeDef I2c_default_Handle = {
	I2C_DEFAULT_INSTANCE,			//instance
	{	
		I2C_DEFAULT_TIMING,			//uint32_t Timing
		0,							//uint32_t OwnAddress1
		I2C_ADDRESSINGMODE_7BIT,	//uint32_t AddressingMode
		I2C_DUALADDRESS_DISABLED,	//uint32_t DualAddressMode
		0,							//uint32_t OwnAddress2
		0,							//uint32_t OwnAddress2Masks
		I2C_GENERALCALL_DISABLED,	//uint32_t GeneralCallMode
		I2C_NOSTRETCH_DISABLED		//uint32_t NoStretchMode
		
	},
};



#default DEF_I2C_MSP_INIT_PROC(i2chadle) 

/* Device 1 - Touchscreen sensor */

#define DEV1_INSTANCE I2C_DEFAULT_INSTANCE
#define DEV1_MSP_INIT_PROC DEF_I2C_MSP_INIT_PROC

#define DEV1_ADDR 0x55AA
#define DEV1_CFG I2c_default_Handle
#define DEV1_I2C_MSP_INIT DEF_I2C_MSP_INIT_PROC

/* Device 2 - example */

/*static const I2C_HandleTypeDef I2c_dev2_Handle = {
	I2C_DEFAULT_INSTANCE,			//instance
	{	
		I2C_DEFAULT_TIMING,			//uint32_t Timing
		0,							//uint32_t OwnAddress1
		I2C_ADDRESSINGMODE_7BIT,	//uint32_t AddressingMode
		I2C_DUALADDRESS_DISABLED,	//uint32_t DualAddressMode
		0,							//uint32_t OwnAddress2
		0,							//uint32_t OwnAddress2Masks
		I2C_GENERALCALL_DISABLED,	//uint32_t GeneralCallMode
		I2C_NOSTRETCH_DISABLED		//uint32_t NoStretchMode
		
	},
};*/

#define DEV2_ADDR 0x55AA
#define DEV2_HANDLE I2c_default_Handle
#define DEV2_I2C_MSP_INIT DEF_I2C_MSP_INIT_PROC

/* Device 3 - example */
#define DEV3_ADDR 0x55AA
#define DEV3_HANDLE I2c_default_Handle
#define DEV3_I2C_MSP_INIT DEF_I2C_MSP_INIT_PROC