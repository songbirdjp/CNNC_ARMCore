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
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }}; /* Subindex43 - Reference to 0x7010.43 */

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
"SubIndex 043\000\377";
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
} OBJ_STRUCT_PACKED_END
TOBJ1601;
#endif //#ifndef _LAN9252_APP_OBJECTS_H_

/**
* \brief Object variable
*/
PROTO TOBJ1601 OutputDataProcessDataMapping0x1601
#if defined(_LAN9252_APP_) && (_LAN9252_APP_ == 1)
={43,0x70100108,0x70100208,0x70100320,0x70100408,0x70100508,0x70100610,0x70100708,0x70100808,0x70100920,0x70100A20,0x70100B08,0x70100C08,0x70100D10,0x70100E20,0x70100F20,0x70101008,0x70101108,0x70101220,0x70101320,0x70101408,0x70101508,0x70101620,0x70101720,0x70101808,0x70101908,0x70101A20,0x70101B20,0x70101C08,0x70101D08,0x70101E20,0x70101F20,0x70102008,0x70102108,0x70102208,0x70102308,0x70102420,0x70102520,0x70102608,0x70102708,0x70102820,0x70102920,0x70102A08,0x70102B08}
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
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }}; /* Subindex107 - Reference to 0x6000.107 */

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
"SubIndex 107\000\377";
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
} OBJ_STRUCT_PACKED_END
TOBJ1A00;
#endif //#ifndef _LAN9252_APP_OBJECTS_H_

/**
* \brief Object variable
*/
PROTO TOBJ1A00 InputDataProcessDataMapping0x1A00
#if defined(_LAN9252_APP_) && (_LAN9252_APP_ == 1)
={107,0x60000108,0x60000208,0x60000320,0x60000408,0x60000508,0x60000610,0x60000708,0x60000808,0x60000910,0x60000A08,0x60000B08,0x60000C10,0x60000D20,0x60000E20,0x60000F20,0x60001020,0x60001108,0x60001208,0x60001310,0x60001408,0x60001508,0x60001610,0x60001720,0x60001820,0x60001920,0x60001A20,0x60001B08,0x60001C08,0x60001D10,0x60001E08,0x60001F08,0x60002010,0x60002120,0x60002220,0x60002320,0x60002420,0x60002520,0x60002620,0x60002720,0x60002808,0x60002908,0x60002A20,0x60002B20,0x60002C20,0x60002D08,0x60002E08,0x60002F20,0x60003020,0x60003120,0x60003208,0x60003308,0x60003410,0x60003508,0x60003608,0x60003710,0x60003820,0x60003920,0x60003A20,0x60003B20,0x60003C08,0x60003D08,0x60003E10,0x60003F08,0x60004008,0x60004110,0x60004220,0x60004320,0x60004420,0x60004520,0x60004608,0x60004708,0x60004810,0x60004908,0x60004A08,0x60004B10,0x60004C20,0x60004D20,0x60004E20,0x60004F20,0x60005020,0x60005120,0x60005220,0x60005308,0x60005408,0x60005510,0x60005608,0x60005708,0x60005810,0x60005920,0x60005A20,0x60005B20,0x60005C20,0x60005D20,0x60005E20,0x60005F20,0x60006020,0x60006120,0x60006220,0x60006320,0x60006420,0x60006520,0x60006620,0x60006720,0x60006820,0x60006920,0x60006A10,0x60006B20}
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
* SubIndex 9 - InU16_reserved2<br>
* SubIndex 10 - InU8_rtm_on_arm_beam_id<br>
* SubIndex 11 - InU8_reserved3<br>
* SubIndex 12 - InU16_rtm_on_arm_radiation_index<br>
* SubIndex 13 - InU32_rtm_on_arm_not_ready_event<br>
* SubIndex 14 - InU32_rtm_on_arm_warning_interlock<br>
* SubIndex 15 - InU32_rtm_on_arm_minor_interlock<br>
* SubIndex 16 - InU32_rtm_on_arm_serious_interlock<br>
* SubIndex 17 - InU8_icm_fsm_state_current<br>
* SubIndex 18 - InU8_icm_ctrl_mode_cur<br>
* SubIndex 19 - InU16_reserved4<br>
* SubIndex 20 - InU8_icm_beam_id<br>
* SubIndex 21 - InU8_reserved5<br>
* SubIndex 22 - InU16_icm_radiation_index<br>
* SubIndex 23 - InU32_icm_not_ready_event<br>
* SubIndex 24 - InU32_icm_warning_interlock<br>
* SubIndex 25 - InU32_icm_minor_interlock<br>
* SubIndex 26 - InU32_icm_serious_interlock<br>
* SubIndex 27 - InU8_bgm_fsm_state_current<br>
* SubIndex 28 - InU8_bgm_ctrl_mode_cur<br>
* SubIndex 29 - InU16_reserved6<br>
* SubIndex 30 - InU8_bgm_beam_id<br>
* SubIndex 31 - InU8_reserved7<br>
* SubIndex 32 - InU16_bgm_radiation_index<br>
* SubIndex 33 - InU32_bgm_not_ready_event<br>
* SubIndex 34 - InU32_bgm_warning_interlock<br>
* SubIndex 35 - InU32_bgm_minor_interlock<br>
* SubIndex 36 - InU32_bgm_serious_interlock<br>
* SubIndex 37 - InF_beam_on_time<br>
* SubIndex 38 - InF_primary_dose_current<br>
* SubIndex 39 - InF_primary_dose_rate_current<br>
* SubIndex 40 - InU8_primary_dose_fsm_state<br>
* SubIndex 41 - InU8_reserved8<br>
* SubIndex 42 - InU32_primary_dose_interlock<br>
* SubIndex 43 - InF_second_dose_current<br>
* SubIndex 44 - InF_secondry_dose_rate_current<br>
* SubIndex 45 - InU8_secondry_dose_fsm_rate<br>
* SubIndex 46 - InU8_reserved9<br>
* SubIndex 47 - InU32_secondry_dose_interlock<br>
* SubIndex 48 - InU32_afc_interlock<br>
* SubIndex 49 - InF_afc_position_current<br>
* SubIndex 50 - InU8_qam_fsm_state_current<br>
* SubIndex 51 - InU8_qam_ctrl_mode_cur<br>
* SubIndex 52 - InU16_reserved10<br>
* SubIndex 53 - InU8_qam_beam_id<br>
* SubIndex 54 - InU8_reserved11<br>
* SubIndex 55 - InU16_qam_radiation_index<br>
* SubIndex 56 - InU32_qam_not_ready_event<br>
* SubIndex 57 - InU32_qam_warning_interlock<br>
* SubIndex 58 - InU32_qam_minor_interlock<br>
* SubIndex 59 - InU32_qam_serious_interlock<br>
* SubIndex 60 - InU8_rtm_off_fsm_state_current<br>
* SubIndex 61 - InU8_rtm_off_ctrl_mode_cur<br>
* SubIndex 62 - InU16_reserved12<br>
* SubIndex 63 - InU8_rtm_off_beam_id<br>
* SubIndex 64 - InU8_reserved13<br>
* SubIndex 65 - InU16_rtm_off_radiation_index<br>
* SubIndex 66 - InU32_rtm_off_not_ready_event<br>
* SubIndex 67 - InU32_rtm_off_warning_interlock<br>
* SubIndex 68 - InU32_rtm_off_minor_interlock<br>
* SubIndex 69 - InU32_rtm_off_serious_interlock<br>
* SubIndex 70 - InU8_gmm_fsm_state_current<br>
* SubIndex 71 - InU8_gmm_ctrl_mode_cur<br>
* SubIndex 72 - InU16_reserved14<br>
* SubIndex 73 - InU8_gmm_beam_id<br>
* SubIndex 74 - InU8_reserved15<br>
* SubIndex 75 - InU16_gmm_radiation_index<br>
* SubIndex 76 - InU32_gmm_not_ready_event<br>
* SubIndex 77 - InU32_gmm_warning_interlock<br>
* SubIndex 78 - InU32_gmm_minor_interlock<br>
* SubIndex 79 - InU32_gmm_serious_interlock<br>
* SubIndex 80 - InU32_gmm_move_status<br>
* SubIndex 81 - InF_gmm_position_cur<br>
* SubIndex 82 - InF_gmm_velocity_cur<br>
* SubIndex 83 - InU8_psm_fsm_state_current<br>
* SubIndex 84 - InU8_psm_ctrl_mode_cur<br>
* SubIndex 85 - InU16_reserved16<br>
* SubIndex 86 - InU8_psm_beam_id<br>
* SubIndex 87 - InU8_reserved17<br>
* SubIndex 88 - InU16_psm_radiation_index<br>
* SubIndex 89 - InU32_psm_not_ready_event<br>
* SubIndex 90 - InU32_psm_warning_interlock<br>
* SubIndex 91 - InU32_psm_minor_interlock<br>
* SubIndex 92 - InU32_psm_serious_interlock<br>
* SubIndex 93 - InU32_psm_move_status<br>
* SubIndex 94 - InF_psm_position_x_cur<br>
* SubIndex 95 - InF_psm_position_y_cur<br>
* SubIndex 96 - InF_psm_position_z_cur<br>
* SubIndex 97 - InF_psm_position_x_r_cur<br>
* SubIndex 98 - InF_psm_position_y_r_cur<br>
* SubIndex 99 - InF_psm_position_z_r_cur<br>
* SubIndex 100 - InF_psm_velocity_x_cur<br>
* SubIndex 101 - InF_psm_velocity_y_cur<br>
* SubIndex 102 - InF_psm_velocity_z_cur<br>
* SubIndex 103 - InF_psm_velocity_x_r_cur<br>
* SubIndex 104 - InF_psm_velocity_y_r_cur<br>
* SubIndex 105 - InF_psm_velocity_z_r_cur<br>
* SubIndex 106 - InU16_FkpButton<br>
* SubIndex 107 - InU32_CpgButton<br>
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
{ DEFTYPE_UNSIGNED16 , 0x10 , ACCESS_READ }, /* Subindex9 - InU16_reserved2 */
{ DEFTYPE_UNSIGNED8 , 0x08 , ACCESS_READ }, /* Subindex10 - InU8_rtm_on_arm_beam_id */
{ DEFTYPE_UNSIGNED8 , 0x08 , ACCESS_READ }, /* Subindex11 - InU8_reserved3 */
{ DEFTYPE_UNSIGNED16 , 0x10 , ACCESS_READ }, /* Subindex12 - InU16_rtm_on_arm_radiation_index */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex13 - InU32_rtm_on_arm_not_ready_event */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex14 - InU32_rtm_on_arm_warning_interlock */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex15 - InU32_rtm_on_arm_minor_interlock */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex16 - InU32_rtm_on_arm_serious_interlock */
{ DEFTYPE_UNSIGNED8 , 0x08 , ACCESS_READ }, /* Subindex17 - InU8_icm_fsm_state_current */
{ DEFTYPE_UNSIGNED8 , 0x08 , ACCESS_READ }, /* Subindex18 - InU8_icm_ctrl_mode_cur */
{ DEFTYPE_UNSIGNED16 , 0x10 , ACCESS_READ }, /* Subindex19 - InU16_reserved4 */
{ DEFTYPE_UNSIGNED8 , 0x08 , ACCESS_READ }, /* Subindex20 - InU8_icm_beam_id */
{ DEFTYPE_UNSIGNED8 , 0x08 , ACCESS_READ }, /* Subindex21 - InU8_reserved5 */
{ DEFTYPE_UNSIGNED16 , 0x10 , ACCESS_READ }, /* Subindex22 - InU16_icm_radiation_index */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex23 - InU32_icm_not_ready_event */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex24 - InU32_icm_warning_interlock */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex25 - InU32_icm_minor_interlock */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex26 - InU32_icm_serious_interlock */
{ DEFTYPE_UNSIGNED8 , 0x08 , ACCESS_READ }, /* Subindex27 - InU8_bgm_fsm_state_current */
{ DEFTYPE_UNSIGNED8 , 0x08 , ACCESS_READ }, /* Subindex28 - InU8_bgm_ctrl_mode_cur */
{ DEFTYPE_UNSIGNED16 , 0x10 , ACCESS_READ }, /* Subindex29 - InU16_reserved6 */
{ DEFTYPE_UNSIGNED8 , 0x08 , ACCESS_READ }, /* Subindex30 - InU8_bgm_beam_id */
{ DEFTYPE_UNSIGNED8 , 0x08 , ACCESS_READ }, /* Subindex31 - InU8_reserved7 */
{ DEFTYPE_UNSIGNED16 , 0x10 , ACCESS_READ }, /* Subindex32 - InU16_bgm_radiation_index */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex33 - InU32_bgm_not_ready_event */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex34 - InU32_bgm_warning_interlock */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex35 - InU32_bgm_minor_interlock */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex36 - InU32_bgm_serious_interlock */
{ DEFTYPE_REAL32 , 0x20 , ACCESS_READ }, /* Subindex37 - InF_beam_on_time */
{ DEFTYPE_REAL32 , 0x20 , ACCESS_READ }, /* Subindex38 - InF_primary_dose_current */
{ DEFTYPE_REAL32 , 0x20 , ACCESS_READ }, /* Subindex39 - InF_primary_dose_rate_current */
{ DEFTYPE_UNSIGNED8 , 0x08 , ACCESS_READ }, /* Subindex40 - InU8_primary_dose_fsm_state */
{ DEFTYPE_UNSIGNED8 , 0x08 , ACCESS_READ }, /* Subindex41 - InU8_reserved8 */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex42 - InU32_primary_dose_interlock */
{ DEFTYPE_REAL32 , 0x20 , ACCESS_READ }, /* Subindex43 - InF_second_dose_current */
{ DEFTYPE_REAL32 , 0x20 , ACCESS_READ }, /* Subindex44 - InF_secondry_dose_rate_current */
{ DEFTYPE_UNSIGNED8 , 0x08 , ACCESS_READ }, /* Subindex45 - InU8_secondry_dose_fsm_rate */
{ DEFTYPE_UNSIGNED8 , 0x08 , ACCESS_READ }, /* Subindex46 - InU8_reserved9 */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex47 - InU32_secondry_dose_interlock */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex48 - InU32_afc_interlock */
{ DEFTYPE_REAL32 , 0x20 , ACCESS_READ }, /* Subindex49 - InF_afc_position_current */
{ DEFTYPE_UNSIGNED8 , 0x08 , ACCESS_READ }, /* Subindex50 - InU8_qam_fsm_state_current */
{ DEFTYPE_UNSIGNED8 , 0x08 , ACCESS_READ }, /* Subindex51 - InU8_qam_ctrl_mode_cur */
{ DEFTYPE_UNSIGNED16 , 0x10 , ACCESS_READ }, /* Subindex52 - InU16_reserved10 */
{ DEFTYPE_UNSIGNED8 , 0x08 , ACCESS_READ }, /* Subindex53 - InU8_qam_beam_id */
{ DEFTYPE_UNSIGNED8 , 0x08 , ACCESS_READ }, /* Subindex54 - InU8_reserved11 */
{ DEFTYPE_UNSIGNED16 , 0x10 , ACCESS_READ }, /* Subindex55 - InU16_qam_radiation_index */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex56 - InU32_qam_not_ready_event */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex57 - InU32_qam_warning_interlock */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex58 - InU32_qam_minor_interlock */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex59 - InU32_qam_serious_interlock */
{ DEFTYPE_UNSIGNED8 , 0x08 , ACCESS_READ }, /* Subindex60 - InU8_rtm_off_fsm_state_current */
{ DEFTYPE_UNSIGNED8 , 0x08 , ACCESS_READ }, /* Subindex61 - InU8_rtm_off_ctrl_mode_cur */
{ DEFTYPE_UNSIGNED16 , 0x10 , ACCESS_READ }, /* Subindex62 - InU16_reserved12 */
{ DEFTYPE_UNSIGNED8 , 0x08 , ACCESS_READ }, /* Subindex63 - InU8_rtm_off_beam_id */
{ DEFTYPE_UNSIGNED8 , 0x08 , ACCESS_READ }, /* Subindex64 - InU8_reserved13 */
{ DEFTYPE_UNSIGNED16 , 0x10 , ACCESS_READ }, /* Subindex65 - InU16_rtm_off_radiation_index */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex66 - InU32_rtm_off_not_ready_event */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex67 - InU32_rtm_off_warning_interlock */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex68 - InU32_rtm_off_minor_interlock */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex69 - InU32_rtm_off_serious_interlock */
{ DEFTYPE_UNSIGNED8 , 0x08 , ACCESS_READ }, /* Subindex70 - InU8_gmm_fsm_state_current */
{ DEFTYPE_UNSIGNED8 , 0x08 , ACCESS_READ }, /* Subindex71 - InU8_gmm_ctrl_mode_cur */
{ DEFTYPE_UNSIGNED16 , 0x10 , ACCESS_READ }, /* Subindex72 - InU16_reserved14 */
{ DEFTYPE_UNSIGNED8 , 0x08 , ACCESS_READ }, /* Subindex73 - InU8_gmm_beam_id */
{ DEFTYPE_UNSIGNED8 , 0x08 , ACCESS_READ }, /* Subindex74 - InU8_reserved15 */
{ DEFTYPE_UNSIGNED16 , 0x10 , ACCESS_READ }, /* Subindex75 - InU16_gmm_radiation_index */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex76 - InU32_gmm_not_ready_event */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex77 - InU32_gmm_warning_interlock */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex78 - InU32_gmm_minor_interlock */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex79 - InU32_gmm_serious_interlock */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex80 - InU32_gmm_move_status */
{ DEFTYPE_REAL32 , 0x20 , ACCESS_READ }, /* Subindex81 - InF_gmm_position_cur */
{ DEFTYPE_REAL32 , 0x20 , ACCESS_READ }, /* Subindex82 - InF_gmm_velocity_cur */
{ DEFTYPE_UNSIGNED8 , 0x08 , ACCESS_READ }, /* Subindex83 - InU8_psm_fsm_state_current */
{ DEFTYPE_UNSIGNED8 , 0x08 , ACCESS_READ }, /* Subindex84 - InU8_psm_ctrl_mode_cur */
{ DEFTYPE_UNSIGNED16 , 0x10 , ACCESS_READ }, /* Subindex85 - InU16_reserved16 */
{ DEFTYPE_UNSIGNED8 , 0x08 , ACCESS_READ }, /* Subindex86 - InU8_psm_beam_id */
{ DEFTYPE_UNSIGNED8 , 0x08 , ACCESS_READ }, /* Subindex87 - InU8_reserved17 */
{ DEFTYPE_UNSIGNED16 , 0x10 , ACCESS_READ }, /* Subindex88 - InU16_psm_radiation_index */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex89 - InU32_psm_not_ready_event */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex90 - InU32_psm_warning_interlock */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex91 - InU32_psm_minor_interlock */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex92 - InU32_psm_serious_interlock */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex93 - InU32_psm_move_status */
{ DEFTYPE_REAL32 , 0x20 , ACCESS_READ }, /* Subindex94 - InF_psm_position_x_cur */
{ DEFTYPE_REAL32 , 0x20 , ACCESS_READ }, /* Subindex95 - InF_psm_position_y_cur */
{ DEFTYPE_REAL32 , 0x20 , ACCESS_READ }, /* Subindex96 - InF_psm_position_z_cur */
{ DEFTYPE_REAL32 , 0x20 , ACCESS_READ }, /* Subindex97 - InF_psm_position_x_r_cur */
{ DEFTYPE_REAL32 , 0x20 , ACCESS_READ }, /* Subindex98 - InF_psm_position_y_r_cur */
{ DEFTYPE_REAL32 , 0x20 , ACCESS_READ }, /* Subindex99 - InF_psm_position_z_r_cur */
{ DEFTYPE_REAL32 , 0x20 , ACCESS_READ }, /* Subindex100 - InF_psm_velocity_x_cur */
{ DEFTYPE_REAL32 , 0x20 , ACCESS_READ }, /* Subindex101 - InF_psm_velocity_y_cur */
{ DEFTYPE_REAL32 , 0x20 , ACCESS_READ }, /* Subindex102 - InF_psm_velocity_z_cur */
{ DEFTYPE_REAL32 , 0x20 , ACCESS_READ }, /* Subindex103 - InF_psm_velocity_x_r_cur */
{ DEFTYPE_REAL32 , 0x20 , ACCESS_READ }, /* Subindex104 - InF_psm_velocity_y_r_cur */
{ DEFTYPE_REAL32 , 0x20 , ACCESS_READ }, /* Subindex105 - InF_psm_velocity_z_r_cur */
{ DEFTYPE_UNSIGNED16 , 0x10 , ACCESS_READ }, /* Subindex106 - InU16_FkpButton */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }}; /* Subindex107 - InU32_CpgButton */

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
"InU16_reserved2\000"
"InU8_rtm_on_arm_beam_id\000"
"InU8_reserved3\000"
"InU16_rtm_on_arm_radiation_index\000"
"InU32_rtm_on_arm_not_ready_event\000"
"InU32_rtm_on_arm_warning_interlock\000"
"InU32_rtm_on_arm_minor_interlock\000"
"InU32_rtm_on_arm_serious_interlock\000"
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
UINT16 InU16_reserved2; /* Subindex9 - InU16_reserved2 */
UINT8 InU8_rtm_on_arm_beam_id; /* Subindex10 - InU8_rtm_on_arm_beam_id */
UINT8 InU8_reserved3; /* Subindex11 - InU8_reserved3 */
UINT16 InU16_rtm_on_arm_radiation_index; /* Subindex12 - InU16_rtm_on_arm_radiation_index */
UINT32 InU32_rtm_on_arm_not_ready_event; /* Subindex13 - InU32_rtm_on_arm_not_ready_event */
UINT32 InU32_rtm_on_arm_warning_interlock; /* Subindex14 - InU32_rtm_on_arm_warning_interlock */
UINT32 InU32_rtm_on_arm_minor_interlock; /* Subindex15 - InU32_rtm_on_arm_minor_interlock */
UINT32 InU32_rtm_on_arm_serious_interlock; /* Subindex16 - InU32_rtm_on_arm_serious_interlock */
UINT8 InU8_icm_fsm_state_current; /* Subindex17 - InU8_icm_fsm_state_current */
UINT8 InU8_icm_ctrl_mode_cur; /* Subindex18 - InU8_icm_ctrl_mode_cur */
UINT16 InU16_reserved4; /* Subindex19 - InU16_reserved4 */
UINT8 InU8_icm_beam_id; /* Subindex20 - InU8_icm_beam_id */
UINT8 InU8_reserved5; /* Subindex21 - InU8_reserved5 */
UINT16 InU16_icm_radiation_index; /* Subindex22 - InU16_icm_radiation_index */
UINT32 InU32_icm_not_ready_event; /* Subindex23 - InU32_icm_not_ready_event */
UINT32 InU32_icm_warning_interlock; /* Subindex24 - InU32_icm_warning_interlock */
UINT32 InU32_icm_minor_interlock; /* Subindex25 - InU32_icm_minor_interlock */
UINT32 InU32_icm_serious_interlock; /* Subindex26 - InU32_icm_serious_interlock */
UINT8 InU8_bgm_fsm_state_current; /* Subindex27 - InU8_bgm_fsm_state_current */
UINT8 InU8_bgm_ctrl_mode_cur; /* Subindex28 - InU8_bgm_ctrl_mode_cur */
UINT16 InU16_reserved6; /* Subindex29 - InU16_reserved6 */
UINT8 InU8_bgm_beam_id; /* Subindex30 - InU8_bgm_beam_id */
UINT8 InU8_reserved7; /* Subindex31 - InU8_reserved7 */
UINT16 InU16_bgm_radiation_index; /* Subindex32 - InU16_bgm_radiation_index */
UINT32 InU32_bgm_not_ready_event; /* Subindex33 - InU32_bgm_not_ready_event */
UINT32 InU32_bgm_warning_interlock; /* Subindex34 - InU32_bgm_warning_interlock */
UINT32 InU32_bgm_minor_interlock; /* Subindex35 - InU32_bgm_minor_interlock */
UINT32 InU32_bgm_serious_interlock; /* Subindex36 - InU32_bgm_serious_interlock */
REAL32 InF_beam_on_time; /* Subindex37 - InF_beam_on_time */
REAL32 InF_primary_dose_current; /* Subindex38 - InF_primary_dose_current */
REAL32 InF_primary_dose_rate_current; /* Subindex39 - InF_primary_dose_rate_current */
UINT8 InU8_primary_dose_fsm_state; /* Subindex40 - InU8_primary_dose_fsm_state */
UINT8 InU8_reserved8; /* Subindex41 - InU8_reserved8 */
UINT32 InU32_primary_dose_interlock; /* Subindex42 - InU32_primary_dose_interlock */
REAL32 InF_second_dose_current; /* Subindex43 - InF_second_dose_current */
REAL32 InF_secondry_dose_rate_current; /* Subindex44 - InF_secondry_dose_rate_current */
UINT8 InU8_secondry_dose_fsm_rate; /* Subindex45 - InU8_secondry_dose_fsm_rate */
UINT8 InU8_reserved9; /* Subindex46 - InU8_reserved9 */
UINT32 InU32_secondry_dose_interlock; /* Subindex47 - InU32_secondry_dose_interlock */
UINT32 InU32_afc_interlock; /* Subindex48 - InU32_afc_interlock */
REAL32 InF_afc_position_current; /* Subindex49 - InF_afc_position_current */
UINT8 InU8_qam_fsm_state_current; /* Subindex50 - InU8_qam_fsm_state_current */
UINT8 InU8_qam_ctrl_mode_cur; /* Subindex51 - InU8_qam_ctrl_mode_cur */
UINT16 InU16_reserved10; /* Subindex52 - InU16_reserved10 */
UINT8 InU8_qam_beam_id; /* Subindex53 - InU8_qam_beam_id */
UINT8 InU8_reserved11; /* Subindex54 - InU8_reserved11 */
UINT16 InU16_qam_radiation_index; /* Subindex55 - InU16_qam_radiation_index */
UINT32 InU32_qam_not_ready_event; /* Subindex56 - InU32_qam_not_ready_event */
UINT32 InU32_qam_warning_interlock; /* Subindex57 - InU32_qam_warning_interlock */
UINT32 InU32_qam_minor_interlock; /* Subindex58 - InU32_qam_minor_interlock */
UINT32 InU32_qam_serious_interlock; /* Subindex59 - InU32_qam_serious_interlock */
UINT8 InU8_rtm_off_fsm_state_current; /* Subindex60 - InU8_rtm_off_fsm_state_current */
UINT8 InU8_rtm_off_ctrl_mode_cur; /* Subindex61 - InU8_rtm_off_ctrl_mode_cur */
UINT16 InU16_reserved12; /* Subindex62 - InU16_reserved12 */
UINT8 InU8_rtm_off_beam_id; /* Subindex63 - InU8_rtm_off_beam_id */
UINT8 InU8_reserved13; /* Subindex64 - InU8_reserved13 */
UINT16 InU16_rtm_off_radiation_index; /* Subindex65 - InU16_rtm_off_radiation_index */
UINT32 InU32_rtm_off_not_ready_event; /* Subindex66 - InU32_rtm_off_not_ready_event */
UINT32 InU32_rtm_off_warning_interlock; /* Subindex67 - InU32_rtm_off_warning_interlock */
UINT32 InU32_rtm_off_minor_interlock; /* Subindex68 - InU32_rtm_off_minor_interlock */
UINT32 InU32_rtm_off_serious_interlock; /* Subindex69 - InU32_rtm_off_serious_interlock */
UINT8 InU8_gmm_fsm_state_current; /* Subindex70 - InU8_gmm_fsm_state_current */
UINT8 InU8_gmm_ctrl_mode_cur; /* Subindex71 - InU8_gmm_ctrl_mode_cur */
UINT16 InU16_reserved14; /* Subindex72 - InU16_reserved14 */
UINT8 InU8_gmm_beam_id; /* Subindex73 - InU8_gmm_beam_id */
UINT8 InU8_reserved15; /* Subindex74 - InU8_reserved15 */
UINT16 InU16_gmm_radiation_index; /* Subindex75 - InU16_gmm_radiation_index */
UINT32 InU32_gmm_not_ready_event; /* Subindex76 - InU32_gmm_not_ready_event */
UINT32 InU32_gmm_warning_interlock; /* Subindex77 - InU32_gmm_warning_interlock */
UINT32 InU32_gmm_minor_interlock; /* Subindex78 - InU32_gmm_minor_interlock */
UINT32 InU32_gmm_serious_interlock; /* Subindex79 - InU32_gmm_serious_interlock */
UINT32 InU32_gmm_move_status; /* Subindex80 - InU32_gmm_move_status */
REAL32 InF_gmm_position_cur; /* Subindex81 - InF_gmm_position_cur */
REAL32 InF_gmm_velocity_cur; /* Subindex82 - InF_gmm_velocity_cur */
UINT8 InU8_psm_fsm_state_current; /* Subindex83 - InU8_psm_fsm_state_current */
UINT8 InU8_psm_ctrl_mode_cur; /* Subindex84 - InU8_psm_ctrl_mode_cur */
UINT16 InU16_reserved16; /* Subindex85 - InU16_reserved16 */
UINT8 InU8_psm_beam_id; /* Subindex86 - InU8_psm_beam_id */
UINT8 InU8_reserved17; /* Subindex87 - InU8_reserved17 */
UINT16 InU16_psm_radiation_index; /* Subindex88 - InU16_psm_radiation_index */
UINT32 InU32_psm_not_ready_event; /* Subindex89 - InU32_psm_not_ready_event */
UINT32 InU32_psm_warning_interlock; /* Subindex90 - InU32_psm_warning_interlock */
UINT32 InU32_psm_minor_interlock; /* Subindex91 - InU32_psm_minor_interlock */
UINT32 InU32_psm_serious_interlock; /* Subindex92 - InU32_psm_serious_interlock */
UINT32 InU32_psm_move_status; /* Subindex93 - InU32_psm_move_status */
REAL32 InF_psm_position_x_cur; /* Subindex94 - InF_psm_position_x_cur */
REAL32 InF_psm_position_y_cur; /* Subindex95 - InF_psm_position_y_cur */
REAL32 InF_psm_position_z_cur; /* Subindex96 - InF_psm_position_z_cur */
REAL32 InF_psm_position_x_r_cur; /* Subindex97 - InF_psm_position_x_r_cur */
REAL32 InF_psm_position_y_r_cur; /* Subindex98 - InF_psm_position_y_r_cur */
REAL32 InF_psm_position_z_r_cur; /* Subindex99 - InF_psm_position_z_r_cur */
REAL32 InF_psm_velocity_x_cur; /* Subindex100 - InF_psm_velocity_x_cur */
REAL32 InF_psm_velocity_y_cur; /* Subindex101 - InF_psm_velocity_y_cur */
REAL32 InF_psm_velocity_z_cur; /* Subindex102 - InF_psm_velocity_z_cur */
REAL32 InF_psm_velocity_x_r_cur; /* Subindex103 - InF_psm_velocity_x_r_cur */
REAL32 InF_psm_velocity_y_r_cur; /* Subindex104 - InF_psm_velocity_y_r_cur */
REAL32 InF_psm_velocity_z_r_cur; /* Subindex105 - InF_psm_velocity_z_r_cur */
UINT16 InU16_FkpButton; /* Subindex106 - InU16_FkpButton */
UINT32 InU32_CpgButton; /* Subindex107 - InU32_CpgButton */
} OBJ_STRUCT_PACKED_END
__attribute__((aligned(1), packed))TOBJ6000;
#endif //#ifndef _LAN9252_APP_OBJECTS_H_

/**
* \brief Object variable
*/
PROTO TOBJ6000 InputData0x6000
#if defined(_LAN9252_APP_) && (_LAN9252_APP_ == 1)
={107,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}
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
* SubIndex 5 - OutU8_reserved1<br>
* SubIndex 6 - OutU16_radiation_index<br>
* SubIndex 7 - OutU8_rtm_on_require_state<br>
* SubIndex 8 - OutU8_rtm_on_require_ctrl_mode<br>
* SubIndex 9 - OutU32_rtm_on_interlock_override<br>
* SubIndex 10 - OutU32_rtm_on_unready_override<br>
* SubIndex 11 - OutU8_icm_require_state<br>
* SubIndex 12 - OutU8_icm_require_ctrl_mode<br>
* SubIndex 13 - OutU16_ct_status<br>
* SubIndex 14 - OutU32_icm_interlock_override<br>
* SubIndex 15 - OutU32_icm_unready_override<br>
* SubIndex 16 - OutU8_bgm_require_state<br>
* SubIndex 17 - OutU8_bgm_require_ctrl_mode<br>
* SubIndex 18 - OutU32_bgm_interlock_override<br>
* SubIndex 19 - OutU32_bgm_unready_override<br>
* SubIndex 20 - OutU8_qam_require_state<br>
* SubIndex 21 - OutU8_qam_require_ctrl_mode<br>
* SubIndex 22 - OutU32_qam_interlock_override<br>
* SubIndex 23 - OutU32_qam_unready_override<br>
* SubIndex 24 - OutU8_bsm_require_state<br>
* SubIndex 25 - OutU8_bsm_require_ctrl_mode<br>
* SubIndex 26 - OutU32_bsm_interlock_override<br>
* SubIndex 27 - OutU32_bsm_unready_override<br>
* SubIndex 28 - OutU8_rtm_off_require_state<br>
* SubIndex 29 - OutU8_rtm_off_require_ctrl_mode<br>
* SubIndex 30 - OutU32_rtm_off_interlock_override<br>
* SubIndex 31 - OutU32_rtm_off_unready_override<br>
* SubIndex 32 - OutU8_led_belt<br>
* SubIndex 33 - OutU8_reserved2<br>
* SubIndex 34 - OutU8_gmm_require_state<br>
* SubIndex 35 - OutU8_gmm_require_ctrl_mode<br>
* SubIndex 36 - OutU32_gmm_interlock_override<br>
* SubIndex 37 - OutU32_gmm_unready_override<br>
* SubIndex 38 - OutU8_psm_require_state<br>
* SubIndex 39 - OutU8_psm_require_ctrl_mode<br>
* SubIndex 40 - OutU32_psm_interlock_override<br>
* SubIndex 41 - OutU32_psm_unready_override<br>
* SubIndex 42 - OutU8_fkp_led_blink<br>
* SubIndex 43 - OutU8_cpg_led_blink<br>
*/
OBJCONST TSDOINFOENTRYDESC    OBJMEM asEntryDesc0x7010[] = {
{ DEFTYPE_UNSIGNED8 , 0x8 , ACCESS_READ },
{ DEFTYPE_UNSIGNED8 , 0x08 , ACCESS_READ }, /* Subindex1 - OutU8_BoardID */
{ DEFTYPE_UNSIGNED8 , 0x08 , ACCESS_READ }, /* Subindex2 - OutU8_Reserved0 */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex3 - OutU32_FirmWareVersion */
{ DEFTYPE_UNSIGNED8 , 0x08 , ACCESS_READ }, /* Subindex4 - OutU8_beam_id */
{ DEFTYPE_UNSIGNED8 , 0x08 , ACCESS_READ }, /* Subindex5 - OutU8_reserved1 */
{ DEFTYPE_UNSIGNED16 , 0x10 , ACCESS_READ }, /* Subindex6 - OutU16_radiation_index */
{ DEFTYPE_UNSIGNED8 , 0x08 , ACCESS_READ }, /* Subindex7 - OutU8_rtm_on_require_state */
{ DEFTYPE_UNSIGNED8 , 0x08 , ACCESS_READ }, /* Subindex8 - OutU8_rtm_on_require_ctrl_mode */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex9 - OutU32_rtm_on_interlock_override */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex10 - OutU32_rtm_on_unready_override */
{ DEFTYPE_UNSIGNED8 , 0x08 , ACCESS_READ }, /* Subindex11 - OutU8_icm_require_state */
{ DEFTYPE_UNSIGNED8 , 0x08 , ACCESS_READ }, /* Subindex12 - OutU8_icm_require_ctrl_mode */
{ DEFTYPE_UNSIGNED16 , 0x10 , ACCESS_READ }, /* Subindex13 - OutU16_ct_status */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex14 - OutU32_icm_interlock_override */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex15 - OutU32_icm_unready_override */
{ DEFTYPE_UNSIGNED8 , 0x08 , ACCESS_READ }, /* Subindex16 - OutU8_bgm_require_state */
{ DEFTYPE_UNSIGNED8 , 0x08 , ACCESS_READ }, /* Subindex17 - OutU8_bgm_require_ctrl_mode */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex18 - OutU32_bgm_interlock_override */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex19 - OutU32_bgm_unready_override */
{ DEFTYPE_UNSIGNED8 , 0x08 , ACCESS_READ }, /* Subindex20 - OutU8_qam_require_state */
{ DEFTYPE_UNSIGNED8 , 0x08 , ACCESS_READ }, /* Subindex21 - OutU8_qam_require_ctrl_mode */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex22 - OutU32_qam_interlock_override */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex23 - OutU32_qam_unready_override */
{ DEFTYPE_UNSIGNED8 , 0x08 , ACCESS_READ }, /* Subindex24 - OutU8_bsm_require_state */
{ DEFTYPE_UNSIGNED8 , 0x08 , ACCESS_READ }, /* Subindex25 - OutU8_bsm_require_ctrl_mode */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex26 - OutU32_bsm_interlock_override */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex27 - OutU32_bsm_unready_override */
{ DEFTYPE_UNSIGNED8 , 0x08 , ACCESS_READ }, /* Subindex28 - OutU8_rtm_off_require_state */
{ DEFTYPE_UNSIGNED8 , 0x08 , ACCESS_READ }, /* Subindex29 - OutU8_rtm_off_require_ctrl_mode */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex30 - OutU32_rtm_off_interlock_override */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex31 - OutU32_rtm_off_unready_override */
{ DEFTYPE_UNSIGNED8 , 0x08 , ACCESS_READ }, /* Subindex32 - OutU8_led_belt */
{ DEFTYPE_UNSIGNED8 , 0x08 , ACCESS_READ }, /* Subindex33 - OutU8_reserved2 */
{ DEFTYPE_UNSIGNED8 , 0x08 , ACCESS_READ }, /* Subindex34 - OutU8_gmm_require_state */
{ DEFTYPE_UNSIGNED8 , 0x08 , ACCESS_READ }, /* Subindex35 - OutU8_gmm_require_ctrl_mode */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex36 - OutU32_gmm_interlock_override */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex37 - OutU32_gmm_unready_override */
{ DEFTYPE_UNSIGNED8 , 0x08 , ACCESS_READ }, /* Subindex38 - OutU8_psm_require_state */
{ DEFTYPE_UNSIGNED8 , 0x08 , ACCESS_READ }, /* Subindex39 - OutU8_psm_require_ctrl_mode */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex40 - OutU32_psm_interlock_override */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex41 - OutU32_psm_unready_override */
{ DEFTYPE_UNSIGNED8 , 0x08 , ACCESS_READ }, /* Subindex42 - OutU8_fkp_led_blink */
{ DEFTYPE_UNSIGNED8 , 0x08 , ACCESS_READ }}; /* Subindex43 - OutU8_cpg_led_blink */

/**
* \brief Object/Entry names
*/
OBJCONST UCHAR OBJMEM aName0x7010[] = "OutputData\000"
"OutU8_BoardID\000"
"OutU8_Reserved0\000"
"OutU32_FirmWareVersion\000"
"OutU8_beam_id\000"
"OutU8_reserved1\000"
"OutU16_radiation_index\000"
"OutU8_rtm_on_require_state\000"
"OutU8_rtm_on_require_ctrl_mode\000"
"OutU32_rtm_on_interlock_override\000"
"OutU32_rtm_on_unready_override\000"
"OutU8_icm_require_state\000"
"OutU8_icm_require_ctrl_mode\000"
"OutU16_ct_status\000"
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
"OutU8_cpg_led_blink\000\377";
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
UINT8 OutU8_reserved1; /* Subindex5 - OutU8_reserved1 */
UINT16 OutU16_radiation_index; /* Subindex6 - OutU16_radiation_index */
UINT8 OutU8_rtm_on_require_state; /* Subindex7 - OutU8_rtm_on_require_state */
UINT8 OutU8_rtm_on_require_ctrl_mode; /* Subindex8 - OutU8_rtm_on_require_ctrl_mode */
UINT32 OutU32_rtm_on_interlock_override; /* Subindex9 - OutU32_rtm_on_interlock_override */
UINT32 OutU32_rtm_on_unready_override; /* Subindex10 - OutU32_rtm_on_unready_override */
UINT8 OutU8_icm_require_state; /* Subindex11 - OutU8_icm_require_state */
UINT8 OutU8_icm_require_ctrl_mode; /* Subindex12 - OutU8_icm_require_ctrl_mode */
UINT16 OutU16_ct_status; /* Subindex13 - OutU16_ct_status */
UINT32 OutU32_icm_interlock_override; /* Subindex14 - OutU32_icm_interlock_override */
UINT32 OutU32_icm_unready_override; /* Subindex15 - OutU32_icm_unready_override */
UINT8 OutU8_bgm_require_state; /* Subindex16 - OutU8_bgm_require_state */
UINT8 OutU8_bgm_require_ctrl_mode; /* Subindex17 - OutU8_bgm_require_ctrl_mode */
UINT32 OutU32_bgm_interlock_override; /* Subindex18 - OutU32_bgm_interlock_override */
UINT32 OutU32_bgm_unready_override; /* Subindex19 - OutU32_bgm_unready_override */
UINT8 OutU8_qam_require_state; /* Subindex20 - OutU8_qam_require_state */
UINT8 OutU8_qam_require_ctrl_mode; /* Subindex21 - OutU8_qam_require_ctrl_mode */
UINT32 OutU32_qam_interlock_override; /* Subindex22 - OutU32_qam_interlock_override */
UINT32 OutU32_qam_unready_override; /* Subindex23 - OutU32_qam_unready_override */
UINT8 OutU8_bsm_require_state; /* Subindex24 - OutU8_bsm_require_state */
UINT8 OutU8_bsm_require_ctrl_mode; /* Subindex25 - OutU8_bsm_require_ctrl_mode */
UINT32 OutU32_bsm_interlock_override; /* Subindex26 - OutU32_bsm_interlock_override */
UINT32 OutU32_bsm_unready_override; /* Subindex27 - OutU32_bsm_unready_override */
UINT8 OutU8_rtm_off_require_state; /* Subindex28 - OutU8_rtm_off_require_state */
UINT8 OutU8_rtm_off_require_ctrl_mode; /* Subindex29 - OutU8_rtm_off_require_ctrl_mode */
UINT32 OutU32_rtm_off_interlock_override; /* Subindex30 - OutU32_rtm_off_interlock_override */
UINT32 OutU32_rtm_off_unready_override; /* Subindex31 - OutU32_rtm_off_unready_override */
UINT8 OutU8_led_belt; /* Subindex32 - OutU8_led_belt */
UINT8 OutU8_reserved2; /* Subindex33 - OutU8_reserved2 */
UINT8 OutU8_gmm_require_state; /* Subindex34 - OutU8_gmm_require_state */
UINT8 OutU8_gmm_require_ctrl_mode; /* Subindex35 - OutU8_gmm_require_ctrl_mode */
UINT32 OutU32_gmm_interlock_override; /* Subindex36 - OutU32_gmm_interlock_override */
UINT32 OutU32_gmm_unready_override; /* Subindex37 - OutU32_gmm_unready_override */
UINT8 OutU8_psm_require_state; /* Subindex38 - OutU8_psm_require_state */
UINT8 OutU8_psm_require_ctrl_mode; /* Subindex39 - OutU8_psm_require_ctrl_mode */
UINT32 OutU32_psm_interlock_override; /* Subindex40 - OutU32_psm_interlock_override */
UINT32 OutU32_psm_unready_override; /* Subindex41 - OutU32_psm_unready_override */
UINT8 OutU8_fkp_led_blink; /* Subindex42 - OutU8_fkp_led_blink */
UINT8 OutU8_cpg_led_blink; /* Subindex43 - OutU8_cpg_led_blink */
} OBJ_STRUCT_PACKED_END
__attribute__((aligned(1), packed))TOBJ7010;
#endif //#ifndef _LAN9252_APP_OBJECTS_H_

/**
* \brief Object variable
*/
PROTO TOBJ7010 OutputData0x7010
#if defined(_LAN9252_APP_) && (_LAN9252_APP_ == 1)
={43,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}
#endif
;
/** @}*/







#ifdef _OBJD_
TOBJECT    OBJMEM ApplicationObjDic[] = {
/* Object 0x1601 */
{NULL , NULL ,  0x1601 , {DEFTYPE_PDOMAPPING , 43 | (OBJCODE_REC << 8)} , asEntryDesc0x1601 , aName0x1601 , &OutputDataProcessDataMapping0x1601 , NULL , NULL , 0x0000 },
/* Object 0x1A00 */
{NULL , NULL ,  0x1A00 , {DEFTYPE_PDOMAPPING , 107 | (OBJCODE_REC << 8)} , asEntryDesc0x1A00 , aName0x1A00 , &InputDataProcessDataMapping0x1A00 , NULL , NULL , 0x0000 },
/* Object 0x1C12 */
{NULL , NULL ,  0x1C12 , {DEFTYPE_UNSIGNED16 , 1 | (OBJCODE_ARR << 8)} , asEntryDesc0x1C12 , aName0x1C12 , &sRxPDOassign , NULL , NULL , 0x0000 },
/* Object 0x1C13 */
{NULL , NULL ,  0x1C13 , {DEFTYPE_UNSIGNED16 , 1 | (OBJCODE_ARR << 8)} , asEntryDesc0x1C13 , aName0x1C13 , &sTxPDOassign , NULL , NULL , 0x0000 },
/* Object 0x6000 */
{NULL , NULL ,  0x6000 , {DEFTYPE_RECORD , 107 | (OBJCODE_REC << 8)} , asEntryDesc0x6000 , aName0x6000 , &InputData0x6000 , NULL , NULL , 0x0000 },
/* Object 0x7010 */
{NULL , NULL ,  0x7010 , {DEFTYPE_RECORD , 43 | (OBJCODE_REC << 8)} , asEntryDesc0x7010 , aName0x7010 , &OutputData0x7010 , NULL , NULL , 0x0000 },
{NULL,NULL, 0xFFFF, {0, 0}, NULL, NULL, NULL, NULL}};
#endif    //#ifdef _OBJD_

#undef PROTO

/** @}*/
#define _LAN9252_APP_OBJECTS_H_
