/*
* This source file is part of the EtherCAT Slave Stack Code licensed by Beckhoff Automation GmbH & Co KG, 33415 Verl, Germany.
* The corresponding license agreement applies. This hint shall not be removed.
*/

/**
* \addtogroup lan9252_app lan9252_app
* @{
*/

/**
\file lan9252_appObjects
\author ET9300Utilities.ApplicationHandler (Version 1.6.4.0) | EthercatSSC@beckhoff.com

\brief lan9252_app specific objects<br>
\brief NOTE : This file will be overwritten if a new object dictionary is generated!<br>
*/

#if defined(_LAN9252_APP_) && (_LAN9252_APP_ == 1)
#define PROTO
#else
#define PROTO extern
#endif
/******************************************************************************
*                    Object 0x1601 : OutputData process data mapping
******************************************************************************/
/**
* \addtogroup 0x1601 0x1601 | OutputData process data mapping
* @{
* \brief Object 0x1601 (OutputData process data mapping) definition
*/
#ifdef _OBJD_
/**
* \brief Object entry descriptions<br>
* <br>
* SubIndex 0<br>
* SubIndex 1 - Reference to 0x7010.1<br>
* SubIndex 2 - Reference to 0x7010.2<br>
* SubIndex 3 - Reference to 0x7010.3<br>
* SubIndex 4 - Reference to 0x7010.4<br>
* SubIndex 5 - Reference to 0x7010.5<br>
* SubIndex 6 - Reference to 0x7010.6<br>
* SubIndex 7 - Reference to 0x7010.7<br>
* SubIndex 8 - Reference to 0x7010.8<br>
* SubIndex 9 - Reference to 0x7010.9<br>
* SubIndex 10 - Reference to 0x7010.10<br>
* SubIndex 11 - Reference to 0x7010.11<br>
* SubIndex 12 - Reference to 0x7010.12<br>
* SubIndex 13 - Reference to 0x7010.13<br>
* SubIndex 14 - Reference to 0x7010.14<br>
* SubIndex 15 - Reference to 0x7010.15<br>
* SubIndex 16 - Reference to 0x7010.16<br>
* SubIndex 17 - Reference to 0x7010.17<br>
* SubIndex 18 - Reference to 0x7010.18<br>
* SubIndex 19 - Reference to 0x7010.19<br>
* SubIndex 20 - Reference to 0x7010.20<br>
* SubIndex 21 - Reference to 0x7010.21<br>
* SubIndex 22 - Reference to 0x7010.22<br>
* SubIndex 23 - Reference to 0x7010.23<br>
* SubIndex 24 - Reference to 0x7010.24<br>
* SubIndex 25 - Reference to 0x7010.25<br>
* SubIndex 26 - Reference to 0x7010.26<br>
* SubIndex 27 - Reference to 0x7010.27<br>
* SubIndex 28 - Reference to 0x7010.28<br>
* SubIndex 29 - Reference to 0x7010.29<br>
* SubIndex 30 - Reference to 0x7010.30<br>
* SubIndex 31 - Reference to 0x7010.31<br>
* SubIndex 32 - Reference to 0x7010.32<br>
* SubIndex 33 - Reference to 0x7010.33<br>
* SubIndex 34 - Reference to 0x7010.34<br>
* SubIndex 35 - Reference to 0x7010.35<br>
* SubIndex 36 - Reference to 0x7010.36<br>
* SubIndex 37 - Reference to 0x7010.37<br>
* SubIndex 38 - Reference to 0x7010.38<br>
* SubIndex 39 - Reference to 0x7010.39<br>
* SubIndex 40 - Reference to 0x7010.40<br>
* SubIndex 41 - Reference to 0x7010.41<br>
* SubIndex 42 - Reference to 0x7010.42<br>
* SubIndex 43 - Reference to 0x7010.43<br>
* SubIndex 44 - Reference to 0x7010.44<br>
* SubIndex 45 - Reference to 0x7010.45<br>
* SubIndex 46 - Reference to 0x7010.46<br>
* SubIndex 47 - Reference to 0x7010.47<br>
* SubIndex 48 - Reference to 0x7010.48<br>
* SubIndex 49 - Reference to 0x7010.49<br>
* SubIndex 50 - Reference to 0x7010.50<br>
* SubIndex 51 - Reference to 0x7010.51<br>
* SubIndex 52 - Reference to 0x7010.52<br>
* SubIndex 53 - Reference to 0x7010.53<br>
* SubIndex 54 - Reference to 0x7010.54<br>
* SubIndex 55 - Reference to 0x7010.55<br>
* SubIndex 56 - Reference to 0x7010.56<br>
* SubIndex 57 - Reference to 0x7010.57<br>
* SubIndex 58 - Reference to 0x7010.58<br>
* SubIndex 59 - Reference to 0x7010.59<br>
* SubIndex 60 - Reference to 0x7010.60<br>
*/
OBJCONST TSDOINFOENTRYDESC    OBJMEM asEntryDesc0x1601[] = {
{ DEFTYPE_UNSIGNED8 , 0x8 , ACCESS_READ },
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex1 - Reference to 0x7010.1 */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex2 - Reference to 0x7010.2 */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex3 - Reference to 0x7010.3 */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex4 - Reference to 0x7010.4 */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex5 - Reference to 0x7010.5 */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex6 - Reference to 0x7010.6 */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex7 - Reference to 0x7010.7 */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex8 - Reference to 0x7010.8 */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex9 - Reference to 0x7010.9 */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex10 - Reference to 0x7010.10 */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex11 - Reference to 0x7010.11 */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex12 - Reference to 0x7010.12 */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex13 - Reference to 0x7010.13 */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex14 - Reference to 0x7010.14 */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex15 - Reference to 0x7010.15 */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex16 - Reference to 0x7010.16 */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex17 - Reference to 0x7010.17 */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex18 - Reference to 0x7010.18 */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex19 - Reference to 0x7010.19 */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex20 - Reference to 0x7010.20 */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex21 - Reference to 0x7010.21 */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex22 - Reference to 0x7010.22 */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex23 - Reference to 0x7010.23 */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex24 - Reference to 0x7010.24 */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex25 - Reference to 0x7010.25 */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex26 - Reference to 0x7010.26 */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex27 - Reference to 0x7010.27 */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex28 - Reference to 0x7010.28 */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex29 - Reference to 0x7010.29 */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex30 - Reference to 0x7010.30 */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex31 - Reference to 0x7010.31 */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex32 - Reference to 0x7010.32 */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex33 - Reference to 0x7010.33 */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex34 - Reference to 0x7010.34 */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex35 - Reference to 0x7010.35 */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex36 - Reference to 0x7010.36 */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex37 - Reference to 0x7010.37 */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex38 - Reference to 0x7010.38 */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex39 - Reference to 0x7010.39 */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex40 - Reference to 0x7010.40 */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex41 - Reference to 0x7010.41 */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex42 - Reference to 0x7010.42 */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex43 - Reference to 0x7010.43 */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex44 - Reference to 0x7010.44 */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex45 - Reference to 0x7010.45 */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex46 - Reference to 0x7010.46 */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex47 - Reference to 0x7010.47 */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex48 - Reference to 0x7010.48 */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex49 - Reference to 0x7010.49 */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex50 - Reference to 0x7010.50 */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex51 - Reference to 0x7010.51 */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex52 - Reference to 0x7010.52 */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex53 - Reference to 0x7010.53 */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex54 - Reference to 0x7010.54 */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex55 - Reference to 0x7010.55 */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex56 - Reference to 0x7010.56 */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex57 - Reference to 0x7010.57 */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex58 - Reference to 0x7010.58 */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex59 - Reference to 0x7010.59 */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }}; /* Subindex60 - Reference to 0x7010.60 */

/**
* \brief Object/Entry names
*/
OBJCONST UCHAR OBJMEM aName0x1601[] = "OutputData process data mapping\000"
"SubIndex 001\000"
"SubIndex 002\000"
"SubIndex 003\000"
"SubIndex 004\000"
"SubIndex 005\000"
"SubIndex 006\000"
"SubIndex 007\000"
"SubIndex 008\000"
"SubIndex 009\000"
"SubIndex 010\000"
"SubIndex 011\000"
"SubIndex 012\000"
"SubIndex 013\000"
"SubIndex 014\000"
"SubIndex 015\000"
"SubIndex 016\000"
"SubIndex 017\000"
"SubIndex 018\000"
"SubIndex 019\000"
"SubIndex 020\000"
"SubIndex 021\000"
"SubIndex 022\000"
"SubIndex 023\000"
"SubIndex 024\000"
"SubIndex 025\000"
"SubIndex 026\000"
"SubIndex 027\000"
"SubIndex 028\000"
"SubIndex 029\000"
"SubIndex 030\000"
"SubIndex 031\000"
"SubIndex 032\000"
"SubIndex 033\000"
"SubIndex 034\000"
"SubIndex 035\000"
"SubIndex 036\000"
"SubIndex 037\000"
"SubIndex 038\000"
"SubIndex 039\000"
"SubIndex 040\000"
"SubIndex 041\000"
"SubIndex 042\000"
"SubIndex 043\000"
"SubIndex 044\000"
"SubIndex 045\000"
"SubIndex 046\000"
"SubIndex 047\000"
"SubIndex 048\000"
"SubIndex 049\000"
"SubIndex 050\000"
"SubIndex 051\000"
"SubIndex 052\000"
"SubIndex 053\000"
"SubIndex 054\000"
"SubIndex 055\000"
"SubIndex 056\000"
"SubIndex 057\000"
"SubIndex 058\000"
"SubIndex 059\000"
"SubIndex 060\000\377";
#endif //#ifdef _OBJD_

#ifndef _LAN9252_APP_OBJECTS_H_
/**
* \brief Object structure
*/
typedef struct OBJ_STRUCT_PACKED_START {
UINT16 u16SubIndex0;
UINT32 SI1; /* Subindex1 - Reference to 0x7010.1 */
UINT32 SI2; /* Subindex2 - Reference to 0x7010.2 */
UINT32 SI3; /* Subindex3 - Reference to 0x7010.3 */
UINT32 SI4; /* Subindex4 - Reference to 0x7010.4 */
UINT32 SI5; /* Subindex5 - Reference to 0x7010.5 */
UINT32 SI6; /* Subindex6 - Reference to 0x7010.6 */
UINT32 SI7; /* Subindex7 - Reference to 0x7010.7 */
UINT32 SI8; /* Subindex8 - Reference to 0x7010.8 */
UINT32 SI9; /* Subindex9 - Reference to 0x7010.9 */
UINT32 SI10; /* Subindex10 - Reference to 0x7010.10 */
UINT32 SI11; /* Subindex11 - Reference to 0x7010.11 */
UINT32 SI12; /* Subindex12 - Reference to 0x7010.12 */
UINT32 SI13; /* Subindex13 - Reference to 0x7010.13 */
UINT32 SI14; /* Subindex14 - Reference to 0x7010.14 */
UINT32 SI15; /* Subindex15 - Reference to 0x7010.15 */
UINT32 SI16; /* Subindex16 - Reference to 0x7010.16 */
UINT32 SI17; /* Subindex17 - Reference to 0x7010.17 */
UINT32 SI18; /* Subindex18 - Reference to 0x7010.18 */
UINT32 SI19; /* Subindex19 - Reference to 0x7010.19 */
UINT32 SI20; /* Subindex20 - Reference to 0x7010.20 */
UINT32 SI21; /* Subindex21 - Reference to 0x7010.21 */
UINT32 SI22; /* Subindex22 - Reference to 0x7010.22 */
UINT32 SI23; /* Subindex23 - Reference to 0x7010.23 */
UINT32 SI24; /* Subindex24 - Reference to 0x7010.24 */
UINT32 SI25; /* Subindex25 - Reference to 0x7010.25 */
UINT32 SI26; /* Subindex26 - Reference to 0x7010.26 */
UINT32 SI27; /* Subindex27 - Reference to 0x7010.27 */
UINT32 SI28; /* Subindex28 - Reference to 0x7010.28 */
UINT32 SI29; /* Subindex29 - Reference to 0x7010.29 */
UINT32 SI30; /* Subindex30 - Reference to 0x7010.30 */
UINT32 SI31; /* Subindex31 - Reference to 0x7010.31 */
UINT32 SI32; /* Subindex32 - Reference to 0x7010.32 */
UINT32 SI33; /* Subindex33 - Reference to 0x7010.33 */
UINT32 SI34; /* Subindex34 - Reference to 0x7010.34 */
UINT32 SI35; /* Subindex35 - Reference to 0x7010.35 */
UINT32 SI36; /* Subindex36 - Reference to 0x7010.36 */
UINT32 SI37; /* Subindex37 - Reference to 0x7010.37 */
UINT32 SI38; /* Subindex38 - Reference to 0x7010.38 */
UINT32 SI39; /* Subindex39 - Reference to 0x7010.39 */
UINT32 SI40; /* Subindex40 - Reference to 0x7010.40 */
UINT32 SI41; /* Subindex41 - Reference to 0x7010.41 */
UINT32 SI42; /* Subindex42 - Reference to 0x7010.42 */
UINT32 SI43; /* Subindex43 - Reference to 0x7010.43 */
UINT32 SI44; /* Subindex44 - Reference to 0x7010.44 */
UINT32 SI45; /* Subindex45 - Reference to 0x7010.45 */
UINT32 SI46; /* Subindex46 - Reference to 0x7010.46 */
UINT32 SI47; /* Subindex47 - Reference to 0x7010.47 */
UINT32 SI48; /* Subindex48 - Reference to 0x7010.48 */
UINT32 SI49; /* Subindex49 - Reference to 0x7010.49 */
UINT32 SI50; /* Subindex50 - Reference to 0x7010.50 */
UINT32 SI51; /* Subindex51 - Reference to 0x7010.51 */
UINT32 SI52; /* Subindex52 - Reference to 0x7010.52 */
UINT32 SI53; /* Subindex53 - Reference to 0x7010.53 */
UINT32 SI54; /* Subindex54 - Reference to 0x7010.54 */
UINT32 SI55; /* Subindex55 - Reference to 0x7010.55 */
UINT32 SI56; /* Subindex56 - Reference to 0x7010.56 */
UINT32 SI57; /* Subindex57 - Reference to 0x7010.57 */
UINT32 SI58; /* Subindex58 - Reference to 0x7010.58 */
UINT32 SI59; /* Subindex59 - Reference to 0x7010.59 */
UINT32 SI60; /* Subindex60 - Reference to 0x7010.60 */
} OBJ_STRUCT_PACKED_END
TOBJ1601;
#endif //#ifndef _LAN9252_APP_OBJECTS_H_

/**
* \brief Object variable
*/
PROTO TOBJ1601 OutputDataProcessDataMapping0x1601
#if defined(_LAN9252_APP_) && (_LAN9252_APP_ == 1)
={60,0x70100108,0x70100208,0x70100320,0x70100408,0x70100508,0x70100610,0x70100708,0x70100808,0x70100910,0x70100A08,0x70100B08,0x70100C08,0x70100D08,0x70100E10,0x70100F20,0x70101020,0x70101110,0x70101208,0x70101308,0x70101410,0x70101520,0x70101620,0x70101720,0x70101808,0x70101908,0x70101A20,0x70101B20,0x70101C08,0x70101D08,0x70101E20,0x70101F20,0x70102008,0x70102108,0x70102220,0x70102320,0x70102408,0x70102508,0x70102610,0x70102720,0x70102820,0x70102908,0x70102A08,0x70102B20,0x70102C10,0x70102D08,0x70102E08,0x70102F20,0x70103020,0x70103108,0x70103208,0x70103320,0x70103420,0x70103510,0x70103608,0x70103708,0x70103808,0x70103908,0x70103A08,0x70103B08,0x70103C08}
#endif
;
/** @}*/



/******************************************************************************
*                    Object 0x1A00 : InputData process data mapping
******************************************************************************/
/**
* \addtogroup 0x1A00 0x1A00 | InputData process data mapping
* @{
* \brief Object 0x1A00 (InputData process data mapping) definition
*/
#ifdef _OBJD_
/**
* \brief Object entry descriptions<br>
* <br>
* SubIndex 0<br>
* SubIndex 1 - Reference to 0x6000.1<br>
* SubIndex 2 - Reference to 0x6000.2<br>
* SubIndex 3 - Reference to 0x6000.3<br>
* SubIndex 4 - Reference to 0x6000.4<br>
* SubIndex 5 - Reference to 0x6000.5<br>
* SubIndex 6 - Reference to 0x6000.6<br>
* SubIndex 7 - Reference to 0x6000.7<br>
* SubIndex 8 - Reference to 0x6000.8<br>
* SubIndex 9 - Reference to 0x6000.9<br>
* SubIndex 10 - Reference to 0x6000.10<br>
* SubIndex 11 - Reference to 0x6000.11<br>
* SubIndex 12 - Reference to 0x6000.12<br>
* SubIndex 13 - Reference to 0x6000.13<br>
* SubIndex 14 - Reference to 0x6000.14<br>
* SubIndex 15 - Reference to 0x6000.15<br>
* SubIndex 16 - Reference to 0x6000.16<br>
* SubIndex 17 - Reference to 0x6000.17<br>
* SubIndex 18 - Reference to 0x6000.18<br>
* SubIndex 19 - Reference to 0x6000.19<br>
* SubIndex 20 - Reference to 0x6000.20<br>
* SubIndex 21 - Reference to 0x6000.21<br>
* SubIndex 22 - Reference to 0x6000.22<br>
* SubIndex 23 - Reference to 0x6000.23<br>
* SubIndex 24 - Reference to 0x6000.24<br>
* SubIndex 25 - Reference to 0x6000.25<br>
* SubIndex 26 - Reference to 0x6000.26<br>
* SubIndex 27 - Reference to 0x6000.27<br>
* SubIndex 28 - Reference to 0x6000.28<br>
* SubIndex 29 - Reference to 0x6000.29<br>
* SubIndex 30 - Reference to 0x6000.30<br>
* SubIndex 31 - Reference to 0x6000.31<br>
* SubIndex 32 - Reference to 0x6000.32<br>
* SubIndex 33 - Reference to 0x6000.33<br>
* SubIndex 34 - Reference to 0x6000.34<br>
* SubIndex 35 - Reference to 0x6000.35<br>
* SubIndex 36 - Reference to 0x6000.36<br>
* SubIndex 37 - Reference to 0x6000.37<br>
* SubIndex 38 - Reference to 0x6000.38<br>
* SubIndex 39 - Reference to 0x6000.39<br>
* SubIndex 40 - Reference to 0x6000.40<br>
* SubIndex 41 - Reference to 0x6000.41<br>
* SubIndex 42 - Reference to 0x6000.42<br>
* SubIndex 43 - Reference to 0x6000.43<br>
* SubIndex 44 - Reference to 0x6000.44<br>
* SubIndex 45 - Reference to 0x6000.45<br>
* SubIndex 46 - Reference to 0x6000.46<br>
* SubIndex 47 - Reference to 0x6000.47<br>
* SubIndex 48 - Reference to 0x6000.48<br>
* SubIndex 49 - Reference to 0x6000.49<br>
* SubIndex 50 - Reference to 0x6000.50<br>
* SubIndex 51 - Reference to 0x6000.51<br>
* SubIndex 52 - Reference to 0x6000.52<br>
* SubIndex 53 - Reference to 0x6000.53<br>
* SubIndex 54 - Reference to 0x6000.54<br>
* SubIndex 55 - Reference to 0x6000.55<br>
* SubIndex 56 - Reference to 0x6000.56<br>
* SubIndex 57 - Reference to 0x6000.57<br>
* SubIndex 58 - Reference to 0x6000.58<br>
* SubIndex 59 - Reference to 0x6000.59<br>
* SubIndex 60 - Reference to 0x6000.60<br>
* SubIndex 61 - Reference to 0x6000.61<br>
* SubIndex 62 - Reference to 0x6000.62<br>
* SubIndex 63 - Reference to 0x6000.63<br>
* SubIndex 64 - Reference to 0x6000.64<br>
* SubIndex 65 - Reference to 0x6000.65<br>
* SubIndex 66 - Reference to 0x6000.66<br>
* SubIndex 67 - Reference to 0x6000.67<br>
* SubIndex 68 - Reference to 0x6000.68<br>
* SubIndex 69 - Reference to 0x6000.69<br>
* SubIndex 70 - Reference to 0x6000.70<br>
* SubIndex 71 - Reference to 0x6000.71<br>
* SubIndex 72 - Reference to 0x6000.72<br>
* SubIndex 73 - Reference to 0x6000.73<br>
* SubIndex 74 - Reference to 0x6000.74<br>
* SubIndex 75 - Reference to 0x6000.75<br>
* SubIndex 76 - Reference to 0x6000.76<br>
* SubIndex 77 - Reference to 0x6000.77<br>
* SubIndex 78 - Reference to 0x6000.78<br>
* SubIndex 79 - Reference to 0x6000.79<br>
* SubIndex 80 - Reference to 0x6000.80<br>
* SubIndex 81 - Reference to 0x6000.81<br>
* SubIndex 82 - Reference to 0x6000.82<br>
* SubIndex 83 - Reference to 0x6000.83<br>
* SubIndex 84 - Reference to 0x6000.84<br>
* SubIndex 85 - Reference to 0x6000.85<br>
* SubIndex 86 - Reference to 0x6000.86<br>
* SubIndex 87 - Reference to 0x6000.87<br>
* SubIndex 88 - Reference to 0x6000.88<br>
* SubIndex 89 - Reference to 0x6000.89<br>
* SubIndex 90 - Reference to 0x6000.90<br>
* SubIndex 91 - Reference to 0x6000.91<br>
* SubIndex 92 - Reference to 0x6000.92<br>
* SubIndex 93 - Reference to 0x6000.93<br>
* SubIndex 94 - Reference to 0x6000.94<br>
* SubIndex 95 - Reference to 0x6000.95<br>
* SubIndex 96 - Reference to 0x6000.96<br>
* SubIndex 97 - Reference to 0x6000.97<br>
* SubIndex 98 - Reference to 0x6000.98<br>
* SubIndex 99 - Reference to 0x6000.99<br>
* SubIndex 100 - Reference to 0x6000.100<br>
* SubIndex 101 - Reference to 0x6000.101<br>
* SubIndex 102 - Reference to 0x6000.102<br>
* SubIndex 103 - Reference to 0x6000.103<br>
* SubIndex 104 - Reference to 0x6000.104<br>
* SubIndex 105 - Reference to 0x6000.105<br>
* SubIndex 106 - Reference to 0x6000.106<br>
* SubIndex 107 - Reference to 0x6000.107<br>
* SubIndex 108 - Reference to 0x6000.108<br>
* SubIndex 109 - Reference to 0x6000.109<br>
* SubIndex 110 - Reference to 0x6000.110<br>
* SubIndex 111 - Reference to 0x6000.111<br>
* SubIndex 112 - Reference to 0x6000.112<br>
* SubIndex 113 - Reference to 0x6000.113<br>
* SubIndex 114 - Reference to 0x6000.114<br>
* SubIndex 115 - Reference to 0x6000.115<br>
* SubIndex 116 - Reference to 0x6000.116<br>
* SubIndex 117 - Reference to 0x6000.117<br>
* SubIndex 118 - Reference to 0x6000.118<br>
* SubIndex 119 - Reference to 0x6000.119<br>
* SubIndex 120 - Reference to 0x6000.120<br>
* SubIndex 121 - Reference to 0x6000.121<br>
* SubIndex 122 - Reference to 0x6000.122<br>
* SubIndex 123 - Reference to 0x6000.123<br>
* SubIndex 124 - Reference to 0x6000.124<br>
* SubIndex 125 - Reference to 0x6000.125<br>
* SubIndex 126 - Reference to 0x6000.126<br>
* SubIndex 127 - Reference to 0x6000.127<br>
* SubIndex 128 - Reference to 0x6000.128<br>
* SubIndex 129 - Reference to 0x6000.129<br>
* SubIndex 130 - Reference to 0x6000.130<br>
* SubIndex 131 - Reference to 0x6000.131<br>
* SubIndex 132 - Reference to 0x6000.132<br>
* SubIndex 133 - Reference to 0x6000.133<br>
* SubIndex 134 - Reference to 0x6000.134<br>
* SubIndex 135 - Reference to 0x6000.135<br>
* SubIndex 136 - Reference to 0x6000.136<br>
* SubIndex 137 - Reference to 0x6000.137<br>
* SubIndex 138 - Reference to 0x6000.138<br>
* SubIndex 139 - Reference to 0x6000.139<br>
* SubIndex 140 - Reference to 0x6000.140<br>
* SubIndex 141 - Reference to 0x6000.141<br>
* SubIndex 142 - Reference to 0x6000.142<br>
* SubIndex 143 - Reference to 0x6000.143<br>
* SubIndex 144 - Reference to 0x6000.144<br>
* SubIndex 145 - Reference to 0x6000.145<br>
* SubIndex 146 - Reference to 0x6000.146<br>
* SubIndex 147 - Reference to 0x6000.147<br>
* SubIndex 148 - Reference to 0x6000.148<br>
* SubIndex 149 - Reference to 0x6000.149<br>
* SubIndex 150 - Reference to 0x6000.150<br>
* SubIndex 151 - Reference to 0x6000.151<br>
* SubIndex 152 - Reference to 0x6000.152<br>
* SubIndex 153 - Reference to 0x6000.153<br>
* SubIndex 154 - Reference to 0x6000.154<br>
* SubIndex 155 - Reference to 0x6000.155<br>
* SubIndex 156 - Reference to 0x6000.156<br>
* SubIndex 157 - Reference to 0x6000.157<br>
* SubIndex 158 - Reference to 0x6000.158<br>
* SubIndex 159 - Reference to 0x6000.159<br>
* SubIndex 160 - Reference to 0x6000.160<br>
* SubIndex 161 - Reference to 0x6000.161<br>
* SubIndex 162 - Reference to 0x6000.162<br>
* SubIndex 163 - Reference to 0x6000.163<br>
* SubIndex 164 - Reference to 0x6000.164<br>
* SubIndex 165 - Reference to 0x6000.165<br>
* SubIndex 166 - Reference to 0x6000.166<br>
* SubIndex 167 - Reference to 0x6000.167<br>
* SubIndex 168 - Reference to 0x6000.168<br>
* SubIndex 169 - Reference to 0x6000.169<br>
* SubIndex 170 - Reference to 0x6000.170<br>
* SubIndex 171 - Reference to 0x6000.171<br>
* SubIndex 172 - Reference to 0x6000.172<br>
* SubIndex 173 - Reference to 0x6000.173<br>
* SubIndex 174 - Reference to 0x6000.174<br>
* SubIndex 175 - Reference to 0x6000.175<br>
* SubIndex 176 - Reference to 0x6000.176<br>
* SubIndex 177 - Reference to 0x6000.177<br>
* SubIndex 178 - Reference to 0x6000.178<br>
* SubIndex 179 - Reference to 0x6000.179<br>
* SubIndex 180 - Reference to 0x6000.180<br>
* SubIndex 181 - Reference to 0x6000.181<br>
* SubIndex 182 - Reference to 0x6000.182<br>
* SubIndex 183 - Reference to 0x6000.183<br>
* SubIndex 184 - Reference to 0x6000.184<br>
* SubIndex 185 - Reference to 0x6000.185<br>
* SubIndex 186 - Reference to 0x6000.186<br>
* SubIndex 187 - Reference to 0x6000.187<br>
* SubIndex 188 - Reference to 0x6000.188<br>
* SubIndex 189 - Reference to 0x6000.189<br>
* SubIndex 190 - Reference to 0x6000.190<br>
* SubIndex 191 - Reference to 0x6000.191<br>
* SubIndex 192 - Reference to 0x6000.192<br>
* SubIndex 193 - Reference to 0x6000.193<br>
* SubIndex 194 - Reference to 0x6000.194<br>
* SubIndex 195 - Reference to 0x6000.195<br>
* SubIndex 196 - Reference to 0x6000.196<br>
* SubIndex 197 - Reference to 0x6000.197<br>
* SubIndex 198 - Reference to 0x6000.198<br>
* SubIndex 199 - Reference to 0x6000.199<br>
* SubIndex 200 - Reference to 0x6000.200<br>
* SubIndex 201 - Reference to 0x6000.201<br>
* SubIndex 202 - Reference to 0x6000.202<br>
* SubIndex 203 - Reference to 0x6000.203<br>
* SubIndex 204 - Reference to 0x6000.204<br>
* SubIndex 205 - Reference to 0x6000.205<br>
* SubIndex 206 - Reference to 0x6000.206<br>
* SubIndex 207 - Reference to 0x6000.207<br>
* SubIndex 208 - Reference to 0x6000.208<br>
* SubIndex 209 - Reference to 0x6000.209<br>
* SubIndex 210 - Reference to 0x6000.210<br>
* SubIndex 211 - Reference to 0x6000.211<br>
* SubIndex 212 - Reference to 0x6000.212<br>
* SubIndex 213 - Reference to 0x6000.213<br>
* SubIndex 214 - Reference to 0x6000.214<br>
* SubIndex 215 - Reference to 0x6000.215<br>
* SubIndex 216 - Reference to 0x6000.216<br>
* SubIndex 217 - Reference to 0x6000.217<br>
* SubIndex 218 - Reference to 0x6000.218<br>
* SubIndex 219 - Reference to 0x6000.219<br>
* SubIndex 220 - Reference to 0x6000.220<br>
* SubIndex 221 - Reference to 0x6000.221<br>
* SubIndex 222 - Reference to 0x6000.222<br>
* SubIndex 223 - Reference to 0x6000.223<br>
* SubIndex 224 - Reference to 0x6000.224<br>
* SubIndex 225 - Reference to 0x6000.225<br>
* SubIndex 226 - Reference to 0x6000.226<br>
* SubIndex 227 - Reference to 0x6000.227<br>
* SubIndex 228 - Reference to 0x6000.228<br>
* SubIndex 229 - Reference to 0x6000.229<br>
* SubIndex 230 - Reference to 0x6000.230<br>
* SubIndex 231 - Reference to 0x6000.231<br>
* SubIndex 232 - Reference to 0x6000.232<br>
* SubIndex 233 - Reference to 0x6000.233<br>
* SubIndex 234 - Reference to 0x6000.234<br>
* SubIndex 235 - Reference to 0x6000.235<br>
* SubIndex 236 - Reference to 0x6000.236<br>
* SubIndex 237 - Reference to 0x6000.237<br>
* SubIndex 238 - Reference to 0x6000.238<br>
* SubIndex 239 - Reference to 0x6000.239<br>
* SubIndex 240 - Reference to 0x6000.240<br>
* SubIndex 241 - Reference to 0x6000.241<br>
* SubIndex 242 - Reference to 0x6000.242<br>
* SubIndex 243 - Reference to 0x6000.243<br>
* SubIndex 244 - Reference to 0x6000.244<br>
* SubIndex 245 - Reference to 0x6000.245<br>
* SubIndex 246 - Reference to 0x6000.246<br>
* SubIndex 247 - Reference to 0x6000.247<br>
* SubIndex 248 - Reference to 0x6000.248<br>
* SubIndex 249 - Reference to 0x6000.249<br>
* SubIndex 250 - Reference to 0x6000.250<br>
* SubIndex 251 - Reference to 0x6000.251<br>
* SubIndex 252 - Reference to 0x6000.252<br>
* SubIndex 253 - Reference to 0x6000.253<br>
*/
OBJCONST TSDOINFOENTRYDESC    OBJMEM asEntryDesc0x1A00[] = {
{ DEFTYPE_UNSIGNED8 , 0x8 , ACCESS_READ },
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex1 - Reference to 0x6000.1 */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex2 - Reference to 0x6000.2 */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex3 - Reference to 0x6000.3 */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex4 - Reference to 0x6000.4 */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex5 - Reference to 0x6000.5 */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex6 - Reference to 0x6000.6 */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex7 - Reference to 0x6000.7 */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex8 - Reference to 0x6000.8 */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex9 - Reference to 0x6000.9 */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex10 - Reference to 0x6000.10 */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex11 - Reference to 0x6000.11 */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex12 - Reference to 0x6000.12 */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex13 - Reference to 0x6000.13 */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex14 - Reference to 0x6000.14 */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex15 - Reference to 0x6000.15 */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex16 - Reference to 0x6000.16 */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex17 - Reference to 0x6000.17 */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex18 - Reference to 0x6000.18 */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex19 - Reference to 0x6000.19 */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex20 - Reference to 0x6000.20 */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex21 - Reference to 0x6000.21 */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex22 - Reference to 0x6000.22 */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex23 - Reference to 0x6000.23 */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex24 - Reference to 0x6000.24 */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex25 - Reference to 0x6000.25 */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex26 - Reference to 0x6000.26 */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex27 - Reference to 0x6000.27 */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex28 - Reference to 0x6000.28 */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex29 - Reference to 0x6000.29 */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex30 - Reference to 0x6000.30 */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex31 - Reference to 0x6000.31 */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex32 - Reference to 0x6000.32 */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex33 - Reference to 0x6000.33 */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex34 - Reference to 0x6000.34 */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex35 - Reference to 0x6000.35 */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex36 - Reference to 0x6000.36 */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex37 - Reference to 0x6000.37 */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex38 - Reference to 0x6000.38 */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex39 - Reference to 0x6000.39 */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex40 - Reference to 0x6000.40 */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex41 - Reference to 0x6000.41 */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex42 - Reference to 0x6000.42 */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex43 - Reference to 0x6000.43 */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex44 - Reference to 0x6000.44 */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex45 - Reference to 0x6000.45 */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex46 - Reference to 0x6000.46 */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex47 - Reference to 0x6000.47 */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex48 - Reference to 0x6000.48 */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex49 - Reference to 0x6000.49 */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex50 - Reference to 0x6000.50 */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex51 - Reference to 0x6000.51 */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex52 - Reference to 0x6000.52 */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex53 - Reference to 0x6000.53 */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex54 - Reference to 0x6000.54 */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex55 - Reference to 0x6000.55 */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex56 - Reference to 0x6000.56 */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex57 - Reference to 0x6000.57 */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex58 - Reference to 0x6000.58 */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex59 - Reference to 0x6000.59 */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex60 - Reference to 0x6000.60 */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex61 - Reference to 0x6000.61 */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex62 - Reference to 0x6000.62 */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex63 - Reference to 0x6000.63 */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex64 - Reference to 0x6000.64 */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex65 - Reference to 0x6000.65 */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex66 - Reference to 0x6000.66 */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex67 - Reference to 0x6000.67 */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex68 - Reference to 0x6000.68 */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex69 - Reference to 0x6000.69 */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex70 - Reference to 0x6000.70 */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex71 - Reference to 0x6000.71 */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex72 - Reference to 0x6000.72 */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex73 - Reference to 0x6000.73 */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex74 - Reference to 0x6000.74 */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex75 - Reference to 0x6000.75 */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex76 - Reference to 0x6000.76 */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex77 - Reference to 0x6000.77 */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex78 - Reference to 0x6000.78 */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex79 - Reference to 0x6000.79 */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex80 - Reference to 0x6000.80 */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex81 - Reference to 0x6000.81 */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex82 - Reference to 0x6000.82 */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex83 - Reference to 0x6000.83 */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex84 - Reference to 0x6000.84 */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex85 - Reference to 0x6000.85 */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex86 - Reference to 0x6000.86 */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex87 - Reference to 0x6000.87 */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex88 - Reference to 0x6000.88 */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex89 - Reference to 0x6000.89 */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex90 - Reference to 0x6000.90 */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex91 - Reference to 0x6000.91 */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex92 - Reference to 0x6000.92 */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex93 - Reference to 0x6000.93 */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex94 - Reference to 0x6000.94 */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex95 - Reference to 0x6000.95 */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex96 - Reference to 0x6000.96 */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex97 - Reference to 0x6000.97 */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex98 - Reference to 0x6000.98 */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex99 - Reference to 0x6000.99 */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex100 - Reference to 0x6000.100 */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex101 - Reference to 0x6000.101 */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex102 - Reference to 0x6000.102 */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex103 - Reference to 0x6000.103 */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex104 - Reference to 0x6000.104 */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex105 - Reference to 0x6000.105 */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex106 - Reference to 0x6000.106 */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex107 - Reference to 0x6000.107 */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex108 - Reference to 0x6000.108 */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex109 - Reference to 0x6000.109 */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex110 - Reference to 0x6000.110 */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex111 - Reference to 0x6000.111 */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex112 - Reference to 0x6000.112 */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex113 - Reference to 0x6000.113 */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex114 - Reference to 0x6000.114 */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex115 - Reference to 0x6000.115 */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex116 - Reference to 0x6000.116 */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex117 - Reference to 0x6000.117 */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex118 - Reference to 0x6000.118 */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex119 - Reference to 0x6000.119 */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex120 - Reference to 0x6000.120 */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex121 - Reference to 0x6000.121 */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex122 - Reference to 0x6000.122 */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex123 - Reference to 0x6000.123 */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex124 - Reference to 0x6000.124 */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex125 - Reference to 0x6000.125 */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex126 - Reference to 0x6000.126 */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex127 - Reference to 0x6000.127 */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex128 - Reference to 0x6000.128 */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex129 - Reference to 0x6000.129 */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex130 - Reference to 0x6000.130 */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex131 - Reference to 0x6000.131 */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex132 - Reference to 0x6000.132 */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex133 - Reference to 0x6000.133 */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex134 - Reference to 0x6000.134 */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex135 - Reference to 0x6000.135 */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex136 - Reference to 0x6000.136 */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex137 - Reference to 0x6000.137 */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex138 - Reference to 0x6000.138 */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex139 - Reference to 0x6000.139 */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex140 - Reference to 0x6000.140 */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex141 - Reference to 0x6000.141 */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex142 - Reference to 0x6000.142 */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex143 - Reference to 0x6000.143 */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex144 - Reference to 0x6000.144 */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex145 - Reference to 0x6000.145 */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex146 - Reference to 0x6000.146 */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex147 - Reference to 0x6000.147 */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex148 - Reference to 0x6000.148 */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex149 - Reference to 0x6000.149 */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex150 - Reference to 0x6000.150 */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex151 - Reference to 0x6000.151 */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex152 - Reference to 0x6000.152 */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex153 - Reference to 0x6000.153 */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex154 - Reference to 0x6000.154 */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex155 - Reference to 0x6000.155 */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex156 - Reference to 0x6000.156 */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex157 - Reference to 0x6000.157 */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex158 - Reference to 0x6000.158 */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex159 - Reference to 0x6000.159 */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex160 - Reference to 0x6000.160 */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex161 - Reference to 0x6000.161 */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex162 - Reference to 0x6000.162 */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex163 - Reference to 0x6000.163 */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex164 - Reference to 0x6000.164 */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex165 - Reference to 0x6000.165 */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex166 - Reference to 0x6000.166 */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex167 - Reference to 0x6000.167 */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex168 - Reference to 0x6000.168 */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex169 - Reference to 0x6000.169 */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex170 - Reference to 0x6000.170 */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex171 - Reference to 0x6000.171 */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex172 - Reference to 0x6000.172 */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex173 - Reference to 0x6000.173 */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex174 - Reference to 0x6000.174 */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex175 - Reference to 0x6000.175 */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex176 - Reference to 0x6000.176 */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex177 - Reference to 0x6000.177 */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex178 - Reference to 0x6000.178 */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex179 - Reference to 0x6000.179 */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex180 - Reference to 0x6000.180 */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex181 - Reference to 0x6000.181 */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex182 - Reference to 0x6000.182 */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex183 - Reference to 0x6000.183 */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex184 - Reference to 0x6000.184 */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex185 - Reference to 0x6000.185 */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex186 - Reference to 0x6000.186 */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex187 - Reference to 0x6000.187 */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex188 - Reference to 0x6000.188 */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex189 - Reference to 0x6000.189 */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex190 - Reference to 0x6000.190 */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex191 - Reference to 0x6000.191 */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex192 - Reference to 0x6000.192 */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex193 - Reference to 0x6000.193 */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex194 - Reference to 0x6000.194 */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex195 - Reference to 0x6000.195 */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex196 - Reference to 0x6000.196 */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex197 - Reference to 0x6000.197 */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex198 - Reference to 0x6000.198 */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex199 - Reference to 0x6000.199 */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex200 - Reference to 0x6000.200 */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex201 - Reference to 0x6000.201 */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex202 - Reference to 0x6000.202 */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex203 - Reference to 0x6000.203 */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex204 - Reference to 0x6000.204 */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex205 - Reference to 0x6000.205 */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex206 - Reference to 0x6000.206 */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex207 - Reference to 0x6000.207 */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex208 - Reference to 0x6000.208 */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex209 - Reference to 0x6000.209 */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex210 - Reference to 0x6000.210 */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex211 - Reference to 0x6000.211 */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex212 - Reference to 0x6000.212 */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex213 - Reference to 0x6000.213 */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex214 - Reference to 0x6000.214 */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex215 - Reference to 0x6000.215 */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex216 - Reference to 0x6000.216 */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex217 - Reference to 0x6000.217 */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex218 - Reference to 0x6000.218 */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex219 - Reference to 0x6000.219 */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex220 - Reference to 0x6000.220 */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex221 - Reference to 0x6000.221 */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex222 - Reference to 0x6000.222 */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex223 - Reference to 0x6000.223 */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex224 - Reference to 0x6000.224 */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex225 - Reference to 0x6000.225 */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex226 - Reference to 0x6000.226 */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex227 - Reference to 0x6000.227 */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex228 - Reference to 0x6000.228 */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex229 - Reference to 0x6000.229 */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex230 - Reference to 0x6000.230 */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex231 - Reference to 0x6000.231 */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex232 - Reference to 0x6000.232 */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex233 - Reference to 0x6000.233 */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex234 - Reference to 0x6000.234 */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex235 - Reference to 0x6000.235 */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex236 - Reference to 0x6000.236 */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex237 - Reference to 0x6000.237 */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex238 - Reference to 0x6000.238 */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex239 - Reference to 0x6000.239 */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex240 - Reference to 0x6000.240 */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex241 - Reference to 0x6000.241 */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex242 - Reference to 0x6000.242 */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex243 - Reference to 0x6000.243 */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex244 - Reference to 0x6000.244 */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex245 - Reference to 0x6000.245 */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex246 - Reference to 0x6000.246 */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex247 - Reference to 0x6000.247 */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex248 - Reference to 0x6000.248 */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex249 - Reference to 0x6000.249 */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex250 - Reference to 0x6000.250 */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex251 - Reference to 0x6000.251 */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex252 - Reference to 0x6000.252 */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }}; /* Subindex253 - Reference to 0x6000.253 */

/**
* \brief Object/Entry names
*/
OBJCONST UCHAR OBJMEM aName0x1A00[] = "InputData process data mapping\000"
"SubIndex 001\000"
"SubIndex 002\000"
"SubIndex 003\000"
"SubIndex 004\000"
"SubIndex 005\000"
"SubIndex 006\000"
"SubIndex 007\000"
"SubIndex 008\000"
"SubIndex 009\000"
"SubIndex 010\000"
"SubIndex 011\000"
"SubIndex 012\000"
"SubIndex 013\000"
"SubIndex 014\000"
"SubIndex 015\000"
"SubIndex 016\000"
"SubIndex 017\000"
"SubIndex 018\000"
"SubIndex 019\000"
"SubIndex 020\000"
"SubIndex 021\000"
"SubIndex 022\000"
"SubIndex 023\000"
"SubIndex 024\000"
"SubIndex 025\000"
"SubIndex 026\000"
"SubIndex 027\000"
"SubIndex 028\000"
"SubIndex 029\000"
"SubIndex 030\000"
"SubIndex 031\000"
"SubIndex 032\000"
"SubIndex 033\000"
"SubIndex 034\000"
"SubIndex 035\000"
"SubIndex 036\000"
"SubIndex 037\000"
"SubIndex 038\000"
"SubIndex 039\000"
"SubIndex 040\000"
"SubIndex 041\000"
"SubIndex 042\000"
"SubIndex 043\000"
"SubIndex 044\000"
"SubIndex 045\000"
"SubIndex 046\000"
"SubIndex 047\000"
"SubIndex 048\000"
"SubIndex 049\000"
"SubIndex 050\000"
"SubIndex 051\000"
"SubIndex 052\000"
"SubIndex 053\000"
"SubIndex 054\000"
"SubIndex 055\000"
"SubIndex 056\000"
"SubIndex 057\000"
"SubIndex 058\000"
"SubIndex 059\000"
"SubIndex 060\000"
"SubIndex 061\000"
"SubIndex 062\000"
"SubIndex 063\000"
"SubIndex 064\000"
"SubIndex 065\000"
"SubIndex 066\000"
"SubIndex 067\000"
"SubIndex 068\000"
"SubIndex 069\000"
"SubIndex 070\000"
"SubIndex 071\000"
"SubIndex 072\000"
"SubIndex 073\000"
"SubIndex 074\000"
"SubIndex 075\000"
"SubIndex 076\000"
"SubIndex 077\000"
"SubIndex 078\000"
"SubIndex 079\000"
"SubIndex 080\000"
"SubIndex 081\000"
"SubIndex 082\000"
"SubIndex 083\000"
"SubIndex 084\000"
"SubIndex 085\000"
"SubIndex 086\000"
"SubIndex 087\000"
"SubIndex 088\000"
"SubIndex 089\000"
"SubIndex 090\000"
"SubIndex 091\000"
"SubIndex 092\000"
"SubIndex 093\000"
"SubIndex 094\000"
"SubIndex 095\000"
"SubIndex 096\000"
"SubIndex 097\000"
"SubIndex 098\000"
"SubIndex 099\000"
"SubIndex 100\000"
"SubIndex 101\000"
"SubIndex 102\000"
"SubIndex 103\000"
"SubIndex 104\000"
"SubIndex 105\000"
"SubIndex 106\000"
"SubIndex 107\000"
"SubIndex 108\000"
"SubIndex 109\000"
"SubIndex 110\000"
"SubIndex 111\000"
"SubIndex 112\000"
"SubIndex 113\000"
"SubIndex 114\000"
"SubIndex 115\000"
"SubIndex 116\000"
"SubIndex 117\000"
"SubIndex 118\000"
"SubIndex 119\000"
"SubIndex 120\000"
"SubIndex 121\000"
"SubIndex 122\000"
"SubIndex 123\000"
"SubIndex 124\000"
"SubIndex 125\000"
"SubIndex 126\000"
"SubIndex 127\000"
"SubIndex 128\000"
"SubIndex 129\000"
"SubIndex 130\000"
"SubIndex 131\000"
"SubIndex 132\000"
"SubIndex 133\000"
"SubIndex 134\000"
"SubIndex 135\000"
"SubIndex 136\000"
"SubIndex 137\000"
"SubIndex 138\000"
"SubIndex 139\000"
"SubIndex 140\000"
"SubIndex 141\000"
"SubIndex 142\000"
"SubIndex 143\000"
"SubIndex 144\000"
"SubIndex 145\000"
"SubIndex 146\000"
"SubIndex 147\000"
"SubIndex 148\000"
"SubIndex 149\000"
"SubIndex 150\000"
"SubIndex 151\000"
"SubIndex 152\000"
"SubIndex 153\000"
"SubIndex 154\000"
"SubIndex 155\000"
"SubIndex 156\000"
"SubIndex 157\000"
"SubIndex 158\000"
"SubIndex 159\000"
"SubIndex 160\000"
"SubIndex 161\000"
"SubIndex 162\000"
"SubIndex 163\000"
"SubIndex 164\000"
"SubIndex 165\000"
"SubIndex 166\000"
"SubIndex 167\000"
"SubIndex 168\000"
"SubIndex 169\000"
"SubIndex 170\000"
"SubIndex 171\000"
"SubIndex 172\000"
"SubIndex 173\000"
"SubIndex 174\000"
"SubIndex 175\000"
"SubIndex 176\000"
"SubIndex 177\000"
"SubIndex 178\000"
"SubIndex 179\000"
"SubIndex 180\000"
"SubIndex 181\000"
"SubIndex 182\000"
"SubIndex 183\000"
"SubIndex 184\000"
"SubIndex 185\000"
"SubIndex 186\000"
"SubIndex 187\000"
"SubIndex 188\000"
"SubIndex 189\000"
"SubIndex 190\000"
"SubIndex 191\000"
"SubIndex 192\000"
"SubIndex 193\000"
"SubIndex 194\000"
"SubIndex 195\000"
"SubIndex 196\000"
"SubIndex 197\000"
"SubIndex 198\000"
"SubIndex 199\000"
"SubIndex 200\000"
"SubIndex 201\000"
"SubIndex 202\000"
"SubIndex 203\000"
"SubIndex 204\000"
"SubIndex 205\000"
"SubIndex 206\000"
"SubIndex 207\000"
"SubIndex 208\000"
"SubIndex 209\000"
"SubIndex 210\000"
"SubIndex 211\000"
"SubIndex 212\000"
"SubIndex 213\000"
"SubIndex 214\000"
"SubIndex 215\000"
"SubIndex 216\000"
"SubIndex 217\000"
"SubIndex 218\000"
"SubIndex 219\000"
"SubIndex 220\000"
"SubIndex 221\000"
"SubIndex 222\000"
"SubIndex 223\000"
"SubIndex 224\000"
"SubIndex 225\000"
"SubIndex 226\000"
"SubIndex 227\000"
"SubIndex 228\000"
"SubIndex 229\000"
"SubIndex 230\000"
"SubIndex 231\000"
"SubIndex 232\000"
"SubIndex 233\000"
"SubIndex 234\000"
"SubIndex 235\000"
"SubIndex 236\000"
"SubIndex 237\000"
"SubIndex 238\000"
"SubIndex 239\000"
"SubIndex 240\000"
"SubIndex 241\000"
"SubIndex 242\000"
"SubIndex 243\000"
"SubIndex 244\000"
"SubIndex 245\000"
"SubIndex 246\000"
"SubIndex 247\000"
"SubIndex 248\000"
"SubIndex 249\000"
"SubIndex 250\000"
"SubIndex 251\000"
"SubIndex 252\000"
"SubIndex 253\000\377";
#endif //#ifdef _OBJD_

#ifndef _LAN9252_APP_OBJECTS_H_
/**
* \brief Object structure
*/
typedef struct OBJ_STRUCT_PACKED_START {
UINT16 u16SubIndex0;
UINT32 SI1; /* Subindex1 - Reference to 0x6000.1 */
UINT32 SI2; /* Subindex2 - Reference to 0x6000.2 */
UINT32 SI3; /* Subindex3 - Reference to 0x6000.3 */
UINT32 SI4; /* Subindex4 - Reference to 0x6000.4 */
UINT32 SI5; /* Subindex5 - Reference to 0x6000.5 */
UINT32 SI6; /* Subindex6 - Reference to 0x6000.6 */
UINT32 SI7; /* Subindex7 - Reference to 0x6000.7 */
UINT32 SI8; /* Subindex8 - Reference to 0x6000.8 */
UINT32 SI9; /* Subindex9 - Reference to 0x6000.9 */
UINT32 SI10; /* Subindex10 - Reference to 0x6000.10 */
UINT32 SI11; /* Subindex11 - Reference to 0x6000.11 */
UINT32 SI12; /* Subindex12 - Reference to 0x6000.12 */
UINT32 SI13; /* Subindex13 - Reference to 0x6000.13 */
UINT32 SI14; /* Subindex14 - Reference to 0x6000.14 */
UINT32 SI15; /* Subindex15 - Reference to 0x6000.15 */
UINT32 SI16; /* Subindex16 - Reference to 0x6000.16 */
UINT32 SI17; /* Subindex17 - Reference to 0x6000.17 */
UINT32 SI18; /* Subindex18 - Reference to 0x6000.18 */
UINT32 SI19; /* Subindex19 - Reference to 0x6000.19 */
UINT32 SI20; /* Subindex20 - Reference to 0x6000.20 */
UINT32 SI21; /* Subindex21 - Reference to 0x6000.21 */
UINT32 SI22; /* Subindex22 - Reference to 0x6000.22 */
UINT32 SI23; /* Subindex23 - Reference to 0x6000.23 */
UINT32 SI24; /* Subindex24 - Reference to 0x6000.24 */
UINT32 SI25; /* Subindex25 - Reference to 0x6000.25 */
UINT32 SI26; /* Subindex26 - Reference to 0x6000.26 */
UINT32 SI27; /* Subindex27 - Reference to 0x6000.27 */
UINT32 SI28; /* Subindex28 - Reference to 0x6000.28 */
UINT32 SI29; /* Subindex29 - Reference to 0x6000.29 */
UINT32 SI30; /* Subindex30 - Reference to 0x6000.30 */
UINT32 SI31; /* Subindex31 - Reference to 0x6000.31 */
UINT32 SI32; /* Subindex32 - Reference to 0x6000.32 */
UINT32 SI33; /* Subindex33 - Reference to 0x6000.33 */
UINT32 SI34; /* Subindex34 - Reference to 0x6000.34 */
UINT32 SI35; /* Subindex35 - Reference to 0x6000.35 */
UINT32 SI36; /* Subindex36 - Reference to 0x6000.36 */
UINT32 SI37; /* Subindex37 - Reference to 0x6000.37 */
UINT32 SI38; /* Subindex38 - Reference to 0x6000.38 */
UINT32 SI39; /* Subindex39 - Reference to 0x6000.39 */
UINT32 SI40; /* Subindex40 - Reference to 0x6000.40 */
UINT32 SI41; /* Subindex41 - Reference to 0x6000.41 */
UINT32 SI42; /* Subindex42 - Reference to 0x6000.42 */
UINT32 SI43; /* Subindex43 - Reference to 0x6000.43 */
UINT32 SI44; /* Subindex44 - Reference to 0x6000.44 */
UINT32 SI45; /* Subindex45 - Reference to 0x6000.45 */
UINT32 SI46; /* Subindex46 - Reference to 0x6000.46 */
UINT32 SI47; /* Subindex47 - Reference to 0x6000.47 */
UINT32 SI48; /* Subindex48 - Reference to 0x6000.48 */
UINT32 SI49; /* Subindex49 - Reference to 0x6000.49 */
UINT32 SI50; /* Subindex50 - Reference to 0x6000.50 */
UINT32 SI51; /* Subindex51 - Reference to 0x6000.51 */
UINT32 SI52; /* Subindex52 - Reference to 0x6000.52 */
UINT32 SI53; /* Subindex53 - Reference to 0x6000.53 */
UINT32 SI54; /* Subindex54 - Reference to 0x6000.54 */
UINT32 SI55; /* Subindex55 - Reference to 0x6000.55 */
UINT32 SI56; /* Subindex56 - Reference to 0x6000.56 */
UINT32 SI57; /* Subindex57 - Reference to 0x6000.57 */
UINT32 SI58; /* Subindex58 - Reference to 0x6000.58 */
UINT32 SI59; /* Subindex59 - Reference to 0x6000.59 */
UINT32 SI60; /* Subindex60 - Reference to 0x6000.60 */
UINT32 SI61; /* Subindex61 - Reference to 0x6000.61 */
UINT32 SI62; /* Subindex62 - Reference to 0x6000.62 */
UINT32 SI63; /* Subindex63 - Reference to 0x6000.63 */
UINT32 SI64; /* Subindex64 - Reference to 0x6000.64 */
UINT32 SI65; /* Subindex65 - Reference to 0x6000.65 */
UINT32 SI66; /* Subindex66 - Reference to 0x6000.66 */
UINT32 SI67; /* Subindex67 - Reference to 0x6000.67 */
UINT32 SI68; /* Subindex68 - Reference to 0x6000.68 */
UINT32 SI69; /* Subindex69 - Reference to 0x6000.69 */
UINT32 SI70; /* Subindex70 - Reference to 0x6000.70 */
UINT32 SI71; /* Subindex71 - Reference to 0x6000.71 */
UINT32 SI72; /* Subindex72 - Reference to 0x6000.72 */
UINT32 SI73; /* Subindex73 - Reference to 0x6000.73 */
UINT32 SI74; /* Subindex74 - Reference to 0x6000.74 */
UINT32 SI75; /* Subindex75 - Reference to 0x6000.75 */
UINT32 SI76; /* Subindex76 - Reference to 0x6000.76 */
UINT32 SI77; /* Subindex77 - Reference to 0x6000.77 */
UINT32 SI78; /* Subindex78 - Reference to 0x6000.78 */
UINT32 SI79; /* Subindex79 - Reference to 0x6000.79 */
UINT32 SI80; /* Subindex80 - Reference to 0x6000.80 */
UINT32 SI81; /* Subindex81 - Reference to 0x6000.81 */
UINT32 SI82; /* Subindex82 - Reference to 0x6000.82 */
UINT32 SI83; /* Subindex83 - Reference to 0x6000.83 */
UINT32 SI84; /* Subindex84 - Reference to 0x6000.84 */
UINT32 SI85; /* Subindex85 - Reference to 0x6000.85 */
UINT32 SI86; /* Subindex86 - Reference to 0x6000.86 */
UINT32 SI87; /* Subindex87 - Reference to 0x6000.87 */
UINT32 SI88; /* Subindex88 - Reference to 0x6000.88 */
UINT32 SI89; /* Subindex89 - Reference to 0x6000.89 */
UINT32 SI90; /* Subindex90 - Reference to 0x6000.90 */
UINT32 SI91; /* Subindex91 - Reference to 0x6000.91 */
UINT32 SI92; /* Subindex92 - Reference to 0x6000.92 */
UINT32 SI93; /* Subindex93 - Reference to 0x6000.93 */
UINT32 SI94; /* Subindex94 - Reference to 0x6000.94 */
UINT32 SI95; /* Subindex95 - Reference to 0x6000.95 */
UINT32 SI96; /* Subindex96 - Reference to 0x6000.96 */
UINT32 SI97; /* Subindex97 - Reference to 0x6000.97 */
UINT32 SI98; /* Subindex98 - Reference to 0x6000.98 */
UINT32 SI99; /* Subindex99 - Reference to 0x6000.99 */
UINT32 SI100; /* Subindex100 - Reference to 0x6000.100 */
UINT32 SI101; /* Subindex101 - Reference to 0x6000.101 */
UINT32 SI102; /* Subindex102 - Reference to 0x6000.102 */
UINT32 SI103; /* Subindex103 - Reference to 0x6000.103 */
UINT32 SI104; /* Subindex104 - Reference to 0x6000.104 */
UINT32 SI105; /* Subindex105 - Reference to 0x6000.105 */
UINT32 SI106; /* Subindex106 - Reference to 0x6000.106 */
UINT32 SI107; /* Subindex107 - Reference to 0x6000.107 */
UINT32 SI108; /* Subindex108 - Reference to 0x6000.108 */
UINT32 SI109; /* Subindex109 - Reference to 0x6000.109 */
UINT32 SI110; /* Subindex110 - Reference to 0x6000.110 */
UINT32 SI111; /* Subindex111 - Reference to 0x6000.111 */
UINT32 SI112; /* Subindex112 - Reference to 0x6000.112 */
UINT32 SI113; /* Subindex113 - Reference to 0x6000.113 */
UINT32 SI114; /* Subindex114 - Reference to 0x6000.114 */
UINT32 SI115; /* Subindex115 - Reference to 0x6000.115 */
UINT32 SI116; /* Subindex116 - Reference to 0x6000.116 */
UINT32 SI117; /* Subindex117 - Reference to 0x6000.117 */
UINT32 SI118; /* Subindex118 - Reference to 0x6000.118 */
UINT32 SI119; /* Subindex119 - Reference to 0x6000.119 */
UINT32 SI120; /* Subindex120 - Reference to 0x6000.120 */
UINT32 SI121; /* Subindex121 - Reference to 0x6000.121 */
UINT32 SI122; /* Subindex122 - Reference to 0x6000.122 */
UINT32 SI123; /* Subindex123 - Reference to 0x6000.123 */
UINT32 SI124; /* Subindex124 - Reference to 0x6000.124 */
UINT32 SI125; /* Subindex125 - Reference to 0x6000.125 */
UINT32 SI126; /* Subindex126 - Reference to 0x6000.126 */
UINT32 SI127; /* Subindex127 - Reference to 0x6000.127 */
UINT32 SI128; /* Subindex128 - Reference to 0x6000.128 */
UINT32 SI129; /* Subindex129 - Reference to 0x6000.129 */
UINT32 SI130; /* Subindex130 - Reference to 0x6000.130 */
UINT32 SI131; /* Subindex131 - Reference to 0x6000.131 */
UINT32 SI132; /* Subindex132 - Reference to 0x6000.132 */
UINT32 SI133; /* Subindex133 - Reference to 0x6000.133 */
UINT32 SI134; /* Subindex134 - Reference to 0x6000.134 */
UINT32 SI135; /* Subindex135 - Reference to 0x6000.135 */
UINT32 SI136; /* Subindex136 - Reference to 0x6000.136 */
UINT32 SI137; /* Subindex137 - Reference to 0x6000.137 */
UINT32 SI138; /* Subindex138 - Reference to 0x6000.138 */
UINT32 SI139; /* Subindex139 - Reference to 0x6000.139 */
UINT32 SI140; /* Subindex140 - Reference to 0x6000.140 */
UINT32 SI141; /* Subindex141 - Reference to 0x6000.141 */
UINT32 SI142; /* Subindex142 - Reference to 0x6000.142 */
UINT32 SI143; /* Subindex143 - Reference to 0x6000.143 */
UINT32 SI144; /* Subindex144 - Reference to 0x6000.144 */
UINT32 SI145; /* Subindex145 - Reference to 0x6000.145 */
UINT32 SI146; /* Subindex146 - Reference to 0x6000.146 */
UINT32 SI147; /* Subindex147 - Reference to 0x6000.147 */
UINT32 SI148; /* Subindex148 - Reference to 0x6000.148 */
UINT32 SI149; /* Subindex149 - Reference to 0x6000.149 */
UINT32 SI150; /* Subindex150 - Reference to 0x6000.150 */
UINT32 SI151; /* Subindex151 - Reference to 0x6000.151 */
UINT32 SI152; /* Subindex152 - Reference to 0x6000.152 */
UINT32 SI153; /* Subindex153 - Reference to 0x6000.153 */
UINT32 SI154; /* Subindex154 - Reference to 0x6000.154 */
UINT32 SI155; /* Subindex155 - Reference to 0x6000.155 */
UINT32 SI156; /* Subindex156 - Reference to 0x6000.156 */
UINT32 SI157; /* Subindex157 - Reference to 0x6000.157 */
UINT32 SI158; /* Subindex158 - Reference to 0x6000.158 */
UINT32 SI159; /* Subindex159 - Reference to 0x6000.159 */
UINT32 SI160; /* Subindex160 - Reference to 0x6000.160 */
UINT32 SI161; /* Subindex161 - Reference to 0x6000.161 */
UINT32 SI162; /* Subindex162 - Reference to 0x6000.162 */
UINT32 SI163; /* Subindex163 - Reference to 0x6000.163 */
UINT32 SI164; /* Subindex164 - Reference to 0x6000.164 */
UINT32 SI165; /* Subindex165 - Reference to 0x6000.165 */
UINT32 SI166; /* Subindex166 - Reference to 0x6000.166 */
UINT32 SI167; /* Subindex167 - Reference to 0x6000.167 */
UINT32 SI168; /* Subindex168 - Reference to 0x6000.168 */
UINT32 SI169; /* Subindex169 - Reference to 0x6000.169 */
UINT32 SI170; /* Subindex170 - Reference to 0x6000.170 */
UINT32 SI171; /* Subindex171 - Reference to 0x6000.171 */
UINT32 SI172; /* Subindex172 - Reference to 0x6000.172 */
UINT32 SI173; /* Subindex173 - Reference to 0x6000.173 */
UINT32 SI174; /* Subindex174 - Reference to 0x6000.174 */
UINT32 SI175; /* Subindex175 - Reference to 0x6000.175 */
UINT32 SI176; /* Subindex176 - Reference to 0x6000.176 */
UINT32 SI177; /* Subindex177 - Reference to 0x6000.177 */
UINT32 SI178; /* Subindex178 - Reference to 0x6000.178 */
UINT32 SI179; /* Subindex179 - Reference to 0x6000.179 */
UINT32 SI180; /* Subindex180 - Reference to 0x6000.180 */
UINT32 SI181; /* Subindex181 - Reference to 0x6000.181 */
UINT32 SI182; /* Subindex182 - Reference to 0x6000.182 */
UINT32 SI183; /* Subindex183 - Reference to 0x6000.183 */
UINT32 SI184; /* Subindex184 - Reference to 0x6000.184 */
UINT32 SI185; /* Subindex185 - Reference to 0x6000.185 */
UINT32 SI186; /* Subindex186 - Reference to 0x6000.186 */
UINT32 SI187; /* Subindex187 - Reference to 0x6000.187 */
UINT32 SI188; /* Subindex188 - Reference to 0x6000.188 */
UINT32 SI189; /* Subindex189 - Reference to 0x6000.189 */
UINT32 SI190; /* Subindex190 - Reference to 0x6000.190 */
UINT32 SI191; /* Subindex191 - Reference to 0x6000.191 */
UINT32 SI192; /* Subindex192 - Reference to 0x6000.192 */
UINT32 SI193; /* Subindex193 - Reference to 0x6000.193 */
UINT32 SI194; /* Subindex194 - Reference to 0x6000.194 */
UINT32 SI195; /* Subindex195 - Reference to 0x6000.195 */
UINT32 SI196; /* Subindex196 - Reference to 0x6000.196 */
UINT32 SI197; /* Subindex197 - Reference to 0x6000.197 */
UINT32 SI198; /* Subindex198 - Reference to 0x6000.198 */
UINT32 SI199; /* Subindex199 - Reference to 0x6000.199 */
UINT32 SI200; /* Subindex200 - Reference to 0x6000.200 */
UINT32 SI201; /* Subindex201 - Reference to 0x6000.201 */
UINT32 SI202; /* Subindex202 - Reference to 0x6000.202 */
UINT32 SI203; /* Subindex203 - Reference to 0x6000.203 */
UINT32 SI204; /* Subindex204 - Reference to 0x6000.204 */
UINT32 SI205; /* Subindex205 - Reference to 0x6000.205 */
UINT32 SI206; /* Subindex206 - Reference to 0x6000.206 */
UINT32 SI207; /* Subindex207 - Reference to 0x6000.207 */
UINT32 SI208; /* Subindex208 - Reference to 0x6000.208 */
UINT32 SI209; /* Subindex209 - Reference to 0x6000.209 */
UINT32 SI210; /* Subindex210 - Reference to 0x6000.210 */
UINT32 SI211; /* Subindex211 - Reference to 0x6000.211 */
UINT32 SI212; /* Subindex212 - Reference to 0x6000.212 */
UINT32 SI213; /* Subindex213 - Reference to 0x6000.213 */
UINT32 SI214; /* Subindex214 - Reference to 0x6000.214 */
UINT32 SI215; /* Subindex215 - Reference to 0x6000.215 */
UINT32 SI216; /* Subindex216 - Reference to 0x6000.216 */
UINT32 SI217; /* Subindex217 - Reference to 0x6000.217 */
UINT32 SI218; /* Subindex218 - Reference to 0x6000.218 */
UINT32 SI219; /* Subindex219 - Reference to 0x6000.219 */
UINT32 SI220; /* Subindex220 - Reference to 0x6000.220 */
UINT32 SI221; /* Subindex221 - Reference to 0x6000.221 */
UINT32 SI222; /* Subindex222 - Reference to 0x6000.222 */
UINT32 SI223; /* Subindex223 - Reference to 0x6000.223 */
UINT32 SI224; /* Subindex224 - Reference to 0x6000.224 */
UINT32 SI225; /* Subindex225 - Reference to 0x6000.225 */
UINT32 SI226; /* Subindex226 - Reference to 0x6000.226 */
UINT32 SI227; /* Subindex227 - Reference to 0x6000.227 */
UINT32 SI228; /* Subindex228 - Reference to 0x6000.228 */
UINT32 SI229; /* Subindex229 - Reference to 0x6000.229 */
UINT32 SI230; /* Subindex230 - Reference to 0x6000.230 */
UINT32 SI231; /* Subindex231 - Reference to 0x6000.231 */
UINT32 SI232; /* Subindex232 - Reference to 0x6000.232 */
UINT32 SI233; /* Subindex233 - Reference to 0x6000.233 */
UINT32 SI234; /* Subindex234 - Reference to 0x6000.234 */
UINT32 SI235; /* Subindex235 - Reference to 0x6000.235 */
UINT32 SI236; /* Subindex236 - Reference to 0x6000.236 */
UINT32 SI237; /* Subindex237 - Reference to 0x6000.237 */
UINT32 SI238; /* Subindex238 - Reference to 0x6000.238 */
UINT32 SI239; /* Subindex239 - Reference to 0x6000.239 */
UINT32 SI240; /* Subindex240 - Reference to 0x6000.240 */
UINT32 SI241; /* Subindex241 - Reference to 0x6000.241 */
UINT32 SI242; /* Subindex242 - Reference to 0x6000.242 */
UINT32 SI243; /* Subindex243 - Reference to 0x6000.243 */
UINT32 SI244; /* Subindex244 - Reference to 0x6000.244 */
UINT32 SI245; /* Subindex245 - Reference to 0x6000.245 */
UINT32 SI246; /* Subindex246 - Reference to 0x6000.246 */
UINT32 SI247; /* Subindex247 - Reference to 0x6000.247 */
UINT32 SI248; /* Subindex248 - Reference to 0x6000.248 */
UINT32 SI249; /* Subindex249 - Reference to 0x6000.249 */
UINT32 SI250; /* Subindex250 - Reference to 0x6000.250 */
UINT32 SI251; /* Subindex251 - Reference to 0x6000.251 */
UINT32 SI252; /* Subindex252 - Reference to 0x6000.252 */
UINT32 SI253; /* Subindex253 - Reference to 0x6000.253 */
} OBJ_STRUCT_PACKED_END
TOBJ1A00;
#endif //#ifndef _LAN9252_APP_OBJECTS_H_

/**
* \brief Object variable
*/
PROTO TOBJ1A00 InputDataProcessDataMapping0x1A00
#if defined(_LAN9252_APP_) && (_LAN9252_APP_ == 1)
={253,0x60000108,0x60000208,0x60000320,0x60000408,0x60000508,0x60000620,0x60000708,0x60000808,0x60000920,0x60000A08,0x60000B08,0x60000C20,0x60000D08,0x60000E08,0x60000F20,0x60001008,0x60001108,0x60001220,0x60001308,0x60001408,0x60001520,0x60001608,0x60001708,0x60001820,0x60001908,0x60001A08,0x60001B20,0x60001C08,0x60001D08,0x60001E20,0x60001F08,0x60002008,0x60002120,0x60002208,0x60002308,0x60002420,0x60002508,0x60002608,0x60002720,0x60002808,0x60002908,0x60002A10,0x60002B08,0x60002C08,0x60002D08,0x60002E08,0x60002F08,0x60003008,0x60003110,0x60003220,0x60003320,0x60003420,0x60003520,0x60003610,0x60003710,0x60003810,0x60003901,0x60003A01,0x60003B01,0x60003C01,0x60003D01,0x60003E01,0x60003F01,0x60004001,0x60004101,0x60004201,0x60004301,0x60004401,0x60004504,0x60004601,0x60004701,0x60004801,0x60004901,0x60004A01,0x60004B01,0x60004C01,0x60004D01,0x60004E08,0x60004F08,0x60005001,0x60005101,0x60005201,0x60005301,0x60005401,0x60005501,0x60005601,0x60005701,0x60005801,0x60005901,0x60005A01,0x60005B01,0x60005C01,0x60005D01,0x60005E01,0x60005F01,0x60006001,0x60006101,0x60006201,0x60006301,0x60006401,0x60006501,0x60006601,0x60006701,0x60006801,0x60006901,0x60006A01,0x60006B01,0x60006C01,0x60006D01,0x60006E01,0x60006F01,0x60007001,0x60007101,0x60007201,0x60007301,0x60007401,0x60007501,0x60007601,0x60007701,0x60007810,0x60007901,0x60007A07,0x60007B08,0x60007C01,0x60007D01,0x60007E01,0x60007F01,0x60008001,0x60008101,0x60008201,0x60008301,0x60008401,0x60008507,0x60008610,0x60008701,0x60008801,0x60008901,0x60008A01,0x60008B01,0x60008C01,0x60008D01,0x60008E01,0x60008F08,0x60009008,0x60009108,0x60009210,0x60009308,0x60009408,0x60009510,0x60009620,0x60009720,0x60009820,0x60009920,0x60009A10,0x60009B10,0x60009C10,0x60009D08,0x60009E08,0x60009F10,0x6000A008,0x6000A108,0x6000A210,0x6000A320,0x6000A420,0x6000A520,0x6000A620,0x6000A710,0x6000A810,0x6000A910,0x6000AA20,0x6000AB20,0x6000AC20,0x6000AD08,0x6000AE08,0x6000AF20,0x6000B020,0x6000B120,0x6000B208,0x6000B308,0x6000B420,0x6000B520,0x6000B620,0x6000B708,0x6000B808,0x6000B910,0x6000BA08,0x6000BB08,0x6000BC10,0x6000BD20,0x6000BE20,0x6000BF20,0x6000C020,0x6000C110,0x6000C210,0x6000C310,0x6000C408,0x6000C508,0x6000C610,0x6000C708,0x6000C808,0x6000C910,0x6000CA20,0x6000CB20,0x6000CC20,0x6000CD20,0x6000CE10,0x6000CF10,0x6000D010,0x6000D120,0x6000D208,0x6000D308,0x6000D410,0x6000D508,0x6000D608,0x6000D710,0x6000D820,0x6000D920,0x6000DA20,0x6000DB20,0x6000DC10,0x6000DD10,0x6000DE10,0x6000DF20,0x6000E020,0x6000E120,0x6000E208,0x6000E308,0x6000E410,0x6000E508,0x6000E608,0x6000E710,0x6000E820,0x6000E920,0x6000EA20,0x6000EB20,0x6000EC10,0x6000ED10,0x6000EE10,0x6000EF20,0x6000F020,0x6000F120,0x6000F220,0x6000F320,0x6000F420,0x6000F520,0x6000F620,0x6000F720,0x6000F820,0x6000F920,0x6000FA20,0x6000FB20,0x6000FC08,0x6000FD08}
#endif
;
/** @}*/



/******************************************************************************
*                    Object 0x1A01 : InputData process data mapping
******************************************************************************/
/**
* \addtogroup 0x1A01 0x1A01 | InputData process data mapping
* @{
* \brief Object 0x1A01 (InputData process data mapping) definition
*/
#ifdef _OBJD_
/**
* \brief Object entry descriptions<br>
* <br>
* SubIndex 0<br>
* SubIndex 1 - Reference to 0x6010.1<br>
* SubIndex 2 - Reference to 0x6010.2<br>
* SubIndex 3 - Reference to 0x6010.3<br>
* SubIndex 4 - Reference to 0x6010.4<br>
* SubIndex 5 - Reference to 0x6010.5<br>
* SubIndex 6 - Reference to 0x6010.6<br>
* SubIndex 7 - Reference to 0x6010.7<br>
* SubIndex 8 - Reference to 0x6010.8<br>
* SubIndex 9 - Reference to 0x6010.9<br>
* SubIndex 10 - Reference to 0x6010.10<br>
* SubIndex 11 - Reference to 0x6010.11<br>
* SubIndex 12 - Reference to 0x6010.12<br>
* SubIndex 13 - Reference to 0x6010.13<br>
* SubIndex 14 - Reference to 0x6010.14<br>
* SubIndex 15 - Reference to 0x6010.15<br>
* SubIndex 16 - Reference to 0x6010.16<br>
* SubIndex 17 - Reference to 0x6010.17<br>
* SubIndex 18 - Reference to 0x6010.18<br>
* SubIndex 19 - Reference to 0x6010.19<br>
* SubIndex 20 - Reference to 0x6010.20<br>
* SubIndex 21 - Reference to 0x6010.21<br>
* SubIndex 22 - Reference to 0x6010.22<br>
* SubIndex 23 - Reference to 0x6010.23<br>
* SubIndex 24 - Reference to 0x6010.24<br>
* SubIndex 25 - Reference to 0x6010.25<br>
* SubIndex 26 - Reference to 0x6010.26<br>
*/
OBJCONST TSDOINFOENTRYDESC    OBJMEM asEntryDesc0x1A01[] = {
{ DEFTYPE_UNSIGNED8 , 0x8 , ACCESS_READ },
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex1 - Reference to 0x6010.1 */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex2 - Reference to 0x6010.2 */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex3 - Reference to 0x6010.3 */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex4 - Reference to 0x6010.4 */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex5 - Reference to 0x6010.5 */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex6 - Reference to 0x6010.6 */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex7 - Reference to 0x6010.7 */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex8 - Reference to 0x6010.8 */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex9 - Reference to 0x6010.9 */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex10 - Reference to 0x6010.10 */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex11 - Reference to 0x6010.11 */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex12 - Reference to 0x6010.12 */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex13 - Reference to 0x6010.13 */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex14 - Reference to 0x6010.14 */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex15 - Reference to 0x6010.15 */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex16 - Reference to 0x6010.16 */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex17 - Reference to 0x6010.17 */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex18 - Reference to 0x6010.18 */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex19 - Reference to 0x6010.19 */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex20 - Reference to 0x6010.20 */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex21 - Reference to 0x6010.21 */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex22 - Reference to 0x6010.22 */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex23 - Reference to 0x6010.23 */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex24 - Reference to 0x6010.24 */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex25 - Reference to 0x6010.25 */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }}; /* Subindex26 - Reference to 0x6010.26 */

/**
* \brief Object/Entry names
*/
OBJCONST UCHAR OBJMEM aName0x1A01[] = "InputData process data mapping\000"
"SubIndex 001\000"
"SubIndex 002\000"
"SubIndex 003\000"
"SubIndex 004\000"
"SubIndex 005\000"
"SubIndex 006\000"
"SubIndex 007\000"
"SubIndex 008\000"
"SubIndex 009\000"
"SubIndex 010\000"
"SubIndex 011\000"
"SubIndex 012\000"
"SubIndex 013\000"
"SubIndex 014\000"
"SubIndex 015\000"
"SubIndex 016\000"
"SubIndex 017\000"
"SubIndex 018\000"
"SubIndex 019\000"
"SubIndex 020\000"
"SubIndex 021\000"
"SubIndex 022\000"
"SubIndex 023\000"
"SubIndex 024\000"
"SubIndex 025\000"
"SubIndex 026\000\377";
#endif //#ifdef _OBJD_

#ifndef _LAN9252_APP_OBJECTS_H_
/**
* \brief Object structure
*/
typedef struct OBJ_STRUCT_PACKED_START {
UINT16 u16SubIndex0;
UINT32 SI1; /* Subindex1 - Reference to 0x6010.1 */
UINT32 SI2; /* Subindex2 - Reference to 0x6010.2 */
UINT32 SI3; /* Subindex3 - Reference to 0x6010.3 */
UINT32 SI4; /* Subindex4 - Reference to 0x6010.4 */
UINT32 SI5; /* Subindex5 - Reference to 0x6010.5 */
UINT32 SI6; /* Subindex6 - Reference to 0x6010.6 */
UINT32 SI7; /* Subindex7 - Reference to 0x6010.7 */
UINT32 SI8; /* Subindex8 - Reference to 0x6010.8 */
UINT32 SI9; /* Subindex9 - Reference to 0x6010.9 */
UINT32 SI10; /* Subindex10 - Reference to 0x6010.10 */
UINT32 SI11; /* Subindex11 - Reference to 0x6010.11 */
UINT32 SI12; /* Subindex12 - Reference to 0x6010.12 */
UINT32 SI13; /* Subindex13 - Reference to 0x6010.13 */
UINT32 SI14; /* Subindex14 - Reference to 0x6010.14 */
UINT32 SI15; /* Subindex15 - Reference to 0x6010.15 */
UINT32 SI16; /* Subindex16 - Reference to 0x6010.16 */
UINT32 SI17; /* Subindex17 - Reference to 0x6010.17 */
UINT32 SI18; /* Subindex18 - Reference to 0x6010.18 */
UINT32 SI19; /* Subindex19 - Reference to 0x6010.19 */
UINT32 SI20; /* Subindex20 - Reference to 0x6010.20 */
UINT32 SI21; /* Subindex21 - Reference to 0x6010.21 */
UINT32 SI22; /* Subindex22 - Reference to 0x6010.22 */
UINT32 SI23; /* Subindex23 - Reference to 0x6010.23 */
UINT32 SI24; /* Subindex24 - Reference to 0x6010.24 */
UINT32 SI25; /* Subindex25 - Reference to 0x6010.25 */
UINT32 SI26; /* Subindex26 - Reference to 0x6010.26 */
} OBJ_STRUCT_PACKED_END
TOBJ1A01;
#endif //#ifndef _LAN9252_APP_OBJECTS_H_

/**
* \brief Object variable
*/
PROTO TOBJ1A01 InputDataProcessDataMapping0x1A01
#if defined(_LAN9252_APP_) && (_LAN9252_APP_ == 1)
={26,0x60100120,0x60100210,0x60100310,0x60100410,0x60100520,0x60100610,0x60100710,0x60100810,0x60100920,0x60100A10,0x60100B10,0x60100C10,0x60100D10,0x60100E20,0x60100F20,0x60101020,0x60101120,0x60101220,0x60101320,0x60101420,0x60101520,0x60101620,0x60101720,0x60101820,0x60101920,0x60101A08}
#endif
;
/** @}*/



/******************************************************************************
*                    Object 0x1C12 : SyncManager 2 assignment
******************************************************************************/
/**
* \addtogroup 0x1C12 0x1C12 | SyncManager 2 assignment
* @{
* \brief Object 0x1C12 (SyncManager 2 assignment) definition
*/
#ifdef _OBJD_
/**
* \brief Entry descriptions<br>
* 
* Subindex 0<br>
* Subindex 1 - n (the same entry description is used)<br>
*/
OBJCONST TSDOINFOENTRYDESC    OBJMEM asEntryDesc0x1C12[] = {
{ DEFTYPE_UNSIGNED8 , 0x8 , ACCESS_READ },
{ DEFTYPE_UNSIGNED16 , 0x10 , ACCESS_READ }};

/**
* \brief Object name definition<br>
* For Subindex 1 to n the syntax 'Subindex XXX' is used
*/
OBJCONST UCHAR OBJMEM aName0x1C12[] = "SyncManager 2 assignment\000\377";
#endif //#ifdef _OBJD_

#ifndef _LAN9252_APP_OBJECTS_H_
/**
* \brief Object structure
*/
typedef struct OBJ_STRUCT_PACKED_START {
UINT16   u16SubIndex0;  /**< \brief Subindex 0 */
UINT16 aEntries[1];  /**< \brief Subindex 1 - 1 */
} OBJ_STRUCT_PACKED_END
TOBJ1C12;
#endif //#ifndef _LAN9252_APP_OBJECTS_H_

/**
* \brief Object variable
*/
PROTO TOBJ1C12 sRxPDOassign
#if defined(_LAN9252_APP_) && (_LAN9252_APP_ == 1)
={1,{0x1601}}
#endif
;
/** @}*/



/******************************************************************************
*                    Object 0x1C13 : SyncManager 3 assignment
******************************************************************************/
/**
* \addtogroup 0x1C13 0x1C13 | SyncManager 3 assignment
* @{
* \brief Object 0x1C13 (SyncManager 3 assignment) definition
*/
#ifdef _OBJD_
/**
* \brief Entry descriptions<br>
* 
* Subindex 0<br>
* Subindex 1 - n (the same entry description is used)<br>
*/
OBJCONST TSDOINFOENTRYDESC    OBJMEM asEntryDesc0x1C13[] = {
{ DEFTYPE_UNSIGNED8 , 0x8 , ACCESS_READ },
{ DEFTYPE_UNSIGNED16 , 0x10 , ACCESS_READ }};

/**
* \brief Object name definition<br>
* For Subindex 1 to n the syntax 'Subindex XXX' is used
*/
OBJCONST UCHAR OBJMEM aName0x1C13[] = "SyncManager 3 assignment\000\377";
#endif //#ifdef _OBJD_

#ifndef _LAN9252_APP_OBJECTS_H_
/**
* \brief Object structure
*/
typedef struct OBJ_STRUCT_PACKED_START {
UINT16   u16SubIndex0;  /**< \brief Subindex 0 */
UINT16 aEntries[2];  /**< \brief Subindex 1 - 2 */
} OBJ_STRUCT_PACKED_END
TOBJ1C13;
#endif //#ifndef _LAN9252_APP_OBJECTS_H_

/**
* \brief Object variable
*/
PROTO TOBJ1C13 sTxPDOassign
#if defined(_LAN9252_APP_) && (_LAN9252_APP_ == 1)
={2,{0x1A00,0x1A01}}
#endif
;
/** @}*/



/******************************************************************************
*                    Object 0x6000 : InputData
******************************************************************************/
/**
* \addtogroup 0x6000 0x6000 | InputData
* @{
* \brief Object 0x6000 (InputData) definition
*/
#ifdef _OBJD_
/**
* \brief Object entry descriptions<br>
* <br>
* SubIndex 0<br>
* SubIndex 1 - InU8_BoardID_RTM_ON_ARM<br>
* SubIndex 2 - InU8_Reserved_RTM_ON_ARM<br>
* SubIndex 3 - InU32_FirmWareVersion_RTM_ON_ARM<br>
* SubIndex 4 - InU8_BoardID_ICM<br>
* SubIndex 5 - InU8_Reserved_ICM<br>
* SubIndex 6 - InU32_FirmWareVersion_ICM<br>
* SubIndex 7 - InU8_BoardID_BGM<br>
* SubIndex 8 - InU8_Reserved_BGM<br>
* SubIndex 9 - InU32_FirmWareVersion_BGM<br>
* SubIndex 10 - InU8_BoardID_primary_dose<br>
* SubIndex 11 - InU8_Reserved_primary_dose<br>
* SubIndex 12 - InU32_FirmWareVersion_primary_dose<br>
* SubIndex 13 - InU8_BoardID_second_dose<br>
* SubIndex 14 - InU8_Reserved_second_dose<br>
* SubIndex 15 - InU32_FirmWareVersion_second_dose<br>
* SubIndex 16 - InU8_BoardID_AFC<br>
* SubIndex 17 - InU8_Reserved_AFC<br>
* SubIndex 18 - InU32_FirmWareVersion_AFC<br>
* SubIndex 19 - InU8_BoardID_QAM<br>
* SubIndex 20 - InU8_Reserved_QAM<br>
* SubIndex 21 - InU32_FirmWareVersion_QAM<br>
* SubIndex 22 - InU8_BoardID_RTM_OFF_ARM<br>
* SubIndex 23 - InU8_Reserved_RTM_OFF_ARM<br>
* SubIndex 24 - InU32_FirmWareVersion_RTM_OFF_ARM<br>
* SubIndex 25 - InU8_BoardID_PSM<br>
* SubIndex 26 - InU8_Reserved_PSM<br>
* SubIndex 27 - InU32_FirmWareVersion_PSM<br>
* SubIndex 28 - InU8_BoardID_GMM<br>
* SubIndex 29 - InU8_Reserved_GMM<br>
* SubIndex 30 - InU32_FirmWareVersion_GMM<br>
* SubIndex 31 - InU8_BoardID_FKP<br>
* SubIndex 32 - InU8_Reserved_FKP<br>
* SubIndex 33 - InU32_FirmWareVersion_FKP<br>
* SubIndex 34 - InU8_BoardID_CPG_L<br>
* SubIndex 35 - InU8_Reserved_CPG_L<br>
* SubIndex 36 - InU32_FirmWareVersion_CPG_L<br>
* SubIndex 37 - InU8_BoardID_CPG_R<br>
* SubIndex 38 - InU8_Reserved_CPG_R<br>
* SubIndex 39 - InU32_FirmWareVersion_CPG_R<br>
* SubIndex 40 - InU8_beam_id<br>
* SubIndex 41 - InU8_reserved1<br>
* SubIndex 42 - InU16_radiation_index<br>
* SubIndex 43 - InU8_rtm_on_arm_fsm_state_current<br>
* SubIndex 44 - InU8_rtm_on_arm_ctrl_mode_cur<br>
* SubIndex 45 - InU8_ethercat_Link_state<br>
* SubIndex 46 - InU8_reserved2<br>
* SubIndex 47 - InU8_rtm_on_arm_beam_id<br>
* SubIndex 48 - InU8_reserved3<br>
* SubIndex 49 - InU16_rtm_on_arm_radiation_index<br>
* SubIndex 50 - InU32_rtm_on_arm_not_ready_event<br>
* SubIndex 51 - InU32_rtm_on_arm_warning_interlock<br>
* SubIndex 52 - InU32_rtm_on_arm_minor_interlock<br>
* SubIndex 53 - InU32_rtm_on_arm_serious_interlock<br>
* SubIndex 54 - InU16_TotalStep_RTM_ON_ARM<br>
* SubIndex 55 - InU16_CurrentStep_RTM_ON_ARM<br>
* SubIndex 56 - InU16_ErrorCode_RTM_ON_ARM<br>
* SubIndex 57 - InB1_ON_DI_RTC_WD_OK_IN<br>
* SubIndex 58 - InB1_ON_DI_Reserve0<br>
* SubIndex 59 - InB1_ON_DI_BSM_NOT_READY<br>
* SubIndex 60 - InB1_ON_DI_MV_TreatmentEN<br>
* SubIndex 61 - InB1_ON_DI_DI_HVEN<br>
* SubIndex 62 - InB1_ON_DI_Pulse_Inhibit<br>
* SubIndex 63 - InB1_ON_DI_KV_TreatmentEN<br>
* SubIndex 64 - InB1_ON_DI_Power_cut_FB<br>
* SubIndex 65 - InB1_ON_DI_GATING_IN<br>
* SubIndex 66 - InB1_ON_DI_Slipring_HVEN_IN<br>
* SubIndex 67 - InB1_ON_DI_Slipring_KV_TreatmentEN_IN<br>
* SubIndex 68 - InB1_ON_DI_Slipring_MV_TreatmentEN_IN<br>
* SubIndex 69 - InB4_ON_DI_reserve1<br>
* SubIndex 70 - InB1_ON_DO_reserve0<br>
* SubIndex 71 - InB1_ON_DO_MV_TreatmentEN<br>
* SubIndex 72 - InB1_ON_DO_KV_TreatmentEN<br>
* SubIndex 73 - InB1_ON_DO_Emergency<br>
* SubIndex 74 - InB1_ON_DO_Pulse_Inhibit<br>
* SubIndex 75 - InB1_ON_DO_HVEN<br>
* SubIndex 76 - InB1_ON_DO_PowerCut<br>
* SubIndex 77 - InB1_ON_DO_reserve1<br>
* SubIndex 78 - InU8_ON_DO_reserve2<br>
* SubIndex 79 - InU8_OFF_reserve0<br>
* SubIndex 80 - InB1_OFF_DI_CITB_EMERGENCY2<br>
* SubIndex 81 - InB1_OFF_DI_UPS_LOAD_PORT<br>
* SubIndex 82 - InB1_OFF_DI_STAND_BREAKER1<br>
* SubIndex 83 - InB1_OFF_DI_STAND_EMERGENCY<br>
* SubIndex 84 - InB1_OFF_DI_TouchGuard<br>
* SubIndex 85 - InB1_OFF_DI_HvEn<br>
* SubIndex 86 - InB1_OFF_DI_MV_TreatmentEN<br>
* SubIndex 87 - InB1_OFF_DI_KV_TreatmentEN<br>
* SubIndex 88 - InB1_OFF_DI_STAND_BREAKER7<br>
* SubIndex 89 - InB1_OFF_DI_STAND_BREAKER3<br>
* SubIndex 90 - InB1_OFF_DI_UPS_ON_BYPASS<br>
* SubIndex 91 - InB1_OFF_DI_STAND_BREAKER5<br>
* SubIndex 92 - InB1_OFF_DI_CITB_EMERGENCY3<br>
* SubIndex 93 - InB1_OFF_DI_COVER3<br>
* SubIndex 94 - InB1_OFF_DI_POWER_CUT<br>
* SubIndex 95 - InB1_OFF_DI_STAND_BREAKER4<br>
* SubIndex 96 - InB1_OFF_DI_CITB_TREATMENT_ROOM_DOOR1<br>
* SubIndex 97 - InB1_OFF_DI_CITB_TREATMENT_ROOM_DOOR2<br>
* SubIndex 98 - InB1_OFF_DI_CITB_EMERGENCY1<br>
* SubIndex 99 - InB1_OFF_DI_CITB_SEARCH_TREATMENT_ROOM<br>
* SubIndex 100 - InB1_OFF_DI_CONTROL_ROOM_BREAKER1<br>
* SubIndex 101 - InB1_OFF_DI_UPS_ON_BATT<br>
* SubIndex 102 - InB1_OFF_DI_CONTROL_ROOM_BREAKER5<br>
* SubIndex 103 - InB1_OFF_DI_STAND_BREAKER6<br>
* SubIndex 104 - InB1_OFF_DI_CITB_EMERGENCY5<br>
* SubIndex 105 - InB1_OFF_DI_COVER1<br>
* SubIndex 106 - InB1_OFF_DI_STAND_RESERVE<br>
* SubIndex 107 - InB1_OFF_DI_TREATMENT_ROOM_DOOR_READY<br>
* SubIndex 108 - InB1_OFF_DI_COVER2<br>
* SubIndex 109 - InB1_OFF_DI_CONTROL_ROOM_BREAKER2<br>
* SubIndex 110 - InB1_OFF_DI_CONTROL_ROOM_BREAKER4<br>
* SubIndex 111 - InB1_OFF_DI_STAND_CONTACTOR2<br>
* SubIndex 112 - InB1_OFF_DI_STAND_CONTACTOR1<br>
* SubIndex 113 - InB1_OFF_DI_UPS_LOW_BATT<br>
* SubIndex 114 - InB1_OFF_DI_HvKey<br>
* SubIndex 115 - InB1_OFF_DI_COVER4<br>
* SubIndex 116 - InB1_OFF_DI_CITB_EMERGENCY4<br>
* SubIndex 117 - InB1_OFF_DI_CONTROL_ROOM_BREAKER3<br>
* SubIndex 118 - InB1_OFF_DI_STAND_BREAKER2<br>
* SubIndex 119 - InB1_OFF_RTC_WD_OK_IN<br>
* SubIndex 120 - InU16_OFF_mcp23017_di_reserve<br>
* SubIndex 121 - InB1_OFF_DI_GATING<br>
* SubIndex 122 - InB7_OFF_reserve1<br>
* SubIndex 123 - InU8_OFF_reserve2<br>
* SubIndex 124 - InB1_OFF_DO_RTM_AutoPowerUp<br>
* SubIndex 125 - InB1_OFF_DO_SearchTreatmentRoomRelay<br>
* SubIndex 126 - InB1_OFF_DO_Laser<br>
* SubIndex 127 - InB1_OFF_DO_Power_CUT<br>
* SubIndex 128 - InB1_OFF_DO_STAND_RESERVE<br>
* SubIndex 129 - InB1_OFF_DO_TreatmentRoomLight<br>
* SubIndex 130 - InB1_OFF_DO_RadiationIndicator<br>
* SubIndex 131 - InB1_OFF_DO_ReadyIndicator<br>
* SubIndex 132 - InB1_OFF_DO_RTM_SystemShutDown<br>
* SubIndex 133 - InB7_OFF_reserve3<br>
* SubIndex 134 - InU16_OFF_mcp23017_do_reserve<br>
* SubIndex 135 - InB1_OFF_DO_SoftwareHvEn<br>
* SubIndex 136 - InB1_OFF_DO_SoftwareKVTreatmentEn<br>
* SubIndex 137 - InB1_OFF_DO_SoftwareMVTreatmentEn<br>
* SubIndex 138 - InB1_OFF_DO_ThreePhasePowerOn<br>
* SubIndex 139 - InB1_OFF_DO_softwareMoveEN<br>
* SubIndex 140 - InB1_OFF_DO_TreatmentMotionEnable<br>
* SubIndex 141 - InB1_OFF_DO_AsuMotionEnable<br>
* SubIndex 142 - InB1_OFF_reserve4<br>
* SubIndex 143 - InU8_OFF_reserve5<br>
* SubIndex 144 - InU8_icm_fsm_state_current<br>
* SubIndex 145 - InU8_icm_ctrl_mode_cur<br>
* SubIndex 146 - InU16_reserved4<br>
* SubIndex 147 - InU8_icm_beam_id<br>
* SubIndex 148 - InU8_reserved5<br>
* SubIndex 149 - InU16_icm_radiation_index<br>
* SubIndex 150 - InU32_icm_not_ready_event<br>
* SubIndex 151 - InU32_icm_warning_interlock<br>
* SubIndex 152 - InU32_icm_minor_interlock<br>
* SubIndex 153 - InU32_icm_serious_interlock<br>
* SubIndex 154 - InU16_TotalStep_ICM<br>
* SubIndex 155 - InU16_CurrentStep_ICM<br>
* SubIndex 156 - InU16_ErrorCode_ICM<br>
* SubIndex 157 - InU8_bgm_fsm_state_current<br>
* SubIndex 158 - InU8_bgm_ctrl_mode_cur<br>
* SubIndex 159 - InU16_reserved6<br>
* SubIndex 160 - InU8_bgm_beam_id<br>
* SubIndex 161 - InU8_reserved7<br>
* SubIndex 162 - InU16_bgm_radiation_index<br>
* SubIndex 163 - InU32_bgm_not_ready_event<br>
* SubIndex 164 - InU32_bgm_warning_interlock<br>
* SubIndex 165 - InU32_bgm_minor_interlock<br>
* SubIndex 166 - InU32_bgm_serious_interlock<br>
* SubIndex 167 - InU16_TotalStep_BGM<br>
* SubIndex 168 - InU16_CurrentStep_BGM<br>
* SubIndex 169 - InU16_ErrorCode_BGM<br>
* SubIndex 170 - InF_beam_on_time<br>
* SubIndex 171 - InF_primary_dose_current<br>
* SubIndex 172 - InF_primary_dose_rate_current<br>
* SubIndex 173 - InU8_primary_dose_fsm_state<br>
* SubIndex 174 - InU8_reserved8<br>
* SubIndex 175 - InU32_primary_dose_interlock<br>
* SubIndex 176 - InF_second_dose_current<br>
* SubIndex 177 - InF_secondry_dose_rate_current<br>
* SubIndex 178 - InU8_secondry_dose_fsm_rate<br>
* SubIndex 179 - InU8_reserved9<br>
* SubIndex 180 - InU32_secondry_dose_interlock<br>
* SubIndex 181 - InU32_afc_interlock<br>
* SubIndex 182 - InF_afc_position_current<br>
* SubIndex 183 - InU8_qam_fsm_state_current<br>
* SubIndex 184 - InU8_qam_ctrl_mode_cur<br>
* SubIndex 185 - InU16_reserved10<br>
* SubIndex 186 - InU8_qam_beam_id<br>
* SubIndex 187 - InU8_reserved11<br>
* SubIndex 188 - InU16_qam_radiation_index<br>
* SubIndex 189 - InU32_qam_not_ready_event<br>
* SubIndex 190 - InU32_qam_warning_interlock<br>
* SubIndex 191 - InU32_qam_minor_interlock<br>
* SubIndex 192 - InU32_qam_serious_interlock<br>
* SubIndex 193 - InU16_TotalStep_QAM<br>
* SubIndex 194 - InU16_CurrentStep_QAM<br>
* SubIndex 195 - InU16_ErrorCode_QAM<br>
* SubIndex 196 - InU8_rtm_off_fsm_state_current<br>
* SubIndex 197 - InU8_rtm_off_ctrl_mode_cur<br>
* SubIndex 198 - InU16_reserved12<br>
* SubIndex 199 - InU8_rtm_off_beam_id<br>
* SubIndex 200 - InU8_reserved13<br>
* SubIndex 201 - InU16_rtm_off_radiation_index<br>
* SubIndex 202 - InU32_rtm_off_not_ready_event<br>
* SubIndex 203 - InU32_rtm_off_warning_interlock<br>
* SubIndex 204 - InU32_rtm_off_minor_interlock<br>
* SubIndex 205 - InU32_rtm_off_serious_interlock<br>
* SubIndex 206 - InU16_TotalStep_RTM_OFF_ARM<br>
* SubIndex 207 - InU16_CurrentStep_RTM_OFF_ARM<br>
* SubIndex 208 - InU16_ErrorCode_RTM_OFF_ARM<br>
* SubIndex 209 - InU32_rtm_off_search_timeout_cur<br>
* SubIndex 210 - InU8_gmm_fsm_state_current<br>
* SubIndex 211 - InU8_gmm_ctrl_mode_cur<br>
* SubIndex 212 - InU16_reserved14<br>
* SubIndex 213 - InU8_gmm_beam_id<br>
* SubIndex 214 - InU8_reserved15<br>
* SubIndex 215 - InU16_gmm_radiation_index<br>
* SubIndex 216 - InU32_gmm_not_ready_event<br>
* SubIndex 217 - InU32_gmm_warning_interlock<br>
* SubIndex 218 - InU32_gmm_minor_interlock<br>
* SubIndex 219 - InU32_gmm_serious_interlock<br>
* SubIndex 220 - InU16_TotalStep_GMM<br>
* SubIndex 221 - InU16_CurrentStep_GMM<br>
* SubIndex 222 - InU16_ErrorCode_GMM<br>
* SubIndex 223 - InU32_gmm_move_status<br>
* SubIndex 224 - InF_gmm_position_cur<br>
* SubIndex 225 - InF_gmm_velocity_cur<br>
* SubIndex 226 - InU8_psm_fsm_state_current<br>
* SubIndex 227 - InU8_psm_ctrl_mode_cur<br>
* SubIndex 228 - InU16_reserved16<br>
* SubIndex 229 - InU8_psm_beam_id<br>
* SubIndex 230 - InU8_reserved17<br>
* SubIndex 231 - InU16_psm_radiation_index<br>
* SubIndex 232 - InU32_psm_not_ready_event<br>
* SubIndex 233 - InU32_psm_warning_interlock<br>
* SubIndex 234 - InU32_psm_minor_interlock<br>
* SubIndex 235 - InU32_psm_serious_interlock<br>
* SubIndex 236 - InU16_TotalStep_PSM<br>
* SubIndex 237 - InU16_CurrentStep_PSM<br>
* SubIndex 238 - InU16_ErrorCode_PSM<br>
* SubIndex 239 - InU32_psm_move_status<br>
* SubIndex 240 - InF_psm_position_x_cur<br>
* SubIndex 241 - InF_psm_position_y_cur<br>
* SubIndex 242 - InF_psm_position_z_cur<br>
* SubIndex 243 - InF_psm_position_x_r_cur<br>
* SubIndex 244 - InF_psm_position_y_r_cur<br>
* SubIndex 245 - InF_psm_position_z_r_cur<br>
* SubIndex 246 - InF_psm_velocity_x_cur<br>
* SubIndex 247 - InF_psm_velocity_y_cur<br>
* SubIndex 248 - InF_psm_velocity_z_cur<br>
* SubIndex 249 - InF_psm_velocity_x_r_cur<br>
* SubIndex 250 - InF_psm_velocity_y_r_cur<br>
* SubIndex 251 - InF_psm_velocity_z_r_cur<br>
* SubIndex 252 - InU8_function_button<br>
* SubIndex 253 - InU8_special_button<br>
*/
OBJCONST TSDOINFOENTRYDESC    OBJMEM asEntryDesc0x6000[] = {
{ DEFTYPE_UNSIGNED8 , 0x8 , ACCESS_READ },
{ DEFTYPE_UNSIGNED8 , 0x08 , ACCESS_READ }, /* Subindex1 - InU8_BoardID_RTM_ON_ARM */
{ DEFTYPE_UNSIGNED8 , 0x08 , ACCESS_READ }, /* Subindex2 - InU8_Reserved_RTM_ON_ARM */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex3 - InU32_FirmWareVersion_RTM_ON_ARM */
{ DEFTYPE_UNSIGNED8 , 0x08 , ACCESS_READ }, /* Subindex4 - InU8_BoardID_ICM */
{ DEFTYPE_UNSIGNED8 , 0x08 , ACCESS_READ }, /* Subindex5 - InU8_Reserved_ICM */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex6 - InU32_FirmWareVersion_ICM */
{ DEFTYPE_UNSIGNED8 , 0x08 , ACCESS_READ }, /* Subindex7 - InU8_BoardID_BGM */
{ DEFTYPE_UNSIGNED8 , 0x08 , ACCESS_READ }, /* Subindex8 - InU8_Reserved_BGM */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex9 - InU32_FirmWareVersion_BGM */
{ DEFTYPE_UNSIGNED8 , 0x08 , ACCESS_READ }, /* Subindex10 - InU8_BoardID_primary_dose */
{ DEFTYPE_UNSIGNED8 , 0x08 , ACCESS_READ }, /* Subindex11 - InU8_Reserved_primary_dose */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex12 - InU32_FirmWareVersion_primary_dose */
{ DEFTYPE_UNSIGNED8 , 0x08 , ACCESS_READ }, /* Subindex13 - InU8_BoardID_second_dose */
{ DEFTYPE_UNSIGNED8 , 0x08 , ACCESS_READ }, /* Subindex14 - InU8_Reserved_second_dose */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex15 - InU32_FirmWareVersion_second_dose */
{ DEFTYPE_UNSIGNED8 , 0x08 , ACCESS_READ }, /* Subindex16 - InU8_BoardID_AFC */
{ DEFTYPE_UNSIGNED8 , 0x08 , ACCESS_READ }, /* Subindex17 - InU8_Reserved_AFC */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex18 - InU32_FirmWareVersion_AFC */
{ DEFTYPE_UNSIGNED8 , 0x08 , ACCESS_READ }, /* Subindex19 - InU8_BoardID_QAM */
{ DEFTYPE_UNSIGNED8 , 0x08 , ACCESS_READ }, /* Subindex20 - InU8_Reserved_QAM */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex21 - InU32_FirmWareVersion_QAM */
{ DEFTYPE_UNSIGNED8 , 0x08 , ACCESS_READ }, /* Subindex22 - InU8_BoardID_RTM_OFF_ARM */
{ DEFTYPE_UNSIGNED8 , 0x08 , ACCESS_READ }, /* Subindex23 - InU8_Reserved_RTM_OFF_ARM */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex24 - InU32_FirmWareVersion_RTM_OFF_ARM */
{ DEFTYPE_UNSIGNED8 , 0x08 , ACCESS_READ }, /* Subindex25 - InU8_BoardID_PSM */
{ DEFTYPE_UNSIGNED8 , 0x08 , ACCESS_READ }, /* Subindex26 - InU8_Reserved_PSM */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex27 - InU32_FirmWareVersion_PSM */
{ DEFTYPE_UNSIGNED8 , 0x08 , ACCESS_READ }, /* Subindex28 - InU8_BoardID_GMM */
{ DEFTYPE_UNSIGNED8 , 0x08 , ACCESS_READ }, /* Subindex29 - InU8_Reserved_GMM */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex30 - InU32_FirmWareVersion_GMM */
{ DEFTYPE_UNSIGNED8 , 0x08 , ACCESS_READ }, /* Subindex31 - InU8_BoardID_FKP */
{ DEFTYPE_UNSIGNED8 , 0x08 , ACCESS_READ }, /* Subindex32 - InU8_Reserved_FKP */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex33 - InU32_FirmWareVersion_FKP */
{ DEFTYPE_UNSIGNED8 , 0x08 , ACCESS_READ }, /* Subindex34 - InU8_BoardID_CPG_L */
{ DEFTYPE_UNSIGNED8 , 0x08 , ACCESS_READ }, /* Subindex35 - InU8_Reserved_CPG_L */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex36 - InU32_FirmWareVersion_CPG_L */
{ DEFTYPE_UNSIGNED8 , 0x08 , ACCESS_READ }, /* Subindex37 - InU8_BoardID_CPG_R */
{ DEFTYPE_UNSIGNED8 , 0x08 , ACCESS_READ }, /* Subindex38 - InU8_Reserved_CPG_R */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex39 - InU32_FirmWareVersion_CPG_R */
{ DEFTYPE_UNSIGNED8 , 0x08 , ACCESS_READ }, /* Subindex40 - InU8_beam_id */
{ DEFTYPE_UNSIGNED8 , 0x08 , ACCESS_READ }, /* Subindex41 - InU8_reserved1 */
{ DEFTYPE_UNSIGNED16 , 0x10 , ACCESS_READ }, /* Subindex42 - InU16_radiation_index */
{ DEFTYPE_UNSIGNED8 , 0x08 , ACCESS_READ }, /* Subindex43 - InU8_rtm_on_arm_fsm_state_current */
{ DEFTYPE_UNSIGNED8 , 0x08 , ACCESS_READ }, /* Subindex44 - InU8_rtm_on_arm_ctrl_mode_cur */
{ DEFTYPE_UNSIGNED8 , 0x08 , ACCESS_READ }, /* Subindex45 - InU8_ethercat_Link_state */
{ DEFTYPE_UNSIGNED8 , 0x08 , ACCESS_READ }, /* Subindex46 - InU8_reserved2 */
{ DEFTYPE_UNSIGNED8 , 0x08 , ACCESS_READ }, /* Subindex47 - InU8_rtm_on_arm_beam_id */
{ DEFTYPE_UNSIGNED8 , 0x08 , ACCESS_READ }, /* Subindex48 - InU8_reserved3 */
{ DEFTYPE_UNSIGNED16 , 0x10 , ACCESS_READ }, /* Subindex49 - InU16_rtm_on_arm_radiation_index */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex50 - InU32_rtm_on_arm_not_ready_event */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex51 - InU32_rtm_on_arm_warning_interlock */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex52 - InU32_rtm_on_arm_minor_interlock */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex53 - InU32_rtm_on_arm_serious_interlock */
{ DEFTYPE_UNSIGNED16 , 0x10 , ACCESS_READ }, /* Subindex54 - InU16_TotalStep_RTM_ON_ARM */
{ DEFTYPE_UNSIGNED16 , 0x10 , ACCESS_READ }, /* Subindex55 - InU16_CurrentStep_RTM_ON_ARM */
{ DEFTYPE_UNSIGNED16 , 0x10 , ACCESS_READ }, /* Subindex56 - InU16_ErrorCode_RTM_ON_ARM */
{ DEFTYPE_BOOLEAN , 0x01 , ACCESS_READ }, /* Subindex57 - InB1_ON_DI_RTC_WD_OK_IN */
{ DEFTYPE_BOOLEAN , 0x01 , ACCESS_READ }, /* Subindex58 - InB1_ON_DI_Reserve0 */
{ DEFTYPE_BOOLEAN , 0x01 , ACCESS_READ }, /* Subindex59 - InB1_ON_DI_BSM_NOT_READY */
{ DEFTYPE_BOOLEAN , 0x01 , ACCESS_READ }, /* Subindex60 - InB1_ON_DI_MV_TreatmentEN */
{ DEFTYPE_BOOLEAN , 0x01 , ACCESS_READ }, /* Subindex61 - InB1_ON_DI_DI_HVEN */
{ DEFTYPE_BOOLEAN , 0x01 , ACCESS_READ }, /* Subindex62 - InB1_ON_DI_Pulse_Inhibit */
{ DEFTYPE_BOOLEAN , 0x01 , ACCESS_READ }, /* Subindex63 - InB1_ON_DI_KV_TreatmentEN */
{ DEFTYPE_BOOLEAN , 0x01 , ACCESS_READ }, /* Subindex64 - InB1_ON_DI_Power_cut_FB */
{ DEFTYPE_BOOLEAN , 0x01 , ACCESS_READ }, /* Subindex65 - InB1_ON_DI_GATING_IN */
{ DEFTYPE_BOOLEAN , 0x01 , ACCESS_READ }, /* Subindex66 - InB1_ON_DI_Slipring_HVEN_IN */
{ DEFTYPE_BOOLEAN , 0x01 , ACCESS_READ }, /* Subindex67 - InB1_ON_DI_Slipring_KV_TreatmentEN_IN */
{ DEFTYPE_BOOLEAN , 0x01 , ACCESS_READ }, /* Subindex68 - InB1_ON_DI_Slipring_MV_TreatmentEN_IN */
{ DEFTYPE_BIT4 , 0x04 , ACCESS_READ }, /* Subindex69 - InB4_ON_DI_reserve1 */
{ DEFTYPE_BOOLEAN , 0x01 , ACCESS_READ }, /* Subindex70 - InB1_ON_DO_reserve0 */
{ DEFTYPE_BOOLEAN , 0x01 , ACCESS_READ }, /* Subindex71 - InB1_ON_DO_MV_TreatmentEN */
{ DEFTYPE_BOOLEAN , 0x01 , ACCESS_READ }, /* Subindex72 - InB1_ON_DO_KV_TreatmentEN */
{ DEFTYPE_BOOLEAN , 0x01 , ACCESS_READ }, /* Subindex73 - InB1_ON_DO_Emergency */
{ DEFTYPE_BOOLEAN , 0x01 , ACCESS_READ }, /* Subindex74 - InB1_ON_DO_Pulse_Inhibit */
{ DEFTYPE_BOOLEAN , 0x01 , ACCESS_READ }, /* Subindex75 - InB1_ON_DO_HVEN */
{ DEFTYPE_BOOLEAN , 0x01 , ACCESS_READ }, /* Subindex76 - InB1_ON_DO_PowerCut */
{ DEFTYPE_BOOLEAN , 0x01 , ACCESS_READ }, /* Subindex77 - InB1_ON_DO_reserve1 */
{ DEFTYPE_UNSIGNED8 , 0x08 , ACCESS_READ }, /* Subindex78 - InU8_ON_DO_reserve2 */
{ DEFTYPE_UNSIGNED8 , 0x08 , ACCESS_READ }, /* Subindex79 - InU8_OFF_reserve0 */
{ DEFTYPE_BOOLEAN , 0x01 , ACCESS_READ }, /* Subindex80 - InB1_OFF_DI_CITB_EMERGENCY2 */
{ DEFTYPE_BOOLEAN , 0x01 , ACCESS_READ }, /* Subindex81 - InB1_OFF_DI_UPS_LOAD_PORT */
{ DEFTYPE_BOOLEAN , 0x01 , ACCESS_READ }, /* Subindex82 - InB1_OFF_DI_STAND_BREAKER1 */
{ DEFTYPE_BOOLEAN , 0x01 , ACCESS_READ }, /* Subindex83 - InB1_OFF_DI_STAND_EMERGENCY */
{ DEFTYPE_BOOLEAN , 0x01 , ACCESS_READ }, /* Subindex84 - InB1_OFF_DI_TouchGuard */
{ DEFTYPE_BOOLEAN , 0x01 , ACCESS_READ }, /* Subindex85 - InB1_OFF_DI_HvEn */
{ DEFTYPE_BOOLEAN , 0x01 , ACCESS_READ }, /* Subindex86 - InB1_OFF_DI_MV_TreatmentEN */
{ DEFTYPE_BOOLEAN , 0x01 , ACCESS_READ }, /* Subindex87 - InB1_OFF_DI_KV_TreatmentEN */
{ DEFTYPE_BOOLEAN , 0x01 , ACCESS_READ }, /* Subindex88 - InB1_OFF_DI_STAND_BREAKER7 */
{ DEFTYPE_BOOLEAN , 0x01 , ACCESS_READ }, /* Subindex89 - InB1_OFF_DI_STAND_BREAKER3 */
{ DEFTYPE_BOOLEAN , 0x01 , ACCESS_READ }, /* Subindex90 - InB1_OFF_DI_UPS_ON_BYPASS */
{ DEFTYPE_BOOLEAN , 0x01 , ACCESS_READ }, /* Subindex91 - InB1_OFF_DI_STAND_BREAKER5 */
{ DEFTYPE_BOOLEAN , 0x01 , ACCESS_READ }, /* Subindex92 - InB1_OFF_DI_CITB_EMERGENCY3 */
{ DEFTYPE_BOOLEAN , 0x01 , ACCESS_READ }, /* Subindex93 - InB1_OFF_DI_COVER3 */
{ DEFTYPE_BOOLEAN , 0x01 , ACCESS_READ }, /* Subindex94 - InB1_OFF_DI_POWER_CUT */
{ DEFTYPE_BOOLEAN , 0x01 , ACCESS_READ }, /* Subindex95 - InB1_OFF_DI_STAND_BREAKER4 */
{ DEFTYPE_BOOLEAN , 0x01 , ACCESS_READ }, /* Subindex96 - InB1_OFF_DI_CITB_TREATMENT_ROOM_DOOR1 */
{ DEFTYPE_BOOLEAN , 0x01 , ACCESS_READ }, /* Subindex97 - InB1_OFF_DI_CITB_TREATMENT_ROOM_DOOR2 */
{ DEFTYPE_BOOLEAN , 0x01 , ACCESS_READ }, /* Subindex98 - InB1_OFF_DI_CITB_EMERGENCY1 */
{ DEFTYPE_BOOLEAN , 0x01 , ACCESS_READ }, /* Subindex99 - InB1_OFF_DI_CITB_SEARCH_TREATMENT_ROOM */
{ DEFTYPE_BOOLEAN , 0x01 , ACCESS_READ }, /* Subindex100 - InB1_OFF_DI_CONTROL_ROOM_BREAKER1 */
{ DEFTYPE_BOOLEAN , 0x01 , ACCESS_READ }, /* Subindex101 - InB1_OFF_DI_UPS_ON_BATT */
{ DEFTYPE_BOOLEAN , 0x01 , ACCESS_READ }, /* Subindex102 - InB1_OFF_DI_CONTROL_ROOM_BREAKER5 */
{ DEFTYPE_BOOLEAN , 0x01 , ACCESS_READ }, /* Subindex103 - InB1_OFF_DI_STAND_BREAKER6 */
{ DEFTYPE_BOOLEAN , 0x01 , ACCESS_READ }, /* Subindex104 - InB1_OFF_DI_CITB_EMERGENCY5 */
{ DEFTYPE_BOOLEAN , 0x01 , ACCESS_READ }, /* Subindex105 - InB1_OFF_DI_COVER1 */
{ DEFTYPE_BOOLEAN , 0x01 , ACCESS_READ }, /* Subindex106 - InB1_OFF_DI_STAND_RESERVE */
{ DEFTYPE_BOOLEAN , 0x01 , ACCESS_READ }, /* Subindex107 - InB1_OFF_DI_TREATMENT_ROOM_DOOR_READY */
{ DEFTYPE_BOOLEAN , 0x01 , ACCESS_READ }, /* Subindex108 - InB1_OFF_DI_COVER2 */
{ DEFTYPE_BOOLEAN , 0x01 , ACCESS_READ }, /* Subindex109 - InB1_OFF_DI_CONTROL_ROOM_BREAKER2 */
{ DEFTYPE_BOOLEAN , 0x01 , ACCESS_READ }, /* Subindex110 - InB1_OFF_DI_CONTROL_ROOM_BREAKER4 */
{ DEFTYPE_BOOLEAN , 0x01 , ACCESS_READ }, /* Subindex111 - InB1_OFF_DI_STAND_CONTACTOR2 */
{ DEFTYPE_BOOLEAN , 0x01 , ACCESS_READ }, /* Subindex112 - InB1_OFF_DI_STAND_CONTACTOR1 */
{ DEFTYPE_BOOLEAN , 0x01 , ACCESS_READ }, /* Subindex113 - InB1_OFF_DI_UPS_LOW_BATT */
{ DEFTYPE_BOOLEAN , 0x01 , ACCESS_READ }, /* Subindex114 - InB1_OFF_DI_HvKey */
{ DEFTYPE_BOOLEAN , 0x01 , ACCESS_READ }, /* Subindex115 - InB1_OFF_DI_COVER4 */
{ DEFTYPE_BOOLEAN , 0x01 , ACCESS_READ }, /* Subindex116 - InB1_OFF_DI_CITB_EMERGENCY4 */
{ DEFTYPE_BOOLEAN , 0x01 , ACCESS_READ }, /* Subindex117 - InB1_OFF_DI_CONTROL_ROOM_BREAKER3 */
{ DEFTYPE_BOOLEAN , 0x01 , ACCESS_READ }, /* Subindex118 - InB1_OFF_DI_STAND_BREAKER2 */
{ DEFTYPE_BOOLEAN , 0x01 , ACCESS_READ }, /* Subindex119 - InB1_OFF_RTC_WD_OK_IN */
{ DEFTYPE_UNSIGNED16 , 0x10 , ACCESS_READ }, /* Subindex120 - InU16_OFF_mcp23017_di_reserve */
{ DEFTYPE_BOOLEAN , 0x01 , ACCESS_READ }, /* Subindex121 - InB1_OFF_DI_GATING */
{ DEFTYPE_BIT7 , 0x07 , ACCESS_READ }, /* Subindex122 - InB7_OFF_reserve1 */
{ DEFTYPE_UNSIGNED8 , 0x08 , ACCESS_READ }, /* Subindex123 - InU8_OFF_reserve2 */
{ DEFTYPE_BOOLEAN , 0x01 , ACCESS_READ }, /* Subindex124 - InB1_OFF_DO_RTM_AutoPowerUp */
{ DEFTYPE_BOOLEAN , 0x01 , ACCESS_READ }, /* Subindex125 - InB1_OFF_DO_SearchTreatmentRoomRelay */
{ DEFTYPE_BOOLEAN , 0x01 , ACCESS_READ }, /* Subindex126 - InB1_OFF_DO_Laser */
{ DEFTYPE_BOOLEAN , 0x01 , ACCESS_READ }, /* Subindex127 - InB1_OFF_DO_Power_CUT */
{ DEFTYPE_BOOLEAN , 0x01 , ACCESS_READ }, /* Subindex128 - InB1_OFF_DO_STAND_RESERVE */
{ DEFTYPE_BOOLEAN , 0x01 , ACCESS_READ }, /* Subindex129 - InB1_OFF_DO_TreatmentRoomLight */
{ DEFTYPE_BOOLEAN , 0x01 , ACCESS_READ }, /* Subindex130 - InB1_OFF_DO_RadiationIndicator */
{ DEFTYPE_BOOLEAN , 0x01 , ACCESS_READ }, /* Subindex131 - InB1_OFF_DO_ReadyIndicator */
{ DEFTYPE_BOOLEAN , 0x01 , ACCESS_READ }, /* Subindex132 - InB1_OFF_DO_RTM_SystemShutDown */
{ DEFTYPE_BIT7 , 0x07 , ACCESS_READ }, /* Subindex133 - InB7_OFF_reserve3 */
{ DEFTYPE_UNSIGNED16 , 0x10 , ACCESS_READ }, /* Subindex134 - InU16_OFF_mcp23017_do_reserve */
{ DEFTYPE_BOOLEAN , 0x01 , ACCESS_READ }, /* Subindex135 - InB1_OFF_DO_SoftwareHvEn */
{ DEFTYPE_BOOLEAN , 0x01 , ACCESS_READ }, /* Subindex136 - InB1_OFF_DO_SoftwareKVTreatmentEn */
{ DEFTYPE_BOOLEAN , 0x01 , ACCESS_READ }, /* Subindex137 - InB1_OFF_DO_SoftwareMVTreatmentEn */
{ DEFTYPE_BOOLEAN , 0x01 , ACCESS_READ }, /* Subindex138 - InB1_OFF_DO_ThreePhasePowerOn */
{ DEFTYPE_BOOLEAN , 0x01 , ACCESS_READ }, /* Subindex139 - InB1_OFF_DO_softwareMoveEN */
{ DEFTYPE_BOOLEAN , 0x01 , ACCESS_READ }, /* Subindex140 - InB1_OFF_DO_TreatmentMotionEnable */
{ DEFTYPE_BOOLEAN , 0x01 , ACCESS_READ }, /* Subindex141 - InB1_OFF_DO_AsuMotionEnable */
{ DEFTYPE_BOOLEAN , 0x01 , ACCESS_READ }, /* Subindex142 - InB1_OFF_reserve4 */
{ DEFTYPE_UNSIGNED8 , 0x08 , ACCESS_READ }, /* Subindex143 - InU8_OFF_reserve5 */
{ DEFTYPE_UNSIGNED8 , 0x08 , ACCESS_READ }, /* Subindex144 - InU8_icm_fsm_state_current */
{ DEFTYPE_UNSIGNED8 , 0x08 , ACCESS_READ }, /* Subindex145 - InU8_icm_ctrl_mode_cur */
{ DEFTYPE_UNSIGNED16 , 0x10 , ACCESS_READ }, /* Subindex146 - InU16_reserved4 */
{ DEFTYPE_UNSIGNED8 , 0x08 , ACCESS_READ }, /* Subindex147 - InU8_icm_beam_id */
{ DEFTYPE_UNSIGNED8 , 0x08 , ACCESS_READ }, /* Subindex148 - InU8_reserved5 */
{ DEFTYPE_UNSIGNED16 , 0x10 , ACCESS_READ }, /* Subindex149 - InU16_icm_radiation_index */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex150 - InU32_icm_not_ready_event */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex151 - InU32_icm_warning_interlock */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex152 - InU32_icm_minor_interlock */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex153 - InU32_icm_serious_interlock */
{ DEFTYPE_UNSIGNED16 , 0x10 , ACCESS_READ }, /* Subindex154 - InU16_TotalStep_ICM */
{ DEFTYPE_UNSIGNED16 , 0x10 , ACCESS_READ }, /* Subindex155 - InU16_CurrentStep_ICM */
{ DEFTYPE_UNSIGNED16 , 0x10 , ACCESS_READ }, /* Subindex156 - InU16_ErrorCode_ICM */
{ DEFTYPE_UNSIGNED8 , 0x08 , ACCESS_READ }, /* Subindex157 - InU8_bgm_fsm_state_current */
{ DEFTYPE_UNSIGNED8 , 0x08 , ACCESS_READ }, /* Subindex158 - InU8_bgm_ctrl_mode_cur */
{ DEFTYPE_UNSIGNED16 , 0x10 , ACCESS_READ }, /* Subindex159 - InU16_reserved6 */
{ DEFTYPE_UNSIGNED8 , 0x08 , ACCESS_READ }, /* Subindex160 - InU8_bgm_beam_id */
{ DEFTYPE_UNSIGNED8 , 0x08 , ACCESS_READ }, /* Subindex161 - InU8_reserved7 */
{ DEFTYPE_UNSIGNED16 , 0x10 , ACCESS_READ }, /* Subindex162 - InU16_bgm_radiation_index */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex163 - InU32_bgm_not_ready_event */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex164 - InU32_bgm_warning_interlock */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex165 - InU32_bgm_minor_interlock */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex166 - InU32_bgm_serious_interlock */
{ DEFTYPE_UNSIGNED16 , 0x10 , ACCESS_READ }, /* Subindex167 - InU16_TotalStep_BGM */
{ DEFTYPE_UNSIGNED16 , 0x10 , ACCESS_READ }, /* Subindex168 - InU16_CurrentStep_BGM */
{ DEFTYPE_UNSIGNED16 , 0x10 , ACCESS_READ }, /* Subindex169 - InU16_ErrorCode_BGM */
{ DEFTYPE_REAL32 , 0x20 , ACCESS_READ }, /* Subindex170 - InF_beam_on_time */
{ DEFTYPE_REAL32 , 0x20 , ACCESS_READ }, /* Subindex171 - InF_primary_dose_current */
{ DEFTYPE_REAL32 , 0x20 , ACCESS_READ }, /* Subindex172 - InF_primary_dose_rate_current */
{ DEFTYPE_UNSIGNED8 , 0x08 , ACCESS_READ }, /* Subindex173 - InU8_primary_dose_fsm_state */
{ DEFTYPE_UNSIGNED8 , 0x08 , ACCESS_READ }, /* Subindex174 - InU8_reserved8 */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex175 - InU32_primary_dose_interlock */
{ DEFTYPE_REAL32 , 0x20 , ACCESS_READ }, /* Subindex176 - InF_second_dose_current */
{ DEFTYPE_REAL32 , 0x20 , ACCESS_READ }, /* Subindex177 - InF_secondry_dose_rate_current */
{ DEFTYPE_UNSIGNED8 , 0x08 , ACCESS_READ }, /* Subindex178 - InU8_secondry_dose_fsm_rate */
{ DEFTYPE_UNSIGNED8 , 0x08 , ACCESS_READ }, /* Subindex179 - InU8_reserved9 */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex180 - InU32_secondry_dose_interlock */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex181 - InU32_afc_interlock */
{ DEFTYPE_REAL32 , 0x20 , ACCESS_READ }, /* Subindex182 - InF_afc_position_current */
{ DEFTYPE_UNSIGNED8 , 0x08 , ACCESS_READ }, /* Subindex183 - InU8_qam_fsm_state_current */
{ DEFTYPE_UNSIGNED8 , 0x08 , ACCESS_READ }, /* Subindex184 - InU8_qam_ctrl_mode_cur */
{ DEFTYPE_UNSIGNED16 , 0x10 , ACCESS_READ }, /* Subindex185 - InU16_reserved10 */
{ DEFTYPE_UNSIGNED8 , 0x08 , ACCESS_READ }, /* Subindex186 - InU8_qam_beam_id */
{ DEFTYPE_UNSIGNED8 , 0x08 , ACCESS_READ }, /* Subindex187 - InU8_reserved11 */
{ DEFTYPE_UNSIGNED16 , 0x10 , ACCESS_READ }, /* Subindex188 - InU16_qam_radiation_index */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex189 - InU32_qam_not_ready_event */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex190 - InU32_qam_warning_interlock */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex191 - InU32_qam_minor_interlock */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex192 - InU32_qam_serious_interlock */
{ DEFTYPE_UNSIGNED16 , 0x10 , ACCESS_READ }, /* Subindex193 - InU16_TotalStep_QAM */
{ DEFTYPE_UNSIGNED16 , 0x10 , ACCESS_READ }, /* Subindex194 - InU16_CurrentStep_QAM */
{ DEFTYPE_UNSIGNED16 , 0x10 , ACCESS_READ }, /* Subindex195 - InU16_ErrorCode_QAM */
{ DEFTYPE_UNSIGNED8 , 0x08 , ACCESS_READ }, /* Subindex196 - InU8_rtm_off_fsm_state_current */
{ DEFTYPE_UNSIGNED8 , 0x08 , ACCESS_READ }, /* Subindex197 - InU8_rtm_off_ctrl_mode_cur */
{ DEFTYPE_UNSIGNED16 , 0x10 , ACCESS_READ }, /* Subindex198 - InU16_reserved12 */
{ DEFTYPE_UNSIGNED8 , 0x08 , ACCESS_READ }, /* Subindex199 - InU8_rtm_off_beam_id */
{ DEFTYPE_UNSIGNED8 , 0x08 , ACCESS_READ }, /* Subindex200 - InU8_reserved13 */
{ DEFTYPE_UNSIGNED16 , 0x10 , ACCESS_READ }, /* Subindex201 - InU16_rtm_off_radiation_index */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex202 - InU32_rtm_off_not_ready_event */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex203 - InU32_rtm_off_warning_interlock */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex204 - InU32_rtm_off_minor_interlock */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex205 - InU32_rtm_off_serious_interlock */
{ DEFTYPE_UNSIGNED16 , 0x10 , ACCESS_READ }, /* Subindex206 - InU16_TotalStep_RTM_OFF_ARM */
{ DEFTYPE_UNSIGNED16 , 0x10 , ACCESS_READ }, /* Subindex207 - InU16_CurrentStep_RTM_OFF_ARM */
{ DEFTYPE_UNSIGNED16 , 0x10 , ACCESS_READ }, /* Subindex208 - InU16_ErrorCode_RTM_OFF_ARM */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex209 - InU32_rtm_off_search_timeout_cur */
{ DEFTYPE_UNSIGNED8 , 0x08 , ACCESS_READ }, /* Subindex210 - InU8_gmm_fsm_state_current */
{ DEFTYPE_UNSIGNED8 , 0x08 , ACCESS_READ }, /* Subindex211 - InU8_gmm_ctrl_mode_cur */
{ DEFTYPE_UNSIGNED16 , 0x10 , ACCESS_READ }, /* Subindex212 - InU16_reserved14 */
{ DEFTYPE_UNSIGNED8 , 0x08 , ACCESS_READ }, /* Subindex213 - InU8_gmm_beam_id */
{ DEFTYPE_UNSIGNED8 , 0x08 , ACCESS_READ }, /* Subindex214 - InU8_reserved15 */
{ DEFTYPE_UNSIGNED16 , 0x10 , ACCESS_READ }, /* Subindex215 - InU16_gmm_radiation_index */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex216 - InU32_gmm_not_ready_event */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex217 - InU32_gmm_warning_interlock */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex218 - InU32_gmm_minor_interlock */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex219 - InU32_gmm_serious_interlock */
{ DEFTYPE_UNSIGNED16 , 0x10 , ACCESS_READ }, /* Subindex220 - InU16_TotalStep_GMM */
{ DEFTYPE_UNSIGNED16 , 0x10 , ACCESS_READ }, /* Subindex221 - InU16_CurrentStep_GMM */
{ DEFTYPE_UNSIGNED16 , 0x10 , ACCESS_READ }, /* Subindex222 - InU16_ErrorCode_GMM */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex223 - InU32_gmm_move_status */
{ DEFTYPE_REAL32 , 0x20 , ACCESS_READ }, /* Subindex224 - InF_gmm_position_cur */
{ DEFTYPE_REAL32 , 0x20 , ACCESS_READ }, /* Subindex225 - InF_gmm_velocity_cur */
{ DEFTYPE_UNSIGNED8 , 0x08 , ACCESS_READ }, /* Subindex226 - InU8_psm_fsm_state_current */
{ DEFTYPE_UNSIGNED8 , 0x08 , ACCESS_READ }, /* Subindex227 - InU8_psm_ctrl_mode_cur */
{ DEFTYPE_UNSIGNED16 , 0x10 , ACCESS_READ }, /* Subindex228 - InU16_reserved16 */
{ DEFTYPE_UNSIGNED8 , 0x08 , ACCESS_READ }, /* Subindex229 - InU8_psm_beam_id */
{ DEFTYPE_UNSIGNED8 , 0x08 , ACCESS_READ }, /* Subindex230 - InU8_reserved17 */
{ DEFTYPE_UNSIGNED16 , 0x10 , ACCESS_READ }, /* Subindex231 - InU16_psm_radiation_index */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex232 - InU32_psm_not_ready_event */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex233 - InU32_psm_warning_interlock */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex234 - InU32_psm_minor_interlock */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex235 - InU32_psm_serious_interlock */
{ DEFTYPE_UNSIGNED16 , 0x10 , ACCESS_READ }, /* Subindex236 - InU16_TotalStep_PSM */
{ DEFTYPE_UNSIGNED16 , 0x10 , ACCESS_READ }, /* Subindex237 - InU16_CurrentStep_PSM */
{ DEFTYPE_UNSIGNED16 , 0x10 , ACCESS_READ }, /* Subindex238 - InU16_ErrorCode_PSM */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex239 - InU32_psm_move_status */
{ DEFTYPE_REAL32 , 0x20 , ACCESS_READ }, /* Subindex240 - InF_psm_position_x_cur */
{ DEFTYPE_REAL32 , 0x20 , ACCESS_READ }, /* Subindex241 - InF_psm_position_y_cur */
{ DEFTYPE_REAL32 , 0x20 , ACCESS_READ }, /* Subindex242 - InF_psm_position_z_cur */
{ DEFTYPE_REAL32 , 0x20 , ACCESS_READ }, /* Subindex243 - InF_psm_position_x_r_cur */
{ DEFTYPE_REAL32 , 0x20 , ACCESS_READ }, /* Subindex244 - InF_psm_position_y_r_cur */
{ DEFTYPE_REAL32 , 0x20 , ACCESS_READ }, /* Subindex245 - InF_psm_position_z_r_cur */
{ DEFTYPE_REAL32 , 0x20 , ACCESS_READ }, /* Subindex246 - InF_psm_velocity_x_cur */
{ DEFTYPE_REAL32 , 0x20 , ACCESS_READ }, /* Subindex247 - InF_psm_velocity_y_cur */
{ DEFTYPE_REAL32 , 0x20 , ACCESS_READ }, /* Subindex248 - InF_psm_velocity_z_cur */
{ DEFTYPE_REAL32 , 0x20 , ACCESS_READ }, /* Subindex249 - InF_psm_velocity_x_r_cur */
{ DEFTYPE_REAL32 , 0x20 , ACCESS_READ }, /* Subindex250 - InF_psm_velocity_y_r_cur */
{ DEFTYPE_REAL32 , 0x20 , ACCESS_READ }, /* Subindex251 - InF_psm_velocity_z_r_cur */
{ DEFTYPE_UNSIGNED8 , 0x08 , ACCESS_READ }, /* Subindex252 - InU8_function_button */
{ DEFTYPE_UNSIGNED8 , 0x08 , ACCESS_READ }}; /* Subindex253 - InU8_special_button */

/**
* \brief Object/Entry names
*/
OBJCONST UCHAR OBJMEM aName0x6000[] = "InputData\000"
"InU8_BoardID_RTM_ON_ARM\000"
"InU8_Reserved_RTM_ON_ARM\000"
"InU32_FirmWareVersion_RTM_ON_ARM\000"
"InU8_BoardID_ICM\000"
"InU8_Reserved_ICM\000"
"InU32_FirmWareVersion_ICM\000"
"InU8_BoardID_BGM\000"
"InU8_Reserved_BGM\000"
"InU32_FirmWareVersion_BGM\000"
"InU8_BoardID_primary_dose\000"
"InU8_Reserved_primary_dose\000"
"InU32_FirmWareVersion_primary_dose\000"
"InU8_BoardID_second_dose\000"
"InU8_Reserved_second_dose\000"
"InU32_FirmWareVersion_second_dose\000"
"InU8_BoardID_AFC\000"
"InU8_Reserved_AFC\000"
"InU32_FirmWareVersion_AFC\000"
"InU8_BoardID_QAM\000"
"InU8_Reserved_QAM\000"
"InU32_FirmWareVersion_QAM\000"
"InU8_BoardID_RTM_OFF_ARM\000"
"InU8_Reserved_RTM_OFF_ARM\000"
"InU32_FirmWareVersion_RTM_OFF_ARM\000"
"InU8_BoardID_PSM\000"
"InU8_Reserved_PSM\000"
"InU32_FirmWareVersion_PSM\000"
"InU8_BoardID_GMM\000"
"InU8_Reserved_GMM\000"
"InU32_FirmWareVersion_GMM\000"
"InU8_BoardID_FKP\000"
"InU8_Reserved_FKP\000"
"InU32_FirmWareVersion_FKP\000"
"InU8_BoardID_CPG_L\000"
"InU8_Reserved_CPG_L\000"
"InU32_FirmWareVersion_CPG_L\000"
"InU8_BoardID_CPG_R\000"
"InU8_Reserved_CPG_R\000"
"InU32_FirmWareVersion_CPG_R\000"
"InU8_beam_id\000"
"InU8_reserved1\000"
"InU16_radiation_index\000"
"InU8_rtm_on_arm_fsm_state_current\000"
"InU8_rtm_on_arm_ctrl_mode_cur\000"
"InU8_ethercat_Link_state\000"
"InU8_reserved2\000"
"InU8_rtm_on_arm_beam_id\000"
"InU8_reserved3\000"
"InU16_rtm_on_arm_radiation_index\000"
"InU32_rtm_on_arm_not_ready_event\000"
"InU32_rtm_on_arm_warning_interlock\000"
"InU32_rtm_on_arm_minor_interlock\000"
"InU32_rtm_on_arm_serious_interlock\000"
"InU16_TotalStep_RTM_ON_ARM\000"
"InU16_CurrentStep_RTM_ON_ARM\000"
"InU16_ErrorCode_RTM_ON_ARM\000"
"InB1_ON_DI_RTC_WD_OK_IN\000"
"InB1_ON_DI_Reserve0\000"
"InB1_ON_DI_BSM_NOT_READY\000"
"InB1_ON_DI_MV_TreatmentEN\000"
"InB1_ON_DI_DI_HVEN\000"
"InB1_ON_DI_Pulse_Inhibit\000"
"InB1_ON_DI_KV_TreatmentEN\000"
"InB1_ON_DI_Power_cut_FB\000"
"InB1_ON_DI_GATING_IN\000"
"InB1_ON_DI_Slipring_HVEN_IN\000"
"InB1_ON_DI_Slipring_KV_TreatmentEN_IN\000"
"InB1_ON_DI_Slipring_MV_TreatmentEN_IN\000"
"InB4_ON_DI_reserve1\000"
"InB1_ON_DO_reserve0\000"
"InB1_ON_DO_MV_TreatmentEN\000"
"InB1_ON_DO_KV_TreatmentEN\000"
"InB1_ON_DO_Emergency\000"
"InB1_ON_DO_Pulse_Inhibit\000"
"InB1_ON_DO_HVEN\000"
"InB1_ON_DO_PowerCut\000"
"InB1_ON_DO_reserve1\000"
"InU8_ON_DO_reserve2\000"
"InU8_OFF_reserve0\000"
"InB1_OFF_DI_CITB_EMERGENCY2\000"
"InB1_OFF_DI_UPS_LOAD_PORT\000"
"InB1_OFF_DI_STAND_BREAKER1\000"
"InB1_OFF_DI_STAND_EMERGENCY\000"
"InB1_OFF_DI_TouchGuard\000"
"InB1_OFF_DI_HvEn\000"
"InB1_OFF_DI_MV_TreatmentEN\000"
"InB1_OFF_DI_KV_TreatmentEN\000"
"InB1_OFF_DI_STAND_BREAKER7\000"
"InB1_OFF_DI_STAND_BREAKER3\000"
"InB1_OFF_DI_UPS_ON_BYPASS\000"
"InB1_OFF_DI_STAND_BREAKER5\000"
"InB1_OFF_DI_CITB_EMERGENCY3\000"
"InB1_OFF_DI_COVER3\000"
"InB1_OFF_DI_POWER_CUT\000"
"InB1_OFF_DI_STAND_BREAKER4\000"
"InB1_OFF_DI_CITB_TREATMENT_ROOM_DOOR1\000"
"InB1_OFF_DI_CITB_TREATMENT_ROOM_DOOR2\000"
"InB1_OFF_DI_CITB_EMERGENCY1\000"
"InB1_OFF_DI_CITB_SEARCH_TREATMENT_ROOM\000"
"InB1_OFF_DI_CONTROL_ROOM_BREAKER1\000"
"InB1_OFF_DI_UPS_ON_BATT\000"
"InB1_OFF_DI_CONTROL_ROOM_BREAKER5\000"
"InB1_OFF_DI_STAND_BREAKER6\000"
"InB1_OFF_DI_CITB_EMERGENCY5\000"
"InB1_OFF_DI_COVER1\000"
"InB1_OFF_DI_STAND_RESERVE\000"
"InB1_OFF_DI_TREATMENT_ROOM_DOOR_READY\000"
"InB1_OFF_DI_COVER2\000"
"InB1_OFF_DI_CONTROL_ROOM_BREAKER2\000"
"InB1_OFF_DI_CONTROL_ROOM_BREAKER4\000"
"InB1_OFF_DI_STAND_CONTACTOR2\000"
"InB1_OFF_DI_STAND_CONTACTOR1\000"
"InB1_OFF_DI_UPS_LOW_BATT\000"
"InB1_OFF_DI_HvKey\000"
"InB1_OFF_DI_COVER4\000"
"InB1_OFF_DI_CITB_EMERGENCY4\000"
"InB1_OFF_DI_CONTROL_ROOM_BREAKER3\000"
"InB1_OFF_DI_STAND_BREAKER2\000"
"InB1_OFF_RTC_WD_OK_IN\000"
"InU16_OFF_mcp23017_di_reserve\000"
"InB1_OFF_DI_GATING\000"
"InB7_OFF_reserve1\000"
"InU8_OFF_reserve2\000"
"InB1_OFF_DO_RTM_AutoPowerUp\000"
"InB1_OFF_DO_SearchTreatmentRoomRelay\000"
"InB1_OFF_DO_Laser\000"
"InB1_OFF_DO_Power_CUT\000"
"InB1_OFF_DO_STAND_RESERVE\000"
"InB1_OFF_DO_TreatmentRoomLight\000"
"InB1_OFF_DO_RadiationIndicator\000"
"InB1_OFF_DO_ReadyIndicator\000"
"InB1_OFF_DO_RTM_SystemShutDown\000"
"InB7_OFF_reserve3\000"
"InU16_OFF_mcp23017_do_reserve\000"
"InB1_OFF_DO_SoftwareHvEn\000"
"InB1_OFF_DO_SoftwareKVTreatmentEn\000"
"InB1_OFF_DO_SoftwareMVTreatmentEn\000"
"InB1_OFF_DO_ThreePhasePowerOn\000"
"InB1_OFF_DO_softwareMoveEN\000"
"InB1_OFF_DO_TreatmentMotionEnable\000"
"InB1_OFF_DO_AsuMotionEnable\000"
"InB1_OFF_reserve4\000"
"InU8_OFF_reserve5\000"
"InU8_icm_fsm_state_current\000"
"InU8_icm_ctrl_mode_cur\000"
"InU16_reserved4\000"
"InU8_icm_beam_id\000"
"InU8_reserved5\000"
"InU16_icm_radiation_index\000"
"InU32_icm_not_ready_event\000"
"InU32_icm_warning_interlock\000"
"InU32_icm_minor_interlock\000"
"InU32_icm_serious_interlock\000"
"InU16_TotalStep_ICM\000"
"InU16_CurrentStep_ICM\000"
"InU16_ErrorCode_ICM\000"
"InU8_bgm_fsm_state_current\000"
"InU8_bgm_ctrl_mode_cur\000"
"InU16_reserved6\000"
"InU8_bgm_beam_id\000"
"InU8_reserved7\000"
"InU16_bgm_radiation_index\000"
"InU32_bgm_not_ready_event\000"
"InU32_bgm_warning_interlock\000"
"InU32_bgm_minor_interlock\000"
"InU32_bgm_serious_interlock\000"
"InU16_TotalStep_BGM\000"
"InU16_CurrentStep_BGM\000"
"InU16_ErrorCode_BGM\000"
"InF_beam_on_time\000"
"InF_primary_dose_current\000"
"InF_primary_dose_rate_current\000"
"InU8_primary_dose_fsm_state\000"
"InU8_reserved8\000"
"InU32_primary_dose_interlock\000"
"InF_second_dose_current\000"
"InF_secondry_dose_rate_current\000"
"InU8_secondry_dose_fsm_rate\000"
"InU8_reserved9\000"
"InU32_secondry_dose_interlock\000"
"InU32_afc_interlock\000"
"InF_afc_position_current\000"
"InU8_qam_fsm_state_current\000"
"InU8_qam_ctrl_mode_cur\000"
"InU16_reserved10\000"
"InU8_qam_beam_id\000"
"InU8_reserved11\000"
"InU16_qam_radiation_index\000"
"InU32_qam_not_ready_event\000"
"InU32_qam_warning_interlock\000"
"InU32_qam_minor_interlock\000"
"InU32_qam_serious_interlock\000"
"InU16_TotalStep_QAM\000"
"InU16_CurrentStep_QAM\000"
"InU16_ErrorCode_QAM\000"
"InU8_rtm_off_fsm_state_current\000"
"InU8_rtm_off_ctrl_mode_cur\000"
"InU16_reserved12\000"
"InU8_rtm_off_beam_id\000"
"InU8_reserved13\000"
"InU16_rtm_off_radiation_index\000"
"InU32_rtm_off_not_ready_event\000"
"InU32_rtm_off_warning_interlock\000"
"InU32_rtm_off_minor_interlock\000"
"InU32_rtm_off_serious_interlock\000"
"InU16_TotalStep_RTM_OFF_ARM\000"
"InU16_CurrentStep_RTM_OFF_ARM\000"
"InU16_ErrorCode_RTM_OFF_ARM\000"
"InU32_rtm_off_search_timeout_cur\000"
"InU8_gmm_fsm_state_current\000"
"InU8_gmm_ctrl_mode_cur\000"
"InU16_reserved14\000"
"InU8_gmm_beam_id\000"
"InU8_reserved15\000"
"InU16_gmm_radiation_index\000"
"InU32_gmm_not_ready_event\000"
"InU32_gmm_warning_interlock\000"
"InU32_gmm_minor_interlock\000"
"InU32_gmm_serious_interlock\000"
"InU16_TotalStep_GMM\000"
"InU16_CurrentStep_GMM\000"
"InU16_ErrorCode_GMM\000"
"InU32_gmm_move_status\000"
"InF_gmm_position_cur\000"
"InF_gmm_velocity_cur\000"
"InU8_psm_fsm_state_current\000"
"InU8_psm_ctrl_mode_cur\000"
"InU16_reserved16\000"
"InU8_psm_beam_id\000"
"InU8_reserved17\000"
"InU16_psm_radiation_index\000"
"InU32_psm_not_ready_event\000"
"InU32_psm_warning_interlock\000"
"InU32_psm_minor_interlock\000"
"InU32_psm_serious_interlock\000"
"InU16_TotalStep_PSM\000"
"InU16_CurrentStep_PSM\000"
"InU16_ErrorCode_PSM\000"
"InU32_psm_move_status\000"
"InF_psm_position_x_cur\000"
"InF_psm_position_y_cur\000"
"InF_psm_position_z_cur\000"
"InF_psm_position_x_r_cur\000"
"InF_psm_position_y_r_cur\000"
"InF_psm_position_z_r_cur\000"
"InF_psm_velocity_x_cur\000"
"InF_psm_velocity_y_cur\000"
"InF_psm_velocity_z_cur\000"
"InF_psm_velocity_x_r_cur\000"
"InF_psm_velocity_y_r_cur\000"
"InF_psm_velocity_z_r_cur\000"
"InU8_function_button\000"
"InU8_special_button\000\377";
#endif //#ifdef _OBJD_

#ifndef _LAN9252_APP_OBJECTS_H_
/**
* \brief Object structure
*/
typedef struct OBJ_STRUCT_PACKED_START {
UINT16 u16SubIndex0;
UINT8 InU8_BoardID_RTM_ON_ARM; /* Subindex1 - InU8_BoardID_RTM_ON_ARM */
UINT8 InU8_Reserved_RTM_ON_ARM; /* Subindex2 - InU8_Reserved_RTM_ON_ARM */
UINT32 InU32_FirmWareVersion_RTM_ON_ARM; /* Subindex3 - InU32_FirmWareVersion_RTM_ON_ARM */
UINT8 InU8_BoardID_ICM; /* Subindex4 - InU8_BoardID_ICM */
UINT8 InU8_Reserved_ICM; /* Subindex5 - InU8_Reserved_ICM */
UINT32 InU32_FirmWareVersion_ICM; /* Subindex6 - InU32_FirmWareVersion_ICM */
UINT8 InU8_BoardID_BGM; /* Subindex7 - InU8_BoardID_BGM */
UINT8 InU8_Reserved_BGM; /* Subindex8 - InU8_Reserved_BGM */
UINT32 InU32_FirmWareVersion_BGM; /* Subindex9 - InU32_FirmWareVersion_BGM */
UINT8 InU8_BoardID_primary_dose; /* Subindex10 - InU8_BoardID_primary_dose */
UINT8 InU8_Reserved_primary_dose; /* Subindex11 - InU8_Reserved_primary_dose */
UINT32 InU32_FirmWareVersion_primary_dose; /* Subindex12 - InU32_FirmWareVersion_primary_dose */
UINT8 InU8_BoardID_second_dose; /* Subindex13 - InU8_BoardID_second_dose */
UINT8 InU8_Reserved_second_dose; /* Subindex14 - InU8_Reserved_second_dose */
UINT32 InU32_FirmWareVersion_second_dose; /* Subindex15 - InU32_FirmWareVersion_second_dose */
UINT8 InU8_BoardID_AFC; /* Subindex16 - InU8_BoardID_AFC */
UINT8 InU8_Reserved_AFC; /* Subindex17 - InU8_Reserved_AFC */
UINT32 InU32_FirmWareVersion_AFC; /* Subindex18 - InU32_FirmWareVersion_AFC */
UINT8 InU8_BoardID_QAM; /* Subindex19 - InU8_BoardID_QAM */
UINT8 InU8_Reserved_QAM; /* Subindex20 - InU8_Reserved_QAM */
UINT32 InU32_FirmWareVersion_QAM; /* Subindex21 - InU32_FirmWareVersion_QAM */
UINT8 InU8_BoardID_RTM_OFF_ARM; /* Subindex22 - InU8_BoardID_RTM_OFF_ARM */
UINT8 InU8_Reserved_RTM_OFF_ARM; /* Subindex23 - InU8_Reserved_RTM_OFF_ARM */
UINT32 InU32_FirmWareVersion_RTM_OFF_ARM; /* Subindex24 - InU32_FirmWareVersion_RTM_OFF_ARM */
UINT8 InU8_BoardID_PSM; /* Subindex25 - InU8_BoardID_PSM */
UINT8 InU8_Reserved_PSM; /* Subindex26 - InU8_Reserved_PSM */
UINT32 InU32_FirmWareVersion_PSM; /* Subindex27 - InU32_FirmWareVersion_PSM */
UINT8 InU8_BoardID_GMM; /* Subindex28 - InU8_BoardID_GMM */
UINT8 InU8_Reserved_GMM; /* Subindex29 - InU8_Reserved_GMM */
UINT32 InU32_FirmWareVersion_GMM; /* Subindex30 - InU32_FirmWareVersion_GMM */
UINT8 InU8_BoardID_FKP; /* Subindex31 - InU8_BoardID_FKP */
UINT8 InU8_Reserved_FKP; /* Subindex32 - InU8_Reserved_FKP */
UINT32 InU32_FirmWareVersion_FKP; /* Subindex33 - InU32_FirmWareVersion_FKP */
UINT8 InU8_BoardID_CPG_L; /* Subindex34 - InU8_BoardID_CPG_L */
UINT8 InU8_Reserved_CPG_L; /* Subindex35 - InU8_Reserved_CPG_L */
UINT32 InU32_FirmWareVersion_CPG_L; /* Subindex36 - InU32_FirmWareVersion_CPG_L */
UINT8 InU8_BoardID_CPG_R; /* Subindex37 - InU8_BoardID_CPG_R */
UINT8 InU8_Reserved_CPG_R; /* Subindex38 - InU8_Reserved_CPG_R */
UINT32 InU32_FirmWareVersion_CPG_R; /* Subindex39 - InU32_FirmWareVersion_CPG_R */
UINT8 InU8_beam_id; /* Subindex40 - InU8_beam_id */
UINT8 InU8_reserved1; /* Subindex41 - InU8_reserved1 */
UINT16 InU16_radiation_index; /* Subindex42 - InU16_radiation_index */
UINT8 InU8_rtm_on_arm_fsm_state_current; /* Subindex43 - InU8_rtm_on_arm_fsm_state_current */
UINT8 InU8_rtm_on_arm_ctrl_mode_cur; /* Subindex44 - InU8_rtm_on_arm_ctrl_mode_cur */
UINT8 InU8_ethercat_Link_state; /* Subindex45 - InU8_ethercat_Link_state */
UINT8 InU8_reserved2; /* Subindex46 - InU8_reserved2 */
UINT8 InU8_rtm_on_arm_beam_id; /* Subindex47 - InU8_rtm_on_arm_beam_id */
UINT8 InU8_reserved3; /* Subindex48 - InU8_reserved3 */
UINT16 InU16_rtm_on_arm_radiation_index; /* Subindex49 - InU16_rtm_on_arm_radiation_index */
UINT32 InU32_rtm_on_arm_not_ready_event; /* Subindex50 - InU32_rtm_on_arm_not_ready_event */
UINT32 InU32_rtm_on_arm_warning_interlock; /* Subindex51 - InU32_rtm_on_arm_warning_interlock */
UINT32 InU32_rtm_on_arm_minor_interlock; /* Subindex52 - InU32_rtm_on_arm_minor_interlock */
UINT32 InU32_rtm_on_arm_serious_interlock; /* Subindex53 - InU32_rtm_on_arm_serious_interlock */
UINT16 InU16_TotalStep_RTM_ON_ARM; /* Subindex54 - InU16_TotalStep_RTM_ON_ARM */
UINT16 InU16_CurrentStep_RTM_ON_ARM; /* Subindex55 - InU16_CurrentStep_RTM_ON_ARM */
UINT16 InU16_ErrorCode_RTM_ON_ARM; /* Subindex56 - InU16_ErrorCode_RTM_ON_ARM */
BOOLEAN(InB1_ON_DI_RTC_WD_OK_IN); /* Subindex57 - InB1_ON_DI_RTC_WD_OK_IN */
BOOLEAN(InB1_ON_DI_Reserve0); /* Subindex58 - InB1_ON_DI_Reserve0 */
BOOLEAN(InB1_ON_DI_BSM_NOT_READY); /* Subindex59 - InB1_ON_DI_BSM_NOT_READY */
BOOLEAN(InB1_ON_DI_MV_TreatmentEN); /* Subindex60 - InB1_ON_DI_MV_TreatmentEN */
BOOLEAN(InB1_ON_DI_DI_HVEN); /* Subindex61 - InB1_ON_DI_DI_HVEN */
BOOLEAN(InB1_ON_DI_Pulse_Inhibit); /* Subindex62 - InB1_ON_DI_Pulse_Inhibit */
BOOLEAN(InB1_ON_DI_KV_TreatmentEN); /* Subindex63 - InB1_ON_DI_KV_TreatmentEN */
BOOLEAN(InB1_ON_DI_Power_cut_FB); /* Subindex64 - InB1_ON_DI_Power_cut_FB */
BOOLEAN(InB1_ON_DI_GATING_IN); /* Subindex65 - InB1_ON_DI_GATING_IN */
BOOLEAN(InB1_ON_DI_Slipring_HVEN_IN); /* Subindex66 - InB1_ON_DI_Slipring_HVEN_IN */
BOOLEAN(InB1_ON_DI_Slipring_KV_TreatmentEN_IN); /* Subindex67 - InB1_ON_DI_Slipring_KV_TreatmentEN_IN */
BOOLEAN(InB1_ON_DI_Slipring_MV_TreatmentEN_IN); /* Subindex68 - InB1_ON_DI_Slipring_MV_TreatmentEN_IN */
BIT4(InB4_ON_DI_reserve1); /* Subindex69 - InB4_ON_DI_reserve1 */
BOOLEAN(InB1_ON_DO_reserve0); /* Subindex70 - InB1_ON_DO_reserve0 */
BOOLEAN(InB1_ON_DO_MV_TreatmentEN); /* Subindex71 - InB1_ON_DO_MV_TreatmentEN */
BOOLEAN(InB1_ON_DO_KV_TreatmentEN); /* Subindex72 - InB1_ON_DO_KV_TreatmentEN */
BOOLEAN(InB1_ON_DO_Emergency); /* Subindex73 - InB1_ON_DO_Emergency */
BOOLEAN(InB1_ON_DO_Pulse_Inhibit); /* Subindex74 - InB1_ON_DO_Pulse_Inhibit */
BOOLEAN(InB1_ON_DO_HVEN); /* Subindex75 - InB1_ON_DO_HVEN */
BOOLEAN(InB1_ON_DO_PowerCut); /* Subindex76 - InB1_ON_DO_PowerCut */
BOOLEAN(InB1_ON_DO_reserve1); /* Subindex77 - InB1_ON_DO_reserve1 */
UINT8 InU8_ON_DO_reserve2; /* Subindex78 - InU8_ON_DO_reserve2 */
UINT8 InU8_OFF_reserve0; /* Subindex79 - InU8_OFF_reserve0 */
BOOLEAN(InB1_OFF_DI_CITB_EMERGENCY2); /* Subindex80 - InB1_OFF_DI_CITB_EMERGENCY2 */
BOOLEAN(InB1_OFF_DI_UPS_LOAD_PORT); /* Subindex81 - InB1_OFF_DI_UPS_LOAD_PORT */
BOOLEAN(InB1_OFF_DI_STAND_BREAKER1); /* Subindex82 - InB1_OFF_DI_STAND_BREAKER1 */
BOOLEAN(InB1_OFF_DI_STAND_EMERGENCY); /* Subindex83 - InB1_OFF_DI_STAND_EMERGENCY */
BOOLEAN(InB1_OFF_DI_TouchGuard); /* Subindex84 - InB1_OFF_DI_TouchGuard */
BOOLEAN(InB1_OFF_DI_HvEn); /* Subindex85 - InB1_OFF_DI_HvEn */
BOOLEAN(InB1_OFF_DI_MV_TreatmentEN); /* Subindex86 - InB1_OFF_DI_MV_TreatmentEN */
BOOLEAN(InB1_OFF_DI_KV_TreatmentEN); /* Subindex87 - InB1_OFF_DI_KV_TreatmentEN */
BOOLEAN(InB1_OFF_DI_STAND_BREAKER7); /* Subindex88 - InB1_OFF_DI_STAND_BREAKER7 */
BOOLEAN(InB1_OFF_DI_STAND_BREAKER3); /* Subindex89 - InB1_OFF_DI_STAND_BREAKER3 */
BOOLEAN(InB1_OFF_DI_UPS_ON_BYPASS); /* Subindex90 - InB1_OFF_DI_UPS_ON_BYPASS */
BOOLEAN(InB1_OFF_DI_STAND_BREAKER5); /* Subindex91 - InB1_OFF_DI_STAND_BREAKER5 */
BOOLEAN(InB1_OFF_DI_CITB_EMERGENCY3); /* Subindex92 - InB1_OFF_DI_CITB_EMERGENCY3 */
BOOLEAN(InB1_OFF_DI_COVER3); /* Subindex93 - InB1_OFF_DI_COVER3 */
BOOLEAN(InB1_OFF_DI_POWER_CUT); /* Subindex94 - InB1_OFF_DI_POWER_CUT */
BOOLEAN(InB1_OFF_DI_STAND_BREAKER4); /* Subindex95 - InB1_OFF_DI_STAND_BREAKER4 */
BOOLEAN(InB1_OFF_DI_CITB_TREATMENT_ROOM_DOOR1); /* Subindex96 - InB1_OFF_DI_CITB_TREATMENT_ROOM_DOOR1 */
BOOLEAN(InB1_OFF_DI_CITB_TREATMENT_ROOM_DOOR2); /* Subindex97 - InB1_OFF_DI_CITB_TREATMENT_ROOM_DOOR2 */
BOOLEAN(InB1_OFF_DI_CITB_EMERGENCY1); /* Subindex98 - InB1_OFF_DI_CITB_EMERGENCY1 */
BOOLEAN(InB1_OFF_DI_CITB_SEARCH_TREATMENT_ROOM); /* Subindex99 - InB1_OFF_DI_CITB_SEARCH_TREATMENT_ROOM */
BOOLEAN(InB1_OFF_DI_CONTROL_ROOM_BREAKER1); /* Subindex100 - InB1_OFF_DI_CONTROL_ROOM_BREAKER1 */
BOOLEAN(InB1_OFF_DI_UPS_ON_BATT); /* Subindex101 - InB1_OFF_DI_UPS_ON_BATT */
BOOLEAN(InB1_OFF_DI_CONTROL_ROOM_BREAKER5); /* Subindex102 - InB1_OFF_DI_CONTROL_ROOM_BREAKER5 */
BOOLEAN(InB1_OFF_DI_STAND_BREAKER6); /* Subindex103 - InB1_OFF_DI_STAND_BREAKER6 */
BOOLEAN(InB1_OFF_DI_CITB_EMERGENCY5); /* Subindex104 - InB1_OFF_DI_CITB_EMERGENCY5 */
BOOLEAN(InB1_OFF_DI_COVER1); /* Subindex105 - InB1_OFF_DI_COVER1 */
BOOLEAN(InB1_OFF_DI_STAND_RESERVE); /* Subindex106 - InB1_OFF_DI_STAND_RESERVE */
BOOLEAN(InB1_OFF_DI_TREATMENT_ROOM_DOOR_READY); /* Subindex107 - InB1_OFF_DI_TREATMENT_ROOM_DOOR_READY */
BOOLEAN(InB1_OFF_DI_COVER2); /* Subindex108 - InB1_OFF_DI_COVER2 */
BOOLEAN(InB1_OFF_DI_CONTROL_ROOM_BREAKER2); /* Subindex109 - InB1_OFF_DI_CONTROL_ROOM_BREAKER2 */
BOOLEAN(InB1_OFF_DI_CONTROL_ROOM_BREAKER4); /* Subindex110 - InB1_OFF_DI_CONTROL_ROOM_BREAKER4 */
BOOLEAN(InB1_OFF_DI_STAND_CONTACTOR2); /* Subindex111 - InB1_OFF_DI_STAND_CONTACTOR2 */
BOOLEAN(InB1_OFF_DI_STAND_CONTACTOR1); /* Subindex112 - InB1_OFF_DI_STAND_CONTACTOR1 */
BOOLEAN(InB1_OFF_DI_UPS_LOW_BATT); /* Subindex113 - InB1_OFF_DI_UPS_LOW_BATT */
BOOLEAN(InB1_OFF_DI_HvKey); /* Subindex114 - InB1_OFF_DI_HvKey */
BOOLEAN(InB1_OFF_DI_COVER4); /* Subindex115 - InB1_OFF_DI_COVER4 */
BOOLEAN(InB1_OFF_DI_CITB_EMERGENCY4); /* Subindex116 - InB1_OFF_DI_CITB_EMERGENCY4 */
BOOLEAN(InB1_OFF_DI_CONTROL_ROOM_BREAKER3); /* Subindex117 - InB1_OFF_DI_CONTROL_ROOM_BREAKER3 */
BOOLEAN(InB1_OFF_DI_STAND_BREAKER2); /* Subindex118 - InB1_OFF_DI_STAND_BREAKER2 */
BOOLEAN(InB1_OFF_RTC_WD_OK_IN); /* Subindex119 - InB1_OFF_RTC_WD_OK_IN */
UINT16 InU16_OFF_mcp23017_di_reserve; /* Subindex120 - InU16_OFF_mcp23017_di_reserve */
BOOLEAN(InB1_OFF_DI_GATING); /* Subindex121 - InB1_OFF_DI_GATING */
BIT7(InB7_OFF_reserve1); /* Subindex122 - InB7_OFF_reserve1 */
UINT8 InU8_OFF_reserve2; /* Subindex123 - InU8_OFF_reserve2 */
BOOLEAN(InB1_OFF_DO_RTM_AutoPowerUp); /* Subindex124 - InB1_OFF_DO_RTM_AutoPowerUp */
BOOLEAN(InB1_OFF_DO_SearchTreatmentRoomRelay); /* Subindex125 - InB1_OFF_DO_SearchTreatmentRoomRelay */
BOOLEAN(InB1_OFF_DO_Laser); /* Subindex126 - InB1_OFF_DO_Laser */
BOOLEAN(InB1_OFF_DO_Power_CUT); /* Subindex127 - InB1_OFF_DO_Power_CUT */
BOOLEAN(InB1_OFF_DO_STAND_RESERVE); /* Subindex128 - InB1_OFF_DO_STAND_RESERVE */
BOOLEAN(InB1_OFF_DO_TreatmentRoomLight); /* Subindex129 - InB1_OFF_DO_TreatmentRoomLight */
BOOLEAN(InB1_OFF_DO_RadiationIndicator); /* Subindex130 - InB1_OFF_DO_RadiationIndicator */
BOOLEAN(InB1_OFF_DO_ReadyIndicator); /* Subindex131 - InB1_OFF_DO_ReadyIndicator */
BOOLEAN(InB1_OFF_DO_RTM_SystemShutDown); /* Subindex132 - InB1_OFF_DO_RTM_SystemShutDown */
BIT7(InB7_OFF_reserve3); /* Subindex133 - InB7_OFF_reserve3 */
UINT16 InU16_OFF_mcp23017_do_reserve; /* Subindex134 - InU16_OFF_mcp23017_do_reserve */
BOOLEAN(InB1_OFF_DO_SoftwareHvEn); /* Subindex135 - InB1_OFF_DO_SoftwareHvEn */
BOOLEAN(InB1_OFF_DO_SoftwareKVTreatmentEn); /* Subindex136 - InB1_OFF_DO_SoftwareKVTreatmentEn */
BOOLEAN(InB1_OFF_DO_SoftwareMVTreatmentEn); /* Subindex137 - InB1_OFF_DO_SoftwareMVTreatmentEn */
BOOLEAN(InB1_OFF_DO_ThreePhasePowerOn); /* Subindex138 - InB1_OFF_DO_ThreePhasePowerOn */
BOOLEAN(InB1_OFF_DO_softwareMoveEN); /* Subindex139 - InB1_OFF_DO_softwareMoveEN */
BOOLEAN(InB1_OFF_DO_TreatmentMotionEnable); /* Subindex140 - InB1_OFF_DO_TreatmentMotionEnable */
BOOLEAN(InB1_OFF_DO_AsuMotionEnable); /* Subindex141 - InB1_OFF_DO_AsuMotionEnable */
BOOLEAN(InB1_OFF_reserve4); /* Subindex142 - InB1_OFF_reserve4 */
UINT8 InU8_OFF_reserve5; /* Subindex143 - InU8_OFF_reserve5 */
UINT8 InU8_icm_fsm_state_current; /* Subindex144 - InU8_icm_fsm_state_current */
UINT8 InU8_icm_ctrl_mode_cur; /* Subindex145 - InU8_icm_ctrl_mode_cur */
UINT16 InU16_reserved4; /* Subindex146 - InU16_reserved4 */
UINT8 InU8_icm_beam_id; /* Subindex147 - InU8_icm_beam_id */
UINT8 InU8_reserved5; /* Subindex148 - InU8_reserved5 */
UINT16 InU16_icm_radiation_index; /* Subindex149 - InU16_icm_radiation_index */
UINT32 InU32_icm_not_ready_event; /* Subindex150 - InU32_icm_not_ready_event */
UINT32 InU32_icm_warning_interlock; /* Subindex151 - InU32_icm_warning_interlock */
UINT32 InU32_icm_minor_interlock; /* Subindex152 - InU32_icm_minor_interlock */
UINT32 InU32_icm_serious_interlock; /* Subindex153 - InU32_icm_serious_interlock */
UINT16 InU16_TotalStep_ICM; /* Subindex154 - InU16_TotalStep_ICM */
UINT16 InU16_CurrentStep_ICM; /* Subindex155 - InU16_CurrentStep_ICM */
UINT16 InU16_ErrorCode_ICM; /* Subindex156 - InU16_ErrorCode_ICM */
UINT8 InU8_bgm_fsm_state_current; /* Subindex157 - InU8_bgm_fsm_state_current */
UINT8 InU8_bgm_ctrl_mode_cur; /* Subindex158 - InU8_bgm_ctrl_mode_cur */
UINT16 InU16_reserved6; /* Subindex159 - InU16_reserved6 */
UINT8 InU8_bgm_beam_id; /* Subindex160 - InU8_bgm_beam_id */
UINT8 InU8_reserved7; /* Subindex161 - InU8_reserved7 */
UINT16 InU16_bgm_radiation_index; /* Subindex162 - InU16_bgm_radiation_index */
UINT32 InU32_bgm_not_ready_event; /* Subindex163 - InU32_bgm_not_ready_event */
UINT32 InU32_bgm_warning_interlock; /* Subindex164 - InU32_bgm_warning_interlock */
UINT32 InU32_bgm_minor_interlock; /* Subindex165 - InU32_bgm_minor_interlock */
UINT32 InU32_bgm_serious_interlock; /* Subindex166 - InU32_bgm_serious_interlock */
UINT16 InU16_TotalStep_BGM; /* Subindex167 - InU16_TotalStep_BGM */
UINT16 InU16_CurrentStep_BGM; /* Subindex168 - InU16_CurrentStep_BGM */
UINT16 InU16_ErrorCode_BGM; /* Subindex169 - InU16_ErrorCode_BGM */
REAL32 InF_beam_on_time; /* Subindex170 - InF_beam_on_time */
REAL32 InF_primary_dose_current; /* Subindex171 - InF_primary_dose_current */
REAL32 InF_primary_dose_rate_current; /* Subindex172 - InF_primary_dose_rate_current */
UINT8 InU8_primary_dose_fsm_state; /* Subindex173 - InU8_primary_dose_fsm_state */
UINT8 InU8_reserved8; /* Subindex174 - InU8_reserved8 */
UINT32 InU32_primary_dose_interlock; /* Subindex175 - InU32_primary_dose_interlock */
REAL32 InF_second_dose_current; /* Subindex176 - InF_second_dose_current */
REAL32 InF_secondry_dose_rate_current; /* Subindex177 - InF_secondry_dose_rate_current */
UINT8 InU8_secondry_dose_fsm_rate; /* Subindex178 - InU8_secondry_dose_fsm_rate */
UINT8 InU8_reserved9; /* Subindex179 - InU8_reserved9 */
UINT32 InU32_secondry_dose_interlock; /* Subindex180 - InU32_secondry_dose_interlock */
UINT32 InU32_afc_interlock; /* Subindex181 - InU32_afc_interlock */
REAL32 InF_afc_position_current; /* Subindex182 - InF_afc_position_current */
UINT8 InU8_qam_fsm_state_current; /* Subindex183 - InU8_qam_fsm_state_current */
UINT8 InU8_qam_ctrl_mode_cur; /* Subindex184 - InU8_qam_ctrl_mode_cur */
UINT16 InU16_reserved10; /* Subindex185 - InU16_reserved10 */
UINT8 InU8_qam_beam_id; /* Subindex186 - InU8_qam_beam_id */
UINT8 InU8_reserved11; /* Subindex187 - InU8_reserved11 */
UINT16 InU16_qam_radiation_index; /* Subindex188 - InU16_qam_radiation_index */
UINT32 InU32_qam_not_ready_event; /* Subindex189 - InU32_qam_not_ready_event */
UINT32 InU32_qam_warning_interlock; /* Subindex190 - InU32_qam_warning_interlock */
UINT32 InU32_qam_minor_interlock; /* Subindex191 - InU32_qam_minor_interlock */
UINT32 InU32_qam_serious_interlock; /* Subindex192 - InU32_qam_serious_interlock */
UINT16 InU16_TotalStep_QAM; /* Subindex193 - InU16_TotalStep_QAM */
UINT16 InU16_CurrentStep_QAM; /* Subindex194 - InU16_CurrentStep_QAM */
UINT16 InU16_ErrorCode_QAM; /* Subindex195 - InU16_ErrorCode_QAM */
UINT8 InU8_rtm_off_fsm_state_current; /* Subindex196 - InU8_rtm_off_fsm_state_current */
UINT8 InU8_rtm_off_ctrl_mode_cur; /* Subindex197 - InU8_rtm_off_ctrl_mode_cur */
UINT16 InU16_reserved12; /* Subindex198 - InU16_reserved12 */
UINT8 InU8_rtm_off_beam_id; /* Subindex199 - InU8_rtm_off_beam_id */
UINT8 InU8_reserved13; /* Subindex200 - InU8_reserved13 */
UINT16 InU16_rtm_off_radiation_index; /* Subindex201 - InU16_rtm_off_radiation_index */
UINT32 InU32_rtm_off_not_ready_event; /* Subindex202 - InU32_rtm_off_not_ready_event */
UINT32 InU32_rtm_off_warning_interlock; /* Subindex203 - InU32_rtm_off_warning_interlock */
UINT32 InU32_rtm_off_minor_interlock; /* Subindex204 - InU32_rtm_off_minor_interlock */
UINT32 InU32_rtm_off_serious_interlock; /* Subindex205 - InU32_rtm_off_serious_interlock */
UINT16 InU16_TotalStep_RTM_OFF_ARM; /* Subindex206 - InU16_TotalStep_RTM_OFF_ARM */
UINT16 InU16_CurrentStep_RTM_OFF_ARM; /* Subindex207 - InU16_CurrentStep_RTM_OFF_ARM */
UINT16 InU16_ErrorCode_RTM_OFF_ARM; /* Subindex208 - InU16_ErrorCode_RTM_OFF_ARM */
UINT32 InU32_rtm_off_search_timeout_cur; /* Subindex209 - InU32_rtm_off_search_timeout_cur */
UINT8 InU8_gmm_fsm_state_current; /* Subindex210 - InU8_gmm_fsm_state_current */
UINT8 InU8_gmm_ctrl_mode_cur; /* Subindex211 - InU8_gmm_ctrl_mode_cur */
UINT16 InU16_reserved14; /* Subindex212 - InU16_reserved14 */
UINT8 InU8_gmm_beam_id; /* Subindex213 - InU8_gmm_beam_id */
UINT8 InU8_reserved15; /* Subindex214 - InU8_reserved15 */
UINT16 InU16_gmm_radiation_index; /* Subindex215 - InU16_gmm_radiation_index */
UINT32 InU32_gmm_not_ready_event; /* Subindex216 - InU32_gmm_not_ready_event */
UINT32 InU32_gmm_warning_interlock; /* Subindex217 - InU32_gmm_warning_interlock */
UINT32 InU32_gmm_minor_interlock; /* Subindex218 - InU32_gmm_minor_interlock */
UINT32 InU32_gmm_serious_interlock; /* Subindex219 - InU32_gmm_serious_interlock */
UINT16 InU16_TotalStep_GMM; /* Subindex220 - InU16_TotalStep_GMM */
UINT16 InU16_CurrentStep_GMM; /* Subindex221 - InU16_CurrentStep_GMM */
UINT16 InU16_ErrorCode_GMM; /* Subindex222 - InU16_ErrorCode_GMM */
UINT32 InU32_gmm_move_status; /* Subindex223 - InU32_gmm_move_status */
REAL32 InF_gmm_position_cur; /* Subindex224 - InF_gmm_position_cur */
REAL32 InF_gmm_velocity_cur; /* Subindex225 - InF_gmm_velocity_cur */
UINT8 InU8_psm_fsm_state_current; /* Subindex226 - InU8_psm_fsm_state_current */
UINT8 InU8_psm_ctrl_mode_cur; /* Subindex227 - InU8_psm_ctrl_mode_cur */
UINT16 InU16_reserved16; /* Subindex228 - InU16_reserved16 */
UINT8 InU8_psm_beam_id; /* Subindex229 - InU8_psm_beam_id */
UINT8 InU8_reserved17; /* Subindex230 - InU8_reserved17 */
UINT16 InU16_psm_radiation_index; /* Subindex231 - InU16_psm_radiation_index */
UINT32 InU32_psm_not_ready_event; /* Subindex232 - InU32_psm_not_ready_event */
UINT32 InU32_psm_warning_interlock; /* Subindex233 - InU32_psm_warning_interlock */
UINT32 InU32_psm_minor_interlock; /* Subindex234 - InU32_psm_minor_interlock */
UINT32 InU32_psm_serious_interlock; /* Subindex235 - InU32_psm_serious_interlock */
UINT16 InU16_TotalStep_PSM; /* Subindex236 - InU16_TotalStep_PSM */
UINT16 InU16_CurrentStep_PSM; /* Subindex237 - InU16_CurrentStep_PSM */
UINT16 InU16_ErrorCode_PSM; /* Subindex238 - InU16_ErrorCode_PSM */
UINT32 InU32_psm_move_status; /* Subindex239 - InU32_psm_move_status */
REAL32 InF_psm_position_x_cur; /* Subindex240 - InF_psm_position_x_cur */
REAL32 InF_psm_position_y_cur; /* Subindex241 - InF_psm_position_y_cur */
REAL32 InF_psm_position_z_cur; /* Subindex242 - InF_psm_position_z_cur */
REAL32 InF_psm_position_x_r_cur; /* Subindex243 - InF_psm_position_x_r_cur */
REAL32 InF_psm_position_y_r_cur; /* Subindex244 - InF_psm_position_y_r_cur */
REAL32 InF_psm_position_z_r_cur; /* Subindex245 - InF_psm_position_z_r_cur */
REAL32 InF_psm_velocity_x_cur; /* Subindex246 - InF_psm_velocity_x_cur */
REAL32 InF_psm_velocity_y_cur; /* Subindex247 - InF_psm_velocity_y_cur */
REAL32 InF_psm_velocity_z_cur; /* Subindex248 - InF_psm_velocity_z_cur */
REAL32 InF_psm_velocity_x_r_cur; /* Subindex249 - InF_psm_velocity_x_r_cur */
REAL32 InF_psm_velocity_y_r_cur; /* Subindex250 - InF_psm_velocity_y_r_cur */
REAL32 InF_psm_velocity_z_r_cur; /* Subindex251 - InF_psm_velocity_z_r_cur */
UINT8 InU8_function_button; /* Subindex252 - InU8_function_button */
UINT8 InU8_special_button; /* Subindex253 - InU8_special_button */
} OBJ_STRUCT_PACKED_END
__attribute__((aligned(1), packed)) TOBJ6000;
#endif //#ifndef _LAN9252_APP_OBJECTS_H_

/**
* \brief Object variable
*/
PROTO TOBJ6000 InputData0x6000
#if defined(_LAN9252_APP_) && (_LAN9252_APP_ == 1)
={253,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}
#endif
;
/** @}*/



/******************************************************************************
*                    Object 0x6010 : InputData
******************************************************************************/
/**
* \addtogroup 0x6010 0x6010 | InputData
* @{
* \brief Object 0x6010 (InputData) definition
*/
#ifdef _OBJD_
/**
* \brief Object entry descriptions<br>
* <br>
* SubIndex 0<br>
* SubIndex 1 - InU32_FkpButtonFaultMask<br>
* SubIndex 2 - InU16_TotalStep_FKP<br>
* SubIndex 3 - InU16_CurrentStep_FKP<br>
* SubIndex 4 - InU16_ErrorCode_FKP<br>
* SubIndex 5 - InU32_CpgButtonFaultMask_L<br>
* SubIndex 6 - InU16_TotalStep_CPG_L<br>
* SubIndex 7 - InU16_CurrentStep_CPG_L<br>
* SubIndex 8 - InU16_ErrorCode_CPG_L<br>
* SubIndex 9 - InU32_CpgButtonFaultMask_R<br>
* SubIndex 10 - InU16_TotalStep_CPG_R<br>
* SubIndex 11 - InU16_CurrentStep_CPG_R<br>
* SubIndex 12 - InU16_ErrorCode_CPG_R<br>
* SubIndex 13 - InU16_data_valid_flag<br>
* SubIndex 14 - InUF_load_position_x_cur<br>
* SubIndex 15 - InUF_load_position_y_cur<br>
* SubIndex 16 - InUF_load_position_z_cur<br>
* SubIndex 17 - InUF_load_position_x_r_cur<br>
* SubIndex 18 - InUF_load_position_y_r_cur<br>
* SubIndex 19 - InUF_load_position_z_r_cur<br>
* SubIndex 20 - InUF_position_x_tar<br>
* SubIndex 21 - InUF_position_y_tar<br>
* SubIndex 22 - InUF_position_z_tar<br>
* SubIndex 23 - InUF_position_x_r_tar<br>
* SubIndex 24 - InUF_position_y_r_tar<br>
* SubIndex 25 - InUF_position_z_r_tar<br>
* SubIndex 26 - InU8_trm_require_state<br>
*/
OBJCONST TSDOINFOENTRYDESC    OBJMEM asEntryDesc0x6010[] = {
{ DEFTYPE_UNSIGNED8 , 0x8 , ACCESS_READ },
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex1 - InU32_FkpButtonFaultMask */
{ DEFTYPE_UNSIGNED16 , 0x10 , ACCESS_READ }, /* Subindex2 - InU16_TotalStep_FKP */
{ DEFTYPE_UNSIGNED16 , 0x10 , ACCESS_READ }, /* Subindex3 - InU16_CurrentStep_FKP */
{ DEFTYPE_UNSIGNED16 , 0x10 , ACCESS_READ }, /* Subindex4 - InU16_ErrorCode_FKP */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex5 - InU32_CpgButtonFaultMask_L */
{ DEFTYPE_UNSIGNED16 , 0x10 , ACCESS_READ }, /* Subindex6 - InU16_TotalStep_CPG_L */
{ DEFTYPE_UNSIGNED16 , 0x10 , ACCESS_READ }, /* Subindex7 - InU16_CurrentStep_CPG_L */
{ DEFTYPE_UNSIGNED16 , 0x10 , ACCESS_READ }, /* Subindex8 - InU16_ErrorCode_CPG_L */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex9 - InU32_CpgButtonFaultMask_R */
{ DEFTYPE_UNSIGNED16 , 0x10 , ACCESS_READ }, /* Subindex10 - InU16_TotalStep_CPG_R */
{ DEFTYPE_UNSIGNED16 , 0x10 , ACCESS_READ }, /* Subindex11 - InU16_CurrentStep_CPG_R */
{ DEFTYPE_UNSIGNED16 , 0x10 , ACCESS_READ }, /* Subindex12 - InU16_ErrorCode_CPG_R */
{ DEFTYPE_UNSIGNED16 , 0x10 , ACCESS_READ }, /* Subindex13 - InU16_data_valid_flag */
{ DEFTYPE_REAL32 , 0x20 , ACCESS_READ }, /* Subindex14 - InUF_load_position_x_cur */
{ DEFTYPE_REAL32 , 0x20 , ACCESS_READ }, /* Subindex15 - InUF_load_position_y_cur */
{ DEFTYPE_REAL32 , 0x20 , ACCESS_READ }, /* Subindex16 - InUF_load_position_z_cur */
{ DEFTYPE_REAL32 , 0x20 , ACCESS_READ }, /* Subindex17 - InUF_load_position_x_r_cur */
{ DEFTYPE_REAL32 , 0x20 , ACCESS_READ }, /* Subindex18 - InUF_load_position_y_r_cur */
{ DEFTYPE_REAL32 , 0x20 , ACCESS_READ }, /* Subindex19 - InUF_load_position_z_r_cur */
{ DEFTYPE_REAL32 , 0x20 , ACCESS_READ }, /* Subindex20 - InUF_position_x_tar */
{ DEFTYPE_REAL32 , 0x20 , ACCESS_READ }, /* Subindex21 - InUF_position_y_tar */
{ DEFTYPE_REAL32 , 0x20 , ACCESS_READ }, /* Subindex22 - InUF_position_z_tar */
{ DEFTYPE_REAL32 , 0x20 , ACCESS_READ }, /* Subindex23 - InUF_position_x_r_tar */
{ DEFTYPE_REAL32 , 0x20 , ACCESS_READ }, /* Subindex24 - InUF_position_y_r_tar */
{ DEFTYPE_REAL32 , 0x20 , ACCESS_READ }, /* Subindex25 - InUF_position_z_r_tar */
{ DEFTYPE_UNSIGNED8 , 0x08 , ACCESS_READ }}; /* Subindex26 - InU8_trm_require_state */

/**
* \brief Object/Entry names
*/
OBJCONST UCHAR OBJMEM aName0x6010[] = "InputData\000"
"InU32_FkpButtonFaultMask\000"
"InU16_TotalStep_FKP\000"
"InU16_CurrentStep_FKP\000"
"InU16_ErrorCode_FKP\000"
"InU32_CpgButtonFaultMask_L\000"
"InU16_TotalStep_CPG_L\000"
"InU16_CurrentStep_CPG_L\000"
"InU16_ErrorCode_CPG_L\000"
"InU32_CpgButtonFaultMask_R\000"
"InU16_TotalStep_CPG_R\000"
"InU16_CurrentStep_CPG_R\000"
"InU16_ErrorCode_CPG_R\000"
"InU16_data_valid_flag\000"
"InUF_load_position_x_cur\000"
"InUF_load_position_y_cur\000"
"InUF_load_position_z_cur\000"
"InUF_load_position_x_r_cur\000"
"InUF_load_position_y_r_cur\000"
"InUF_load_position_z_r_cur\000"
"InUF_position_x_tar\000"
"InUF_position_y_tar\000"
"InUF_position_z_tar\000"
"InUF_position_x_r_tar\000"
"InUF_position_y_r_tar\000"
"InUF_position_z_r_tar\000"
"InU8_trm_require_state\000\377";
#endif //#ifdef _OBJD_

#ifndef _LAN9252_APP_OBJECTS_H_
/**
* \brief Object structure
*/
typedef struct OBJ_STRUCT_PACKED_START {
UINT16 u16SubIndex0;
UINT32 InU32_FkpButtonFaultMask; /* Subindex1 - InU32_FkpButtonFaultMask */
UINT16 InU16_TotalStep_FKP; /* Subindex2 - InU16_TotalStep_FKP */
UINT16 InU16_CurrentStep_FKP; /* Subindex3 - InU16_CurrentStep_FKP */
UINT16 InU16_ErrorCode_FKP; /* Subindex4 - InU16_ErrorCode_FKP */
UINT32 InU32_CpgButtonFaultMask_L; /* Subindex5 - InU32_CpgButtonFaultMask_L */
UINT16 InU16_TotalStep_CPG_L; /* Subindex6 - InU16_TotalStep_CPG_L */
UINT16 InU16_CurrentStep_CPG_L; /* Subindex7 - InU16_CurrentStep_CPG_L */
UINT16 InU16_ErrorCode_CPG_L; /* Subindex8 - InU16_ErrorCode_CPG_L */
UINT32 InU32_CpgButtonFaultMask_R; /* Subindex9 - InU32_CpgButtonFaultMask_R */
UINT16 InU16_TotalStep_CPG_R; /* Subindex10 - InU16_TotalStep_CPG_R */
UINT16 InU16_CurrentStep_CPG_R; /* Subindex11 - InU16_CurrentStep_CPG_R */
UINT16 InU16_ErrorCode_CPG_R; /* Subindex12 - InU16_ErrorCode_CPG_R */
UINT16 InU16_data_valid_flag; /* Subindex13 - InU16_data_valid_flag */
REAL32 InUF_load_position_x_cur; /* Subindex14 - InUF_load_position_x_cur */
REAL32 InUF_load_position_y_cur; /* Subindex15 - InUF_load_position_y_cur */
REAL32 InUF_load_position_z_cur; /* Subindex16 - InUF_load_position_z_cur */
REAL32 InUF_load_position_x_r_cur; /* Subindex17 - InUF_load_position_x_r_cur */
REAL32 InUF_load_position_y_r_cur; /* Subindex18 - InUF_load_position_y_r_cur */
REAL32 InUF_load_position_z_r_cur; /* Subindex19 - InUF_load_position_z_r_cur */
REAL32 InUF_position_x_tar; /* Subindex20 - InUF_position_x_tar */
REAL32 InUF_position_y_tar; /* Subindex21 - InUF_position_y_tar */
REAL32 InUF_position_z_tar; /* Subindex22 - InUF_position_z_tar */
REAL32 InUF_position_x_r_tar; /* Subindex23 - InUF_position_x_r_tar */
REAL32 InUF_position_y_r_tar; /* Subindex24 - InUF_position_y_r_tar */
REAL32 InUF_position_z_r_tar; /* Subindex25 - InUF_position_z_r_tar */
UINT8 InU8_trm_require_state; /* Subindex26 - InU8_trm_require_state */
} OBJ_STRUCT_PACKED_END
__attribute__((aligned(1), packed)) TOBJ6010;
#endif //#ifndef _LAN9252_APP_OBJECTS_H_

/**
* \brief Object variable
*/
PROTO TOBJ6010 InputData0x6010
#if defined(_LAN9252_APP_) && (_LAN9252_APP_ == 1)
={26,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}
#endif
;
/** @}*/



/******************************************************************************
*                    Object 0x7010 : OutputData
******************************************************************************/
/**
* \addtogroup 0x7010 0x7010 | OutputData
* @{
* \brief Object 0x7010 (OutputData) definition
*/
#ifdef _OBJD_
/**
* \brief Object entry descriptions<br>
* <br>
* SubIndex 0<br>
* SubIndex 1 - OutU8_BoardID<br>
* SubIndex 2 - OutU8_Reserved0<br>
* SubIndex 3 - OutU32_FirmWareVersion<br>
* SubIndex 4 - OutU8_beam_id<br>
* SubIndex 5 - OutU8_state_sync<br>
* SubIndex 6 - OutU16_radiation_index<br>
* SubIndex 7 - OutU8_fault_clear<br>
* SubIndex 8 - OutU8_systemCurrentState<br>
* SubIndex 9 - OutU16_reboot<br>
* SubIndex 10 - OutU8_treatment_record_state<br>
* SubIndex 11 - OutU8_ethercat_Link_state<br>
* SubIndex 12 - OutU8_rtm_on_require_state<br>
* SubIndex 13 - OutU8_rtm_on_require_ctrl_mode<br>
* SubIndex 14 - OutU16_rtm_on_plc_info<br>
* SubIndex 15 - OutU32_rtm_on_interlock_override<br>
* SubIndex 16 - OutU32_rtm_on_unready_override<br>
* SubIndex 17 - OutU16_rtm_on_treatment_mode<br>
* SubIndex 18 - OutU8_icm_require_state<br>
* SubIndex 19 - OutU8_icm_require_ctrl_mode<br>
* SubIndex 20 - OutU16_ct_status<br>
* SubIndex 21 - OutF_gantry_velocity_prepare<br>
* SubIndex 22 - OutU32_icm_interlock_override<br>
* SubIndex 23 - OutU32_icm_unready_override<br>
* SubIndex 24 - OutU8_bgm_require_state<br>
* SubIndex 25 - OutU8_bgm_require_ctrl_mode<br>
* SubIndex 26 - OutU32_bgm_interlock_override<br>
* SubIndex 27 - OutU32_bgm_unready_override<br>
* SubIndex 28 - OutU8_qam_require_state<br>
* SubIndex 29 - OutU8_qam_require_ctrl_mode<br>
* SubIndex 30 - OutU32_qam_interlock_override<br>
* SubIndex 31 - OutU32_qam_unready_override<br>
* SubIndex 32 - OutU8_bsm_require_state<br>
* SubIndex 33 - OutU8_bsm_require_ctrl_mode<br>
* SubIndex 34 - OutU32_bsm_interlock_override<br>
* SubIndex 35 - OutU32_bsm_unready_override<br>
* SubIndex 36 - OutU8_rtm_off_require_state<br>
* SubIndex 37 - OutU8_rtm_off_require_ctrl_mode<br>
* SubIndex 38 - OutU16_rtm_off_plc_info<br>
* SubIndex 39 - OutU32_rtm_off_interlock_override<br>
* SubIndex 40 - OutU32_rtm_off_unready_override<br>
* SubIndex 41 - OutU8_led_belt<br>
* SubIndex 42 - OutU8_reserved2<br>
* SubIndex 43 - OutU32_rtm_off_search_timeout<br>
* SubIndex 44 - OutU16_rtm_off_treatment_mode<br>
* SubIndex 45 - OutU8_gmm_require_state<br>
* SubIndex 46 - OutU8_gmm_require_ctrl_mode<br>
* SubIndex 47 - OutU32_gmm_interlock_override<br>
* SubIndex 48 - OutU32_gmm_unready_override<br>
* SubIndex 49 - OutU8_psm_require_state<br>
* SubIndex 50 - OutU8_psm_require_ctrl_mode<br>
* SubIndex 51 - OutU32_psm_interlock_override<br>
* SubIndex 52 - OutU32_psm_unready_override<br>
* SubIndex 53 - OutU16_fkp_year<br>
* SubIndex 54 - OutU8_fkp_month<br>
* SubIndex 55 - OutU8_fkp_day<br>
* SubIndex 56 - OutU8_fkp_hour<br>
* SubIndex 57 - OutU8_fkp_minute<br>
* SubIndex 58 - OutU8_fkp_totalFractions<br>
* SubIndex 59 - OutU8_fkp_fractions<br>
* SubIndex 60 - OutU8_reserved3<br>
*/
OBJCONST TSDOINFOENTRYDESC    OBJMEM asEntryDesc0x7010[] = {
{ DEFTYPE_UNSIGNED8 , 0x8 , ACCESS_READ },
{ DEFTYPE_UNSIGNED8 , 0x08 , ACCESS_READ }, /* Subindex1 - OutU8_BoardID */
{ DEFTYPE_UNSIGNED8 , 0x08 , ACCESS_READ }, /* Subindex2 - OutU8_Reserved0 */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex3 - OutU32_FirmWareVersion */
{ DEFTYPE_UNSIGNED8 , 0x08 , ACCESS_READ }, /* Subindex4 - OutU8_beam_id */
{ DEFTYPE_UNSIGNED8 , 0x08 , ACCESS_READ }, /* Subindex5 - OutU8_state_sync */
{ DEFTYPE_UNSIGNED16 , 0x10 , ACCESS_READ }, /* Subindex6 - OutU16_radiation_index */
{ DEFTYPE_UNSIGNED8 , 0x08 , ACCESS_READ }, /* Subindex7 - OutU8_fault_clear */
{ DEFTYPE_UNSIGNED8 , 0x08 , ACCESS_READ }, /* Subindex8 - OutU8_systemCurrentState */
{ DEFTYPE_UNSIGNED16 , 0x10 , ACCESS_READ }, /* Subindex9 - OutU16_reboot */
{ DEFTYPE_UNSIGNED8 , 0x08 , ACCESS_READ }, /* Subindex10 - OutU8_treatment_record_state */
{ DEFTYPE_UNSIGNED8 , 0x08 , ACCESS_READ }, /* Subindex11 - OutU8_ethercat_Link_state */
{ DEFTYPE_UNSIGNED8 , 0x08 , ACCESS_READ }, /* Subindex12 - OutU8_rtm_on_require_state */
{ DEFTYPE_UNSIGNED8 , 0x08 , ACCESS_READ }, /* Subindex13 - OutU8_rtm_on_require_ctrl_mode */
{ DEFTYPE_UNSIGNED16 , 0x10 , ACCESS_READ }, /* Subindex14 - OutU16_rtm_on_plc_info */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex15 - OutU32_rtm_on_interlock_override */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex16 - OutU32_rtm_on_unready_override */
{ DEFTYPE_UNSIGNED16 , 0x10 , ACCESS_READ }, /* Subindex17 - OutU16_rtm_on_treatment_mode */
{ DEFTYPE_UNSIGNED8 , 0x08 , ACCESS_READ }, /* Subindex18 - OutU8_icm_require_state */
{ DEFTYPE_UNSIGNED8 , 0x08 , ACCESS_READ }, /* Subindex19 - OutU8_icm_require_ctrl_mode */
{ DEFTYPE_UNSIGNED16 , 0x10 , ACCESS_READ }, /* Subindex20 - OutU16_ct_status */
{ DEFTYPE_REAL32 , 0x20 , ACCESS_READ }, /* Subindex21 - OutF_gantry_velocity_prepare */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex22 - OutU32_icm_interlock_override */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex23 - OutU32_icm_unready_override */
{ DEFTYPE_UNSIGNED8 , 0x08 , ACCESS_READ }, /* Subindex24 - OutU8_bgm_require_state */
{ DEFTYPE_UNSIGNED8 , 0x08 , ACCESS_READ }, /* Subindex25 - OutU8_bgm_require_ctrl_mode */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex26 - OutU32_bgm_interlock_override */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex27 - OutU32_bgm_unready_override */
{ DEFTYPE_UNSIGNED8 , 0x08 , ACCESS_READ }, /* Subindex28 - OutU8_qam_require_state */
{ DEFTYPE_UNSIGNED8 , 0x08 , ACCESS_READ }, /* Subindex29 - OutU8_qam_require_ctrl_mode */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex30 - OutU32_qam_interlock_override */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex31 - OutU32_qam_unready_override */
{ DEFTYPE_UNSIGNED8 , 0x08 , ACCESS_READ }, /* Subindex32 - OutU8_bsm_require_state */
{ DEFTYPE_UNSIGNED8 , 0x08 , ACCESS_READ }, /* Subindex33 - OutU8_bsm_require_ctrl_mode */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex34 - OutU32_bsm_interlock_override */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex35 - OutU32_bsm_unready_override */
{ DEFTYPE_UNSIGNED8 , 0x08 , ACCESS_READ }, /* Subindex36 - OutU8_rtm_off_require_state */
{ DEFTYPE_UNSIGNED8 , 0x08 , ACCESS_READ }, /* Subindex37 - OutU8_rtm_off_require_ctrl_mode */
{ DEFTYPE_UNSIGNED16 , 0x10 , ACCESS_READ }, /* Subindex38 - OutU16_rtm_off_plc_info */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex39 - OutU32_rtm_off_interlock_override */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex40 - OutU32_rtm_off_unready_override */
{ DEFTYPE_UNSIGNED8 , 0x08 , ACCESS_READ }, /* Subindex41 - OutU8_led_belt */
{ DEFTYPE_UNSIGNED8 , 0x08 , ACCESS_READ }, /* Subindex42 - OutU8_reserved2 */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex43 - OutU32_rtm_off_search_timeout */
{ DEFTYPE_UNSIGNED16 , 0x10 , ACCESS_READ }, /* Subindex44 - OutU16_rtm_off_treatment_mode */
{ DEFTYPE_UNSIGNED8 , 0x08 , ACCESS_READ }, /* Subindex45 - OutU8_gmm_require_state */
{ DEFTYPE_UNSIGNED8 , 0x08 , ACCESS_READ }, /* Subindex46 - OutU8_gmm_require_ctrl_mode */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex47 - OutU32_gmm_interlock_override */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex48 - OutU32_gmm_unready_override */
{ DEFTYPE_UNSIGNED8 , 0x08 , ACCESS_READ }, /* Subindex49 - OutU8_psm_require_state */
{ DEFTYPE_UNSIGNED8 , 0x08 , ACCESS_READ }, /* Subindex50 - OutU8_psm_require_ctrl_mode */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex51 - OutU32_psm_interlock_override */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex52 - OutU32_psm_unready_override */
{ DEFTYPE_UNSIGNED16 , 0x10 , ACCESS_READ }, /* Subindex53 - OutU16_fkp_year */
{ DEFTYPE_UNSIGNED8 , 0x08 , ACCESS_READ }, /* Subindex54 - OutU8_fkp_month */
{ DEFTYPE_UNSIGNED8 , 0x08 , ACCESS_READ }, /* Subindex55 - OutU8_fkp_day */
{ DEFTYPE_UNSIGNED8 , 0x08 , ACCESS_READ }, /* Subindex56 - OutU8_fkp_hour */
{ DEFTYPE_UNSIGNED8 , 0x08 , ACCESS_READ }, /* Subindex57 - OutU8_fkp_minute */
{ DEFTYPE_UNSIGNED8 , 0x08 , ACCESS_READ }, /* Subindex58 - OutU8_fkp_totalFractions */
{ DEFTYPE_UNSIGNED8 , 0x08 , ACCESS_READ }, /* Subindex59 - OutU8_fkp_fractions */
{ DEFTYPE_UNSIGNED8 , 0x08 , ACCESS_READ }}; /* Subindex60 - OutU8_reserved3 */

/**
* \brief Object/Entry names
*/
OBJCONST UCHAR OBJMEM aName0x7010[] = "OutputData\000"
"OutU8_BoardID\000"
"OutU8_Reserved0\000"
"OutU32_FirmWareVersion\000"
"OutU8_beam_id\000"
"OutU8_state_sync\000"
"OutU16_radiation_index\000"
"OutU8_fault_clear\000"
"OutU8_systemCurrentState\000"
"OutU16_reboot\000"
"OutU8_treatment_record_state\000"
"OutU8_ethercat_Link_state\000"
"OutU8_rtm_on_require_state\000"
"OutU8_rtm_on_require_ctrl_mode\000"
"OutU16_rtm_on_plc_info\000"
"OutU32_rtm_on_interlock_override\000"
"OutU32_rtm_on_unready_override\000"
"OutU16_rtm_on_treatment_mode\000"
"OutU8_icm_require_state\000"
"OutU8_icm_require_ctrl_mode\000"
"OutU16_ct_status\000"
"OutF_gantry_velocity_prepare\000"
"OutU32_icm_interlock_override\000"
"OutU32_icm_unready_override\000"
"OutU8_bgm_require_state\000"
"OutU8_bgm_require_ctrl_mode\000"
"OutU32_bgm_interlock_override\000"
"OutU32_bgm_unready_override\000"
"OutU8_qam_require_state\000"
"OutU8_qam_require_ctrl_mode\000"
"OutU32_qam_interlock_override\000"
"OutU32_qam_unready_override\000"
"OutU8_bsm_require_state\000"
"OutU8_bsm_require_ctrl_mode\000"
"OutU32_bsm_interlock_override\000"
"OutU32_bsm_unready_override\000"
"OutU8_rtm_off_require_state\000"
"OutU8_rtm_off_require_ctrl_mode\000"
"OutU16_rtm_off_plc_info\000"
"OutU32_rtm_off_interlock_override\000"
"OutU32_rtm_off_unready_override\000"
"OutU8_led_belt\000"
"OutU8_reserved2\000"
"OutU32_rtm_off_search_timeout\000"
"OutU16_rtm_off_treatment_mode\000"
"OutU8_gmm_require_state\000"
"OutU8_gmm_require_ctrl_mode\000"
"OutU32_gmm_interlock_override\000"
"OutU32_gmm_unready_override\000"
"OutU8_psm_require_state\000"
"OutU8_psm_require_ctrl_mode\000"
"OutU32_psm_interlock_override\000"
"OutU32_psm_unready_override\000"
"OutU16_fkp_year\000"
"OutU8_fkp_month\000"
"OutU8_fkp_day\000"
"OutU8_fkp_hour\000"
"OutU8_fkp_minute\000"
"OutU8_fkp_totalFractions\000"
"OutU8_fkp_fractions\000"
"OutU8_reserved3\000\377";
#endif //#ifdef _OBJD_

#ifndef _LAN9252_APP_OBJECTS_H_
/**
* \brief Object structure
*/
typedef struct OBJ_STRUCT_PACKED_START {
UINT16 u16SubIndex0;
UINT8 OutU8_BoardID; /* Subindex1 - OutU8_BoardID */
UINT8 OutU8_Reserved0; /* Subindex2 - OutU8_Reserved0 */
UINT32 OutU32_FirmWareVersion; /* Subindex3 - OutU32_FirmWareVersion */
UINT8 OutU8_beam_id; /* Subindex4 - OutU8_beam_id */
UINT8 OutU8_state_sync; /* Subindex5 - OutU8_state_sync */
UINT16 OutU16_radiation_index; /* Subindex6 - OutU16_radiation_index */
UINT8 OutU8_fault_clear; /* Subindex7 - OutU8_fault_clear */
UINT8 OutU8_systemCurrentState; /* Subindex8 - OutU8_systemCurrentState */
UINT16 OutU16_reboot; /* Subindex9 - OutU16_reboot */
UINT8 OutU8_treatment_record_state; /* Subindex10 - OutU8_treatment_record_state */
UINT8 OutU8_ethercat_Link_state; /* Subindex11 - OutU8_ethercat_Link_state */
UINT8 OutU8_rtm_on_require_state; /* Subindex12 - OutU8_rtm_on_require_state */
UINT8 OutU8_rtm_on_require_ctrl_mode; /* Subindex13 - OutU8_rtm_on_require_ctrl_mode */
UINT16 OutU16_rtm_on_plc_info; /* Subindex14 - OutU16_rtm_on_plc_info */
UINT32 OutU32_rtm_on_interlock_override; /* Subindex15 - OutU32_rtm_on_interlock_override */
UINT32 OutU32_rtm_on_unready_override; /* Subindex16 - OutU32_rtm_on_unready_override */
UINT16 OutU16_rtm_on_treatment_mode; /* Subindex17 - OutU16_rtm_on_treatment_mode */
UINT8 OutU8_icm_require_state; /* Subindex18 - OutU8_icm_require_state */
UINT8 OutU8_icm_require_ctrl_mode; /* Subindex19 - OutU8_icm_require_ctrl_mode */
UINT16 OutU16_ct_status; /* Subindex20 - OutU16_ct_status */
REAL32 OutF_gantry_velocity_prepare; /* Subindex21 - OutF_gantry_velocity_prepare */
UINT32 OutU32_icm_interlock_override; /* Subindex22 - OutU32_icm_interlock_override */
UINT32 OutU32_icm_unready_override; /* Subindex23 - OutU32_icm_unready_override */
UINT8 OutU8_bgm_require_state; /* Subindex24 - OutU8_bgm_require_state */
UINT8 OutU8_bgm_require_ctrl_mode; /* Subindex25 - OutU8_bgm_require_ctrl_mode */
UINT32 OutU32_bgm_interlock_override; /* Subindex26 - OutU32_bgm_interlock_override */
UINT32 OutU32_bgm_unready_override; /* Subindex27 - OutU32_bgm_unready_override */
UINT8 OutU8_qam_require_state; /* Subindex28 - OutU8_qam_require_state */
UINT8 OutU8_qam_require_ctrl_mode; /* Subindex29 - OutU8_qam_require_ctrl_mode */
UINT32 OutU32_qam_interlock_override; /* Subindex30 - OutU32_qam_interlock_override */
UINT32 OutU32_qam_unready_override; /* Subindex31 - OutU32_qam_unready_override */
UINT8 OutU8_bsm_require_state; /* Subindex32 - OutU8_bsm_require_state */
UINT8 OutU8_bsm_require_ctrl_mode; /* Subindex33 - OutU8_bsm_require_ctrl_mode */
UINT32 OutU32_bsm_interlock_override; /* Subindex34 - OutU32_bsm_interlock_override */
UINT32 OutU32_bsm_unready_override; /* Subindex35 - OutU32_bsm_unready_override */
UINT8 OutU8_rtm_off_require_state; /* Subindex36 - OutU8_rtm_off_require_state */
UINT8 OutU8_rtm_off_require_ctrl_mode; /* Subindex37 - OutU8_rtm_off_require_ctrl_mode */
UINT16 OutU16_rtm_off_plc_info; /* Subindex38 - OutU16_rtm_off_plc_info */
UINT32 OutU32_rtm_off_interlock_override; /* Subindex39 - OutU32_rtm_off_interlock_override */
UINT32 OutU32_rtm_off_unready_override; /* Subindex40 - OutU32_rtm_off_unready_override */
UINT8 OutU8_led_belt; /* Subindex41 - OutU8_led_belt */
UINT8 OutU8_reserved2; /* Subindex42 - OutU8_reserved2 */
UINT32 OutU32_rtm_off_search_timeout; /* Subindex43 - OutU32_rtm_off_search_timeout */
UINT16 OutU16_rtm_off_treatment_mode; /* Subindex44 - OutU16_rtm_off_treatment_mode */
UINT8 OutU8_gmm_require_state; /* Subindex45 - OutU8_gmm_require_state */
UINT8 OutU8_gmm_require_ctrl_mode; /* Subindex46 - OutU8_gmm_require_ctrl_mode */
UINT32 OutU32_gmm_interlock_override; /* Subindex47 - OutU32_gmm_interlock_override */
UINT32 OutU32_gmm_unready_override; /* Subindex48 - OutU32_gmm_unready_override */
UINT8 OutU8_psm_require_state; /* Subindex49 - OutU8_psm_require_state */
UINT8 OutU8_psm_require_ctrl_mode; /* Subindex50 - OutU8_psm_require_ctrl_mode */
UINT32 OutU32_psm_interlock_override; /* Subindex51 - OutU32_psm_interlock_override */
UINT32 OutU32_psm_unready_override; /* Subindex52 - OutU32_psm_unready_override */
UINT16 OutU16_fkp_year; /* Subindex53 - OutU16_fkp_year */
UINT8 OutU8_fkp_month; /* Subindex54 - OutU8_fkp_month */
UINT8 OutU8_fkp_day; /* Subindex55 - OutU8_fkp_day */
UINT8 OutU8_fkp_hour; /* Subindex56 - OutU8_fkp_hour */
UINT8 OutU8_fkp_minute; /* Subindex57 - OutU8_fkp_minute */
UINT8 OutU8_fkp_totalFractions; /* Subindex58 - OutU8_fkp_totalFractions */
UINT8 OutU8_fkp_fractions; /* Subindex59 - OutU8_fkp_fractions */
UINT8 OutU8_reserved3; /* Subindex60 - OutU8_reserved3 */
} OBJ_STRUCT_PACKED_END
__attribute__((aligned(1), packed)) TOBJ7010;
#endif //#ifndef _LAN9252_APP_OBJECTS_H_

/**
* \brief Object variable
*/
PROTO TOBJ7010 OutputData0x7010
#if defined(_LAN9252_APP_) && (_LAN9252_APP_ == 1)
={60,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}
#endif
;
/** @}*/







#ifdef _OBJD_
TOBJECT    OBJMEM ApplicationObjDic[] = {
/* Object 0x1601 */
{NULL , NULL ,  0x1601 , {DEFTYPE_PDOMAPPING , 60 | (OBJCODE_REC << 8)} , asEntryDesc0x1601 , aName0x1601 , &OutputDataProcessDataMapping0x1601 , NULL , NULL , 0x0000 },
/* Object 0x1A00 */
{NULL , NULL ,  0x1A00 , {DEFTYPE_PDOMAPPING , 253 | (OBJCODE_REC << 8)} , asEntryDesc0x1A00 , aName0x1A00 , &InputDataProcessDataMapping0x1A00 , NULL , NULL , 0x0000 },
/* Object 0x1A01 */
{NULL , NULL ,  0x1A01 , {DEFTYPE_PDOMAPPING , 26 | (OBJCODE_REC << 8)} , asEntryDesc0x1A01 , aName0x1A01 , &InputDataProcessDataMapping0x1A01 , NULL , NULL , 0x0000 },
/* Object 0x1C12 */
{NULL , NULL ,  0x1C12 , {DEFTYPE_UNSIGNED16 , 1 | (OBJCODE_ARR << 8)} , asEntryDesc0x1C12 , aName0x1C12 , &sRxPDOassign , NULL , NULL , 0x0000 },
/* Object 0x1C13 */
{NULL , NULL ,  0x1C13 , {DEFTYPE_UNSIGNED16 , 2 | (OBJCODE_ARR << 8)} , asEntryDesc0x1C13 , aName0x1C13 , &sTxPDOassign , NULL , NULL , 0x0000 },
/* Object 0x6000 */
{NULL , NULL ,  0x6000 , {DEFTYPE_RECORD , 253 | (OBJCODE_REC << 8)} , asEntryDesc0x6000 , aName0x6000 , &InputData0x6000 , NULL , NULL , 0x0000 },
/* Object 0x6010 */
{NULL , NULL ,  0x6010 , {DEFTYPE_RECORD , 26 | (OBJCODE_REC << 8)} , asEntryDesc0x6010 , aName0x6010 , &InputData0x6010 , NULL , NULL , 0x0000 },
/* Object 0x7010 */
{NULL , NULL ,  0x7010 , {DEFTYPE_RECORD , 60 | (OBJCODE_REC << 8)} , asEntryDesc0x7010 , aName0x7010 , &OutputData0x7010 , NULL , NULL , 0x0000 },
{NULL,NULL, 0xFFFF, {0, 0}, NULL, NULL, NULL, NULL}};
#endif    //#ifdef _OBJD_

#undef PROTO

/** @}*/
#define _LAN9252_APP_OBJECTS_H_
