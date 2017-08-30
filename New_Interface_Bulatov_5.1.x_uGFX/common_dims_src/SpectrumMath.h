#ifndef __SPECTRUM_MATH_H
#define __SPECTRUM_MATH_H

#include <stdint.h>
#include "main.h"

#define HISTORY_DEEP (1<<2)

#define PT_EXPL_MIN_Z(a) (1<<(a*2+1))

typedef enum PeakType	// 
{ 
  PEAK_T_EXPL_ZONE_0            = (1<<(0*2+0)),
  PEAK_T_EXPL_MIN_ZONE_0        = (1<<(0*2+1)),
  PEAK_T_EXPL_ZONE_1            = (1<<(1*2+0)),
  PEAK_T_EXPL_MIN_ZONE_1        = (1<<(1*2+1)),
  PEAK_T_EXPL_ZONE_2            = (1<<(2*2+0)),
  PEAK_T_EXPL_MIN_ZONE_2        = (1<<(2*2+1)),
  PEAK_T_EXPL_ZONE_3            = (1<<(3*2+0)),
  PEAK_T_EXPL_MIN_ZONE_3        = (1<<(3*2+1)),
  PEAK_T_EXPL_ZONE_4            = (1<<(4*2+0)),
  PEAK_T_EXPL_MIN_ZONE_4        = (1<<(4*2+1)), 
  PEAK_T_EXPL_ZONE_5            = (1<<(5*2+0)),
  PEAK_T_EXPL_MIN_ZONE_5        = (1<<(5*2+1)),
    
  PEAK_T_RP_IN_ZONE             = (1<<16),
  PEAK_T_RP_MIN_IN_ZONE         = (1<<17),
  PEAK_T_RP_OUT_ZONE            = (1<<18),
  PEAK_T_AN_IN_TNT_ZONE         = (1<<19),
  PEAK_T_AN_MIN_IN_TNT_ZONE     = (1<<20),
  PEAK_T_AN_NO_ZONE             = (0)
} PeakType_TypeDef;            

/* For defenition impossible value scan voltage */
#define NO_PEAK_FLOAT (-1000000.f)
#define NO_PEAK_ID (-1)
/* Peak info default parameters */
#define PEAK_INFO_DEF { PEAK_INFO_KEEP_DEF,    \
                        PEAK_INFO_AMPL_ID_DEF, \
                        PEAK_INFO_SCAN_ID_DEF, \
                        PEAK_INFO_AMPL_V_DEF,  \
                        PEAK_INFO_SCAN_PA_DEF, \
                        PEAK_T_AN_NO_ZONE }

#define PEAK_INFO_KEEP_DEF    (NO_KEEP)
#define PEAK_INFO_AMPL_ID_DEF (0)
#define PEAK_INFO_SCAN_ID_DEF (NO_PEAK_ID)
#define PEAK_INFO_AMPL_V_DEF  (0.f)
#define PEAK_INFO_SCAN_PA_DEF (NO_PEAK_FLOAT)

/* Defenition of peaks data */
typedef struct PeaksDataStruct
{
  int16_t* psSpectraData;                                       /* Pointer to spectra data */
  /* For maximum data */
  uint32_t pulIndexMax[MAX_COUNT_MAX_PEAKS];                    /* Array of index psSpectraData there find maximum */
  PeakType_TypeDef pePeakTypeMax[MAX_COUNT_MAX_PEAKS];          /* Peak type for maximum peaks */
  uint32_t ulNumMax;                                            /* Number of find max peaks */
  /* For minimum data */
  uint32_t pulIndexMin[MAX_COUNT_MIN_PEAKS];                    /* Array of index psSpectraData there find minimum */
  PeakType_TypeDef pePeakTypeMin[MAX_COUNT_MAX_PEAKS];          /* Peak type for minimum peaks */
  uint32_t ulNumMin;                                            /* Number of find min peaks */
  /* Level data */
  int32_t  lLevel;                                              /* Level for find peaks */ 
} PeaksData_TypeDef;

typedef enum KEEP
{
  KEEP = 0,
  NO_KEEP = !KEEP
} Keep_TypeDef;

typedef struct PeakInfoStruct 
{
  Keep_TypeDef          eKeep;
  uint16_t              sAmpId;      /* Peaks amplitude in spectra data units */
  uint16_t              sScanId;     /* Scan voltega in inde of spectr units  */
  float                 fAmpPA;      /* Not use, amplitude in pA              */
  float                 fScanV;      /* Scan voltage in voltage units         */
  PeakType_TypeDef      eType;       /* Type of peak */
}
PeakInfo_TypeDef;


typedef struct ScanDataStruct
{
  int16_t sVoltScanFrom;        /* Voltage from spectr scan     */
  int16_t sVoltScanTo;          /* Voltage to spectr scan       */
  int16_t sVoltRpFindFrom;      /* Voltage from find reper      */
  int16_t sVoltRpFindTo;        /* Voltage to find reper        */
} ScanData_TypeDef;

typedef struct RpZoneStructName
{
  uint32_t ulLeftId;    /* Left border of reper zone in index of array  */
  uint32_t ulRightId;   /* Righ border of reper zone in index of array  */
  uint32_t ulCenterId;  /* Center of reper zone in index of spctr array */
}
RpZone_TypeDef;

typedef struct ZoneStructName
{
  uint32_t ulLeftId;    /* Left border of explosive zone in index of array  */
  uint32_t ulRightId;   /* Righ border of explosive zone in index of array  */
  uint32_t ulCenterId;  /* Center of explosive zone in index of spctr array */
}
ExplZone_TypeDef;

typedef enum EZoneType
{
  Z_LEFT,
  Z_RIGHT,
  Z_CENTER,
  Z_ERROR = -1
}
EZoneType_TypeDef;

extern PeaksData_TypeDef xRpPeaksData; 
extern PeaksData_TypeDef xAnPeaksData;
extern PeakInfo_TypeDef xRpPeakInfo;
extern PeakInfo_TypeDef xTntPeakInfo;
extern PeakInfo_TypeDef xExplPeakInfo[EXPLOSIV_COUNT];

/* Variable for explosive zone id info */
extern ExplZone_TypeDef xExplZone[EXPLOSIV_COUNT];

extern int16_t psSpectrAn[SPECTRA_SIZE];
extern int16_t psSpectrRp[SPECTRA_SIZE];

extern int16_t psSpectrHistory[HISTORY_DEEP][SPECTRA_SIZE];
extern uint32_t ulPntSpctrHstr;

extern int16_t rpIdWnd;
extern int16_t rpIdOffset;
extern int16_t rpPeakSerchLevel;

u16             usScanVtoId     (s16 scanV);
Keep_TypeDef    eRpIsKeep       (void);
uint32_t        ulGetRpZoneId   (EZoneType_TypeDef eZone);
uint32_t        ulGetScanId     (EZoneType_TypeDef eZone);
uint32_t        ulGetRpZoneClbId(EZoneType_TypeDef sel);

#endif /* ____SPECTRUM_MATH_H */
