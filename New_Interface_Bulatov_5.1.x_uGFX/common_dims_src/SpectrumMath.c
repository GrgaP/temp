
#include <string.h>

#include "SpectrumMath.h"

#include "main.h"

int16_t rpIdWnd = 8*SPECTRA_SIZE/100;
int16_t rpIdOffset = 0*SPECTRA_SIZE/100;
int16_t rpPeakSerchLevel = RP_PEAK_SERCH_LEVEL_DEF;

/* Array of analitic spectra, reper specctra for calculating*/
int16_t psSpectrAn[SPECTRA_SIZE]={0};
int16_t psSpectrRp[SPECTRA_SIZE]={0};

/* Array for spectra history show */
int16_t psSpectrHistory[HISTORY_DEEP][SPECTRA_SIZE];
uint32_t ulPntSpctrHstr;

/* Reper, analitic peak data structure */
PeaksData_TypeDef xRpPeaksData = {0}; 
PeaksData_TypeDef xAnPeaksData = {0};

/* Repear peak info structura: information about maximum peak if xRpPeaksData*/
PeakInfo_TypeDef xRpPeakInfo = PEAK_INFO_DEF; 
PeakInfo_TypeDef xTntPeakInfo = PEAK_INFO_DEF;
PeakInfo_TypeDef xExplPeakInfo[EXPLOSIV_COUNT] = PEAK_INFO_DEF;

/* Structura to local save ScanData */
ScanData_TypeDef xScanData;

/* Variable for explosive zone calcuate */
ExplZone_TypeDef xExplZone[EXPLOSIV_COUNT];

/* Functiom prototype */
void  vReperCalculation(void);
void  vAnaliticTntCalculation(void);
void vRpZoneCalculate(void);
void vAnaliticExplosiveCalculation(uint32_t num);
ExplZone_TypeDef xCalculateExpZoneId(uint32_t num);

/*----------------------------------------------------------------------------*/
/* Function of zoom and copy Specctra */
void prvSpctrCopyZoom(int16_t* psDataOut, int16_t* psDataIn,  uint32_t ulSize, float fZoomKf)
{
  for (uint32_t i = 0; i < ulSize; i++ )
  {
    psDataOut[i] = (int16_t)__SSAT((int32_t)(psDataIn[i]*fZoomKf), 16); 
  }
}
/*----------------------------------------------------------------------------*/

/* Assert scan data. If reper window more then scan window, 
repwind = scanwindow */
void vAssertScanData(ScanData_TypeDef* xScan)
{
  
  xScan->sVoltRpFindFrom = (xScan->sVoltRpFindFrom < xScan->sVoltScanFrom) ? xScan->sVoltScanFrom : xScan->sVoltRpFindFrom;
  xScan->sVoltRpFindFrom = (xScan->sVoltRpFindFrom > xScan->sVoltScanTo) ? xScan->sVoltScanTo : xScan->sVoltRpFindFrom;  
  
  
  xScan->sVoltRpFindTo = (xScan->sVoltRpFindTo > xScan->sVoltScanTo) ?  xScan->sVoltScanTo : xScan->sVoltRpFindTo;
  xScan->sVoltRpFindTo = (xScan->sVoltRpFindTo < xScan->sVoltScanFrom) ?  xScan->sVoltScanFrom : xScan->sVoltRpFindTo;
  
  xScan->sVoltRpFindTo = (xScan->sVoltRpFindFrom > xScan->sVoltRpFindTo) ? xScan->sVoltRpFindFrom : xScan->sVoltRpFindTo; 
}

/* Copy recive spectra data for following calculate this copy external 
dependet */
void prvSpctrDataCopy(void)
{
    /* Copy spectra data for following calculate */
    prvSpctrCopyZoom( psSpectrAn, (int16_t*)&GETPARAMS.Spectra[0][0], 
                      SPECTRA_SIZE, 1.05f );
    prvSpctrCopyZoom( psSpectrRp, (int16_t*)&GETPARAMS.Spectra[1][0],
                      SPECTRA_SIZE, 1.05f );
    /* Copy counter of spectra */
    ulPntSpctrHstr = GETPARAMS.cntSpctr;
    
    /* Copy current analitic spectr to history this 1.0 scale */
    prvSpctrCopyZoom( &psSpectrHistory[ulPntSpctrHstr&(HISTORY_DEEP-1)][0], 
            (int16_t*)&GETPARAMS.Spectra[0][0], SPECTRA_SIZE, 1.00f);
    
    /* Depends copy of scan info */
    xScanData.sVoltScanFrom =   (s16)(GETPARAMS.iTrackValue[6]);
    xScanData.sVoltScanTo =     (s16)(GETPARAMS.iTrackValue[7]);
    xScanData.sVoltRpFindFrom = (s16)(GETPARAMS.iTrackValue[4]);
    xScanData.sVoltRpFindTo =   (s16)(GETPARAMS.iTrackValue[5]);
}
/*----------------------------------------------------------------------------*/

/* Peak serch function */
u16 d_p(    s16     *data,                  /* the data */ 
            u32     data_count,             /* row count of data */
            u32     *emi_peaks,             /* (max) emission peaks will be put here */
            u32     *num_emi_peaks,         /* number of emission peaks found */
            u32     max_emi_peaks,          /* maximum number of emission peaks*/ 
            u32     *absop_peaks,           /* (min) absorption peaks will be put here */
            u32     *num_absop_peaks,       /* number of absorption peaks found */
            u32     max_absop_peaks,        /* maximum number of absorption peaks*/
            s32     delta,                  /* delta used for distinguishing peaks */
             u8     emi_first               /* absorption peak first? */
       )
{
    u32 i;
    s32 mx;
    s32 mn;
    u32 mx_pos = 0;
    u32 mn_pos = 0;
    s32 is_detecting_emi = emi_first;
    
    mx = data[0];
    mn = data[0];

    *num_emi_peaks = 0;
    *num_absop_peaks = 0;

    for(i = 1; i < data_count; ++i)
    {     
     
      if ((data[i]==mx)&&(data[i+1]==mx))
      { 
        mx_pos++ ;
        i+=1;
      }
     
      if ((data[i]==mn)&&(data[i+1]==mn))
      { 
        mn_pos++ ;
        i+=1;
      }
      if(data[i] > mx)
      {
        mx_pos = i;
        mx = data[i];
      }

      if(data[i] < mn)
      {
        mn_pos = i;
        mn = data[i];
      }
      

      if(is_detecting_emi && data[i] < mx - delta)
      {
        if(*num_emi_peaks >= (max_emi_peaks)) /* not enough spaces */
        return 1;
  
        emi_peaks[*num_emi_peaks] = mx_pos;
        ++ (*num_emi_peaks);

        is_detecting_emi = 0;

        i = mx_pos - 1;

        mn = data[mx_pos];
        mn_pos = mx_pos;        
      }
      else if((!is_detecting_emi) && data[i] > mn + delta)
      {
        if(*num_absop_peaks >= (max_absop_peaks))
        return 2;

        absop_peaks[*num_absop_peaks] = mn_pos;
        ++ (*num_absop_peaks);

        is_detecting_emi = 1;

        i = mn_pos - 1;

        mx = data[mn_pos];
        mx_pos = mn_pos;
}
}

return 0;
}
/*----------------------------------------------------------------------------*/

/* Peak serch and place data in pxPeakData */
void vPeaksSearch( int16_t* psSctr, PeaksData_TypeDef* pxPeakData, int32_t lLevel)
{
    pxPeakData->psSpectraData = psSctr;
    pxPeakData->lLevel = lLevel;
    
    d_p(
          pxPeakData->psSpectraData,
          SPECTRA_SIZE,
          pxPeakData->pulIndexMax,
          &(pxPeakData->ulNumMax),
          MAX_COUNT_MAX_PEAKS,
          pxPeakData->pulIndexMin,
          &(pxPeakData->ulNumMin),
          MAX_COUNT_MIN_PEAKS,
          pxPeakData->lLevel, 
          0 ); 
}
/*----------------------------------------------------------------------------*/

/* Calculate specctra data */
void vSpectraCalcAutomat(void)
{
  /* Chekc that DMA transfer is not process, for atomic acsess. DMA run in 
  USART_DMA_Automat(), and USART status clear in DMA IRQ */
  if (eUSARTgetState() != UsartState_WaitCopyRx)
  {
    /* Muscelnaus spectr data copy, external dependet 
    and copy data scan from, to, reper find from,
    reper find to  for scan and find */
    prvSpctrDataCopy();
  }
  else 
  {
    /* DMA transfer in process. New data not copy */
  }
  
  /* Calculate spectra math. Find peaks in analitic and reper spectr. 
  All data returned in xAnPeaksData, xRpPeaksData */
  vPeaksSearch( psSpectrAn, &xAnPeaksData, GETPARAMS.iTrackValue[2]);
  vPeaksSearch( psSpectrRp, &xRpPeaksData, rpPeakSerchLevel);
  
  /* Assert window of reper find */
  vAssertScanData(&xScanData);
  
  /* Reset value of peak info for next calculation */
  for (uint32_t i = 0; i < EXPLOSIV_COUNT; i++)
  {
    xExplPeakInfo[i].eKeep = PEAK_INFO_KEEP_DEF;
    xExplPeakInfo[i].eType = PEAK_T_AN_NO_ZONE;
    xExplPeakInfo[i].fAmpPA = PEAK_INFO_AMPL_V_DEF;
    xExplPeakInfo[i].fScanV = PEAK_INFO_SCAN_PA_DEF;
    xExplPeakInfo[i].sAmpId = PEAK_INFO_AMPL_ID_DEF;
  }
  /* Find peaks result in xXXPeakInfo, and fill PeakType fild */
  vReperCalculation();
  /* Calculate zone from for reper */
  vRpZoneCalculate();
  /* Find peaks result in xXXPeakInfo, and fill PeakType fild */
  vAnaliticTntCalculation();
  
  /* Calculate explosive zone*/
  for (uint32_t i = 0; i < EXPLOSIV_COUNT; i++)
  {
    xExplZone[i] = xCalculateExpZoneId(i);
  }
  
  /* Find peaks result in xExplXPeakInfo, and fill PeakType fild */
  for (uint32_t i = 1; i < EXPLOSIV_COUNT; i++)
  {
    vAnaliticExplosiveCalculation(i);
  }
  
  /* Write reper info to send in MainBoard */
  SETPARAMS.reperAmpl = xRpPeakInfo.sAmpId;
  SETPARAMS.reperId = xRpPeakInfo.sScanId;
}

/*
 *******************************************************************************
 * @brief   vReperCalculation calculate repear peaks, fills xRpPeaksData, 
 *          calculate xRpPeakInfo.
 *
 ******************************************************************************* 
*/
void vReperCalculation(void)
{
  /* Calculate zone reper find in index array of spectra */
  u16 usLeftIdx = usScanVtoId(xScanData.sVoltRpFindFrom);
  u16 usRightIdx = usScanVtoId(xScanData.sVoltRpFindTo);
 // uint16_t usLeftIdx = VInt_to_PeakIdx(xScanData.sVoltRpFindFrom);
 // uint16_t usRightIdx = VInt_to_PeakIdx(xScanData.sVoltRpFindTo);
  
  /* xRpPeakInfo reset init */
  xRpPeakInfo.sAmpId = PEAK_INFO_AMPL_ID_DEF; 
  xRpPeakInfo.sScanId = PEAK_INFO_SCAN_ID_DEF;
  xRpPeakInfo.fAmpPA = PEAK_INFO_AMPL_V_DEF;
  xRpPeakInfo.fAmpPA = PEAK_INFO_SCAN_PA_DEF;
    
  /* Locals variables for find maximum amplitude of reper peaks */
  uint16_t sMaxAbsPeaksAmpl = 0;
  uint16_t sMaxAbsPeaksCnt = 0;
  /* Calculate type of peaks in reper min  data peaks */                                 
  for (uint32_t i = 0; i < xRpPeaksData.ulNumMin; i++ )
  {
    if ((xRpPeaksData.pulIndexMin[i] > usLeftIdx) && (xRpPeaksData.pulIndexMin[i] < usRightIdx))
    {
      /* Peaks in reper find zone */
      xRpPeaksData.pePeakTypeMin[i] = PEAK_T_RP_IN_ZONE;
      /* Check peaks amount (find max peaks) */
      if ((s32)((1<<15)-1) - xRpPeaksData.psSpectraData[xRpPeaksData.pulIndexMin[i]] > sMaxAbsPeaksAmpl)
      {
        sMaxAbsPeaksAmpl = (s32)((1<<15)-1) - (xRpPeaksData.psSpectraData[xRpPeaksData.pulIndexMin[i]]);
        sMaxAbsPeaksCnt = i; 
      }
    }
    else
    {
      /* Peaks in out reper find zone */
      xRpPeaksData.pePeakTypeMin[i] = PEAK_T_RP_OUT_ZONE;
    }
  }
  if (sMaxAbsPeaksAmpl) 
  {
    /* Set PEAK_T_RP_MIN_IN_ZONE in xRpPeaksData */
    xRpPeaksData.pePeakTypeMin[sMaxAbsPeaksCnt] = PEAK_T_RP_MIN_IN_ZONE;
    xRpPeakInfo.sAmpId = (s32)((1<<15)-1) - xRpPeaksData.psSpectraData[xRpPeaksData.pulIndexMin[sMaxAbsPeaksCnt]];
    xRpPeakInfo.sScanId = xRpPeaksData.pulIndexMin[sMaxAbsPeaksCnt];
    /* xRpPeakInfo.fAmpPA = function( xRpPeakInfo.sAmpId) - not use; */
    xRpPeakInfo.fScanV = convertPeakIdFloat(xRpPeakInfo.sScanId);
    xRpPeakInfo.eKeep = KEEP;
  }
  else 
  {
    /* No picks in zone for reper find clear peakInfo not necessary*/
    /*  xRpPeakInfo.sAmpId = PEAK_INFO_AMPL_ID_DEF; 
        xRpPeakInfo.sScanId = PEAK_INFO_SCAN_ID_DEF;
        xRpPeakInfo.fAmpPA = PEAK_INFO_AMPL_V_DEF;
        xRpPeakInfo.fAmpPA = PEAK_INFO_SCAN_PA_DEF; */
    xRpPeakInfo.eKeep = NO_KEEP; /* not necessary */
  }
}

/*
 *******************************************************************************
 * @brief   vReperAnalitic calculation peaks, fills xAnPeaksData, 
 *          calculate xTntPeakInfo.
 *
 ******************************************************************************* 
 */
void vAnaliticTntCalculation(void)
{
  /* Now only use for TNT calculstion need be to rewrite!!!!!!! */
  /* Calculate zone reper find in index array of spectra */
  uint32_t lLeftIdx = ulGetRpZoneId(Z_LEFT);
  uint32_t lRightIdx = ulGetRpZoneId(Z_RIGHT);
  
  uint16_t usMaxAbsPeaksAmpl = 0;
  uint16_t usMaxAbsPeaksCnt = 0;
  /* Calculate type of peaks in analitic chanel peaks */                                 //на самом деле мы должны искать максимумы но у макса почемуто они перевернуты
  for (uint32_t i = 0; i < xAnPeaksData.ulNumMin; i++ )
  {
    /* Preset peak type for future bit use */
    xAnPeaksData.pePeakTypeMin[i] = PEAK_T_AN_NO_ZONE;
    
    if ((xAnPeaksData.pulIndexMin[i] > lLeftIdx) && (xAnPeaksData.pulIndexMin[i] < lRightIdx))
    {
      /* Peaks in reper find zone */
      xAnPeaksData.pePeakTypeMin[i] |= PEAK_T_AN_IN_TNT_ZONE;
      /* Check is max of prevision */
      if ( ((s16)xAnPeaksData.psSpectraData[xAnPeaksData.pulIndexMax[i]] - (s16)xAnPeaksData.psSpectraData[xAnPeaksData.pulIndexMin[i]]) > usMaxAbsPeaksAmpl)
      {
        usMaxAbsPeaksAmpl = ((s16)xAnPeaksData.psSpectraData[xAnPeaksData.pulIndexMax[i]] - (s16)xAnPeaksData.psSpectraData[xAnPeaksData.pulIndexMin[i]]);
        usMaxAbsPeaksCnt = i;
      }
    }
    else
    {
      /* Peaks in out reper find zone */
      xAnPeaksData.pePeakTypeMin[i] = PEAK_T_AN_NO_ZONE;
    }
  }
  if (usMaxAbsPeaksAmpl) 
  {
    /* Check peaks is alarm */
    if ((usMaxAbsPeaksAmpl) >= 2*Explosive_Set[0].usLevelAlarm) 
    {
      xAnPeaksData.pePeakTypeMin[usMaxAbsPeaksCnt] |= PEAK_T_AN_MIN_IN_TNT_ZONE;
      xTntPeakInfo.eType = PEAK_T_AN_MIN_IN_TNT_ZONE;
    }
    else /*peak is not alarm */
    {
      xTntPeakInfo.eType = PEAK_T_AN_IN_TNT_ZONE;
    }
   // xTntPeakInfo.sAmpId = xAnPeaksData.psSpectraData[xAnPeaksData.pulIndexMin[usMaxAbsPeaksCnt]];
    xTntPeakInfo.sAmpId = ((s16)xAnPeaksData.psSpectraData[xAnPeaksData.pulIndexMax[usMaxAbsPeaksCnt]] - (s16)xAnPeaksData.psSpectraData[xAnPeaksData.pulIndexMin[usMaxAbsPeaksCnt]]);
    xTntPeakInfo.sScanId = xAnPeaksData.pulIndexMin[usMaxAbsPeaksCnt];
    /* xTntPeakInfo.fAmpPA = function( xTntPeakInfo.sAmpId) - not use; */
    xTntPeakInfo.fScanV = convertPeakIdFloat(xTntPeakInfo.sScanId);
    xTntPeakInfo.eKeep = KEEP;
  }
  else 
  {
    /* No picks in zone for reper find clear peakInfo not necessary*/
    /*  xTntPeakInfo.sAmpId = PEAK_INFO_AMPL_ID_DEF; 
        xTntPeakInfo.sScanId = PEAK_INFO_SCAN_ID_DEF;
        xTntPeakInfo.fAmpPA = PEAK_INFO_AMPL_V_DEF;
        xTntPeakInfo.fAmpPA = PEAK_INFO_SCAN_PA_DEF; */
    xTntPeakInfo.eKeep = NO_KEEP;
  }
}

/* Calculate xExplpeakInfo ad fill xAnPeaksData */
void vAnaliticExplosiveCalculation(uint32_t num)
{
  
  uint32_t lLeftIdx = xExplZone[num].ulLeftId;
  uint32_t lRightIdx = xExplZone[num].ulRightId;
  
  uint16_t usMaxAbsPeaksAmpl = 0;
  uint16_t usMaxAbsPeaksCnt = 0;
  /* Calculate type of peaks in analitic chanel peaks */                                
  for (uint32_t i = 0; i < xAnPeaksData.ulNumMin; i++ )
  {
    
    /* Preset peak type for future bit use */
    if ((xAnPeaksData.pulIndexMin[i] > lLeftIdx) && (xAnPeaksData.pulIndexMin[i] < lRightIdx))
    {
      /* Peaks in find zone, set flag */
      xAnPeaksData.pePeakTypeMin[i] |= (1 << (num*2+0));
      /* Check is max of prevision */
      if ( ((s16)xAnPeaksData.psSpectraData[xAnPeaksData.pulIndexMax[i]] - (s16)xAnPeaksData.psSpectraData[xAnPeaksData.pulIndexMin[i]]) > usMaxAbsPeaksAmpl)
      {
        usMaxAbsPeaksAmpl = ((s16)xAnPeaksData.psSpectraData[xAnPeaksData.pulIndexMax[i]] - (s16)xAnPeaksData.psSpectraData[xAnPeaksData.pulIndexMin[i]]);
        usMaxAbsPeaksCnt = i;
      }
    }
    else
    {
      /* Peaks in out reper find zone */
    }
 
    if (usMaxAbsPeaksAmpl)  
    {
      /* Check pick is alrm  */
      if ((usMaxAbsPeaksAmpl) > (2*Explosive_Set[num].usLevelAlarm))
      {
        xExplPeakInfo[num].eType =  (PeakType_TypeDef)(1 << (num*2+1));
        xAnPeaksData.pePeakTypeMin[usMaxAbsPeaksCnt] |=  (PeakType_TypeDef)(1 << (num*2+1));
      }
      else /* picks is not alarm */
      {
        xExplPeakInfo[num].eType = (PeakType_TypeDef)(1 << (num*2+0));
      }
      // xTntPeakInfo.sAmpId = xAnPeaksData.psSpectraData[xAnPeaksData.pulIndexMin[usMaxAbsPeaksCnt]];
      xExplPeakInfo[num].sAmpId = ((s16)xAnPeaksData.psSpectraData[xAnPeaksData.pulIndexMax[usMaxAbsPeaksCnt]] - (s16)xAnPeaksData.psSpectraData[xAnPeaksData.pulIndexMin[usMaxAbsPeaksCnt]]);
      xExplPeakInfo[num].sScanId = xAnPeaksData.pulIndexMin[usMaxAbsPeaksCnt];
      /* xTntPeakInfo.fAmpPA = function( xTntPeakInfo.sAmpId) - not use; */
      xExplPeakInfo[num].fScanV = convertPeakIdFloat(xExplPeakInfo[num].sScanId);
      xExplPeakInfo[num].eKeep = KEEP;
    }
    else 
    {
      /* No picks in zone for reper find clear peakInfo not necessary*/
      /*  xTntPeakInfo.sAmpId = PEAK_INFO_AMPL_ID_DEF; 
        xTntPeakInfo.sScanId = PEAK_INFO_SCAN_ID_DEF;
        xTntPeakInfo.fAmpPA = PEAK_INFO_AMPL_V_DEF;
        xTntPeakInfo.fAmpPA = PEAK_INFO_SCAN_PA_DEF; */
      xExplPeakInfo[num].eKeep = NO_KEEP;
    }
  }
}


/* Calculate explosive zone left, right, center */
ExplZone_TypeDef xCalculateExpZoneId(uint32_t num)
{
    
  int32_t ulZoneLeftId, ulZoneRightId ,temp;
 
  int32_t rpCentrZoneId = ulGetRpZoneId(Z_CENTER);
  
  temp =  (int32_t)rpCentrZoneId-(int32_t)Explosive_Set[num].ulWndId+(int32_t)Explosive_Set[num].lOffsetId;
  ulZoneLeftId = (temp < 0) ? 0 : temp;
  ulZoneLeftId = (ulZoneLeftId >=  SPECTRA_SIZE) ? ( SPECTRA_SIZE-1) : ulZoneLeftId; 
  
  
  temp =  (int32_t)rpCentrZoneId+(int32_t)Explosive_Set[num].ulWndId+(int32_t)Explosive_Set[num].lOffsetId;
  ulZoneRightId = (temp <  0) ?  0 : temp;
  ulZoneRightId = (ulZoneRightId >=  SPECTRA_SIZE) ? ( SPECTRA_SIZE-1) : ulZoneRightId; 

  ExplZone_TypeDef xZone;
  
  xZone.ulLeftId = ulZoneLeftId;
  xZone.ulRightId =  ulZoneRightId;
  xZone.ulCenterId =  (ulZoneLeftId + ulZoneRightId)/2;
  
  return (xZone);
};

/* Confert value of scan voltage in int_16 to spectr index */
uint16_t usScanVtoId (s16 scanV)
{
	return (u16) ((scanV + (0-/*From_V*/(s16)xScanData.sVoltScanFrom))\
	/((float)(/*To_V*/(s16)xScanData.sVoltScanTo-/*From_V*/(s16)xScanData.sVoltScanFrom) / (SPECTRA_SIZE-1)));
}


/*----------------------------------------------------------------------------*/

/* Variable save zone data */
static RpZone_TypeDef xRpZone;

uint32_t ulGetRpZoneClbId(EZoneType_TypeDef sel)
{
  uint32_t ulLeftId = usScanVtoId(xScanData.sVoltRpFindFrom);
  uint32_t ulRightId = usScanVtoId(xScanData.sVoltRpFindTo);
  uint32_t ulCenterId = ulGetRpZoneId(Z_CENTER);
  
   if (xRpPeakInfo.eKeep == KEEP)
  {
    /* Thats mean that center this offset */
    int32_t temp;
    temp =  (int32_t)ulCenterId-(int32_t)rpIdWnd;
    ulLeftId = (temp < 0) ? 0 : temp;
  
    temp =  (int32_t)ulCenterId+(int32_t)rpIdWnd;
    ulRightId = (temp >=  SPECTRA_SIZE) ? ( SPECTRA_SIZE-1) : temp; 

  }
  else 
  {
    /* Thats mean that center is not have offset */
    int32_t temp;
    temp =  (int32_t)ulCenterId-(int32_t)rpIdWnd+(int32_t)rpIdOffset;
    ulLeftId = (temp < 0) ? 0 : temp;
  
    temp =  (int32_t)ulCenterId+(int32_t)rpIdWnd+(int32_t)rpIdOffset;
    ulRightId = (temp >=  SPECTRA_SIZE) ? ( SPECTRA_SIZE-1) : temp; 
  }
  
  switch (sel)
  {
    case Z_LEFT:
      return(ulLeftId);
    break;
    case Z_RIGHT:
      return(ulRightId);
    break;
    case Z_CENTER:
      return(ulCenterId);
    break;
    default: return (Z_ERROR);
  }

}



void vRpZoneCalculate(void)
{
  xRpZone.ulLeftId = 0;
  xRpZone.ulRightId = 0;
  
  if (xRpPeakInfo.eKeep == KEEP)
  {
    /* Reper peak is find */
    int32_t temp;
    temp =  (int32_t)xRpPeakInfo.sScanId-(int32_t)rpIdWnd+(int32_t)rpIdOffset;
    xRpZone.ulLeftId= (temp < 0) ? 0 : temp;
    temp =  (int32_t)xRpPeakInfo.sScanId+(int32_t)rpIdWnd+(int32_t)rpIdOffset;
    xRpZone.ulRightId = (temp >=  SPECTRA_SIZE) ? ( SPECTRA_SIZE-1) : temp; 
    temp =  (int32_t)xRpPeakInfo.sScanId+(int32_t)rpIdOffset;
    xRpZone.ulCenterId = temp;
  }
  else 
  {
    /* Reper peak is not find */
    xRpZone.ulLeftId = usScanVtoId(xScanData.sVoltRpFindFrom);
    xRpZone.ulRightId = usScanVtoId(xScanData.sVoltRpFindTo);
    xRpZone.ulCenterId = xRpZone.ulLeftId+((xRpZone.ulRightId-xRpZone.ulLeftId)/2);
  }
}

/* Return find peak of Reper */
Keep_TypeDef eRpIsKeep(void)
{
  return xRpPeakInfo.eKeep;
}

uint32_t ulGetRpZoneId(EZoneType_TypeDef eZone)
{
  switch (eZone)
  {
    case Z_LEFT:
      return(xRpZone.ulLeftId);
    break;
    case Z_RIGHT:
      return(xRpZone.ulRightId);
    break;
    case Z_CENTER:
      return(xRpZone.ulCenterId);
    break;
    default: return (Z_ERROR);
  }
}

uint32_t ulGetScanId(EZoneType_TypeDef eZone)
{
  switch (eZone)
  {
    case Z_LEFT:
      return(usScanVtoId(xScanData.sVoltRpFindFrom));
    break;
    case Z_RIGHT:
      return(usScanVtoId(xScanData.sVoltRpFindTo));
    break;
    default: return (Z_ERROR);
  }
}
  
    




