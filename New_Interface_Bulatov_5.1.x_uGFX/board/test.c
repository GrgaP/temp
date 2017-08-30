

#include "test.h"
#include <stdint.h>
#include "gfx.h"

void vToggleTest(void)
{
  
  uint8_t* state = 0;
  state = (uint8_t*) ginputGetToggle (GINPUT_TOGGLE_LEFT);
  LCDBUFFER_Num_Shadow (*state, 100, 100, WHITE, 4, 4);
  
  state = (uint8_t*) ginputGetToggle (GINPUT_TOGGLE_RIGHT);
  LCDBUFFER_Num_Shadow (*state, 125, 100, WHITE, 4, 4);
  
   state = (uint8_t*) ginputGetToggle (GINPUT_TOGGLE_UP);
  LCDBUFFER_Num_Shadow (*state, 100, 60, WHITE, 4, 4);
  
  state = (uint8_t*) ginputGetToggle (GINPUT_TOGGLE_DOWN);
  LCDBUFFER_Num_Shadow (*state, 125, 60, WHITE, 4, 4);
  
  state = (uint8_t*) ginputGetToggle (GINPUT_TOGGLE_RET);
  LCDBUFFER_Num_Shadow (*state, 100, 140, WHITE, 4, 4);
  
  state = (uint8_t*) ginputGetToggle (GINPUT_TOGGLE_ENTER);
  LCDBUFFER_Num_Shadow (*state, 125, 140, WHITE, 4, 4);
}