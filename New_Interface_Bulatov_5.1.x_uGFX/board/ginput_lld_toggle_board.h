
/**
 * @file    ginput_lld_toggle_board.h
 * @brief   GINPUT Toggle low level driver source for DIMS_DSP_V5.x.x board
 */

#ifndef _GDISP_LLD_TOGGLE_BOARD_H
#define _GDISP_LLD_TOGGLE_BOARD_H

#define GINPUT_TOGGLE_NUM_PORTS		6  // The total number of toggle inputs
#define GINPUT_TOGGLE_CONFIG_ENTRIES    1  // The total number of GToggleConfig entries

#define GINPUT_TOGGLE_ENTER		5  // ENTER
#define GINPUT_TOGGLE_RET		4  // EXIT
#define GINPUT_TOGGLE_LEFT		2  
#define GINPUT_TOGGLE_DOWN		1  
#define GINPUT_TOGGLE_UP		0  
#define GINPUT_TOGGLE_RIGHT		3  
				

typedef struct IOBus_t
{
  uint16_t *portid;
} IOBus; 



#define GINPUT_TOGGLE_DECLARE_STRUCTURE()											\
	const GToggleConfig GInputToggleConfigTable[GINPUT_TOGGLE_CONFIG_ENTRIES] = 	\
		{  0,                                       \
                   KEY_U|KEY_D|KEY_L|KEY_R|KEY_RET|KEY_ENT, \
                   KEY_U|KEY_D|KEY_L|KEY_R|KEY_RET|KEY_ENT, \
                   0                                        \
                }	\
	;

static void palSetGroupMode(void* addr, uint16_t mask, uint16_t ext, uint16_t mode)
{
  
}

static uint16_t palReadBus(IOBus* addr)
{
  asm("NOP");
  return SETPARAMS.iKey;
}

#endif /* _GDISP_LLD_TOGGLE_BOARD_H */