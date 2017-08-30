/**
 ******************************************************************************
 * @file    security.h
 * @author  a.bulatov
 * @version V1.1.0
 * @date    08.07.2016
 * @brief       Header file of securyty mode.
 *              $Id$
 ****************************************************************************** 
 */ 
 
#ifndef __SECURITY_H_		
#define __SECURITY_H_

/**
 * @defgroup    security [Security module for DIMS].
 * @brief       Consist enter paswword for algorithm for DIMS, and 
 *              DisplayModer_Security pars.
 * @{ 
 */

/*(DisplayMode==DisplayMode_Securyty)*/

#include <stdint.h>

/**
 * @brief       Function init and crate text edit and keyboard.
 */ 
void vSecurityInit(void);


void vSecurityEnter(void);
void vSecurityDisplay(void);
void vSecurityExit(void);
void vSendSecurityMes(uint8_t* AddrMenuPos, uint8_t ValMenuPos, uint32_t* AddrMenuCur, uint32_t ValMenuCur);
void ReturnMenu(void);
/**
 *@}
 */ 

#endif /* __SECURITY_H_ */


