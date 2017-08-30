#include "flash_memory.h"
#include "main.h"
#include "ex_types.h"

  #define FLASH_ADRESS 0x08008000		// ����� ������ 1-�� ������� (32��)

u16 FLASH_OFFSET=0;				// ��������. ������������ ��� ������/������ ������
volatile u16 Flash_ReWrites;	// ���������� ����������� ������ ��� ���������� ��������

#define FABR_SECT _Pragma("location = \"FABR_DEF_SECTION\"")
#define FABR_VAR FABR_SECT __root const

FABR_VAR char pFabrExplName[EXPLOSIV_COUNT][EXPLOSIVE_NAME_LENGHT] = 
            {   
                EXP0_NAME,
                EXP1_NAME,
                EXP2_NAME,
                EXP3_NAME,
                EXP4_NAME,
                EXP5_NAME
            } ;
FABR_VAR Explosive_TypeDef pFabrExplDefSettings[EXPLOSIV_COUNT] = 
            {   
              {{EXP0_NAME}, EXP0_COLOR, EXP0_OFFSET, EXP0_WND, EXP0_LEVEL, EXP0_ONOFF},
              {{EXP1_NAME}, EXP1_COLOR, EXP1_OFFSET, EXP1_WND, EXP1_LEVEL, EXP1_ONOFF},
              {{EXP2_NAME}, EXP2_COLOR, EXP2_OFFSET, EXP2_WND, EXP2_LEVEL, EXP2_ONOFF},
              {{EXP3_NAME}, EXP3_COLOR, EXP3_OFFSET, EXP3_WND, EXP3_LEVEL, EXP3_ONOFF},
              {{EXP4_NAME}, EXP4_COLOR, EXP4_OFFSET, EXP4_WND, EXP4_LEVEL, EXP4_ONOFF},
              {{EXP5_NAME}, EXP5_COLOR, EXP5_OFFSET, EXP5_WND, EXP5_LEVEL, EXP5_ONOFF}
            } ;

FABR_VAR int16_t psFabrTrackDefVal[] = TRACK_VAL_DEF;
FABR_VAR int8_t cFabrEPullDefVal = E_PULL_DEF;
FABR_VAR int8_t cFabrGainDefVal = GAIN_DEF;   
FABR_VAR uint16_t usFabrFlagsBitsDefVal = FLAG_BITS_DEF;
FABR_VAR int16_t sFabrRpIdWndDefVal = RP_ID_WND_DEF;
FABR_VAR int16_t sFabrRpIdOffsetDefVal = RP_ID_OFFSET_DEF;
FABR_VAR usBits_TypeDef usFabrMenuFlagsDefVal = {MENU_FLAGS_DEF};
FABR_VAR uint16_t usFabrMenuAlphaDefVal = MENU_ALPHA_DEF;

FABR_VAR SystemSettingsTypeDef xFabrSystemSettings =  SYSTEM_SETTINGS_DEF;

//===============================================================
// ������������� ����-������ ����� �������
//===============================================================
#define FLASH_KEY1 ((uint32_t)0x45670123)
#define FLASH_KEY2 ((uint32_t)0xCDEF89AB)
void Flash_unlock (void)
{
	
  HAL_FLASH_Unlock();

      //FLASH->KEYR = FLASH_KEY1;
      //FLASH->KEYR = FLASH_KEY2;
}


//===============================================================
// ���������� ����-������ ����� ������
//===============================================================
void Flash_lock (void)
{
	
  HAL_FLASH_Lock();
  //FLASH->CR |= FLASH_CR_LOCK;
}


//===============================================================
// ������� ������� ���� ������
//===============================================================
// � �������� ��������� ��������� ����� ������� (�� 0 �� 23)
// 0. 0x08000000-0x08007FFF (32 ��)//This sector uses for placing the table of interrupt vectors
// 1. 0x08008000-0x0800FFFF (32 ��)//This sector uses for placing device settings
// 2. 0x08010000-0x08017FFF (32 ��)//This sector uses for storage of the settings default values
// 3. 0x08018000-0x0801FFFF (32 ��)//This is start sector for firmware
// 4. 0x08020000-0x0803FFFF (128 ��)
// 5. 0x08040000-0x0807FFFF (256 ��)
// 6. 0x08080000-0x080BFFFF (256 ��)
// 7. 0x080C0000-0x080FFFFF (256 ��)


void Flash_erase_sector (u8 sector)
{
      FLASH_EraseInitTypeDef EraseInitStruct;
      
      EraseInitStruct.Sector = sector;
      EraseInitStruct.TypeErase = TYPEERASE_SECTORS;
      EraseInitStruct.VoltageRange = FLASH_VOLTAGE_RANGE_3;
      EraseInitStruct.NbSectors = 1;
      uint32_t SectorError = 0;
    
      if (HAL_FLASHEx_Erase(&EraseInitStruct, &SectorError) != HAL_OK)
        return;
  
        /*FLASH->CR|= FLASH_CR_SER;				// Sector Erase activated
	if (sector>11) sector+=4;				// ���� ���������� 12...23 �������, �� ������������ ����� �������, �������� ��������
	FLASH->CR |= (sector<<3);				// Sector number
	FLASH->CR |= FLASH_CR_STRT;				// ��������� �������� 
	while (FLASH->SR & FLASH_SR_BSY) {};	// ��� ���� ������ �������� 
	FLASH->CR&= ~FLASH_CR_SER;				// Sector Erase deactivated*/
}


//===============================================================
// ������ 16-������� ����� �� ������������ ������
//===============================================================
u16 Flash_read_16bit (u32 address)
{
	u16 temp = (*(volatile u16*) address);
	FLASH_OFFSET+=2;
	return temp;
}


//===============================================================
// ������ 16-������� ����� �� ������������ ������
//===============================================================
void Flash_write_16bit (u32 Address, u16 Data) 
{ 
        HAL_FLASH_Program(FLASH_TYPEPROGRAM_HALFWORD, Address, (uint64_t) Data);
  
	/*FLASH->CR &= CR_PSIZE_MASK;
	FLASH->CR |= FLASH_PSIZE_HALF_WORD;		// ������ �� 16 ���
	FLASH->CR |= FLASH_CR_PG;				// Flash programming activated
	*(volatile u16*)Address = Data; 		// ������ ������ �� ������ Address 
	while (FLASH->SR & FLASH_SR_BSY) {};	// ��� ���� ������ ���������
	FLASH->CR &= (~FLASH_CR_PG);			// Flash programming deactivated*/
	FLASH_OFFSET+=2;
}


//===============================================================
// ������ 8-��� �� ������������ ������
//===============================================================
u16 Flash_read_8bit (u32 Address)
{
	u8 temp = (*(volatile u8*) Address);
	FLASH_OFFSET++;
	return temp;
}


//===============================================================
// ������ 8-��� �� ������������ ������
//===============================================================
void Flash_write_8bit (u32 Address, u8 Data) 
{ 
	HAL_FLASH_Program(FLASH_TYPEPROGRAM_BYTE, Address, (uint64_t) Data);
  
        /*FLASH->CR &= CR_PSIZE_MASK;
	FLASH->CR |= FLASH_PSIZE_BYTE;		    // ������ �� 8 ���
	FLASH->CR |= FLASH_CR_PG;				// Flash programming activated
	*(volatile u8*)Address = Data; 		// ������ ������ �� ������ Address 
	while (FLASH->SR & FLASH_SR_BSY) {};	// ��� ���� ������ ���������
	FLASH->CR &= (~FLASH_CR_PG);			// Flash programming deactivated*/
	FLASH_OFFSET++;
}


//===============================================================
// ��������� ��� ��������� �� ����-������
// ������� ��������������� ��������� �� �������
//===============================================================
void Flash_read_settings (void) 
{
	FLASH_OFFSET=0;
	// 8 �������� iTrackValue
	for (u8 i=0; i<8; i++)
	{
		SETPARAMS.iTrackValue[i] = Flash_read_16bit (FLASH_ADRESS+FLASH_OFFSET);	// �������� ������, ���������� �� 2 �����, �.�. ���������� ���� u16
	}
	SETPARAMS.ePull = (s8)Flash_read_8bit (FLASH_ADRESS+FLASH_OFFSET);
	SETPARAMS.Gain  = (s8)Flash_read_8bit (FLASH_ADRESS+FLASH_OFFSET);
	SETPARAMS.iFlagBits = Flash_read_16bit (FLASH_ADRESS+FLASH_OFFSET);
	// ��������� ���������� �-�
	for (u8 i=0; i<E_COUNT; i++)
	{
		E_Settings[i].Pos    = Flash_read_16bit (FLASH_ADRESS+FLASH_OFFSET);
		E_Settings[i].Itrv   = Flash_read_16bit (FLASH_ADRESS+FLASH_OFFSET);
	}
        // ������ ����� win � offset
        rpIdWnd = Flash_read_16bit (FLASH_ADRESS+FLASH_OFFSET);
        rpIdOffset = Flash_read_16bit (FLASH_ADRESS+FLASH_OFFSET);
        rpPeakSerchLevel = Flash_read_16bit (FLASH_ADRESS+FLASH_OFFSET);
        
        for (uint32_t i=0; i < EXPLOSIV_COUNT; i++)
        {
          Explosive_Set[i] = pFabrExplDefSettings[i]; /* ��� ��������� ����� ���������� ��������� ��������� */
          Explosive_Set[i].usColor      = Flash_read_16bit (FLASH_ADRESS+FLASH_OFFSET);
          Explosive_Set[i].lOffsetId    = Flash_read_16bit (FLASH_ADRESS+FLASH_OFFSET);
          Explosive_Set[i].ulWndId      = Flash_read_16bit (FLASH_ADRESS+FLASH_OFFSET);
          Explosive_Set[i].usLevelAlarm = Flash_read_16bit (FLASH_ADRESS+FLASH_OFFSET);
          Explosive_Set[i].eOnOff = (EOnOff_TypeDef)Flash_read_16bit (FLASH_ADRESS+FLASH_OFFSET);
        }
        
        usMenuFlags.data = (uint16_t)Flash_read_16bit (FLASH_ADRESS+FLASH_OFFSET);
        usMenuAlpha = Flash_read_16bit (FLASH_ADRESS+FLASH_OFFSET); 
        
        for (uint32_t i = 0; i < sizeof(xFabrSystemSettings); i++)
        {
          *(((uint8_t*)(&SETPARAMS.systemSettings))+i) = Flash_read_8bit(FLASH_ADRESS+FLASH_OFFSET);
        }
        
	Flash_check_settings();
}


//===============================================================
// ��������� ��� ��������� �� ����-������
// ������� ��������������� ��������� �� �������
//===============================================================
void Flash_write_settings (void) 
{
	FLASH_OFFSET=0;
	LCDBUFFER_Str("����������...",5,240,RED,BLACK,2,2);
	LCD_Update();
	Flash_ReWrites++;        // ������������� ������� �����������
	Flash_unlock();          // ������������� ����-������ ����� �������
	Flash_erase_sector(1);  // ������ ������ �2(32 ��)
	// 8 �������� iTrackValue
	for (u8 i=0; i<8; i++) Flash_write_16bit (FLASH_ADRESS+FLASH_OFFSET, SETPARAMS.iTrackValue[i]);
	Flash_write_8bit  (FLASH_ADRESS+FLASH_OFFSET, (s8)SETPARAMS.ePull);
	Flash_write_8bit  (FLASH_ADRESS+FLASH_OFFSET, (s8)SETPARAMS.Gain);
	Flash_write_16bit (FLASH_ADRESS+FLASH_OFFSET, SETPARAMS.iFlagBits);
	// ��������� ���������� �-�
	for (u8 i=0; i<E_COUNT; i++)
	{
		Flash_write_16bit (FLASH_ADRESS+FLASH_OFFSET, E_Settings[i].Pos);
		Flash_write_16bit (FLASH_ADRESS+FLASH_OFFSET, E_Settings[i].Itrv);
	}
        // ������ ����� win � offset
        Flash_write_16bit (FLASH_ADRESS+FLASH_OFFSET, rpIdWnd);
	Flash_write_16bit (FLASH_ADRESS+FLASH_OFFSET, rpIdOffset);
	Flash_write_16bit (FLASH_ADRESS+FLASH_OFFSET, rpPeakSerchLevel);
        
        /* Write new variant of explosive */
        for (uint32_t i=0; i < EXPLOSIV_COUNT; i++)
        {
          Flash_write_16bit(FLASH_ADRESS+FLASH_OFFSET, pExplColor[i]);
          Flash_write_16bit(FLASH_ADRESS+FLASH_OFFSET, Explosive_Set[i].lOffsetId );
          Flash_write_16bit(FLASH_ADRESS+FLASH_OFFSET, Explosive_Set[i].ulWndId );
          Flash_write_16bit(FLASH_ADRESS+FLASH_OFFSET, Explosive_Set[i].usLevelAlarm);
          Flash_write_16bit(FLASH_ADRESS+FLASH_OFFSET, Explosive_Set[i].eOnOff);
        }
        
        Flash_write_16bit(FLASH_ADRESS+FLASH_OFFSET, usMenuFlags.data);
        Flash_write_16bit(FLASH_ADRESS+FLASH_OFFSET, usMenuAlpha);
        
        for (uint32_t i = 0; i < sizeof(xFabrSystemSettings); i++)
        {
          Flash_write_8bit(FLASH_ADRESS+FLASH_OFFSET, *(((uint8_t*)(&SETPARAMS.systemSettings))+i));
        }
        
        Flash_lock();  // ���������� ����-������ ����� ������
	LCDBUFFER_Str("���������� ��",5,240,YELLOW,BLACK,2,2);
	LCD_Update();
	Task_del(TASK_SETTINGS_WRITE);
}


//===============================================================
// ��������� ������������ ����������
// ��� ����������� ������������ �������� ������������ ����� �������� �� ���������,
// � ����� ����� ���������������� ��������� �� �������
//===============================================================
void Flash_check_settings (void)
{
	u8 errors=0;	// ������� ������������ ��������
	for (u8 i=0; i<8; i++) if ((SETPARAMS.iTrackValue[i]<((Settings_sensor+i)->int_from)) || (SETPARAMS.iTrackValue[i]>((Settings_sensor+i)->int_to))) errors++;
	if (SETPARAMS.iFlagBits>255) errors++;
	for (u8 j=0; j<E_COUNT; j++)
	{
		if (((E_Settings[j].Pos-E_Settings[j].Itrv)<0)\
		|| ((E_Settings[j].Pos+E_Settings[j].Itrv)>(SPECTRA_SIZE-1))) errors++;
	}
        if ((rpIdWnd > SPECTRA_SIZE/4)||(rpIdWnd < 0)|| \
            (rpIdOffset > SPECTRA_SIZE/4)||(rpIdOffset < -SPECTRA_SIZE/4)) 
        {
          rpIdWnd = RP_ID_WND_DEF;
          rpIdOffset = RP_ID_OFFSET_DEF;
        }
        if ((rpPeakSerchLevel < 0) || (rpPeakSerchLevel > 32000))
        {
          rpPeakSerchLevel = RP_PEAK_SERCH_LEVEL_DEF;
        }
	 for (uint32_t i = 0; i <EXPLOSIV_COUNT; i++)
         {
             if (
                ( Explosive_Set[i].lOffsetId >  EXP_OFFSET_MAX) ||  (Explosive_Set[i].lOffsetId <  -EXP_OFFSET_MAX) ||
                ( Explosive_Set[i].ulWndId > EXP_WND_MAX )||
                ( Explosive_Set[i].usLevelAlarm > 32000)
                )
             {
                Explosive_Set[i].usColor = pExplColor[i];
                Explosive_Set[i].lOffsetId = EXP_OFFSET_DEF;
                Explosive_Set[i].ulWndId = EXP_WND_DEF;
                Explosive_Set[i].usLevelAlarm = EXP_LEVEL_ALRM_DEF;
             }
         }
        
        usMenuAlpha = (usMenuAlpha > 100) ? 20 : usMenuAlpha;
        
        if (errors) Flash_set_default_settings();
         
}


//===============================================================
// ����� �������� �� ���������
// ����� ���������������� ��������� �� ������� 
//===============================================================
/* ������ ������� �������� � � ��������� ������ �� ������������� */
void Flash_set_default_settings (void)
{
	for (u8 i=0; i<8; i++) SETPARAMS.iTrackValue[i]=(Settings_sensor+i)->int_from;	// iTrackValue[i] = From[i]
	SETPARAMS.ePull=0;
	SETPARAMS.Gain=0;
	SETPARAMS.iFlagBits=0;
	for (u8 i=0; i<E_COUNT; i++)
	{
		E_Settings[i].Pos=SPECTRA_SIZE/2;
		E_Settings[i].Itrv=E_ITRV_MIN;
	}
        rpIdWnd = 8*SPECTRA_SIZE/100;
        rpIdOffset = 0*SPECTRA_SIZE/100;
        rpPeakSerchLevel = RP_PEAK_SERCH_LEVEL_DEF;
}

void vSetFabricationSettings(void)
{ 	
        LCDBUFFER_Str("��������...",5,240,RED,BLACK,2,2);
	LCD_Update();
        for (u8 i=0; i<8; i++) SETPARAMS.iTrackValue[i]=psFabrTrackDefVal[i];	// iTrackValue[i] = From[i]
	SETPARAMS.ePull=E_PULL_DEF;
	SETPARAMS.Gain=GAIN_DEF;
	SETPARAMS.iFlagBits = usFabrFlagsBitsDefVal;
	for (u8 i=0; i<E_COUNT; i++)
	{
		E_Settings[i].Pos=SPECTRA_SIZE/2;
		E_Settings[i].Itrv=E_ITRV_MIN;
	}
        rpIdWnd = sFabrRpIdWndDefVal;
        rpIdOffset = sFabrRpIdOffsetDefVal;
        rpPeakSerchLevel = RP_PEAK_SERCH_LEVEL_DEF;
        
        for (uint32_t i = 0; i <EXPLOSIV_COUNT; i++)
         {
              Explosive_Set[i] = pFabrExplDefSettings[i]; 
         }
        
        usMenuAlpha = usFabrMenuAlphaDefVal;
        usMenuFlags = usFabrMenuFlagsDefVal;
        
        for (uint32_t i = 0; i < sizeof(xFabrSystemSettings); i++)
        {
          *(((uint8_t*)(&SETPARAMS.systemSettings))+i) = *(((uint8_t*)(&xFabrSystemSettings))+i);
        }
        LCDBUFFER_Str("�������� ���������",5,240,YELLOW,BLACK,2,2);
	LCD_Update();
	Task_del(TASK_FABRICATION_LOAD);
        
}

