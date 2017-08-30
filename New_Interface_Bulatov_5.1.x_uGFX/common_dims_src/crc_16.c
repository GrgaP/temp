#include "crc_16.h"


//=============================== 
//       Расчитывает CRC16
//===============================
unsigned short Crc16 ( unsigned char* pData, unsigned long iLen )
{
    unsigned short iCrc = 0xFFFF;
 
    while( iLen-- )
        iCrc = ( iCrc << 8 ) ^ dCrc16Table[ ( iCrc >> 8 ) ^ *pData++ ];
 
    return iCrc;
}