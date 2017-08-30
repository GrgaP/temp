/*
 * This file is subject to the terms of the GFX License. If a copy of
 * the license was not distributed with this file, you can obtain one at:
 *
 *              http://ugfx.org/license.html
 */

#ifndef _LLD_GMOUSE_MCU_BOARD_H
#define _LLD_GMOUSE_MCU_BOARD_H

// Resolution and Accuracy Settings
#define GMOUSE_MCU_PEN_CALIBRATE_ERROR		8
#define GMOUSE_MCU_PEN_CLICK_ERROR			6
#define GMOUSE_MCU_PEN_MOVE_ERROR			4
#define GMOUSE_MCU_FINGER_CALIBRATE_ERROR	14
#define GMOUSE_MCU_FINGER_CLICK_ERROR		18
#define GMOUSE_MCU_FINGER_MOVE_ERROR		14
#define GMOUSE_MCU_Z_MIN					0			// The minimum Z reading
#define GMOUSE_MCU_Z_MAX					1			// The maximum Z reading
#define GMOUSE_MCU_Z_TOUCHON				1			// Values between this and Z_MAX are definitely pressed
#define GMOUSE_MCU_Z_TOUCHOFF				0			// Values between this and Z_MIN are definitely not pressed

// How much extra data to allocate at the end of the GMouse structure for the board's use
#define GMOUSE_MCU_BOARD_DATA_SIZE			10


static uint16_t x = 0;
static uint16_t y = 0;
static uint16_t z = 0;


static bool_t init_board(GMouse *m, unsigned driverinstance) {
  
  x = 120;
  y = 20;
  z = 0; 
  return TRUE;
}

static bool_t read_xyz(GMouse *m, GMouseReading *prd) 
{
  prd->x = x;
  prd->y = y;
  prd->z = z;
  
  return TRUE;
}

void vMouseAutomat(uint16_t iKey)
{
  
  int16_t ix,iy,iz;
  
  ix = x; iy = y; iz = z; 
  
  if ((iKey==KEY_L) && Keyb_SpeedDelay()) { ix--; }
  if ((iKey==KEY_R) && Keyb_SpeedDelay()) { ix++; }
  if ((iKey==KEY_U) && Keyb_SpeedDelay()) { iy++; }
  if ((iKey==KEY_D) && Keyb_SpeedDelay()) { iy--; }
  if ((iKey==KEY_ENT) && Keyb_SpeedDelay()) { iz = 1; }
  if ((iKey==KEY_RET) && Keyb_SpeedDelay()) { iz = 0; }
  
  if (ix >= 240) ix = (240-1);
  if (ix < 0) ix = 0; 
  
  if (iy >= 320) iy = (320-1);
  if (iy < 0) iy = 0; 

  x = ix; y = iy; z = iz; 
};


#endif /* _LLD_GMOUSE_MCU_BOARD_H */
