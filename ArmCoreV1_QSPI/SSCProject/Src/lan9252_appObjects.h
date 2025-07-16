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
={60,0x70100108,0x70100208,0x70100320,0x70100408,0x70100508,0x70100610,0x70100708,0x70100808,0x70100910,0x70100A08,0x70100B08,0x70100C08,0x70100D08,0x70100E10,0x70100F20,0x70101020,0x70101108,0x70101208,0x70101310,0x70101420,0x70101520,0x70101620,0x70101708,0x70101808,0x70101920,0x70101A20,0x70101B08,0x70101C08,0x70101D20,0x70101E20,0x70101F08,0x70102008,0x70102120,0x70102220,0x70102308,0x70102408,0x70102510,0x70102620,0x70102720,0x70102808,0x70102908,0x70102A08,0x70102B08,0x70102C20,0x70102D20,0x70102E08,0x70102F08,0x70103020,0x70103120,0x70103208,0x70103308,0x70103410,0x70103508,0x70103608,0x70103708,0x70103808,0x70103908,0x70103A08,0x70103B20,0x70103C08}
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
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }}; /* Subindex194 - Reference to 0x6000.194 */

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
"SubIndex 194\000\377";
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
} OBJ_STRUCT_PACKED_END
TOBJ1A00;
#endif //#ifndef _LAN9252_APP_OBJECTS_H_

/**
* \brief Object variable
*/
PROTO TOBJ1A00 InputDataProcessDataMapping0x1A00
#if defined(_LAN9252_APP_) && (_LAN9252_APP_ == 1)
={194,0x60000108,0x60000208,0x60000320,0x60000408,0x60000508,0x60000610,0x60000708,0x60000808,0x60000908,0x60000A08,0x60000B08,0x60000C08,0x60000D10,0x60000E20,0x60000F20,0x60001020,0x60001120,0x60001201,0x60001301,0x60001401,0x60001501,0x60001601,0x60001701,0x60001802,0x60001901,0x60001A07,0x60001B04,0x60001C01,0x60001D01,0x60001E01,0x60001F01,0x60002008,0x60002101,0x60002201,0x60002301,0x60002401,0x60002501,0x60002601,0x60002701,0x60002801,0x60002901,0x60002A01,0x60002B01,0x60002C01,0x60002D01,0x60002E01,0x60002F01,0x60003001,0x60003101,0x60003201,0x60003301,0x60003401,0x60003501,0x60003601,0x60003701,0x60003801,0x60003901,0x60003A01,0x60003B01,0x60003C01,0x60003D01,0x60003E01,0x60003F01,0x60004001,0x60004101,0x60004201,0x60004301,0x60004401,0x60004501,0x60004601,0x60004701,0x60004801,0x60004901,0x60004A01,0x60004B01,0x60004C01,0x60004D01,0x60004E01,0x60004F01,0x60005001,0x60005110,0x60005201,0x60005307,0x60005408,0x60005501,0x60005601,0x60005701,0x60005801,0x60005901,0x60005A01,0x60005B01,0x60005C01,0x60005D08,0x60005E10,0x60005F01,0x60006001,0x60006101,0x60006201,0x60006301,0x60006401,0x60006501,0x60006601,0x60006708,0x60006808,0x60006908,0x60006A10,0x60006B08,0x60006C08,0x60006D10,0x60006E20,0x60006F20,0x60007020,0x60007120,0x60007208,0x60007308,0x60007410,0x60007508,0x60007608,0x60007710,0x60007820,0x60007920,0x60007A20,0x60007B20,0x60007C20,0x60007D20,0x60007E20,0x60007F08,0x60008008,0x60008120,0x60008220,0x60008320,0x60008408,0x60008508,0x60008620,0x60008720,0x60008820,0x60008908,0x60008A08,0x60008B10,0x60008C08,0x60008D08,0x60008E10,0x60008F20,0x60009020,0x60009120,0x60009220,0x60009308,0x60009408,0x60009510,0x60009608,0x60009708,0x60009810,0x60009920,0x60009A20,0x60009B20,0x60009C20,0x60009D08,0x60009E08,0x60009F10,0x6000A008,0x6000A108,0x6000A210,0x6000A320,0x6000A420,0x6000A520,0x6000A620,0x6000A720,0x6000A820,0x6000A920,0x6000AA08,0x6000AB08,0x6000AC10,0x6000AD08,0x6000AE08,0x6000AF10,0x6000B020,0x6000B120,0x6000B220,0x6000B320,0x6000B420,0x6000B520,0x6000B620,0x6000B720,0x6000B820,0x6000B920,0x6000BA20,0x6000BB20,0x6000BC20,0x6000BD20,0x6000BE20,0x6000BF20,0x6000C020,0x6000C110,0x6000C220}
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
UINT16 aEntries[1];  /**< \brief Subindex 1 - 1 */
} OBJ_STRUCT_PACKED_END
TOBJ1C13;
#endif //#ifndef _LAN9252_APP_OBJECTS_H_

/**
* \brief Object variable
*/
PROTO TOBJ1C13 sTxPDOassign
#if defined(_LAN9252_APP_) && (_LAN9252_APP_ == 1)
={1,{0x1A00}}
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
* SubIndex 1 - InU8_BoardID<br>
* SubIndex 2 - InU8_Reserved0<br>
* SubIndex 3 - InU32_FirmWareVersion<br>
* SubIndex 4 - InU8_beam_id<br>
* SubIndex 5 - InU8_reserved1<br>
* SubIndex 6 - InU16_radiation_index<br>
* SubIndex 7 - InU8_rtm_on_arm_fsm_state_current<br>
* SubIndex 8 - InU8_rtm_on_arm_ctrl_mode_cur<br>
* SubIndex 9 - InU8_ethercat_Link_state<br>
* SubIndex 10 - InU8_reserved2<br>
* SubIndex 11 - InU8_rtm_on_arm_beam_id<br>
* SubIndex 12 - InU8_reserved3<br>
* SubIndex 13 - InU16_rtm_on_arm_radiation_index<br>
* SubIndex 14 - InU32_rtm_on_arm_not_ready_event<br>
* SubIndex 15 - InU32_rtm_on_arm_warning_interlock<br>
* SubIndex 16 - InU32_rtm_on_arm_minor_interlock<br>
* SubIndex 17 - InU32_rtm_on_arm_serious_interlock<br>
* SubIndex 18 - InB1_ON_DI_RTMON_CONTACTOR_FB<br>
* SubIndex 19 - InB1_ON_DI_Emergency_Reserve<br>
* SubIndex 20 - InB1_ON_DI_HVEN<br>
* SubIndex 21 - InB1_ON_DI_BSM_NOT_READY<br>
* SubIndex 22 - InB1_ON_DI_KV_TreatmentEN<br>
* SubIndex 23 - InB1_ON_DI_MV_TreatmentEN<br>
* SubIndex 24 - InB2_ON_DI_reserve0<br>
* SubIndex 25 - InB1_ON_DI_DI_GATING<br>
* SubIndex 26 - InB7_ON_DI_reserve1<br>
* SubIndex 27 - InB4_ON_DO_reserve0<br>
* SubIndex 28 - InB1_ON_DO_MV_TreatmentEN<br>
* SubIndex 29 - InB1_ON_DO_KV_TreatmentEN<br>
* SubIndex 30 - InB1_ON_DO_Emergency<br>
* SubIndex 31 - InB1_ON_DO_Pulse_Inhibit<br>
* SubIndex 32 - InU8_ON_DO_reserve1<br>
* SubIndex 33 - InB1_OFF_DI_STAND_BREAKER1<br>
* SubIndex 34 - InB1_OFF_DI_STAND_BREAKER2<br>
* SubIndex 35 - InB1_OFF_DI_STAND_BREAKER7<br>
* SubIndex 36 - InB1_OFF_DI_TREATMENT_ROOM_DOOR_READY <br>
* SubIndex 37 - InB1_OFF_DI_CONTROL_ROOM_EMERGENCY <br>
* SubIndex 38 - InB1_OFF_DI_CITB_TREATMENT_ROOM_DOOR2 <br>
* SubIndex 39 - InB1_OFF_DI_CITB_EMERGENCY4 <br>
* SubIndex 40 - InB1_OFF_DI_CONTROL_ROOM_BREAKER1 <br>
* SubIndex 41 - InB1_OFF_DI_UserTreatmentEnable <br>
* SubIndex 42 - InB1_OFF_DI_UserHvEnable <br>
* SubIndex 43 - InB1_OFF_DI_UserMoveEnable <br>
* SubIndex 44 - InB1_OFF_DI_STAND_RESERVE <br>
* SubIndex 45 - InB1_OFF_DI_STAND_EMERGENCY <br>
* SubIndex 46 - InB1_OFF_DI_STAND_BREAKER4 <br>
* SubIndex 47 - InB1_OFF_DI_TouchGuard <br>
* SubIndex 48 - InB1_OFF_DI_reserve0 <br>
* SubIndex 49 - InB1_OFF_DI_STAND_BREAKER3 <br>
* SubIndex 50 - InB1_OFF_DI_COVER_EMERGENCY4 <br>
* SubIndex 51 - InB1_OFF_DI_STAND_CONTACTOR1 <br>
* SubIndex 52 - InB1_OFF_DI_COVER_EMERGENCY1 <br>
* SubIndex 53 - InB1_OFF_DI_CONTROL_ROOM_BREAKER3 <br>
* SubIndex 54 - InB1_OFF_DI_UPS_LOAD_PORT <br>
* SubIndex 55 - InB1_OFF_DI_CITB_TREATMENT_ROOM_DOOR1 <br>
* SubIndex 56 - InB1_OFF_DI_CITB_EMERGENCY2 <br>
* SubIndex 57 - InB1_OFF_DI_COVER_DOOR <br>
* SubIndex 58 - InB1_OFF_DI_HvKey <br>
* SubIndex 59 - InB1_OFF_DI_CONTROL_ROOM_CONTACTOR1 <br>
* SubIndex 60 - InB1_OFF_DI_CITB_EMERGENCY3 <br>
* SubIndex 61 - InB1_OFF_DI_STAND_CONTACTOR2 <br>
* SubIndex 62 - InB1_OFF_DI_COVER_EMERGENCY3 <br>
* SubIndex 63 - InB1_OFF_DI_CONTROL_ROOM_BREAKER5 <br>
* SubIndex 64 - InB1_OFF_DI_UPS_ON_BYPASS <br>
* SubIndex 65 - InB1_OFF_DI_STAND_BREAKER5 <br>
* SubIndex 66 - InB1_OFF_DI_COVER_RESERVE <br>
* SubIndex 67 - InB1_OFF_DI_CONTROL_ROOM_CONTACTOR2 <br>
* SubIndex 68 - InB1_OFF_DI_CITB_EMERGENCY5 <br>
* SubIndex 69 - InB1_OFF_DI_CITB_SEARCH_TREATMENT_ROOM <br>
* SubIndex 70 - InB1_OFF_DI_CITB_EXTERNAL_TERMINATE <br>
* SubIndex 71 - InB1_OFF_DI_UPS_LOW_BATT <br>
* SubIndex 72 - InB1_OFF_DI_CONTROL_ROOM_BREAKER2 <br>
* SubIndex 73 - InB1_OFF_DI_CONTROL_ROOM_BREAKER4 <br>
* SubIndex 74 - InB1_OFF_DI_UPS_ON_BATT <br>
* SubIndex 75 - InB1_OFF_DI_CITB_EMERGENCY1 <br>
* SubIndex 76 - InB1_OFF_DI_COVER_EMERGENCY2 <br>
* SubIndex 77 - InB1_OFF_DI_STAND_BREAKER6 <br>
* SubIndex 78 - InB1_OFF_DI_HvEn <br>
* SubIndex 79 - InB1_OFF_DI_MV_TreatmentEN <br>
* SubIndex 80 - InB1_OFF_DI_KV_TreatmentEN <br>
* SubIndex 81 - InU16_OFF_DI_tca9535_di_reserve1<br>
* SubIndex 82 - InB1_OFF_DI_GATING <br>
* SubIndex 83 - InB7_OFF_DI_reserve2<br>
* SubIndex 84 - InU8_OFF_DI_reserve3<br>
* SubIndex 85 - InB1_OFF_DO_TreatmentRoomLight <br>
* SubIndex 86 - InB1_OFF_DO_STAND_RESERVE <br>
* SubIndex 87 - InB1_OFF_DO_Laser <br>
* SubIndex 88 - InB1_OFF_DO_RadiationIndicator <br>
* SubIndex 89 - InB1_OFF_DO_ReadyIndicator <br>
* SubIndex 90 - InB1_OFF_DO_SearchTreatmentRoomRelay <br>
* SubIndex 91 - InB1_OFF_DO_softwareTouchGuard <br>
* SubIndex 92 - InB1_OFF_DO_reserve4<br>
* SubIndex 93 - InU8_OFF_DO_reserve5<br>
* SubIndex 94 - InU16_OFF_DO_tca9535_do_reserve6<br>
* SubIndex 95 - InB1_OFF_DO_SoftwareMVTreatmentEn <br>
* SubIndex 96 - InB1_OFF_DO_SoftwareKVTreatmentEn <br>
* SubIndex 97 - InB1_OFF_DO_softwareMoveEN <br>
* SubIndex 98 - InB1_OFF_DO_SoftwareHvEn <br>
* SubIndex 99 - InB1_OFF_DO_TreatmentMotionEnable <br>
* SubIndex 100 - InB1_OFF_DO_ThreePhasePowerOn <br>
* SubIndex 101 - InB1_OFF_DO_AsuMotionEnable <br>
* SubIndex 102 - InB1_OFF_DO_reserve7<br>
* SubIndex 103 - InU8_OFF_DO_reserve8<br>
* SubIndex 104 - InU8_icm_fsm_state_current<br>
* SubIndex 105 - InU8_icm_ctrl_mode_cur<br>
* SubIndex 106 - InU16_reserved4<br>
* SubIndex 107 - InU8_icm_beam_id<br>
* SubIndex 108 - InU8_reserved5<br>
* SubIndex 109 - InU16_icm_radiation_index<br>
* SubIndex 110 - InU32_icm_not_ready_event<br>
* SubIndex 111 - InU32_icm_warning_interlock<br>
* SubIndex 112 - InU32_icm_minor_interlock<br>
* SubIndex 113 - InU32_icm_serious_interlock<br>
* SubIndex 114 - InU8_bgm_fsm_state_current<br>
* SubIndex 115 - InU8_bgm_ctrl_mode_cur<br>
* SubIndex 116 - InU16_reserved6<br>
* SubIndex 117 - InU8_bgm_beam_id<br>
* SubIndex 118 - InU8_reserved7<br>
* SubIndex 119 - InU16_bgm_radiation_index<br>
* SubIndex 120 - InU32_bgm_not_ready_event<br>
* SubIndex 121 - InU32_bgm_warning_interlock<br>
* SubIndex 122 - InU32_bgm_minor_interlock<br>
* SubIndex 123 - InU32_bgm_serious_interlock<br>
* SubIndex 124 - InF_beam_on_time<br>
* SubIndex 125 - InF_primary_dose_current<br>
* SubIndex 126 - InF_primary_dose_rate_current<br>
* SubIndex 127 - InU8_primary_dose_fsm_state<br>
* SubIndex 128 - InU8_reserved8<br>
* SubIndex 129 - InU32_primary_dose_interlock<br>
* SubIndex 130 - InF_second_dose_current<br>
* SubIndex 131 - InF_secondry_dose_rate_current<br>
* SubIndex 132 - InU8_secondry_dose_fsm_rate<br>
* SubIndex 133 - InU8_reserved9<br>
* SubIndex 134 - InU32_secondry_dose_interlock<br>
* SubIndex 135 - InU32_afc_interlock<br>
* SubIndex 136 - InF_afc_position_current<br>
* SubIndex 137 - InU8_qam_fsm_state_current<br>
* SubIndex 138 - InU8_qam_ctrl_mode_cur<br>
* SubIndex 139 - InU16_reserved10<br>
* SubIndex 140 - InU8_qam_beam_id<br>
* SubIndex 141 - InU8_reserved11<br>
* SubIndex 142 - InU16_qam_radiation_index<br>
* SubIndex 143 - InU32_qam_not_ready_event<br>
* SubIndex 144 - InU32_qam_warning_interlock<br>
* SubIndex 145 - InU32_qam_minor_interlock<br>
* SubIndex 146 - InU32_qam_serious_interlock<br>
* SubIndex 147 - InU8_rtm_off_fsm_state_current<br>
* SubIndex 148 - InU8_rtm_off_ctrl_mode_cur<br>
* SubIndex 149 - InU16_reserved12<br>
* SubIndex 150 - InU8_rtm_off_beam_id<br>
* SubIndex 151 - InU8_reserved13<br>
* SubIndex 152 - InU16_rtm_off_radiation_index<br>
* SubIndex 153 - InU32_rtm_off_not_ready_event<br>
* SubIndex 154 - InU32_rtm_off_warning_interlock<br>
* SubIndex 155 - InU32_rtm_off_minor_interlock<br>
* SubIndex 156 - InU32_rtm_off_serious_interlock<br>
* SubIndex 157 - InU8_gmm_fsm_state_current<br>
* SubIndex 158 - InU8_gmm_ctrl_mode_cur<br>
* SubIndex 159 - InU16_reserved14<br>
* SubIndex 160 - InU8_gmm_beam_id<br>
* SubIndex 161 - InU8_reserved15<br>
* SubIndex 162 - InU16_gmm_radiation_index<br>
* SubIndex 163 - InU32_gmm_not_ready_event<br>
* SubIndex 164 - InU32_gmm_warning_interlock<br>
* SubIndex 165 - InU32_gmm_minor_interlock<br>
* SubIndex 166 - InU32_gmm_serious_interlock<br>
* SubIndex 167 - InU32_gmm_move_status<br>
* SubIndex 168 - InF_gmm_position_cur<br>
* SubIndex 169 - InF_gmm_velocity_cur<br>
* SubIndex 170 - InU8_psm_fsm_state_current<br>
* SubIndex 171 - InU8_psm_ctrl_mode_cur<br>
* SubIndex 172 - InU16_reserved16<br>
* SubIndex 173 - InU8_psm_beam_id<br>
* SubIndex 174 - InU8_reserved17<br>
* SubIndex 175 - InU16_psm_radiation_index<br>
* SubIndex 176 - InU32_psm_not_ready_event<br>
* SubIndex 177 - InU32_psm_warning_interlock<br>
* SubIndex 178 - InU32_psm_minor_interlock<br>
* SubIndex 179 - InU32_psm_serious_interlock<br>
* SubIndex 180 - InU32_psm_move_status<br>
* SubIndex 181 - InF_psm_position_x_cur<br>
* SubIndex 182 - InF_psm_position_y_cur<br>
* SubIndex 183 - InF_psm_position_z_cur<br>
* SubIndex 184 - InF_psm_position_x_r_cur<br>
* SubIndex 185 - InF_psm_position_y_r_cur<br>
* SubIndex 186 - InF_psm_position_z_r_cur<br>
* SubIndex 187 - InF_psm_velocity_x_cur<br>
* SubIndex 188 - InF_psm_velocity_y_cur<br>
* SubIndex 189 - InF_psm_velocity_z_cur<br>
* SubIndex 190 - InF_psm_velocity_x_r_cur<br>
* SubIndex 191 - InF_psm_velocity_y_r_cur<br>
* SubIndex 192 - InF_psm_velocity_z_r_cur<br>
* SubIndex 193 - InU16_FkpButton<br>
* SubIndex 194 - InU32_CpgButton<br>
*/
OBJCONST TSDOINFOENTRYDESC    OBJMEM asEntryDesc0x6000[] = {
{ DEFTYPE_UNSIGNED8 , 0x8 , ACCESS_READ },
{ DEFTYPE_UNSIGNED8 , 0x08 , ACCESS_READ }, /* Subindex1 - InU8_BoardID */
{ DEFTYPE_UNSIGNED8 , 0x08 , ACCESS_READ }, /* Subindex2 - InU8_Reserved0 */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex3 - InU32_FirmWareVersion */
{ DEFTYPE_UNSIGNED8 , 0x08 , ACCESS_READ }, /* Subindex4 - InU8_beam_id */
{ DEFTYPE_UNSIGNED8 , 0x08 , ACCESS_READ }, /* Subindex5 - InU8_reserved1 */
{ DEFTYPE_UNSIGNED16 , 0x10 , ACCESS_READ }, /* Subindex6 - InU16_radiation_index */
{ DEFTYPE_UNSIGNED8 , 0x08 , ACCESS_READ }, /* Subindex7 - InU8_rtm_on_arm_fsm_state_current */
{ DEFTYPE_UNSIGNED8 , 0x08 , ACCESS_READ }, /* Subindex8 - InU8_rtm_on_arm_ctrl_mode_cur */
{ DEFTYPE_UNSIGNED8 , 0x08 , ACCESS_READ }, /* Subindex9 - InU8_ethercat_Link_state */
{ DEFTYPE_UNSIGNED8 , 0x08 , ACCESS_READ }, /* Subindex10 - InU8_reserved2 */
{ DEFTYPE_UNSIGNED8 , 0x08 , ACCESS_READ }, /* Subindex11 - InU8_rtm_on_arm_beam_id */
{ DEFTYPE_UNSIGNED8 , 0x08 , ACCESS_READ }, /* Subindex12 - InU8_reserved3 */
{ DEFTYPE_UNSIGNED16 , 0x10 , ACCESS_READ }, /* Subindex13 - InU16_rtm_on_arm_radiation_index */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex14 - InU32_rtm_on_arm_not_ready_event */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex15 - InU32_rtm_on_arm_warning_interlock */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex16 - InU32_rtm_on_arm_minor_interlock */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex17 - InU32_rtm_on_arm_serious_interlock */
{ DEFTYPE_BOOLEAN , 0x01 , ACCESS_READ }, /* Subindex18 - InB1_ON_DI_RTMON_CONTACTOR_FB */
{ DEFTYPE_BOOLEAN , 0x01 , ACCESS_READ }, /* Subindex19 - InB1_ON_DI_Emergency_Reserve */
{ DEFTYPE_BOOLEAN , 0x01 , ACCESS_READ }, /* Subindex20 - InB1_ON_DI_HVEN */
{ DEFTYPE_BOOLEAN , 0x01 , ACCESS_READ }, /* Subindex21 - InB1_ON_DI_BSM_NOT_READY */
{ DEFTYPE_BOOLEAN , 0x01 , ACCESS_READ }, /* Subindex22 - InB1_ON_DI_KV_TreatmentEN */
{ DEFTYPE_BOOLEAN , 0x01 , ACCESS_READ }, /* Subindex23 - InB1_ON_DI_MV_TreatmentEN */
{ DEFTYPE_BIT2 , 0x02 , ACCESS_READ }, /* Subindex24 - InB2_ON_DI_reserve0 */
{ DEFTYPE_BOOLEAN , 0x01 , ACCESS_READ }, /* Subindex25 - InB1_ON_DI_DI_GATING */
{ DEFTYPE_BIT7 , 0x07 , ACCESS_READ }, /* Subindex26 - InB7_ON_DI_reserve1 */
{ DEFTYPE_BIT4 , 0x04 , ACCESS_READ }, /* Subindex27 - InB4_ON_DO_reserve0 */
{ DEFTYPE_BOOLEAN , 0x01 , ACCESS_READ }, /* Subindex28 - InB1_ON_DO_MV_TreatmentEN */
{ DEFTYPE_BOOLEAN , 0x01 , ACCESS_READ }, /* Subindex29 - InB1_ON_DO_KV_TreatmentEN */
{ DEFTYPE_BOOLEAN , 0x01 , ACCESS_READ }, /* Subindex30 - InB1_ON_DO_Emergency */
{ DEFTYPE_BOOLEAN , 0x01 , ACCESS_READ }, /* Subindex31 - InB1_ON_DO_Pulse_Inhibit */
{ DEFTYPE_UNSIGNED8 , 0x08 , ACCESS_READ }, /* Subindex32 - InU8_ON_DO_reserve1 */
{ DEFTYPE_BOOLEAN , 0x01 , ACCESS_READ }, /* Subindex33 - InB1_OFF_DI_STAND_BREAKER1 */
{ DEFTYPE_BOOLEAN , 0x01 , ACCESS_READ }, /* Subindex34 - InB1_OFF_DI_STAND_BREAKER2 */
{ DEFTYPE_BOOLEAN , 0x01 , ACCESS_READ }, /* Subindex35 - InB1_OFF_DI_STAND_BREAKER7 */
{ DEFTYPE_BOOLEAN , 0x01 , ACCESS_READ }, /* Subindex36 - InB1_OFF_DI_TREATMENT_ROOM_DOOR_READY  */
{ DEFTYPE_BOOLEAN , 0x01 , ACCESS_READ }, /* Subindex37 - InB1_OFF_DI_CONTROL_ROOM_EMERGENCY  */
{ DEFTYPE_BOOLEAN , 0x01 , ACCESS_READ }, /* Subindex38 - InB1_OFF_DI_CITB_TREATMENT_ROOM_DOOR2  */
{ DEFTYPE_BOOLEAN , 0x01 , ACCESS_READ }, /* Subindex39 - InB1_OFF_DI_CITB_EMERGENCY4  */
{ DEFTYPE_BOOLEAN , 0x01 , ACCESS_READ }, /* Subindex40 - InB1_OFF_DI_CONTROL_ROOM_BREAKER1  */
{ DEFTYPE_BOOLEAN , 0x01 , ACCESS_READ }, /* Subindex41 - InB1_OFF_DI_UserTreatmentEnable  */
{ DEFTYPE_BOOLEAN , 0x01 , ACCESS_READ }, /* Subindex42 - InB1_OFF_DI_UserHvEnable  */
{ DEFTYPE_BOOLEAN , 0x01 , ACCESS_READ }, /* Subindex43 - InB1_OFF_DI_UserMoveEnable  */
{ DEFTYPE_BOOLEAN , 0x01 , ACCESS_READ }, /* Subindex44 - InB1_OFF_DI_STAND_RESERVE  */
{ DEFTYPE_BOOLEAN , 0x01 , ACCESS_READ }, /* Subindex45 - InB1_OFF_DI_STAND_EMERGENCY  */
{ DEFTYPE_BOOLEAN , 0x01 , ACCESS_READ }, /* Subindex46 - InB1_OFF_DI_STAND_BREAKER4  */
{ DEFTYPE_BOOLEAN , 0x01 , ACCESS_READ }, /* Subindex47 - InB1_OFF_DI_TouchGuard  */
{ DEFTYPE_BOOLEAN , 0x01 , ACCESS_READ }, /* Subindex48 - InB1_OFF_DI_reserve0  */
{ DEFTYPE_BOOLEAN , 0x01 , ACCESS_READ }, /* Subindex49 - InB1_OFF_DI_STAND_BREAKER3  */
{ DEFTYPE_BOOLEAN , 0x01 , ACCESS_READ }, /* Subindex50 - InB1_OFF_DI_COVER_EMERGENCY4  */
{ DEFTYPE_BOOLEAN , 0x01 , ACCESS_READ }, /* Subindex51 - InB1_OFF_DI_STAND_CONTACTOR1  */
{ DEFTYPE_BOOLEAN , 0x01 , ACCESS_READ }, /* Subindex52 - InB1_OFF_DI_COVER_EMERGENCY1  */
{ DEFTYPE_BOOLEAN , 0x01 , ACCESS_READ }, /* Subindex53 - InB1_OFF_DI_CONTROL_ROOM_BREAKER3  */
{ DEFTYPE_BOOLEAN , 0x01 , ACCESS_READ }, /* Subindex54 - InB1_OFF_DI_UPS_LOAD_PORT  */
{ DEFTYPE_BOOLEAN , 0x01 , ACCESS_READ }, /* Subindex55 - InB1_OFF_DI_CITB_TREATMENT_ROOM_DOOR1  */
{ DEFTYPE_BOOLEAN , 0x01 , ACCESS_READ }, /* Subindex56 - InB1_OFF_DI_CITB_EMERGENCY2  */
{ DEFTYPE_BOOLEAN , 0x01 , ACCESS_READ }, /* Subindex57 - InB1_OFF_DI_COVER_DOOR  */
{ DEFTYPE_BOOLEAN , 0x01 , ACCESS_READ }, /* Subindex58 - InB1_OFF_DI_HvKey  */
{ DEFTYPE_BOOLEAN , 0x01 , ACCESS_READ }, /* Subindex59 - InB1_OFF_DI_CONTROL_ROOM_CONTACTOR1  */
{ DEFTYPE_BOOLEAN , 0x01 , ACCESS_READ }, /* Subindex60 - InB1_OFF_DI_CITB_EMERGENCY3  */
{ DEFTYPE_BOOLEAN , 0x01 , ACCESS_READ }, /* Subindex61 - InB1_OFF_DI_STAND_CONTACTOR2  */
{ DEFTYPE_BOOLEAN , 0x01 , ACCESS_READ }, /* Subindex62 - InB1_OFF_DI_COVER_EMERGENCY3  */
{ DEFTYPE_BOOLEAN , 0x01 , ACCESS_READ }, /* Subindex63 - InB1_OFF_DI_CONTROL_ROOM_BREAKER5  */
{ DEFTYPE_BOOLEAN , 0x01 , ACCESS_READ }, /* Subindex64 - InB1_OFF_DI_UPS_ON_BYPASS  */
{ DEFTYPE_BOOLEAN , 0x01 , ACCESS_READ }, /* Subindex65 - InB1_OFF_DI_STAND_BREAKER5  */
{ DEFTYPE_BOOLEAN , 0x01 , ACCESS_READ }, /* Subindex66 - InB1_OFF_DI_COVER_RESERVE  */
{ DEFTYPE_BOOLEAN , 0x01 , ACCESS_READ }, /* Subindex67 - InB1_OFF_DI_CONTROL_ROOM_CONTACTOR2  */
{ DEFTYPE_BOOLEAN , 0x01 , ACCESS_READ }, /* Subindex68 - InB1_OFF_DI_CITB_EMERGENCY5  */
{ DEFTYPE_BOOLEAN , 0x01 , ACCESS_READ }, /* Subindex69 - InB1_OFF_DI_CITB_SEARCH_TREATMENT_ROOM  */
{ DEFTYPE_BOOLEAN , 0x01 , ACCESS_READ }, /* Subindex70 - InB1_OFF_DI_CITB_EXTERNAL_TERMINATE  */
{ DEFTYPE_BOOLEAN , 0x01 , ACCESS_READ }, /* Subindex71 - InB1_OFF_DI_UPS_LOW_BATT  */
{ DEFTYPE_BOOLEAN , 0x01 , ACCESS_READ }, /* Subindex72 - InB1_OFF_DI_CONTROL_ROOM_BREAKER2  */
{ DEFTYPE_BOOLEAN , 0x01 , ACCESS_READ }, /* Subindex73 - InB1_OFF_DI_CONTROL_ROOM_BREAKER4  */
{ DEFTYPE_BOOLEAN , 0x01 , ACCESS_READ }, /* Subindex74 - InB1_OFF_DI_UPS_ON_BATT  */
{ DEFTYPE_BOOLEAN , 0x01 , ACCESS_READ }, /* Subindex75 - InB1_OFF_DI_CITB_EMERGENCY1  */
{ DEFTYPE_BOOLEAN , 0x01 , ACCESS_READ }, /* Subindex76 - InB1_OFF_DI_COVER_EMERGENCY2  */
{ DEFTYPE_BOOLEAN , 0x01 , ACCESS_READ }, /* Subindex77 - InB1_OFF_DI_STAND_BREAKER6  */
{ DEFTYPE_BOOLEAN , 0x01 , ACCESS_READ }, /* Subindex78 - InB1_OFF_DI_HvEn  */
{ DEFTYPE_BOOLEAN , 0x01 , ACCESS_READ }, /* Subindex79 - InB1_OFF_DI_MV_TreatmentEN  */
{ DEFTYPE_BOOLEAN , 0x01 , ACCESS_READ }, /* Subindex80 - InB1_OFF_DI_KV_TreatmentEN  */
{ DEFTYPE_UNSIGNED16 , 0x10 , ACCESS_READ }, /* Subindex81 - InU16_OFF_DI_tca9535_di_reserve1 */
{ DEFTYPE_BOOLEAN , 0x01 , ACCESS_READ }, /* Subindex82 - InB1_OFF_DI_GATING  */
{ DEFTYPE_BIT7 , 0x07 , ACCESS_READ }, /* Subindex83 - InB7_OFF_DI_reserve2 */
{ DEFTYPE_UNSIGNED8 , 0x08 , ACCESS_READ }, /* Subindex84 - InU8_OFF_DI_reserve3 */
{ DEFTYPE_BOOLEAN , 0x01 , ACCESS_READ }, /* Subindex85 - InB1_OFF_DO_TreatmentRoomLight  */
{ DEFTYPE_BOOLEAN , 0x01 , ACCESS_READ }, /* Subindex86 - InB1_OFF_DO_STAND_RESERVE  */
{ DEFTYPE_BOOLEAN , 0x01 , ACCESS_READ }, /* Subindex87 - InB1_OFF_DO_Laser  */
{ DEFTYPE_BOOLEAN , 0x01 , ACCESS_READ }, /* Subindex88 - InB1_OFF_DO_RadiationIndicator  */
{ DEFTYPE_BOOLEAN , 0x01 , ACCESS_READ }, /* Subindex89 - InB1_OFF_DO_ReadyIndicator  */
{ DEFTYPE_BOOLEAN , 0x01 , ACCESS_READ }, /* Subindex90 - InB1_OFF_DO_SearchTreatmentRoomRelay  */
{ DEFTYPE_BOOLEAN , 0x01 , ACCESS_READ }, /* Subindex91 - InB1_OFF_DO_softwareTouchGuard  */
{ DEFTYPE_BOOLEAN , 0x01 , ACCESS_READ }, /* Subindex92 - InB1_OFF_DO_reserve4 */
{ DEFTYPE_UNSIGNED8 , 0x08 , ACCESS_READ }, /* Subindex93 - InU8_OFF_DO_reserve5 */
{ DEFTYPE_UNSIGNED16 , 0x10 , ACCESS_READ }, /* Subindex94 - InU16_OFF_DO_tca9535_do_reserve6 */
{ DEFTYPE_BOOLEAN , 0x01 , ACCESS_READ }, /* Subindex95 - InB1_OFF_DO_SoftwareMVTreatmentEn  */
{ DEFTYPE_BOOLEAN , 0x01 , ACCESS_READ }, /* Subindex96 - InB1_OFF_DO_SoftwareKVTreatmentEn  */
{ DEFTYPE_BOOLEAN , 0x01 , ACCESS_READ }, /* Subindex97 - InB1_OFF_DO_softwareMoveEN  */
{ DEFTYPE_BOOLEAN , 0x01 , ACCESS_READ }, /* Subindex98 - InB1_OFF_DO_SoftwareHvEn  */
{ DEFTYPE_BOOLEAN , 0x01 , ACCESS_READ }, /* Subindex99 - InB1_OFF_DO_TreatmentMotionEnable  */
{ DEFTYPE_BOOLEAN , 0x01 , ACCESS_READ }, /* Subindex100 - InB1_OFF_DO_ThreePhasePowerOn  */
{ DEFTYPE_BOOLEAN , 0x01 , ACCESS_READ }, /* Subindex101 - InB1_OFF_DO_AsuMotionEnable  */
{ DEFTYPE_BOOLEAN , 0x01 , ACCESS_READ }, /* Subindex102 - InB1_OFF_DO_reserve7 */
{ DEFTYPE_UNSIGNED8 , 0x08 , ACCESS_READ }, /* Subindex103 - InU8_OFF_DO_reserve8 */
{ DEFTYPE_UNSIGNED8 , 0x08 , ACCESS_READ }, /* Subindex104 - InU8_icm_fsm_state_current */
{ DEFTYPE_UNSIGNED8 , 0x08 , ACCESS_READ }, /* Subindex105 - InU8_icm_ctrl_mode_cur */
{ DEFTYPE_UNSIGNED16 , 0x10 , ACCESS_READ }, /* Subindex106 - InU16_reserved4 */
{ DEFTYPE_UNSIGNED8 , 0x08 , ACCESS_READ }, /* Subindex107 - InU8_icm_beam_id */
{ DEFTYPE_UNSIGNED8 , 0x08 , ACCESS_READ }, /* Subindex108 - InU8_reserved5 */
{ DEFTYPE_UNSIGNED16 , 0x10 , ACCESS_READ }, /* Subindex109 - InU16_icm_radiation_index */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex110 - InU32_icm_not_ready_event */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex111 - InU32_icm_warning_interlock */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex112 - InU32_icm_minor_interlock */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex113 - InU32_icm_serious_interlock */
{ DEFTYPE_UNSIGNED8 , 0x08 , ACCESS_READ }, /* Subindex114 - InU8_bgm_fsm_state_current */
{ DEFTYPE_UNSIGNED8 , 0x08 , ACCESS_READ }, /* Subindex115 - InU8_bgm_ctrl_mode_cur */
{ DEFTYPE_UNSIGNED16 , 0x10 , ACCESS_READ }, /* Subindex116 - InU16_reserved6 */
{ DEFTYPE_UNSIGNED8 , 0x08 , ACCESS_READ }, /* Subindex117 - InU8_bgm_beam_id */
{ DEFTYPE_UNSIGNED8 , 0x08 , ACCESS_READ }, /* Subindex118 - InU8_reserved7 */
{ DEFTYPE_UNSIGNED16 , 0x10 , ACCESS_READ }, /* Subindex119 - InU16_bgm_radiation_index */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex120 - InU32_bgm_not_ready_event */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex121 - InU32_bgm_warning_interlock */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex122 - InU32_bgm_minor_interlock */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex123 - InU32_bgm_serious_interlock */
{ DEFTYPE_REAL32 , 0x20 , ACCESS_READ }, /* Subindex124 - InF_beam_on_time */
{ DEFTYPE_REAL32 , 0x20 , ACCESS_READ }, /* Subindex125 - InF_primary_dose_current */
{ DEFTYPE_REAL32 , 0x20 , ACCESS_READ }, /* Subindex126 - InF_primary_dose_rate_current */
{ DEFTYPE_UNSIGNED8 , 0x08 , ACCESS_READ }, /* Subindex127 - InU8_primary_dose_fsm_state */
{ DEFTYPE_UNSIGNED8 , 0x08 , ACCESS_READ }, /* Subindex128 - InU8_reserved8 */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex129 - InU32_primary_dose_interlock */
{ DEFTYPE_REAL32 , 0x20 , ACCESS_READ }, /* Subindex130 - InF_second_dose_current */
{ DEFTYPE_REAL32 , 0x20 , ACCESS_READ }, /* Subindex131 - InF_secondry_dose_rate_current */
{ DEFTYPE_UNSIGNED8 , 0x08 , ACCESS_READ }, /* Subindex132 - InU8_secondry_dose_fsm_rate */
{ DEFTYPE_UNSIGNED8 , 0x08 , ACCESS_READ }, /* Subindex133 - InU8_reserved9 */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex134 - InU32_secondry_dose_interlock */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex135 - InU32_afc_interlock */
{ DEFTYPE_REAL32 , 0x20 , ACCESS_READ }, /* Subindex136 - InF_afc_position_current */
{ DEFTYPE_UNSIGNED8 , 0x08 , ACCESS_READ }, /* Subindex137 - InU8_qam_fsm_state_current */
{ DEFTYPE_UNSIGNED8 , 0x08 , ACCESS_READ }, /* Subindex138 - InU8_qam_ctrl_mode_cur */
{ DEFTYPE_UNSIGNED16 , 0x10 , ACCESS_READ }, /* Subindex139 - InU16_reserved10 */
{ DEFTYPE_UNSIGNED8 , 0x08 , ACCESS_READ }, /* Subindex140 - InU8_qam_beam_id */
{ DEFTYPE_UNSIGNED8 , 0x08 , ACCESS_READ }, /* Subindex141 - InU8_reserved11 */
{ DEFTYPE_UNSIGNED16 , 0x10 , ACCESS_READ }, /* Subindex142 - InU16_qam_radiation_index */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex143 - InU32_qam_not_ready_event */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex144 - InU32_qam_warning_interlock */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex145 - InU32_qam_minor_interlock */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex146 - InU32_qam_serious_interlock */
{ DEFTYPE_UNSIGNED8 , 0x08 , ACCESS_READ }, /* Subindex147 - InU8_rtm_off_fsm_state_current */
{ DEFTYPE_UNSIGNED8 , 0x08 , ACCESS_READ }, /* Subindex148 - InU8_rtm_off_ctrl_mode_cur */
{ DEFTYPE_UNSIGNED16 , 0x10 , ACCESS_READ }, /* Subindex149 - InU16_reserved12 */
{ DEFTYPE_UNSIGNED8 , 0x08 , ACCESS_READ }, /* Subindex150 - InU8_rtm_off_beam_id */
{ DEFTYPE_UNSIGNED8 , 0x08 , ACCESS_READ }, /* Subindex151 - InU8_reserved13 */
{ DEFTYPE_UNSIGNED16 , 0x10 , ACCESS_READ }, /* Subindex152 - InU16_rtm_off_radiation_index */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex153 - InU32_rtm_off_not_ready_event */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex154 - InU32_rtm_off_warning_interlock */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex155 - InU32_rtm_off_minor_interlock */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex156 - InU32_rtm_off_serious_interlock */
{ DEFTYPE_UNSIGNED8 , 0x08 , ACCESS_READ }, /* Subindex157 - InU8_gmm_fsm_state_current */
{ DEFTYPE_UNSIGNED8 , 0x08 , ACCESS_READ }, /* Subindex158 - InU8_gmm_ctrl_mode_cur */
{ DEFTYPE_UNSIGNED16 , 0x10 , ACCESS_READ }, /* Subindex159 - InU16_reserved14 */
{ DEFTYPE_UNSIGNED8 , 0x08 , ACCESS_READ }, /* Subindex160 - InU8_gmm_beam_id */
{ DEFTYPE_UNSIGNED8 , 0x08 , ACCESS_READ }, /* Subindex161 - InU8_reserved15 */
{ DEFTYPE_UNSIGNED16 , 0x10 , ACCESS_READ }, /* Subindex162 - InU16_gmm_radiation_index */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex163 - InU32_gmm_not_ready_event */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex164 - InU32_gmm_warning_interlock */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex165 - InU32_gmm_minor_interlock */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex166 - InU32_gmm_serious_interlock */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex167 - InU32_gmm_move_status */
{ DEFTYPE_REAL32 , 0x20 , ACCESS_READ }, /* Subindex168 - InF_gmm_position_cur */
{ DEFTYPE_REAL32 , 0x20 , ACCESS_READ }, /* Subindex169 - InF_gmm_velocity_cur */
{ DEFTYPE_UNSIGNED8 , 0x08 , ACCESS_READ }, /* Subindex170 - InU8_psm_fsm_state_current */
{ DEFTYPE_UNSIGNED8 , 0x08 , ACCESS_READ }, /* Subindex171 - InU8_psm_ctrl_mode_cur */
{ DEFTYPE_UNSIGNED16 , 0x10 , ACCESS_READ }, /* Subindex172 - InU16_reserved16 */
{ DEFTYPE_UNSIGNED8 , 0x08 , ACCESS_READ }, /* Subindex173 - InU8_psm_beam_id */
{ DEFTYPE_UNSIGNED8 , 0x08 , ACCESS_READ }, /* Subindex174 - InU8_reserved17 */
{ DEFTYPE_UNSIGNED16 , 0x10 , ACCESS_READ }, /* Subindex175 - InU16_psm_radiation_index */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex176 - InU32_psm_not_ready_event */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex177 - InU32_psm_warning_interlock */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex178 - InU32_psm_minor_interlock */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex179 - InU32_psm_serious_interlock */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex180 - InU32_psm_move_status */
{ DEFTYPE_REAL32 , 0x20 , ACCESS_READ }, /* Subindex181 - InF_psm_position_x_cur */
{ DEFTYPE_REAL32 , 0x20 , ACCESS_READ }, /* Subindex182 - InF_psm_position_y_cur */
{ DEFTYPE_REAL32 , 0x20 , ACCESS_READ }, /* Subindex183 - InF_psm_position_z_cur */
{ DEFTYPE_REAL32 , 0x20 , ACCESS_READ }, /* Subindex184 - InF_psm_position_x_r_cur */
{ DEFTYPE_REAL32 , 0x20 , ACCESS_READ }, /* Subindex185 - InF_psm_position_y_r_cur */
{ DEFTYPE_REAL32 , 0x20 , ACCESS_READ }, /* Subindex186 - InF_psm_position_z_r_cur */
{ DEFTYPE_REAL32 , 0x20 , ACCESS_READ }, /* Subindex187 - InF_psm_velocity_x_cur */
{ DEFTYPE_REAL32 , 0x20 , ACCESS_READ }, /* Subindex188 - InF_psm_velocity_y_cur */
{ DEFTYPE_REAL32 , 0x20 , ACCESS_READ }, /* Subindex189 - InF_psm_velocity_z_cur */
{ DEFTYPE_REAL32 , 0x20 , ACCESS_READ }, /* Subindex190 - InF_psm_velocity_x_r_cur */
{ DEFTYPE_REAL32 , 0x20 , ACCESS_READ }, /* Subindex191 - InF_psm_velocity_y_r_cur */
{ DEFTYPE_REAL32 , 0x20 , ACCESS_READ }, /* Subindex192 - InF_psm_velocity_z_r_cur */
{ DEFTYPE_UNSIGNED16 , 0x10 , ACCESS_READ }, /* Subindex193 - InU16_FkpButton */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }}; /* Subindex194 - InU32_CpgButton */

/**
* \brief Object/Entry names
*/
OBJCONST UCHAR OBJMEM aName0x6000[] = "InputData\000"
"InU8_BoardID\000"
"InU8_Reserved0\000"
"InU32_FirmWareVersion\000"
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
"InB1_ON_DI_RTMON_CONTACTOR_FB\000"
"InB1_ON_DI_Emergency_Reserve\000"
"InB1_ON_DI_HVEN\000"
"InB1_ON_DI_BSM_NOT_READY\000"
"InB1_ON_DI_KV_TreatmentEN\000"
"InB1_ON_DI_MV_TreatmentEN\000"
"InB2_ON_DI_reserve0\000"
"InB1_ON_DI_DI_GATING\000"
"InB7_ON_DI_reserve1\000"
"InB4_ON_DO_reserve0\000"
"InB1_ON_DO_MV_TreatmentEN\000"
"InB1_ON_DO_KV_TreatmentEN\000"
"InB1_ON_DO_Emergency\000"
"InB1_ON_DO_Pulse_Inhibit\000"
"InU8_ON_DO_reserve1\000"
"InB1_OFF_DI_STAND_BREAKER1\000"
"InB1_OFF_DI_STAND_BREAKER2\000"
"InB1_OFF_DI_STAND_BREAKER7\000"
"InB1_OFF_DI_TREATMENT_ROOM_DOOR_READY \000"
"InB1_OFF_DI_CONTROL_ROOM_EMERGENCY \000"
"InB1_OFF_DI_CITB_TREATMENT_ROOM_DOOR2 \000"
"InB1_OFF_DI_CITB_EMERGENCY4 \000"
"InB1_OFF_DI_CONTROL_ROOM_BREAKER1 \000"
"InB1_OFF_DI_UserTreatmentEnable \000"
"InB1_OFF_DI_UserHvEnable \000"
"InB1_OFF_DI_UserMoveEnable \000"
"InB1_OFF_DI_STAND_RESERVE \000"
"InB1_OFF_DI_STAND_EMERGENCY \000"
"InB1_OFF_DI_STAND_BREAKER4 \000"
"InB1_OFF_DI_TouchGuard \000"
"InB1_OFF_DI_reserve0 \000"
"InB1_OFF_DI_STAND_BREAKER3 \000"
"InB1_OFF_DI_COVER_EMERGENCY4 \000"
"InB1_OFF_DI_STAND_CONTACTOR1 \000"
"InB1_OFF_DI_COVER_EMERGENCY1 \000"
"InB1_OFF_DI_CONTROL_ROOM_BREAKER3 \000"
"InB1_OFF_DI_UPS_LOAD_PORT \000"
"InB1_OFF_DI_CITB_TREATMENT_ROOM_DOOR1 \000"
"InB1_OFF_DI_CITB_EMERGENCY2 \000"
"InB1_OFF_DI_COVER_DOOR \000"
"InB1_OFF_DI_HvKey \000"
"InB1_OFF_DI_CONTROL_ROOM_CONTACTOR1 \000"
"InB1_OFF_DI_CITB_EMERGENCY3 \000"
"InB1_OFF_DI_STAND_CONTACTOR2 \000"
"InB1_OFF_DI_COVER_EMERGENCY3 \000"
"InB1_OFF_DI_CONTROL_ROOM_BREAKER5 \000"
"InB1_OFF_DI_UPS_ON_BYPASS \000"
"InB1_OFF_DI_STAND_BREAKER5 \000"
"InB1_OFF_DI_COVER_RESERVE \000"
"InB1_OFF_DI_CONTROL_ROOM_CONTACTOR2 \000"
"InB1_OFF_DI_CITB_EMERGENCY5 \000"
"InB1_OFF_DI_CITB_SEARCH_TREATMENT_ROOM \000"
"InB1_OFF_DI_CITB_EXTERNAL_TERMINATE \000"
"InB1_OFF_DI_UPS_LOW_BATT \000"
"InB1_OFF_DI_CONTROL_ROOM_BREAKER2 \000"
"InB1_OFF_DI_CONTROL_ROOM_BREAKER4 \000"
"InB1_OFF_DI_UPS_ON_BATT \000"
"InB1_OFF_DI_CITB_EMERGENCY1 \000"
"InB1_OFF_DI_COVER_EMERGENCY2 \000"
"InB1_OFF_DI_STAND_BREAKER6 \000"
"InB1_OFF_DI_HvEn \000"
"InB1_OFF_DI_MV_TreatmentEN \000"
"InB1_OFF_DI_KV_TreatmentEN \000"
"InU16_OFF_DI_tca9535_di_reserve1\000"
"InB1_OFF_DI_GATING \000"
"InB7_OFF_DI_reserve2\000"
"InU8_OFF_DI_reserve3\000"
"InB1_OFF_DO_TreatmentRoomLight \000"
"InB1_OFF_DO_STAND_RESERVE \000"
"InB1_OFF_DO_Laser \000"
"InB1_OFF_DO_RadiationIndicator \000"
"InB1_OFF_DO_ReadyIndicator \000"
"InB1_OFF_DO_SearchTreatmentRoomRelay \000"
"InB1_OFF_DO_softwareTouchGuard \000"
"InB1_OFF_DO_reserve4\000"
"InU8_OFF_DO_reserve5\000"
"InU16_OFF_DO_tca9535_do_reserve6\000"
"InB1_OFF_DO_SoftwareMVTreatmentEn \000"
"InB1_OFF_DO_SoftwareKVTreatmentEn \000"
"InB1_OFF_DO_softwareMoveEN \000"
"InB1_OFF_DO_SoftwareHvEn \000"
"InB1_OFF_DO_TreatmentMotionEnable \000"
"InB1_OFF_DO_ThreePhasePowerOn \000"
"InB1_OFF_DO_AsuMotionEnable \000"
"InB1_OFF_DO_reserve7\000"
"InU8_OFF_DO_reserve8\000"
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
"InU16_FkpButton\000"
"InU32_CpgButton\000\377";
#endif //#ifdef _OBJD_

#ifndef _LAN9252_APP_OBJECTS_H_
/**
* \brief Object structure
*/
typedef struct OBJ_STRUCT_PACKED_START {
UINT16 u16SubIndex0;
UINT8 InU8_BoardID; /* Subindex1 - InU8_BoardID */
UINT8 InU8_Reserved0; /* Subindex2 - InU8_Reserved0 */
UINT32 InU32_FirmWareVersion; /* Subindex3 - InU32_FirmWareVersion */
UINT8 InU8_beam_id; /* Subindex4 - InU8_beam_id */
UINT8 InU8_reserved1; /* Subindex5 - InU8_reserved1 */
UINT16 InU16_radiation_index; /* Subindex6 - InU16_radiation_index */
UINT8 InU8_rtm_on_arm_fsm_state_current; /* Subindex7 - InU8_rtm_on_arm_fsm_state_current */
UINT8 InU8_rtm_on_arm_ctrl_mode_cur; /* Subindex8 - InU8_rtm_on_arm_ctrl_mode_cur */
UINT8 InU8_ethercat_Link_state; /* Subindex9 - InU8_ethercat_Link_state */
UINT8 InU8_reserved2; /* Subindex10 - InU8_reserved2 */
UINT8 InU8_rtm_on_arm_beam_id; /* Subindex11 - InU8_rtm_on_arm_beam_id */
UINT8 InU8_reserved3; /* Subindex12 - InU8_reserved3 */
UINT16 InU16_rtm_on_arm_radiation_index; /* Subindex13 - InU16_rtm_on_arm_radiation_index */
UINT32 InU32_rtm_on_arm_not_ready_event; /* Subindex14 - InU32_rtm_on_arm_not_ready_event */
UINT32 InU32_rtm_on_arm_warning_interlock; /* Subindex15 - InU32_rtm_on_arm_warning_interlock */
UINT32 InU32_rtm_on_arm_minor_interlock; /* Subindex16 - InU32_rtm_on_arm_minor_interlock */
UINT32 InU32_rtm_on_arm_serious_interlock; /* Subindex17 - InU32_rtm_on_arm_serious_interlock */
BOOLEAN(InB1_ON_DI_RTMON_CONTACTOR_FB); /* Subindex18 - InB1_ON_DI_RTMON_CONTACTOR_FB */
BOOLEAN(InB1_ON_DI_Emergency_Reserve); /* Subindex19 - InB1_ON_DI_Emergency_Reserve */
BOOLEAN(InB1_ON_DI_HVEN); /* Subindex20 - InB1_ON_DI_HVEN */
BOOLEAN(InB1_ON_DI_BSM_NOT_READY); /* Subindex21 - InB1_ON_DI_BSM_NOT_READY */
BOOLEAN(InB1_ON_DI_KV_TreatmentEN); /* Subindex22 - InB1_ON_DI_KV_TreatmentEN */
BOOLEAN(InB1_ON_DI_MV_TreatmentEN); /* Subindex23 - InB1_ON_DI_MV_TreatmentEN */
BIT2(InB2_ON_DI_reserve0); /* Subindex24 - InB2_ON_DI_reserve0 */
BOOLEAN(InB1_ON_DI_DI_GATING); /* Subindex25 - InB1_ON_DI_DI_GATING */
BIT7(InB7_ON_DI_reserve1); /* Subindex26 - InB7_ON_DI_reserve1 */
BIT4(InB4_ON_DO_reserve0); /* Subindex27 - InB4_ON_DO_reserve0 */
BOOLEAN(InB1_ON_DO_MV_TreatmentEN); /* Subindex28 - InB1_ON_DO_MV_TreatmentEN */
BOOLEAN(InB1_ON_DO_KV_TreatmentEN); /* Subindex29 - InB1_ON_DO_KV_TreatmentEN */
BOOLEAN(InB1_ON_DO_Emergency); /* Subindex30 - InB1_ON_DO_Emergency */
BOOLEAN(InB1_ON_DO_Pulse_Inhibit); /* Subindex31 - InB1_ON_DO_Pulse_Inhibit */
UINT8 InU8_ON_DO_reserve1; /* Subindex32 - InU8_ON_DO_reserve1 */
BOOLEAN(InB1_OFF_DI_STAND_BREAKER1); /* Subindex33 - InB1_OFF_DI_STAND_BREAKER1 */
BOOLEAN(InB1_OFF_DI_STAND_BREAKER2); /* Subindex34 - InB1_OFF_DI_STAND_BREAKER2 */
BOOLEAN(InB1_OFF_DI_STAND_BREAKER7); /* Subindex35 - InB1_OFF_DI_STAND_BREAKER7 */
BOOLEAN(InB1_OFF_DI_TREATMENT_ROOM_DOOR_READY); /* Subindex36 - InB1_OFF_DI_TREATMENT_ROOM_DOOR_READY  */
BOOLEAN(InB1_OFF_DI_CONTROL_ROOM_EMERGENCY); /* Subindex37 - InB1_OFF_DI_CONTROL_ROOM_EMERGENCY  */
BOOLEAN(InB1_OFF_DI_CITB_TREATMENT_ROOM_DOOR2); /* Subindex38 - InB1_OFF_DI_CITB_TREATMENT_ROOM_DOOR2  */
BOOLEAN(InB1_OFF_DI_CITB_EMERGENCY4); /* Subindex39 - InB1_OFF_DI_CITB_EMERGENCY4  */
BOOLEAN(InB1_OFF_DI_CONTROL_ROOM_BREAKER1); /* Subindex40 - InB1_OFF_DI_CONTROL_ROOM_BREAKER1  */
BOOLEAN(InB1_OFF_DI_UserTreatmentEnable); /* Subindex41 - InB1_OFF_DI_UserTreatmentEnable  */
BOOLEAN(InB1_OFF_DI_UserHvEnable); /* Subindex42 - InB1_OFF_DI_UserHvEnable  */
BOOLEAN(InB1_OFF_DI_UserMoveEnable); /* Subindex43 - InB1_OFF_DI_UserMoveEnable  */
BOOLEAN(InB1_OFF_DI_STAND_RESERVE); /* Subindex44 - InB1_OFF_DI_STAND_RESERVE  */
BOOLEAN(InB1_OFF_DI_STAND_EMERGENCY); /* Subindex45 - InB1_OFF_DI_STAND_EMERGENCY  */
BOOLEAN(InB1_OFF_DI_STAND_BREAKER4); /* Subindex46 - InB1_OFF_DI_STAND_BREAKER4  */
BOOLEAN(InB1_OFF_DI_TouchGuard); /* Subindex47 - InB1_OFF_DI_TouchGuard  */
BOOLEAN(InB1_OFF_DI_reserve0); /* Subindex48 - InB1_OFF_DI_reserve0  */
BOOLEAN(InB1_OFF_DI_STAND_BREAKER3); /* Subindex49 - InB1_OFF_DI_STAND_BREAKER3  */
BOOLEAN(InB1_OFF_DI_COVER_EMERGENCY4); /* Subindex50 - InB1_OFF_DI_COVER_EMERGENCY4  */
BOOLEAN(InB1_OFF_DI_STAND_CONTACTOR1); /* Subindex51 - InB1_OFF_DI_STAND_CONTACTOR1  */
BOOLEAN(InB1_OFF_DI_COVER_EMERGENCY1); /* Subindex52 - InB1_OFF_DI_COVER_EMERGENCY1  */
BOOLEAN(InB1_OFF_DI_CONTROL_ROOM_BREAKER3); /* Subindex53 - InB1_OFF_DI_CONTROL_ROOM_BREAKER3  */
BOOLEAN(InB1_OFF_DI_UPS_LOAD_PORT); /* Subindex54 - InB1_OFF_DI_UPS_LOAD_PORT  */
BOOLEAN(InB1_OFF_DI_CITB_TREATMENT_ROOM_DOOR1); /* Subindex55 - InB1_OFF_DI_CITB_TREATMENT_ROOM_DOOR1  */
BOOLEAN(InB1_OFF_DI_CITB_EMERGENCY2); /* Subindex56 - InB1_OFF_DI_CITB_EMERGENCY2  */
BOOLEAN(InB1_OFF_DI_COVER_DOOR); /* Subindex57 - InB1_OFF_DI_COVER_DOOR  */
BOOLEAN(InB1_OFF_DI_HvKey); /* Subindex58 - InB1_OFF_DI_HvKey  */
BOOLEAN(InB1_OFF_DI_CONTROL_ROOM_CONTACTOR1); /* Subindex59 - InB1_OFF_DI_CONTROL_ROOM_CONTACTOR1  */
BOOLEAN(InB1_OFF_DI_CITB_EMERGENCY3); /* Subindex60 - InB1_OFF_DI_CITB_EMERGENCY3  */
BOOLEAN(InB1_OFF_DI_STAND_CONTACTOR2); /* Subindex61 - InB1_OFF_DI_STAND_CONTACTOR2  */
BOOLEAN(InB1_OFF_DI_COVER_EMERGENCY3); /* Subindex62 - InB1_OFF_DI_COVER_EMERGENCY3  */
BOOLEAN(InB1_OFF_DI_CONTROL_ROOM_BREAKER5); /* Subindex63 - InB1_OFF_DI_CONTROL_ROOM_BREAKER5  */
BOOLEAN(InB1_OFF_DI_UPS_ON_BYPASS); /* Subindex64 - InB1_OFF_DI_UPS_ON_BYPASS  */
BOOLEAN(InB1_OFF_DI_STAND_BREAKER5); /* Subindex65 - InB1_OFF_DI_STAND_BREAKER5  */
BOOLEAN(InB1_OFF_DI_COVER_RESERVE); /* Subindex66 - InB1_OFF_DI_COVER_RESERVE  */
BOOLEAN(InB1_OFF_DI_CONTROL_ROOM_CONTACTOR2); /* Subindex67 - InB1_OFF_DI_CONTROL_ROOM_CONTACTOR2  */
BOOLEAN(InB1_OFF_DI_CITB_EMERGENCY5); /* Subindex68 - InB1_OFF_DI_CITB_EMERGENCY5  */
BOOLEAN(InB1_OFF_DI_CITB_SEARCH_TREATMENT_ROOM); /* Subindex69 - InB1_OFF_DI_CITB_SEARCH_TREATMENT_ROOM  */
BOOLEAN(InB1_OFF_DI_CITB_EXTERNAL_TERMINATE); /* Subindex70 - InB1_OFF_DI_CITB_EXTERNAL_TERMINATE  */
BOOLEAN(InB1_OFF_DI_UPS_LOW_BATT); /* Subindex71 - InB1_OFF_DI_UPS_LOW_BATT  */
BOOLEAN(InB1_OFF_DI_CONTROL_ROOM_BREAKER2); /* Subindex72 - InB1_OFF_DI_CONTROL_ROOM_BREAKER2  */
BOOLEAN(InB1_OFF_DI_CONTROL_ROOM_BREAKER4); /* Subindex73 - InB1_OFF_DI_CONTROL_ROOM_BREAKER4  */
BOOLEAN(InB1_OFF_DI_UPS_ON_BATT); /* Subindex74 - InB1_OFF_DI_UPS_ON_BATT  */
BOOLEAN(InB1_OFF_DI_CITB_EMERGENCY1); /* Subindex75 - InB1_OFF_DI_CITB_EMERGENCY1  */
BOOLEAN(InB1_OFF_DI_COVER_EMERGENCY2); /* Subindex76 - InB1_OFF_DI_COVER_EMERGENCY2  */
BOOLEAN(InB1_OFF_DI_STAND_BREAKER6); /* Subindex77 - InB1_OFF_DI_STAND_BREAKER6  */
BOOLEAN(InB1_OFF_DI_HvEn); /* Subindex78 - InB1_OFF_DI_HvEn  */
BOOLEAN(InB1_OFF_DI_MV_TreatmentEN); /* Subindex79 - InB1_OFF_DI_MV_TreatmentEN  */
BOOLEAN(InB1_OFF_DI_KV_TreatmentEN); /* Subindex80 - InB1_OFF_DI_KV_TreatmentEN  */
UINT16 InU16_OFF_DI_tca9535_di_reserve1; /* Subindex81 - InU16_OFF_DI_tca9535_di_reserve1 */
BOOLEAN(InB1_OFF_DI_GATING); /* Subindex82 - InB1_OFF_DI_GATING  */
BIT7(InB7_OFF_DI_reserve2); /* Subindex83 - InB7_OFF_DI_reserve2 */
UINT8 InU8_OFF_DI_reserve3; /* Subindex84 - InU8_OFF_DI_reserve3 */
BOOLEAN(InB1_OFF_DO_TreatmentRoomLight); /* Subindex85 - InB1_OFF_DO_TreatmentRoomLight  */
BOOLEAN(InB1_OFF_DO_STAND_RESERVE); /* Subindex86 - InB1_OFF_DO_STAND_RESERVE  */
BOOLEAN(InB1_OFF_DO_Laser); /* Subindex87 - InB1_OFF_DO_Laser  */
BOOLEAN(InB1_OFF_DO_RadiationIndicator); /* Subindex88 - InB1_OFF_DO_RadiationIndicator  */
BOOLEAN(InB1_OFF_DO_ReadyIndicator); /* Subindex89 - InB1_OFF_DO_ReadyIndicator  */
BOOLEAN(InB1_OFF_DO_SearchTreatmentRoomRelay); /* Subindex90 - InB1_OFF_DO_SearchTreatmentRoomRelay  */
BOOLEAN(InB1_OFF_DO_softwareTouchGuard); /* Subindex91 - InB1_OFF_DO_softwareTouchGuard  */
BOOLEAN(InB1_OFF_DO_reserve4); /* Subindex92 - InB1_OFF_DO_reserve4 */
UINT8 InU8_OFF_DO_reserve5; /* Subindex93 - InU8_OFF_DO_reserve5 */
UINT16 InU16_OFF_DO_tca9535_do_reserve6; /* Subindex94 - InU16_OFF_DO_tca9535_do_reserve6 */
BOOLEAN(InB1_OFF_DO_SoftwareMVTreatmentEn); /* Subindex95 - InB1_OFF_DO_SoftwareMVTreatmentEn  */
BOOLEAN(InB1_OFF_DO_SoftwareKVTreatmentEn); /* Subindex96 - InB1_OFF_DO_SoftwareKVTreatmentEn  */
BOOLEAN(InB1_OFF_DO_softwareMoveEN); /* Subindex97 - InB1_OFF_DO_softwareMoveEN  */
BOOLEAN(InB1_OFF_DO_SoftwareHvEn); /* Subindex98 - InB1_OFF_DO_SoftwareHvEn  */
BOOLEAN(InB1_OFF_DO_TreatmentMotionEnable); /* Subindex99 - InB1_OFF_DO_TreatmentMotionEnable  */
BOOLEAN(InB1_OFF_DO_ThreePhasePowerOn); /* Subindex100 - InB1_OFF_DO_ThreePhasePowerOn  */
BOOLEAN(InB1_OFF_DO_AsuMotionEnable); /* Subindex101 - InB1_OFF_DO_AsuMotionEnable  */
BOOLEAN(InB1_OFF_DO_reserve7); /* Subindex102 - InB1_OFF_DO_reserve7 */
UINT8 InU8_OFF_DO_reserve8; /* Subindex103 - InU8_OFF_DO_reserve8 */
UINT8 InU8_icm_fsm_state_current; /* Subindex104 - InU8_icm_fsm_state_current */
UINT8 InU8_icm_ctrl_mode_cur; /* Subindex105 - InU8_icm_ctrl_mode_cur */
UINT16 InU16_reserved4; /* Subindex106 - InU16_reserved4 */
UINT8 InU8_icm_beam_id; /* Subindex107 - InU8_icm_beam_id */
UINT8 InU8_reserved5; /* Subindex108 - InU8_reserved5 */
UINT16 InU16_icm_radiation_index; /* Subindex109 - InU16_icm_radiation_index */
UINT32 InU32_icm_not_ready_event; /* Subindex110 - InU32_icm_not_ready_event */
UINT32 InU32_icm_warning_interlock; /* Subindex111 - InU32_icm_warning_interlock */
UINT32 InU32_icm_minor_interlock; /* Subindex112 - InU32_icm_minor_interlock */
UINT32 InU32_icm_serious_interlock; /* Subindex113 - InU32_icm_serious_interlock */
UINT8 InU8_bgm_fsm_state_current; /* Subindex114 - InU8_bgm_fsm_state_current */
UINT8 InU8_bgm_ctrl_mode_cur; /* Subindex115 - InU8_bgm_ctrl_mode_cur */
UINT16 InU16_reserved6; /* Subindex116 - InU16_reserved6 */
UINT8 InU8_bgm_beam_id; /* Subindex117 - InU8_bgm_beam_id */
UINT8 InU8_reserved7; /* Subindex118 - InU8_reserved7 */
UINT16 InU16_bgm_radiation_index; /* Subindex119 - InU16_bgm_radiation_index */
UINT32 InU32_bgm_not_ready_event; /* Subindex120 - InU32_bgm_not_ready_event */
UINT32 InU32_bgm_warning_interlock; /* Subindex121 - InU32_bgm_warning_interlock */
UINT32 InU32_bgm_minor_interlock; /* Subindex122 - InU32_bgm_minor_interlock */
UINT32 InU32_bgm_serious_interlock; /* Subindex123 - InU32_bgm_serious_interlock */
REAL32 InF_beam_on_time; /* Subindex124 - InF_beam_on_time */
REAL32 InF_primary_dose_current; /* Subindex125 - InF_primary_dose_current */
REAL32 InF_primary_dose_rate_current; /* Subindex126 - InF_primary_dose_rate_current */
UINT8 InU8_primary_dose_fsm_state; /* Subindex127 - InU8_primary_dose_fsm_state */
UINT8 InU8_reserved8; /* Subindex128 - InU8_reserved8 */
UINT32 InU32_primary_dose_interlock; /* Subindex129 - InU32_primary_dose_interlock */
REAL32 InF_second_dose_current; /* Subindex130 - InF_second_dose_current */
REAL32 InF_secondry_dose_rate_current; /* Subindex131 - InF_secondry_dose_rate_current */
UINT8 InU8_secondry_dose_fsm_rate; /* Subindex132 - InU8_secondry_dose_fsm_rate */
UINT8 InU8_reserved9; /* Subindex133 - InU8_reserved9 */
UINT32 InU32_secondry_dose_interlock; /* Subindex134 - InU32_secondry_dose_interlock */
UINT32 InU32_afc_interlock; /* Subindex135 - InU32_afc_interlock */
REAL32 InF_afc_position_current; /* Subindex136 - InF_afc_position_current */
UINT8 InU8_qam_fsm_state_current; /* Subindex137 - InU8_qam_fsm_state_current */
UINT8 InU8_qam_ctrl_mode_cur; /* Subindex138 - InU8_qam_ctrl_mode_cur */
UINT16 InU16_reserved10; /* Subindex139 - InU16_reserved10 */
UINT8 InU8_qam_beam_id; /* Subindex140 - InU8_qam_beam_id */
UINT8 InU8_reserved11; /* Subindex141 - InU8_reserved11 */
UINT16 InU16_qam_radiation_index; /* Subindex142 - InU16_qam_radiation_index */
UINT32 InU32_qam_not_ready_event; /* Subindex143 - InU32_qam_not_ready_event */
UINT32 InU32_qam_warning_interlock; /* Subindex144 - InU32_qam_warning_interlock */
UINT32 InU32_qam_minor_interlock; /* Subindex145 - InU32_qam_minor_interlock */
UINT32 InU32_qam_serious_interlock; /* Subindex146 - InU32_qam_serious_interlock */
UINT8 InU8_rtm_off_fsm_state_current; /* Subindex147 - InU8_rtm_off_fsm_state_current */
UINT8 InU8_rtm_off_ctrl_mode_cur; /* Subindex148 - InU8_rtm_off_ctrl_mode_cur */
UINT16 InU16_reserved12; /* Subindex149 - InU16_reserved12 */
UINT8 InU8_rtm_off_beam_id; /* Subindex150 - InU8_rtm_off_beam_id */
UINT8 InU8_reserved13; /* Subindex151 - InU8_reserved13 */
UINT16 InU16_rtm_off_radiation_index; /* Subindex152 - InU16_rtm_off_radiation_index */
UINT32 InU32_rtm_off_not_ready_event; /* Subindex153 - InU32_rtm_off_not_ready_event */
UINT32 InU32_rtm_off_warning_interlock; /* Subindex154 - InU32_rtm_off_warning_interlock */
UINT32 InU32_rtm_off_minor_interlock; /* Subindex155 - InU32_rtm_off_minor_interlock */
UINT32 InU32_rtm_off_serious_interlock; /* Subindex156 - InU32_rtm_off_serious_interlock */
UINT8 InU8_gmm_fsm_state_current; /* Subindex157 - InU8_gmm_fsm_state_current */
UINT8 InU8_gmm_ctrl_mode_cur; /* Subindex158 - InU8_gmm_ctrl_mode_cur */
UINT16 InU16_reserved14; /* Subindex159 - InU16_reserved14 */
UINT8 InU8_gmm_beam_id; /* Subindex160 - InU8_gmm_beam_id */
UINT8 InU8_reserved15; /* Subindex161 - InU8_reserved15 */
UINT16 InU16_gmm_radiation_index; /* Subindex162 - InU16_gmm_radiation_index */
UINT32 InU32_gmm_not_ready_event; /* Subindex163 - InU32_gmm_not_ready_event */
UINT32 InU32_gmm_warning_interlock; /* Subindex164 - InU32_gmm_warning_interlock */
UINT32 InU32_gmm_minor_interlock; /* Subindex165 - InU32_gmm_minor_interlock */
UINT32 InU32_gmm_serious_interlock; /* Subindex166 - InU32_gmm_serious_interlock */
UINT32 InU32_gmm_move_status; /* Subindex167 - InU32_gmm_move_status */
REAL32 InF_gmm_position_cur; /* Subindex168 - InF_gmm_position_cur */
REAL32 InF_gmm_velocity_cur; /* Subindex169 - InF_gmm_velocity_cur */
UINT8 InU8_psm_fsm_state_current; /* Subindex170 - InU8_psm_fsm_state_current */
UINT8 InU8_psm_ctrl_mode_cur; /* Subindex171 - InU8_psm_ctrl_mode_cur */
UINT16 InU16_reserved16; /* Subindex172 - InU16_reserved16 */
UINT8 InU8_psm_beam_id; /* Subindex173 - InU8_psm_beam_id */
UINT8 InU8_reserved17; /* Subindex174 - InU8_reserved17 */
UINT16 InU16_psm_radiation_index; /* Subindex175 - InU16_psm_radiation_index */
UINT32 InU32_psm_not_ready_event; /* Subindex176 - InU32_psm_not_ready_event */
UINT32 InU32_psm_warning_interlock; /* Subindex177 - InU32_psm_warning_interlock */
UINT32 InU32_psm_minor_interlock; /* Subindex178 - InU32_psm_minor_interlock */
UINT32 InU32_psm_serious_interlock; /* Subindex179 - InU32_psm_serious_interlock */
UINT32 InU32_psm_move_status; /* Subindex180 - InU32_psm_move_status */
REAL32 InF_psm_position_x_cur; /* Subindex181 - InF_psm_position_x_cur */
REAL32 InF_psm_position_y_cur; /* Subindex182 - InF_psm_position_y_cur */
REAL32 InF_psm_position_z_cur; /* Subindex183 - InF_psm_position_z_cur */
REAL32 InF_psm_position_x_r_cur; /* Subindex184 - InF_psm_position_x_r_cur */
REAL32 InF_psm_position_y_r_cur; /* Subindex185 - InF_psm_position_y_r_cur */
REAL32 InF_psm_position_z_r_cur; /* Subindex186 - InF_psm_position_z_r_cur */
REAL32 InF_psm_velocity_x_cur; /* Subindex187 - InF_psm_velocity_x_cur */
REAL32 InF_psm_velocity_y_cur; /* Subindex188 - InF_psm_velocity_y_cur */
REAL32 InF_psm_velocity_z_cur; /* Subindex189 - InF_psm_velocity_z_cur */
REAL32 InF_psm_velocity_x_r_cur; /* Subindex190 - InF_psm_velocity_x_r_cur */
REAL32 InF_psm_velocity_y_r_cur; /* Subindex191 - InF_psm_velocity_y_r_cur */
REAL32 InF_psm_velocity_z_r_cur; /* Subindex192 - InF_psm_velocity_z_r_cur */
UINT16 InU16_FkpButton; /* Subindex193 - InU16_FkpButton */
UINT32 InU32_CpgButton; /* Subindex194 - InU32_CpgButton */
} OBJ_STRUCT_PACKED_END
TOBJ6000;
#endif //#ifndef _LAN9252_APP_OBJECTS_H_

/**
* \brief Object variable
*/
PROTO TOBJ6000 InputData0x6000
#if defined(_LAN9252_APP_) && (_LAN9252_APP_ == 1)
={194,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}
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
* SubIndex 10 - OutU8_Reserved1<br>
* SubIndex 11 - OutU8_ethercat_Link_state<br>
* SubIndex 12 - OutU8_rtm_on_require_state<br>
* SubIndex 13 - OutU8_rtm_on_require_ctrl_mode<br>
* SubIndex 14 - OutU16_rtm_on_plc_info<br>
* SubIndex 15 - OutU32_rtm_on_interlock_override<br>
* SubIndex 16 - OutU32_rtm_on_unready_override<br>
* SubIndex 17 - OutU8_icm_require_state<br>
* SubIndex 18 - OutU8_icm_require_ctrl_mode<br>
* SubIndex 19 - OutU16_ct_status<br>
* SubIndex 20 - OutF_gantry_velocity_prepare<br>
* SubIndex 21 - OutU32_icm_interlock_override<br>
* SubIndex 22 - OutU32_icm_unready_override<br>
* SubIndex 23 - OutU8_bgm_require_state<br>
* SubIndex 24 - OutU8_bgm_require_ctrl_mode<br>
* SubIndex 25 - OutU32_bgm_interlock_override<br>
* SubIndex 26 - OutU32_bgm_unready_override<br>
* SubIndex 27 - OutU8_qam_require_state<br>
* SubIndex 28 - OutU8_qam_require_ctrl_mode<br>
* SubIndex 29 - OutU32_qam_interlock_override<br>
* SubIndex 30 - OutU32_qam_unready_override<br>
* SubIndex 31 - OutU8_bsm_require_state<br>
* SubIndex 32 - OutU8_bsm_require_ctrl_mode<br>
* SubIndex 33 - OutU32_bsm_interlock_override<br>
* SubIndex 34 - OutU32_bsm_unready_override<br>
* SubIndex 35 - OutU8_rtm_off_require_state<br>
* SubIndex 36 - OutU8_rtm_off_require_ctrl_mode<br>
* SubIndex 37 - OutU16_rtm_off_plc_info<br>
* SubIndex 38 - OutU32_rtm_off_interlock_override<br>
* SubIndex 39 - OutU32_rtm_off_unready_override<br>
* SubIndex 40 - OutU8_led_belt<br>
* SubIndex 41 - OutU8_reserved2<br>
* SubIndex 42 - OutU8_gmm_require_state<br>
* SubIndex 43 - OutU8_gmm_require_ctrl_mode<br>
* SubIndex 44 - OutU32_gmm_interlock_override<br>
* SubIndex 45 - OutU32_gmm_unready_override<br>
* SubIndex 46 - OutU8_psm_require_state<br>
* SubIndex 47 - OutU8_psm_require_ctrl_mode<br>
* SubIndex 48 - OutU32_psm_interlock_override<br>
* SubIndex 49 - OutU32_psm_unready_override<br>
* SubIndex 50 - OutU8_fkp_led_blink<br>
* SubIndex 51 - OutU8_fkp_userPrompt<br>
* SubIndex 52 - OutU16_fkp_year<br>
* SubIndex 53 - OutU8_fkp_month<br>
* SubIndex 54 - OutU8_fkp_day<br>
* SubIndex 55 - OutU8_fkp_hour<br>
* SubIndex 56 - OutU8_fkp_minute<br>
* SubIndex 57 - OutU8_fkp_fractions<br>
* SubIndex 58 - OutU8_reserved3<br>
* SubIndex 59 - OutU32_cpg_led_blink<br>
* SubIndex 60 - OutU8_cpg_vibration<br>
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
{ DEFTYPE_UNSIGNED8 , 0x08 , ACCESS_READ }, /* Subindex10 - OutU8_Reserved1 */
{ DEFTYPE_UNSIGNED8 , 0x08 , ACCESS_READ }, /* Subindex11 - OutU8_ethercat_Link_state */
{ DEFTYPE_UNSIGNED8 , 0x08 , ACCESS_READ }, /* Subindex12 - OutU8_rtm_on_require_state */
{ DEFTYPE_UNSIGNED8 , 0x08 , ACCESS_READ }, /* Subindex13 - OutU8_rtm_on_require_ctrl_mode */
{ DEFTYPE_UNSIGNED16 , 0x10 , ACCESS_READ }, /* Subindex14 - OutU16_rtm_on_plc_info */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex15 - OutU32_rtm_on_interlock_override */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex16 - OutU32_rtm_on_unready_override */
{ DEFTYPE_UNSIGNED8 , 0x08 , ACCESS_READ }, /* Subindex17 - OutU8_icm_require_state */
{ DEFTYPE_UNSIGNED8 , 0x08 , ACCESS_READ }, /* Subindex18 - OutU8_icm_require_ctrl_mode */
{ DEFTYPE_UNSIGNED16 , 0x10 , ACCESS_READ }, /* Subindex19 - OutU16_ct_status */
{ DEFTYPE_REAL32 , 0x20 , ACCESS_READ }, /* Subindex20 - OutF_gantry_velocity_prepare */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex21 - OutU32_icm_interlock_override */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex22 - OutU32_icm_unready_override */
{ DEFTYPE_UNSIGNED8 , 0x08 , ACCESS_READ }, /* Subindex23 - OutU8_bgm_require_state */
{ DEFTYPE_UNSIGNED8 , 0x08 , ACCESS_READ }, /* Subindex24 - OutU8_bgm_require_ctrl_mode */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex25 - OutU32_bgm_interlock_override */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex26 - OutU32_bgm_unready_override */
{ DEFTYPE_UNSIGNED8 , 0x08 , ACCESS_READ }, /* Subindex27 - OutU8_qam_require_state */
{ DEFTYPE_UNSIGNED8 , 0x08 , ACCESS_READ }, /* Subindex28 - OutU8_qam_require_ctrl_mode */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex29 - OutU32_qam_interlock_override */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex30 - OutU32_qam_unready_override */
{ DEFTYPE_UNSIGNED8 , 0x08 , ACCESS_READ }, /* Subindex31 - OutU8_bsm_require_state */
{ DEFTYPE_UNSIGNED8 , 0x08 , ACCESS_READ }, /* Subindex32 - OutU8_bsm_require_ctrl_mode */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex33 - OutU32_bsm_interlock_override */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex34 - OutU32_bsm_unready_override */
{ DEFTYPE_UNSIGNED8 , 0x08 , ACCESS_READ }, /* Subindex35 - OutU8_rtm_off_require_state */
{ DEFTYPE_UNSIGNED8 , 0x08 , ACCESS_READ }, /* Subindex36 - OutU8_rtm_off_require_ctrl_mode */
{ DEFTYPE_UNSIGNED16 , 0x10 , ACCESS_READ }, /* Subindex37 - OutU16_rtm_off_plc_info */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex38 - OutU32_rtm_off_interlock_override */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex39 - OutU32_rtm_off_unready_override */
{ DEFTYPE_UNSIGNED8 , 0x08 , ACCESS_READ }, /* Subindex40 - OutU8_led_belt */
{ DEFTYPE_UNSIGNED8 , 0x08 , ACCESS_READ }, /* Subindex41 - OutU8_reserved2 */
{ DEFTYPE_UNSIGNED8 , 0x08 , ACCESS_READ }, /* Subindex42 - OutU8_gmm_require_state */
{ DEFTYPE_UNSIGNED8 , 0x08 , ACCESS_READ }, /* Subindex43 - OutU8_gmm_require_ctrl_mode */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex44 - OutU32_gmm_interlock_override */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex45 - OutU32_gmm_unready_override */
{ DEFTYPE_UNSIGNED8 , 0x08 , ACCESS_READ }, /* Subindex46 - OutU8_psm_require_state */
{ DEFTYPE_UNSIGNED8 , 0x08 , ACCESS_READ }, /* Subindex47 - OutU8_psm_require_ctrl_mode */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex48 - OutU32_psm_interlock_override */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex49 - OutU32_psm_unready_override */
{ DEFTYPE_UNSIGNED8 , 0x08 , ACCESS_READ }, /* Subindex50 - OutU8_fkp_led_blink */
{ DEFTYPE_UNSIGNED8 , 0x08 , ACCESS_READ }, /* Subindex51 - OutU8_fkp_userPrompt */
{ DEFTYPE_UNSIGNED16 , 0x10 , ACCESS_READ }, /* Subindex52 - OutU16_fkp_year */
{ DEFTYPE_UNSIGNED8 , 0x08 , ACCESS_READ }, /* Subindex53 - OutU8_fkp_month */
{ DEFTYPE_UNSIGNED8 , 0x08 , ACCESS_READ }, /* Subindex54 - OutU8_fkp_day */
{ DEFTYPE_UNSIGNED8 , 0x08 , ACCESS_READ }, /* Subindex55 - OutU8_fkp_hour */
{ DEFTYPE_UNSIGNED8 , 0x08 , ACCESS_READ }, /* Subindex56 - OutU8_fkp_minute */
{ DEFTYPE_UNSIGNED8 , 0x08 , ACCESS_READ }, /* Subindex57 - OutU8_fkp_fractions */
{ DEFTYPE_UNSIGNED8 , 0x08 , ACCESS_READ }, /* Subindex58 - OutU8_reserved3 */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex59 - OutU32_cpg_led_blink */
{ DEFTYPE_UNSIGNED8 , 0x08 , ACCESS_READ }}; /* Subindex60 - OutU8_cpg_vibration */

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
"OutU8_Reserved1\000"
"OutU8_ethercat_Link_state\000"
"OutU8_rtm_on_require_state\000"
"OutU8_rtm_on_require_ctrl_mode\000"
"OutU16_rtm_on_plc_info\000"
"OutU32_rtm_on_interlock_override\000"
"OutU32_rtm_on_unready_override\000"
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
"OutU8_gmm_require_state\000"
"OutU8_gmm_require_ctrl_mode\000"
"OutU32_gmm_interlock_override\000"
"OutU32_gmm_unready_override\000"
"OutU8_psm_require_state\000"
"OutU8_psm_require_ctrl_mode\000"
"OutU32_psm_interlock_override\000"
"OutU32_psm_unready_override\000"
"OutU8_fkp_led_blink\000"
"OutU8_fkp_userPrompt\000"
"OutU16_fkp_year\000"
"OutU8_fkp_month\000"
"OutU8_fkp_day\000"
"OutU8_fkp_hour\000"
"OutU8_fkp_minute\000"
"OutU8_fkp_fractions\000"
"OutU8_reserved3\000"
"OutU32_cpg_led_blink\000"
"OutU8_cpg_vibration\000\377";
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
UINT8 OutU8_Reserved1; /* Subindex10 - OutU8_Reserved1 */
UINT8 OutU8_ethercat_Link_state; /* Subindex11 - OutU8_ethercat_Link_state */
UINT8 OutU8_rtm_on_require_state; /* Subindex12 - OutU8_rtm_on_require_state */
UINT8 OutU8_rtm_on_require_ctrl_mode; /* Subindex13 - OutU8_rtm_on_require_ctrl_mode */
UINT16 OutU16_rtm_on_plc_info; /* Subindex14 - OutU16_rtm_on_plc_info */
UINT32 OutU32_rtm_on_interlock_override; /* Subindex15 - OutU32_rtm_on_interlock_override */
UINT32 OutU32_rtm_on_unready_override; /* Subindex16 - OutU32_rtm_on_unready_override */
UINT8 OutU8_icm_require_state; /* Subindex17 - OutU8_icm_require_state */
UINT8 OutU8_icm_require_ctrl_mode; /* Subindex18 - OutU8_icm_require_ctrl_mode */
UINT16 OutU16_ct_status; /* Subindex19 - OutU16_ct_status */
REAL32 OutF_gantry_velocity_prepare; /* Subindex20 - OutF_gantry_velocity_prepare */
UINT32 OutU32_icm_interlock_override; /* Subindex21 - OutU32_icm_interlock_override */
UINT32 OutU32_icm_unready_override; /* Subindex22 - OutU32_icm_unready_override */
UINT8 OutU8_bgm_require_state; /* Subindex23 - OutU8_bgm_require_state */
UINT8 OutU8_bgm_require_ctrl_mode; /* Subindex24 - OutU8_bgm_require_ctrl_mode */
UINT32 OutU32_bgm_interlock_override; /* Subindex25 - OutU32_bgm_interlock_override */
UINT32 OutU32_bgm_unready_override; /* Subindex26 - OutU32_bgm_unready_override */
UINT8 OutU8_qam_require_state; /* Subindex27 - OutU8_qam_require_state */
UINT8 OutU8_qam_require_ctrl_mode; /* Subindex28 - OutU8_qam_require_ctrl_mode */
UINT32 OutU32_qam_interlock_override; /* Subindex29 - OutU32_qam_interlock_override */
UINT32 OutU32_qam_unready_override; /* Subindex30 - OutU32_qam_unready_override */
UINT8 OutU8_bsm_require_state; /* Subindex31 - OutU8_bsm_require_state */
UINT8 OutU8_bsm_require_ctrl_mode; /* Subindex32 - OutU8_bsm_require_ctrl_mode */
UINT32 OutU32_bsm_interlock_override; /* Subindex33 - OutU32_bsm_interlock_override */
UINT32 OutU32_bsm_unready_override; /* Subindex34 - OutU32_bsm_unready_override */
UINT8 OutU8_rtm_off_require_state; /* Subindex35 - OutU8_rtm_off_require_state */
UINT8 OutU8_rtm_off_require_ctrl_mode; /* Subindex36 - OutU8_rtm_off_require_ctrl_mode */
UINT16 OutU16_rtm_off_plc_info; /* Subindex37 - OutU16_rtm_off_plc_info */
UINT32 OutU32_rtm_off_interlock_override; /* Subindex38 - OutU32_rtm_off_interlock_override */
UINT32 OutU32_rtm_off_unready_override; /* Subindex39 - OutU32_rtm_off_unready_override */
UINT8 OutU8_led_belt; /* Subindex40 - OutU8_led_belt */
UINT8 OutU8_reserved2; /* Subindex41 - OutU8_reserved2 */
UINT8 OutU8_gmm_require_state; /* Subindex42 - OutU8_gmm_require_state */
UINT8 OutU8_gmm_require_ctrl_mode; /* Subindex43 - OutU8_gmm_require_ctrl_mode */
UINT32 OutU32_gmm_interlock_override; /* Subindex44 - OutU32_gmm_interlock_override */
UINT32 OutU32_gmm_unready_override; /* Subindex45 - OutU32_gmm_unready_override */
UINT8 OutU8_psm_require_state; /* Subindex46 - OutU8_psm_require_state */
UINT8 OutU8_psm_require_ctrl_mode; /* Subindex47 - OutU8_psm_require_ctrl_mode */
UINT32 OutU32_psm_interlock_override; /* Subindex48 - OutU32_psm_interlock_override */
UINT32 OutU32_psm_unready_override; /* Subindex49 - OutU32_psm_unready_override */
UINT8 OutU8_fkp_led_blink; /* Subindex50 - OutU8_fkp_led_blink */
UINT8 OutU8_fkp_userPrompt; /* Subindex51 - OutU8_fkp_userPrompt */
UINT16 OutU16_fkp_year; /* Subindex52 - OutU16_fkp_year */
UINT8 OutU8_fkp_month; /* Subindex53 - OutU8_fkp_month */
UINT8 OutU8_fkp_day; /* Subindex54 - OutU8_fkp_day */
UINT8 OutU8_fkp_hour; /* Subindex55 - OutU8_fkp_hour */
UINT8 OutU8_fkp_minute; /* Subindex56 - OutU8_fkp_minute */
UINT8 OutU8_fkp_fractions; /* Subindex57 - OutU8_fkp_fractions */
UINT8 OutU8_reserved3; /* Subindex58 - OutU8_reserved3 */
UINT32 OutU32_cpg_led_blink; /* Subindex59 - OutU32_cpg_led_blink */
UINT8 OutU8_cpg_vibration; /* Subindex60 - OutU8_cpg_vibration */
} OBJ_STRUCT_PACKED_END
TOBJ7010;
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
{NULL , NULL ,  0x1A00 , {DEFTYPE_PDOMAPPING , 194 | (OBJCODE_REC << 8)} , asEntryDesc0x1A00 , aName0x1A00 , &InputDataProcessDataMapping0x1A00 , NULL , NULL , 0x0000 },
/* Object 0x1C12 */
{NULL , NULL ,  0x1C12 , {DEFTYPE_UNSIGNED16 , 1 | (OBJCODE_ARR << 8)} , asEntryDesc0x1C12 , aName0x1C12 , &sRxPDOassign , NULL , NULL , 0x0000 },
/* Object 0x1C13 */
{NULL , NULL ,  0x1C13 , {DEFTYPE_UNSIGNED16 , 1 | (OBJCODE_ARR << 8)} , asEntryDesc0x1C13 , aName0x1C13 , &sTxPDOassign , NULL , NULL , 0x0000 },
/* Object 0x6000 */
{NULL , NULL ,  0x6000 , {DEFTYPE_RECORD , 194 | (OBJCODE_REC << 8)} , asEntryDesc0x6000 , aName0x6000 , &InputData0x6000 , NULL , NULL , 0x0000 },
/* Object 0x7010 */
{NULL , NULL ,  0x7010 , {DEFTYPE_RECORD , 60 | (OBJCODE_REC << 8)} , asEntryDesc0x7010 , aName0x7010 , &OutputData0x7010 , NULL , NULL , 0x0000 },
{NULL,NULL, 0xFFFF, {0, 0}, NULL, NULL, NULL, NULL}};
#endif    //#ifdef _OBJD_

#undef PROTO

/** @}*/
#define _LAN9252_APP_OBJECTS_H_
