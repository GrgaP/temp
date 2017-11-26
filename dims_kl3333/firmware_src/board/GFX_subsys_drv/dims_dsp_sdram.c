
#include "dims_dsp_sdram.h"

static SDRAM_HandleTypeDef SdramHandle;
static FMC_SDRAM_TimingTypeDef Timing;
static FMC_SDRAM_CommandTypeDef Command;

static void MspInit(void);

void BSP_SDRAM_Init(void)
{
  /* SDRAM device configuration */
  SdramHandle.Instance = FMC_SDRAM_DEVICE;

  /* FMC Configuration -------------------------------------------------------*/
  /* FMC SDRAM Bank configuration */
  /* Timing configuration for 90 Mhz of SD clock frequency (180Mhz/2) */
  /* TMRD: 2 Clock cycles */
  Timing.LoadToActiveDelay    = 2;
  /* TXSR: min=70ns (7x11.11ns) */
  Timing.ExitSelfRefreshDelay = 7;
  /* TRAS: min=42ns (4x11.11ns) max=120k (ns) */
  Timing.SelfRefreshTime      = 4;
  /* TRC:  min=70 (7x11.11ns) */
  Timing.RowCycleDelay        = 7;
  /* TWR:  min=1+ 7ns (1+1x11.11ns) */
  Timing.WriteRecoveryTime    = 2;
  /* TRP:  20ns => 2x11.11ns*/
  Timing.RPDelay              = 2;
  /* TRCD: 20ns => 2x11.11ns */
  Timing.RCDDelay             = 2;
  
  /* FMC SDRAM control configuration */
  SdramHandle.Init.SDBank             = FMC_SDRAM_BANK2;
  /* Row addressing: [7:0] */
  SdramHandle.Init.ColumnBitsNumber   = FMC_SDRAM_COLUMN_BITS_NUM_8;
  /* Column addressing: [11:0] */
  SdramHandle.Init.RowBitsNumber      = FMC_SDRAM_ROW_BITS_NUM_12;
  SdramHandle.Init.MemoryDataWidth    = SDRAM_MEMORY_WIDTH;
  SdramHandle.Init.InternalBankNumber = FMC_SDRAM_INTERN_BANKS_NUM_4;
  SdramHandle.Init.CASLatency         = SDRAM_CAS_LATENCY;
  SdramHandle.Init.WriteProtection    = FMC_SDRAM_WRITE_PROTECTION_DISABLE;
  SdramHandle.Init.SDClockPeriod      = SDCLOCK_PERIOD;
  SdramHandle.Init.ReadBurst          = SDRAM_READBURST;
  SdramHandle.Init.ReadPipeDelay      = FMC_SDRAM_RPIPE_DELAY_1;
                    
  /* SDRAM controller initialization */
  MspInit();
  HAL_SDRAM_Init(&SdramHandle, &Timing);
  
  /* SDRAM initialization sequence */
  BSP_SDRAM_Initialization_sequence(REFRESH_COUNT);
}

/**
  * @brief  Programs the SDRAM device.
  * @param  RefreshCount: SDRAM refresh counter value 
  */
void BSP_SDRAM_Initialization_sequence(uint32_t RefreshCount)
{
  __IO uint32_t tmpmrd =0;
  
  /* Step 1:  Configure a clock configuration enable command */
  Command.CommandMode             = FMC_SDRAM_CMD_CLK_ENABLE;
  Command.CommandTarget           = FMC_SDRAM_CMD_TARGET_BANK2;
  Command.AutoRefreshNumber       = 1;
  Command.ModeRegisterDefinition  = 0;

  /* Send the command */
  HAL_SDRAM_SendCommand(&SdramHandle, &Command, SDRAM_TIMEOUT);

  /* Step 2: Insert 100 us minimum delay */ 
  /* Inserted delay is equal to 1 ms due to systick time base unit (ms) */
  HAL_Delay(1);

  /* Step 3: Configure a PALL (precharge all) command */ 
  Command.CommandMode             = FMC_SDRAM_CMD_PALL;
  Command.CommandTarget           = FMC_SDRAM_CMD_TARGET_BANK2;
  Command.AutoRefreshNumber       = 1;
  Command.ModeRegisterDefinition  = 0;

  /* Send the command */
  HAL_SDRAM_SendCommand(&SdramHandle, &Command, SDRAM_TIMEOUT);  
  
  /* Step 4: Configure an Auto Refresh command */ 
  Command.CommandMode             = FMC_SDRAM_CMD_AUTOREFRESH_MODE;
  Command.CommandTarget           = FMC_SDRAM_CMD_TARGET_BANK2;
  Command.AutoRefreshNumber       = 4;
  Command.ModeRegisterDefinition  = 0;

  /* Send the command */
  HAL_SDRAM_SendCommand(&SdramHandle, &Command, SDRAM_TIMEOUT);
  
  /* Step 5: Program the external memory mode register */
  tmpmrd = (uint32_t)SDRAM_MODEREG_BURST_LENGTH_1          |
                     SDRAM_MODEREG_BURST_TYPE_SEQUENTIAL   |
                     SDRAM_MODEREG_CAS_LATENCY_3           |
                     SDRAM_MODEREG_OPERATING_MODE_STANDARD |
                     SDRAM_MODEREG_WRITEBURST_MODE_SINGLE;
  
  Command.CommandMode             = FMC_SDRAM_CMD_LOAD_MODE;
  Command.CommandTarget           = FMC_SDRAM_CMD_TARGET_BANK2;
  Command.AutoRefreshNumber       = 1;
  Command.ModeRegisterDefinition  = tmpmrd;

  /* Send the command */
  HAL_SDRAM_SendCommand(&SdramHandle, &Command, SDRAM_TIMEOUT);
  
  /* Step 6: Set the refresh rate counter */
  /* Set the device refresh rate */
  HAL_SDRAM_ProgramRefreshRate(&SdramHandle, RefreshCount); 
}

/**
  * @brief  Reads an mount of data from the SDRAM memory in polling mode. 
  * @param  uwStartAddress : Read start address
  * @param  pData : Pointer to data to be read  
  * @param  uwDataSize: Size of read data from the memory
  */
void BSP_SDRAM_ReadData_32b(uint32_t uwStartAddress, uint32_t *pData, uint32_t uwDataSize)
{
  HAL_SDRAM_Read_32b(&SdramHandle, (uint32_t *)uwStartAddress, pData, uwDataSize); 
}

/**
  * @brief  Reads an mount of data from the SDRAM memory in polling mode. 
  * @param  uwStartAddress : Read start address
  * @param  pData : Pointer to data to be read  
  * @param  uwDataSize: Size of read data from the memory
  */
void BSP_SDRAM_ReadData_16b(uint32_t uwStartAddress, uint16_t *pData, uint32_t uwDataSize)
{
  HAL_SDRAM_Read_16b(&SdramHandle, (uint32_t *)uwStartAddress, pData, uwDataSize); 
}

/**
  * @brief  Reads an mount of data from the SDRAM memory in polling mode. 
  * @param  uwStartAddress : Read start address
  * @param  pData : Pointer to data to be read  
  * @param  uwDataSize: Size of read data from the memory
  */
void BSP_SDRAM_ReadData_8b(uint32_t uwStartAddress, uint8_t *pData, uint32_t uwDataSize)
{
  HAL_SDRAM_Read_8b(&SdramHandle, (uint32_t *)uwStartAddress, pData, uwDataSize); 
}

/**
  * @brief  Reads an mount of data from the SDRAM memory in DMA mode. 
  * @param  uwStartAddress : Read start address
  * @param  pData : Pointer to data to be read  
  * @param  uwDataSize: Size of read data from the memory
  */
void BSP_SDRAM_ReadData_DMA(uint32_t uwStartAddress, uint32_t *pData, uint32_t uwDataSize)
{
  HAL_SDRAM_Read_DMA(&SdramHandle, (uint32_t *)uwStartAddress, pData, uwDataSize);     
}
  
/**
  * @brief  Writes an mount of data to the SDRAM memory in polling mode.
  * @param  uwStartAddress : Write start address
  * @param  pData : Pointer to data to be written  
  * @param  uwDataSize: Size of written data from the memory
  */
void BSP_SDRAM_WriteData_32b(uint32_t uwStartAddress, uint32_t *pData, uint32_t uwDataSize) 
{
  /* Disable write protection */
  HAL_SDRAM_WriteProtection_Disable(&SdramHandle);
  
  /*Write 32-bit data buffer to SDRAM memory*/
  HAL_SDRAM_Write_32b(&SdramHandle, (uint32_t *)uwStartAddress, pData, uwDataSize);
}

/**
  * @brief  Writes an mount of data to the SDRAM memory in polling mode.
  * @param  uwStartAddress : Write start address
  * @param  pData : Pointer to data to be written  
  * @param  uwDataSize: Size of written data from the memory
  */
void BSP_SDRAM_WriteData_16b(uint32_t uwStartAddress, uint16_t *pData, uint32_t uwDataSize) 
{
  /* Disable write protection */
  HAL_SDRAM_WriteProtection_Disable(&SdramHandle);
  
  /*Write 32-bit data buffer to SDRAM memory*/
  HAL_SDRAM_Write_16b(&SdramHandle, (uint32_t *)uwStartAddress, pData, uwDataSize);
}

/**
  * @brief  Writes an mount of data to the SDRAM memory in polling mode.
  * @param  uwStartAddress : Write start address
  * @param  pData : Pointer to data to be written  
  * @param  uwDataSize: Size of written data from the memory
  */
void BSP_SDRAM_WriteData_8b(uint32_t uwStartAddress, uint8_t *pData, uint32_t uwDataSize) 
{
  /* Disable write protection */
  HAL_SDRAM_WriteProtection_Disable(&SdramHandle);
  
  /*Write 32-bit data buffer to SDRAM memory*/
  HAL_SDRAM_Write_8b(&SdramHandle, (uint32_t *)uwStartAddress, pData, uwDataSize);
}

/**
  * @brief  Writes an mount of data to the SDRAM memory in DMA mode.
  * @param  uwStartAddress : Write start address
  * @param  pData : Pointer to data to be written  
  * @param  uwDataSize: Size of written data from the memory
  */
void BSP_SDRAM_WriteData_DMA(uint32_t uwStartAddress, uint32_t *pData, uint32_t uwDataSize) 
{
  HAL_SDRAM_Write_DMA(&SdramHandle, (uint32_t *)uwStartAddress, pData, uwDataSize); 
}

/**
  * @brief  Sends command to the SDRAM bank.
  * @param  SdramCmd: Pointer to SDRAM command structure 
  * @retval HAL status
  */  
HAL_StatusTypeDef BSP_SDRAM_Sendcmd(FMC_SDRAM_CommandTypeDef *SdramCmd)
{
  return(HAL_SDRAM_SendCommand(&SdramHandle, SdramCmd, SDRAM_TIMEOUT));
}

/**
  * @brief  Handles SDRAM DMA transfer interrupt request.
  */
//void DMA2_Stream0_IRQHandler(void)
void BSP_SDRAM_DMA_IRQHandler(void)
{
  HAL_DMA_IRQHandler(SdramHandle.hdma); 
}

/**
  * @brief  Initializes SDRAM MSP.
  */
static void MspInit(void)
{
  static DMA_HandleTypeDef dmaHandle;
  GPIO_InitTypeDef GPIO_InitStruct;
  SDRAM_HandleTypeDef  *hsdram = &SdramHandle;

  /* Enable FMC clock */
  __FMC_CLK_ENABLE();

  /* Enable chosen DMAx clock */
  __DMAx_CLK_ENABLE();

  /* Enable GPIOs clock */
  __GPIOB_CLK_ENABLE();
  __GPIOC_CLK_ENABLE();
  __GPIOD_CLK_ENABLE();
  __GPIOE_CLK_ENABLE();
  __GPIOF_CLK_ENABLE();
  __GPIOG_CLK_ENABLE();
                            
  /** FMC GPIO Configuration  
  PF0   ------> FMC_A0
  PF1   ------> FMC_A1
  PF2   ------> FMC_A2
  PF3   ------> FMC_A3
  PF4   ------> FMC_A4
  PF5   ------> FMC_A5
  PC0   ------> FMC_SDNWE
  PF11   ------> FMC_SDNRAS
  PF12   ------> FMC_A6
  PF13   ------> FMC_A7
  PF14   ------> FMC_A8
  PF15   ------> FMC_A9
  PG0   ------> FMC_A10
  PG1   ------> FMC_A11
  PE7   ------> FMC_D4
  PE8   ------> FMC_D5
  PE9   ------> FMC_D6
  PE10   ------> FMC_D7
  PE11   ------> FMC_D8
  PE12   ------> FMC_D9
  PE13   ------> FMC_D10
  PE14   ------> FMC_D11
  PE15   ------> FMC_D12
  PD8   ------> FMC_D13
  PD9   ------> FMC_D14
  PD10   ------> FMC_D15
  PD14   ------> FMC_D0
  PD15   ------> FMC_D1
  PG4   ------> FMC_BA0
  PG5   ------> FMC_BA1
  PG8   ------> FMC_SDCLK
  PD0   ------> FMC_D2
  PD1   ------> FMC_D3
  PG15   ------> FMC_SDNCAS
  PB5   ------> FMC_SDCKE1
  PB6   ------> FMC_SDNE1
  PE0   ------> FMC_NBL0
  PE1   ------> FMC_NBL1
  */
  GPIO_InitStruct.Pin = GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3 
                          |GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_11|GPIO_PIN_12 
                          |GPIO_PIN_13|GPIO_PIN_14|GPIO_PIN_15;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;
  GPIO_InitStruct.Alternate = GPIO_AF12_FMC;
  HAL_GPIO_Init(GPIOF, &GPIO_InitStruct);

  GPIO_InitStruct.Pin = GPIO_PIN_0;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;
  GPIO_InitStruct.Alternate = GPIO_AF12_FMC;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  GPIO_InitStruct.Pin = GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_4|GPIO_PIN_5 
                          |GPIO_PIN_8|GPIO_PIN_15;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;
  GPIO_InitStruct.Alternate = GPIO_AF12_FMC;
  HAL_GPIO_Init(GPIOG, &GPIO_InitStruct);

  GPIO_InitStruct.Pin = GPIO_PIN_7|GPIO_PIN_8|GPIO_PIN_9|GPIO_PIN_10 
                          |GPIO_PIN_11|GPIO_PIN_12|GPIO_PIN_13|GPIO_PIN_14 
                          |GPIO_PIN_15|GPIO_PIN_0|GPIO_PIN_1;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;
  GPIO_InitStruct.Alternate = GPIO_AF12_FMC;
  HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);

  GPIO_InitStruct.Pin = GPIO_PIN_8|GPIO_PIN_9|GPIO_PIN_10|GPIO_PIN_14 
                          |GPIO_PIN_15|GPIO_PIN_0|GPIO_PIN_1;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;
  GPIO_InitStruct.Alternate = GPIO_AF12_FMC;
  HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

  GPIO_InitStruct.Pin = GPIO_PIN_5|GPIO_PIN_6;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;
  GPIO_InitStruct.Alternate = GPIO_AF12_FMC;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /* Configure common DMA parameters */
  dmaHandle.Init.Channel             = SDRAM_DMAx_CHANNEL;
  dmaHandle.Init.Direction           = DMA_MEMORY_TO_MEMORY;
  dmaHandle.Init.PeriphInc           = DMA_PINC_DISABLE;
  dmaHandle.Init.MemInc              = DMA_MINC_ENABLE;
  dmaHandle.Init.PeriphDataAlignment = DMA_PDATAALIGN_WORD;
  dmaHandle.Init.MemDataAlignment    = DMA_MDATAALIGN_WORD;
  dmaHandle.Init.Mode                = DMA_NORMAL;
  dmaHandle.Init.Priority            = DMA_PRIORITY_HIGH;
  dmaHandle.Init.FIFOMode            = DMA_FIFOMODE_DISABLE;
  dmaHandle.Init.FIFOThreshold       = DMA_FIFO_THRESHOLD_FULL;
  dmaHandle.Init.MemBurst            = DMA_MBURST_SINGLE;
  dmaHandle.Init.PeriphBurst         = DMA_PBURST_SINGLE; 
  
  dmaHandle.Instance = SDRAM_DMAx_STREAM;
  
  /* Associate the DMA handle */
  __HAL_LINKDMA(hsdram, hdma, dmaHandle);
  
  /* Deinitialize the stream for new transfer */
  HAL_DMA_DeInit(&dmaHandle);
  
  /* Configure the DMA stream */
  HAL_DMA_Init(&dmaHandle); 
  
  /* NVIC configuration for DMA transfer complete interrupt */
  HAL_NVIC_SetPriority(SDRAM_DMAx_IRQn, 5, 0);
  HAL_NVIC_EnableIRQ(SDRAM_DMAx_IRQn);
}
