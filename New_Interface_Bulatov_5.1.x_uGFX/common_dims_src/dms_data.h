/*
 * dms_data.h
 *
 *  Created on: 31.12.2014
 *      Author: v.podkolzin
 */

#ifndef DMS_DATA_H_
#define DMS_DATA_H_

#pragma pack( push, 1 )
typedef struct
{
  uint8_t     iMagicNumber;    // маркер начала комманды. = 0xAA
  uint8_t     iCmdId;          // код комманды.
  uint16_t    iDataSize;       // длина данных комманды в байтах
  uint16_t    iCrc;            // CRC всех байтов заголовка комманды за исключением поля iCrc
} CMDHeder_t;
#pragma pack(pop)

/* список возможных кодов комманд:
=============================== */
#define    CMD_SETPARAMS    1   //
#define    CMD_GETPARAMS    2   //
#define    CMD_GETDATA      3   //
#define    CMD_SETKEYS      4   //
#define    CMD_GETGANDATA   5   //
#define    CMD_GETDATA31    0x31
#define    CMD_GETDATA32    0x32
#define    CMD_GETDATA33    0x33
#define    CMD_GETMULTIDATA 0x34
/*------------------------------*/



#define IMAGIC_NUMBER       0xAA
/*--------------------------------------------------------------------*/
#define CMD_BODY_LENGH  20    // Длина команды  sizeof(CmdBody_Prams_t)

#define iTrack_NUM      8

#pragma pack( push, 1 )
typedef struct
{
    int16_t iTrackValue[ iTrack_NUM ];    // значения трек-баров
    uint16_t iReserved;   // зарезервировано, пусть пока будут =0
    union
    {
        uint16_t iFlagBits; // биты-флаги
        struct
        {
            uint16_t bit0    : 1;
            uint16_t bit1    : 1;
            uint16_t bit2    : 1;
            uint16_t bit3    : 1;
            uint16_t bit4    : 1;
            uint16_t bit5    : 1;
            uint16_t bit6    : 1;
            uint16_t bit7    : 1;
        };
    };

} CMDBody_t;
#pragma pack(pop)

#pragma pack( push, 1 )
typedef struct{
  uint16_t   iKey;
  uint16_t   iReserved;
}CMDKey_t;
#pragma pack(pop)

/*---------------------------------------------------------------------*/


#pragma pack( push, 1 )
typedef struct
{
  CMDHeder_t CMDHeder;
  CMDBody_t CMDBody;
} CMD_t, *pCMD_t;
#pragma pack(pop)

#define SPECCTRA_RAW_SIZE_DEF   (512)
#define SPECCTRA_READY_SIZE_DEF (512)
/* Number of specctra for transmit */
#define MULTI_SPCTR             (3) 

#define EL_1_WIPER_DEFAULT_POS       64
#define EL_2_WIPER_DEFAULT_POS       100

/* DMS Params iTrackValue */
enum {
  dms_v_rf,
  dms_v_pull,
  dms_pAur,
  dms_Asq,
  dms_ChFrom,
  dms_ChTo,
  dms_FrmV,
  dms_ToV
};

#define iTrackValue0DEF         (90)

//#define DMS_PARAMS_DEF   {{iTrackValue0DEF, -633,    2000,   2,   1058, 2100,   0,    9500}, 0, {0x17}}
                         /*Vrf              V_pull1  pAur    Asq   ->    <-     FrmV   ToV   R   F*/

#define DMS_PARAMS_DEF   {{iTrackValue0DEF, -633,    2000,   1,   1058, 2100,   0,   3200}, 0, {0x17}}
                         /*Vrf              V_pull1  pAur    Asq   ->    <-     FrmV   ToV   R   F*/

#define iTrackValue0MAX         (150)
#define iTrackValue0MIN         (0)

#define iTrackValue1MAX         (1900)
#define iTrackValue1MIN         (-1900)
#define iTrackValue2MAX         (30000)
#define iTrackValue2MIN         (-30000)
#define iTrackValue3MAX         (200)
#define iTrackValue3MIN         (1)

#define iTrackValue4MAX         (32767)
#define iTrackValue4MIN         (-32768)
#define iTrackValue5MAX         (31767)
#define iTrackValue5MIN         (-31768)

#define iTrackValue6MAX         (31767)
#define iTrackValue6MIN         (-31768)
#define iTrackValue7MAX         (31767)
#define iTrackValue7MIN         (-31768)

#define V_From DMS_Params.iTrackValue[6]
#define V_To DMS_Params.iTrackValue[7]
#define V_Pull (DMS_Params.iTrackValue[1])
#define V_Rf (DMS_Params.iTrackValue[0])

/*
 * DMS errors
 */
#define DMS_ADC_ERROR_FLAG      (1 << 0)    /* SDADC (DMA) Error  */
#define DMS_USB_CRC_FLAG        (1 << 1)    /* USB Header CRC Error */
#define DMS_HID_RX_ERROR        (1 << 2)    /* USART Receive Error */
#define DMS_HID_RX_NR_FLAG      (1 << 3)    /* USART Char Received Without Requst */
#define DMS_HID_TIMEOUT_FLAG    (1 << 4)    /* Exchange with HID timeout detected */
#define DMS_HID_CRC_FLAG        (1 << 5)    /* HID Header CRC Error */
#define DMS_I2C1_ERR_FLAG       (1 << 6)    /* I2C1 Error */
#define DMS_I2C2_ERR_FLAG       (1 << 7)    /* I2C2 Error */
#define DMS_BT_AT_ERR_FLAG      (1 << 8)    /* Bluetoth At-commands Error */
typedef struct{
  uint16_t eflags;
  uint16_t hidRXerCode;
  uint32_t hidCRCCnt;
  uint16_t i2c1_err;
  uint16_t i2c2_err;
  uint16_t ganspi_err_cnt;                  /* CRC Error.*/
}_DMS_hw_errors;


/******************************************************************************
 * Global Variables
 ******************************************************************************/
extern __IO int16_t                 Specctra_Date_C1[SPECCTRA_RAW_SIZE_DEF];
extern __IO int16_t                 Specctra_Date_C2[SPECCTRA_RAW_SIZE_DEF];


extern  CMDBody_t               DMS_Params;
extern __IO CMDBody_t           DMS_Params_S;              /* Params To Send(HID, PC) */
extern  CMDKey_t                DMS_Keys;

extern  _DMS_hw_errors          DMS_Errors;

#endif /* DMS_DATA_H_ */
