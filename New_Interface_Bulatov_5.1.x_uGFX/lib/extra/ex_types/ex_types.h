/**
 ******************************************************************************
 * @file    ex_types.h
 * @author  a.bulatov
 * @version V1.1.0
 * @date    04.07.2016
 * @brief       File that consist extended typedef.
 *              $Id$
 ****************************************************************************** 
 */

#ifndef __EX_TYPES_H_
#define __EX_TYPES_H_

#include <stdint.h>

/**
 * @defgroup    ex_type Extendet types
 * @brief       Group that consist extended type.
 * @{ 
 */

/**
 * @brief   Generic 'false' boolean constant.
 */
#if !defined(FALSE) 
#define FALSE                   (0)
#endif

/**
 * @brief   Generic 'true' boolean constant.
 */
#if !defined(TRUE) 
#define TRUE                    (!FALSE)
#endif

/**
 *@brief Type for 32 word bit access  
 */
typedef union 
{
  uint32_t dw;                  /**< for double word uint32 access @b bit32.dw */
  struct                        
  {
    uint16_t l;                 /**< for low word uint16 access @b bit32.w.l */
    uint16_t h;                 /**< for highword uint16 access @b bit32.w.h */
  } w;                          /**< Word access structure */
  struct 
  {
    uint8_t byte1;              /**< for first byte uint8 access @b bit32.b.byte1 */
    uint8_t byte2;              /**< for second byte uint8 access @b bit32.b.byte2 */
    uint8_t byte3;              /**< for third byte uint8 access @b bit32.b.byte3  */
    uint8_t byte4;              /**< for fourth byte uint8 access @b bit32.b.byte4 */
  } b;                          /**< Byte access structure */
  struct
  {
    unsigned bit0     : 1;      
    unsigned bit1     : 1;      
    unsigned bit2     : 1;      
    unsigned bit3     : 1;
    unsigned bit4     : 1;
    unsigned bit5     : 1;
    unsigned bit6     : 1;
    unsigned bit7     : 1; 
    unsigned bit8     : 1;
    unsigned bit9     : 1;
    unsigned bit10    : 1;
    unsigned bit11    : 1;
    unsigned bit12    : 1;
    unsigned bit13    : 1;
    unsigned bit14    : 1;
    unsigned bit15    : 1;
    unsigned bit16    : 1; 
    unsigned bit17    : 1;      
    unsigned bit18    : 1;
    unsigned bit19    : 1;
    unsigned bit20    : 1;
    unsigned bit21    : 1;
    unsigned bit22    : 1;
    unsigned bit23    : 1; 
    unsigned bit24    : 1;
    unsigned bit25    : 1;
    unsigned bit26    : 1;
    unsigned bit27    : 1;
    unsigned bit28    : 1;
    unsigned bit29    : 1;
    unsigned bit30    : 1;      
    unsigned bit31    : 1;      
  } bits; /**< For bit access use &b bit32.bits.bitX */
} Bits32_TypeDef; 

/**
 *@brief Type for 32 word bit access  
 */
typedef union 
{
  uint16_t w;   /**< for word uint16 access @b bit16.w */
  struct                        
  {
    uint16_t l;     /**< for low byte uint8 access @b bit16.b.l */
    uint16_t h;     /**< for high byte uint8 access @b bit16.b.h */
  } b;          /**< Byte access structure */
  struct
  {
    unsigned bit0     : 1;      /* bits16.bits.bit0 */      
    unsigned bit1     : 1;      /* bits16.bits.bit1 */       
    unsigned bit2     : 1;      /* bits16.bits.bit2 */ 
    unsigned bit3     : 1;      /* bits16.bits.bit3 */ 
    unsigned bit4     : 1;      /* bits16.bits.bit4 */ 
    unsigned bit5     : 1;      /* bits16.bits.bit5 */ 
    unsigned bit6     : 1;      /* bits16.bits.bit6 */ 
    unsigned bit7     : 1;      /* bits16.bits.bit7 */ 
    unsigned bit8     : 1;      /* bits16.bits.bit8 */
    unsigned bit9     : 1;      /* bits16.bits.bit9 */
    unsigned bit10    : 1;      /* bits16.bits.bit10 */
    unsigned bit11    : 1;      /* bits16.bits.bit11 */
    unsigned bit12    : 1;      /* bits16.bits.bit12 */
    unsigned bit13    : 1;      /* bits16.bits.bit13 */
    unsigned bit14    : 1;      /* bits16.bits.bit14 */
    unsigned bit15    : 1;      /* bits16.bits.bit15 */
  } bits;      /**< For bit access use &b bit16.bits.bitX */
} Bits16_TypeDef; 

/**
 *@}
 */ 
 

#endif /* __EX_TYPES_H_ */
 
