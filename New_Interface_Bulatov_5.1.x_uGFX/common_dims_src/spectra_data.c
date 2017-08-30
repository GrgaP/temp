#include "spectra_data.h"


#if (SPECTRA_SIZE != 1024) 
  //#error Спектры не подходят для такой развертки
#endif

//=================================================================
//           Массив спектров
//=================================================================
const s16 spectra_data_clear[SPECTRA_DATA_CLEAR_NUMBER][1024] = 
{
  { 
    #include "SpectraData\Clear\1.txt" 
  } ,
  { 
    #include "SpectraData\Clear\2.txt" 
  },
  { 
    #include "SpectraData\Clear\3.txt" 
  },
  { 
    #include "SpectraData\Clear\4.txt" 
  },
  {
    #include "SpectraData\Clear\5.txt" 
  },
  {
    #include "SpectraData\Clear\6.txt" 
  },
  {
    #include "SpectraData\Clear\7.txt" 
  },
  {
    #include "SpectraData\Clear\8.txt" 
  },
  {
    #include "SpectraData\Clear\9.txt" 
  },
  {
    #include "SpectraData\Clear\10.txt" 
  },
  {
    #include "SpectraData\Clear\11.txt" 
  },
  {
    #include "SpectraData\Clear\12.txt" 
  },
  {
    #include "SpectraData\Clear\13.txt" 
  },
  {
    #include "SpectraData\Clear\14.txt" 
  },
  {
    #include "SpectraData\Clear\15.txt" 
  },
  {
    #include "SpectraData\Clear\16.txt" 
  },
  {
    #include "SpectraData\Clear\17.txt" 
  },
  {
    #include "SpectraData\Clear\18.txt" 
  },
  {
    #include "SpectraData\Clear\19.txt" 
  },
  {
    #include "SpectraData\Clear\20.txt" 
  },
  { 
    #include "SpectraData\Clear\1.txt" 
  },
  { 
    #include "SpectraData\Clear\2.txt" 
  },
  { 
    #include "SpectraData\Clear\3.txt" 
  },
  { 
    #include "SpectraData\Clear\4.txt" 
  },
  {
    #include "SpectraData\Clear\5.txt" 
  },
  {
    #include "SpectraData\Clear\6.txt" 
  },
  {
    #include "SpectraData\Clear\7.txt" 
  },
  {
    #include "SpectraData\Clear\8.txt" 
  },
  {
    #include "SpectraData\Clear\9.txt" 
  },
  {
    #include "SpectraData\Clear\10.txt" 
  },
  {
    #include "SpectraData\Clear\11.txt" 
  },
  {
    #include "SpectraData\Clear\12.txt" 
  }
};

const s16 spectra_data_TNTlevel1[SPECTRA_DATA_TNT_NUMBER][1024] = 
{
  { 
    #include "SpectraData\TNTlevel1\1.txt" 
  } ,
  { 
    #include "SpectraData\TNTlevel1\2.txt" 
  } ,
  { 
    #include "SpectraData\TNTlevel1\3.txt" 
  } ,
  { 
    #include "SpectraData\TNTlevel1\4.txt" 
  } ,
  { 
    #include "SpectraData\TNTlevel1\5.txt" 
  } ,
  { 
    #include "SpectraData\TNTlevel1\1.txt" 
  } ,
  { 
    #include "SpectraData\TNTlevel1\2.txt" 
  } ,
  { 
    #include "SpectraData\TNTlevel1\3.txt" 
  } 
};

const s16 spectra_data_TNTlevel2[SPECTRA_DATA_TNT_NUMBER][1024] = 
{
  { 
    #include "SpectraData\TNTlevel2\1.txt" 
  } ,
  { 
    #include "SpectraData\TNTlevel2\2.txt" 
  } ,
  { 
    #include "SpectraData\TNTlevel2\3.txt" 
  } ,
  { 
    #include "SpectraData\TNTlevel2\4.txt" 
  } ,
  { 
    #include "SpectraData\TNTlevel2\5.txt" 
  } ,
  { 
    #include "SpectraData\TNTlevel2\1.txt" 
  } ,
  { 
    #include "SpectraData\TNTlevel2\2.txt" 
  } ,
  { 
    #include "SpectraData\TNTlevel2\3.txt" 
  } 
};

const s16 spectra_data_TNTlevel3[SPECTRA_DATA_TNT_NUMBER][1024] = 
{
  { 
    #include "SpectraData\TNTlevel3\1.txt" 
  } ,
  { 
    #include "SpectraData\TNTlevel3\2.txt" 
  } ,
  { 
    #include "SpectraData\TNTlevel3\3.txt" 
  } ,
  { 
    #include "SpectraData\TNTlevel3\4.txt" 
  } ,
  { 
    #include "SpectraData\TNTlevel3\5.txt" 
  } ,
  { 
    #include "SpectraData\TNTlevel3\1.txt" 
  } ,
  { 
    #include "SpectraData\TNTlevel3\2.txt" 
  } ,
  { 
    #include "SpectraData\TNTlevel3\3.txt" 
  } 
};