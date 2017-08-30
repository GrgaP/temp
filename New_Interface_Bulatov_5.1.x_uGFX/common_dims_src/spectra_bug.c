#include "spectra_bug.h"
#include "spectra_data.h"
#include "stdlib.h"

u32 SpectraSelect(void)
{
  static TickCountType previsionUpdateSpectraTime = 0;
  static u32 SelectedSpectr = (u32)(&spectra_data_clear[0][0]);; 
  
  if (WaitUntil(&previsionUpdateSpectraTime, SPECTRA_BUG_RATE)==READY)
    { 
      if (!bug_actual)
      {
        SelectedSpectr = (u32)(&spectra_data_clear[(rand()&(SPECTRA_DATA_CLEAR_NUMBER-1))][0]);
      }
      else
      {
        u8 selectTntZone;
        selectTntZone = (u8)(Grad_Lvl_Trgt_Spctr*SPECTRA_DATA_TNT_LEVEL_NUMBER/16000);
        if (selectTntZone>=SPECTRA_DATA_TNT_LEVEL_NUMBER) 
          selectTntZone = (SPECTRA_DATA_TNT_LEVEL_NUMBER);
        switch (selectTntZone)
        { case 0:
            {
              SelectedSpectr = (u32)(&spectra_data_TNTlevel1[(rand()&(SPECTRA_DATA_TNT_NUMBER-1))][0]);
            } break;
          case 1:
            {
              SelectedSpectr = (u32)(&spectra_data_TNTlevel2[(rand()&(SPECTRA_DATA_TNT_NUMBER-1))][0]);
            } break;
          case 2:
            {
              SelectedSpectr = (u32)(&spectra_data_TNTlevel3[(rand()&(SPECTRA_DATA_TNT_NUMBER-1))][0]);
            } break;
          default: SelectedSpectr = (u32)(&spectra_data_clear[(rand()&(SPECTRA_DATA_CLEAR_NUMBER-1))][0]);
            
        }
      }
    }
  return (SelectedSpectr);
}