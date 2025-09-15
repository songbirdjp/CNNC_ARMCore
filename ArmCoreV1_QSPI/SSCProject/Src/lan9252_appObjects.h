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
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }}; /* Subindex44 - Reference to 0x7010.44 */

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
"SubIndex 044\000\377";
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
} OBJ_STRUCT_PACKED_END
TOBJ1601;
#endif //#ifndef _LAN9252_APP_OBJECTS_H_

/**
* \brief Object variable
*/
PROTO TOBJ1601 OutputDataProcessDataMapping0x1601
#if defined(_LAN9252_APP_) && (_LAN9252_APP_ == 1)
={44,0x70100108,0x70100208,0x70100320,0x70100408,0x70100508,0x70100610,0x70100708,0x70100808,0x70100908,0x70100A08,0x70100B10,0x70100C08,0x70100D08,0x70100E10,0x70100F20,0x70101020,0x70101120,0x70101220,0x70101320,0x70101420,0x70101520,0x70101608,0x70101708,0x70101810,0x70101908,0x70101A08,0x70101B10,0x70101C20,0x70101D20,0x70101E20,0x70101F20,0x70102020,0x70102120,0x70102220,0x70102320,0x70102420,0x70102520,0x70102620,0x70102720,0x70102820,0x70102920,0x70102A20,0x70102B20,0x70102C20}
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
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }}; /* Subindex40 - Reference to 0x6000.40 */

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
"SubIndex 040\000\377";
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
} OBJ_STRUCT_PACKED_END
TOBJ1A00;
#endif //#ifndef _LAN9252_APP_OBJECTS_H_

/**
* \brief Object variable
*/
PROTO TOBJ1A00 InputDataProcessDataMapping0x1A00
#if defined(_LAN9252_APP_) && (_LAN9252_APP_ == 1)
={40,0x60000108,0x60000208,0x60000320,0x60000408,0x60000508,0x60000610,0x60000708,0x60000808,0x60000908,0x60000A08,0x60000B20,0x60000C20,0x60000D20,0x60000E20,0x60000F08,0x60001008,0x60001108,0x60001208,0x60001308,0x60001408,0x60001520,0x60001620,0x60001720,0x60001820,0x60001920,0x60001A20,0x60001B20,0x60001C20,0x60001D20,0x60001E20,0x60001F20,0x60002020,0x60002120,0x60002220,0x60002308,0x60002408,0x60002508,0x60002608,0x60002708,0x60002808}
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
* SubIndex 5 - InU8_state_sync<br>
* SubIndex 6 - InU16_radiation_index<br>
* SubIndex 7 - InU8_fault_clear<br>
* SubIndex 8 - InU8_ethercat_Link_state<br>
* SubIndex 9 - InU8_gmm_require_state<br>
* SubIndex 10 - InU8_gmm_require_ctrl_mode<br>
* SubIndex 11 - InU32_gmm_interlock_override<br>
* SubIndex 12 - InU32_gmm_unready_override<br>
* SubIndex 13 - InF_gmm_position_tar<br>
* SubIndex 14 - InF_gmm_velocity_tar<br>
* SubIndex 15 - InU8_gmm_move_ctrl<br>
* SubIndex 16 - InU8_rtm_off_info<br>
* SubIndex 17 - InU8_reserved3<br>
* SubIndex 18 - InU8_reserved4<br>
* SubIndex 19 - InU8_psm_require_state<br>
* SubIndex 20 - InU8_psm_require_ctrl_mode<br>
* SubIndex 21 - InU32_psm_interlock_override<br>
* SubIndex 22 - InU32_psm_unready_override<br>
* SubIndex 23 - InF_psm_position_x_tar<br>
* SubIndex 24 - InF_psm_position_y_tar<br>
* SubIndex 25 - InF_psm_position_z_tar<br>
* SubIndex 26 - InF_psm_position_x_r_tar<br>
* SubIndex 27 - InF_psm_position_y_r_tar<br>
* SubIndex 28 - InF_psm_position_z_r_tar<br>
* SubIndex 29 - InF_psm_velocity_x_tar<br>
* SubIndex 30 - InF_psm_velocity_y_tar<br>
* SubIndex 31 - InF_psm_velocity_z_tar<br>
* SubIndex 32 - InF_psm_velocity_x_r_tar<br>
* SubIndex 33 - InF_psm_velocity_y_r_tar<br>
* SubIndex 34 - InF_psm_velocity_z_r_tar<br>
* SubIndex 35 - InU8_psm_move_ctrl<br>
* SubIndex 36 - InU8_reserved5<br>
* SubIndex 37 - InU8_reserved6<br>
* SubIndex 38 - InU8_reserved7<br>
* SubIndex 39 - InU8_function_button<br>
* SubIndex 40 - InU8_special_button<br>
*/
OBJCONST TSDOINFOENTRYDESC    OBJMEM asEntryDesc0x6000[] = {
{ DEFTYPE_UNSIGNED8 , 0x8 , ACCESS_READ },
{ DEFTYPE_UNSIGNED8 , 0x08 , ACCESS_READ }, /* Subindex1 - InU8_BoardID */
{ DEFTYPE_UNSIGNED8 , 0x08 , ACCESS_READ }, /* Subindex2 - InU8_Reserved0 */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex3 - InU32_FirmWareVersion */
{ DEFTYPE_UNSIGNED8 , 0x08 , ACCESS_READ }, /* Subindex4 - InU8_beam_id */
{ DEFTYPE_UNSIGNED8 , 0x08 , ACCESS_READ }, /* Subindex5 - InU8_state_sync */
{ DEFTYPE_UNSIGNED16 , 0x10 , ACCESS_READ }, /* Subindex6 - InU16_radiation_index */
{ DEFTYPE_UNSIGNED8 , 0x08 , ACCESS_READ }, /* Subindex7 - InU8_fault_clear */
{ DEFTYPE_UNSIGNED8 , 0x08 , ACCESS_READ }, /* Subindex8 - InU8_ethercat_Link_state */
{ DEFTYPE_UNSIGNED8 , 0x08 , ACCESS_READ }, /* Subindex9 - InU8_gmm_require_state */
{ DEFTYPE_UNSIGNED8 , 0x08 , ACCESS_READ }, /* Subindex10 - InU8_gmm_require_ctrl_mode */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex11 - InU32_gmm_interlock_override */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex12 - InU32_gmm_unready_override */
{ DEFTYPE_REAL32 , 0x20 , ACCESS_READ }, /* Subindex13 - InF_gmm_position_tar */
{ DEFTYPE_REAL32 , 0x20 , ACCESS_READ }, /* Subindex14 - InF_gmm_velocity_tar */
{ DEFTYPE_UNSIGNED8 , 0x08 , ACCESS_READ }, /* Subindex15 - InU8_gmm_move_ctrl */
{ DEFTYPE_UNSIGNED8 , 0x08 , ACCESS_READ }, /* Subindex16 - InU8_rtm_off_info */
{ DEFTYPE_UNSIGNED8 , 0x08 , ACCESS_READ }, /* Subindex17 - InU8_reserved3 */
{ DEFTYPE_UNSIGNED8 , 0x08 , ACCESS_READ }, /* Subindex18 - InU8_reserved4 */
{ DEFTYPE_UNSIGNED8 , 0x08 , ACCESS_READ }, /* Subindex19 - InU8_psm_require_state */
{ DEFTYPE_UNSIGNED8 , 0x08 , ACCESS_READ }, /* Subindex20 - InU8_psm_require_ctrl_mode */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex21 - InU32_psm_interlock_override */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex22 - InU32_psm_unready_override */
{ DEFTYPE_REAL32 , 0x20 , ACCESS_READ }, /* Subindex23 - InF_psm_position_x_tar */
{ DEFTYPE_REAL32 , 0x20 , ACCESS_READ }, /* Subindex24 - InF_psm_position_y_tar */
{ DEFTYPE_REAL32 , 0x20 , ACCESS_READ }, /* Subindex25 - InF_psm_position_z_tar */
{ DEFTYPE_REAL32 , 0x20 , ACCESS_READ }, /* Subindex26 - InF_psm_position_x_r_tar */
{ DEFTYPE_REAL32 , 0x20 , ACCESS_READ }, /* Subindex27 - InF_psm_position_y_r_tar */
{ DEFTYPE_REAL32 , 0x20 , ACCESS_READ }, /* Subindex28 - InF_psm_position_z_r_tar */
{ DEFTYPE_REAL32 , 0x20 , ACCESS_READ }, /* Subindex29 - InF_psm_velocity_x_tar */
{ DEFTYPE_REAL32 , 0x20 , ACCESS_READ }, /* Subindex30 - InF_psm_velocity_y_tar */
{ DEFTYPE_REAL32 , 0x20 , ACCESS_READ }, /* Subindex31 - InF_psm_velocity_z_tar */
{ DEFTYPE_REAL32 , 0x20 , ACCESS_READ }, /* Subindex32 - InF_psm_velocity_x_r_tar */
{ DEFTYPE_REAL32 , 0x20 , ACCESS_READ }, /* Subindex33 - InF_psm_velocity_y_r_tar */
{ DEFTYPE_REAL32 , 0x20 , ACCESS_READ }, /* Subindex34 - InF_psm_velocity_z_r_tar */
{ DEFTYPE_UNSIGNED8 , 0x08 , ACCESS_READ }, /* Subindex35 - InU8_psm_move_ctrl */
{ DEFTYPE_UNSIGNED8 , 0x08 , ACCESS_READ }, /* Subindex36 - InU8_reserved5 */
{ DEFTYPE_UNSIGNED8 , 0x08 , ACCESS_READ }, /* Subindex37 - InU8_reserved6 */
{ DEFTYPE_UNSIGNED8 , 0x08 , ACCESS_READ }, /* Subindex38 - InU8_reserved7 */
{ DEFTYPE_UNSIGNED8 , 0x08 , ACCESS_READ }, /* Subindex39 - InU8_function_button */
{ DEFTYPE_UNSIGNED8 , 0x08 , ACCESS_READ }}; /* Subindex40 - InU8_special_button */

/**
* \brief Object/Entry names
*/
OBJCONST UCHAR OBJMEM aName0x6000[] = "InputData\000"
"InU8_BoardID\000"
"InU8_Reserved0\000"
"InU32_FirmWareVersion\000"
"InU8_beam_id\000"
"InU8_state_sync\000"
"InU16_radiation_index\000"
"InU8_fault_clear\000"
"InU8_ethercat_Link_state\000"
"InU8_gmm_require_state\000"
"InU8_gmm_require_ctrl_mode\000"
"InU32_gmm_interlock_override\000"
"InU32_gmm_unready_override\000"
"InF_gmm_position_tar\000"
"InF_gmm_velocity_tar\000"
"InU8_gmm_move_ctrl\000"
"InU8_rtm_off_info\000"
"InU8_reserved3\000"
"InU8_reserved4\000"
"InU8_psm_require_state\000"
"InU8_psm_require_ctrl_mode\000"
"InU32_psm_interlock_override\000"
"InU32_psm_unready_override\000"
"InF_psm_position_x_tar\000"
"InF_psm_position_y_tar\000"
"InF_psm_position_z_tar\000"
"InF_psm_position_x_r_tar\000"
"InF_psm_position_y_r_tar\000"
"InF_psm_position_z_r_tar\000"
"InF_psm_velocity_x_tar\000"
"InF_psm_velocity_y_tar\000"
"InF_psm_velocity_z_tar\000"
"InF_psm_velocity_x_r_tar\000"
"InF_psm_velocity_y_r_tar\000"
"InF_psm_velocity_z_r_tar\000"
"InU8_psm_move_ctrl\000"
"InU8_reserved5\000"
"InU8_reserved6\000"
"InU8_reserved7\000"
"InU8_function_button\000"
"InU8_special_button\000\377";
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
UINT8 InU8_state_sync; /* Subindex5 - InU8_state_sync */
UINT16 InU16_radiation_index; /* Subindex6 - InU16_radiation_index */
UINT8 InU8_fault_clear; /* Subindex7 - InU8_fault_clear */
UINT8 InU8_ethercat_Link_state; /* Subindex8 - InU8_ethercat_Link_state */
UINT8 InU8_gmm_require_state; /* Subindex9 - InU8_gmm_require_state */
UINT8 InU8_gmm_require_ctrl_mode; /* Subindex10 - InU8_gmm_require_ctrl_mode */
UINT32 InU32_gmm_interlock_override; /* Subindex11 - InU32_gmm_interlock_override */
UINT32 InU32_gmm_unready_override; /* Subindex12 - InU32_gmm_unready_override */
REAL32 InF_gmm_position_tar; /* Subindex13 - InF_gmm_position_tar */
REAL32 InF_gmm_velocity_tar; /* Subindex14 - InF_gmm_velocity_tar */
UINT8 InU8_gmm_move_ctrl; /* Subindex15 - InU8_gmm_move_ctrl */
UINT8 InU8_rtm_off_info; /* Subindex16 - InU8_rtm_off_info */
UINT8 InU8_reserved3; /* Subindex17 - InU8_reserved3 */
UINT8 InU8_reserved4; /* Subindex18 - InU8_reserved4 */
UINT8 InU8_psm_require_state; /* Subindex19 - InU8_psm_require_state */
UINT8 InU8_psm_require_ctrl_mode; /* Subindex20 - InU8_psm_require_ctrl_mode */
UINT32 InU32_psm_interlock_override; /* Subindex21 - InU32_psm_interlock_override */
UINT32 InU32_psm_unready_override; /* Subindex22 - InU32_psm_unready_override */
REAL32 InF_psm_position_x_tar; /* Subindex23 - InF_psm_position_x_tar */
REAL32 InF_psm_position_y_tar; /* Subindex24 - InF_psm_position_y_tar */
REAL32 InF_psm_position_z_tar; /* Subindex25 - InF_psm_position_z_tar */
REAL32 InF_psm_position_x_r_tar; /* Subindex26 - InF_psm_position_x_r_tar */
REAL32 InF_psm_position_y_r_tar; /* Subindex27 - InF_psm_position_y_r_tar */
REAL32 InF_psm_position_z_r_tar; /* Subindex28 - InF_psm_position_z_r_tar */
REAL32 InF_psm_velocity_x_tar; /* Subindex29 - InF_psm_velocity_x_tar */
REAL32 InF_psm_velocity_y_tar; /* Subindex30 - InF_psm_velocity_y_tar */
REAL32 InF_psm_velocity_z_tar; /* Subindex31 - InF_psm_velocity_z_tar */
REAL32 InF_psm_velocity_x_r_tar; /* Subindex32 - InF_psm_velocity_x_r_tar */
REAL32 InF_psm_velocity_y_r_tar; /* Subindex33 - InF_psm_velocity_y_r_tar */
REAL32 InF_psm_velocity_z_r_tar; /* Subindex34 - InF_psm_velocity_z_r_tar */
UINT8 InU8_psm_move_ctrl; /* Subindex35 - InU8_psm_move_ctrl */
UINT8 InU8_reserved5; /* Subindex36 - InU8_reserved5 */
UINT8 InU8_reserved6; /* Subindex37 - InU8_reserved6 */
UINT8 InU8_reserved7; /* Subindex38 - InU8_reserved7 */
UINT8 InU8_function_button; /* Subindex39 - InU8_function_button */
UINT8 InU8_special_button; /* Subindex40 - InU8_special_button */
} OBJ_STRUCT_PACKED_END
TOBJ6000;
#endif //#ifndef _LAN9252_APP_OBJECTS_H_

/**
* \brief Object variable
*/
PROTO TOBJ6000 InputData0x6000
#if defined(_LAN9252_APP_) && (_LAN9252_APP_ == 1)
={40,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}
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
* SubIndex 7 - OutU8_ethercat_Link_state<br>
* SubIndex 8 - OutU8_plc_info<br>
* SubIndex 9 - OutU8_gmm_fsm_state_current<br>
* SubIndex 10 - OutU8_gmm_ctrl_mode_cur<br>
* SubIndex 11 - OutU16_reserved2<br>
* SubIndex 12 - OutU8_gmm_beam_id<br>
* SubIndex 13 - OutU8_reserved3<br>
* SubIndex 14 - OutU16_gmm_radiation_index<br>
* SubIndex 15 - OutU32_gmm_not_ready_event<br>
* SubIndex 16 - OutU32_gmm_warning_interlock<br>
* SubIndex 17 - OutU32_gmm_minor_interlock<br>
* SubIndex 18 - OutU32_gmm_serious_interlock<br>
* SubIndex 19 - OutU32_gmm_move_status<br>
* SubIndex 20 - OutF_gmm_position_cur<br>
* SubIndex 21 - OutF_gmm_velocity_cur<br>
* SubIndex 22 - OutU8_psm_fsm_state_current<br>
* SubIndex 23 - OutU8_psm_ctrl_mode_cur<br>
* SubIndex 24 - OutU16_reserved4<br>
* SubIndex 25 - OutU8_psm_beam_id<br>
* SubIndex 26 - OutU8_reserved5<br>
* SubIndex 27 - OutU16_psm_radiation_index<br>
* SubIndex 28 - OutU32_psm_not_ready_event<br>
* SubIndex 29 - OutU32_psm_warning_interlock<br>
* SubIndex 30 - OutU32_psm_minor_interlock<br>
* SubIndex 31 - OutU32_psm_serious_interlock<br>
* SubIndex 32 - OutU32_psm_move_status<br>
* SubIndex 33 - OutF_psm_position_x_cur<br>
* SubIndex 34 - OutF_psm_position_y_cur<br>
* SubIndex 35 - OutF_psm_position_z_cur<br>
* SubIndex 36 - OutF_psm_position_x_r_cur<br>
* SubIndex 37 - OutF_psm_position_y_r_cur<br>
* SubIndex 38 - OutF_psm_position_z_r_cur<br>
* SubIndex 39 - OutF_psm_velocity_x_cur<br>
* SubIndex 40 - OutF_psm_velocity_y_cur<br>
* SubIndex 41 - OutF_psm_velocity_z_cur<br>
* SubIndex 42 - OutF_psm_velocity_x_r_cur<br>
* SubIndex 43 - OutF_psm_velocity_y_r_cur<br>
* SubIndex 44 - OutF_psm_velocity_z_r_cur<br>
*/
OBJCONST TSDOINFOENTRYDESC    OBJMEM asEntryDesc0x7010[] = {
{ DEFTYPE_UNSIGNED8 , 0x8 , ACCESS_READ },
{ DEFTYPE_UNSIGNED8 , 0x08 , ACCESS_READ }, /* Subindex1 - OutU8_BoardID */
{ DEFTYPE_UNSIGNED8 , 0x08 , ACCESS_READ }, /* Subindex2 - OutU8_Reserved0 */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex3 - OutU32_FirmWareVersion */
{ DEFTYPE_UNSIGNED8 , 0x08 , ACCESS_READ }, /* Subindex4 - OutU8_beam_id */
{ DEFTYPE_UNSIGNED8 , 0x08 , ACCESS_READ }, /* Subindex5 - OutU8_reserved1 */
{ DEFTYPE_UNSIGNED16 , 0x10 , ACCESS_READ }, /* Subindex6 - OutU16_radiation_index */
{ DEFTYPE_UNSIGNED8 , 0x08 , ACCESS_READ }, /* Subindex7 - OutU8_ethercat_Link_state */
{ DEFTYPE_UNSIGNED8 , 0x08 , ACCESS_READ }, /* Subindex8 - OutU8_plc_info */
{ DEFTYPE_UNSIGNED8 , 0x08 , ACCESS_READ }, /* Subindex9 - OutU8_gmm_fsm_state_current */
{ DEFTYPE_UNSIGNED8 , 0x08 , ACCESS_READ }, /* Subindex10 - OutU8_gmm_ctrl_mode_cur */
{ DEFTYPE_UNSIGNED16 , 0x10 , ACCESS_READ }, /* Subindex11 - OutU16_reserved2 */
{ DEFTYPE_UNSIGNED8 , 0x08 , ACCESS_READ }, /* Subindex12 - OutU8_gmm_beam_id */
{ DEFTYPE_UNSIGNED8 , 0x08 , ACCESS_READ }, /* Subindex13 - OutU8_reserved3 */
{ DEFTYPE_UNSIGNED16 , 0x10 , ACCESS_READ }, /* Subindex14 - OutU16_gmm_radiation_index */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex15 - OutU32_gmm_not_ready_event */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex16 - OutU32_gmm_warning_interlock */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex17 - OutU32_gmm_minor_interlock */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex18 - OutU32_gmm_serious_interlock */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex19 - OutU32_gmm_move_status */
{ DEFTYPE_REAL32 , 0x20 , ACCESS_READ }, /* Subindex20 - OutF_gmm_position_cur */
{ DEFTYPE_REAL32 , 0x20 , ACCESS_READ }, /* Subindex21 - OutF_gmm_velocity_cur */
{ DEFTYPE_UNSIGNED8 , 0x08 , ACCESS_READ }, /* Subindex22 - OutU8_psm_fsm_state_current */
{ DEFTYPE_UNSIGNED8 , 0x08 , ACCESS_READ }, /* Subindex23 - OutU8_psm_ctrl_mode_cur */
{ DEFTYPE_UNSIGNED16 , 0x10 , ACCESS_READ }, /* Subindex24 - OutU16_reserved4 */
{ DEFTYPE_UNSIGNED8 , 0x08 , ACCESS_READ }, /* Subindex25 - OutU8_psm_beam_id */
{ DEFTYPE_UNSIGNED8 , 0x08 , ACCESS_READ }, /* Subindex26 - OutU8_reserved5 */
{ DEFTYPE_UNSIGNED16 , 0x10 , ACCESS_READ }, /* Subindex27 - OutU16_psm_radiation_index */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex28 - OutU32_psm_not_ready_event */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex29 - OutU32_psm_warning_interlock */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex30 - OutU32_psm_minor_interlock */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex31 - OutU32_psm_serious_interlock */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex32 - OutU32_psm_move_status */
{ DEFTYPE_REAL32 , 0x20 , ACCESS_READ }, /* Subindex33 - OutF_psm_position_x_cur */
{ DEFTYPE_REAL32 , 0x20 , ACCESS_READ }, /* Subindex34 - OutF_psm_position_y_cur */
{ DEFTYPE_REAL32 , 0x20 , ACCESS_READ }, /* Subindex35 - OutF_psm_position_z_cur */
{ DEFTYPE_REAL32 , 0x20 , ACCESS_READ }, /* Subindex36 - OutF_psm_position_x_r_cur */
{ DEFTYPE_REAL32 , 0x20 , ACCESS_READ }, /* Subindex37 - OutF_psm_position_y_r_cur */
{ DEFTYPE_REAL32 , 0x20 , ACCESS_READ }, /* Subindex38 - OutF_psm_position_z_r_cur */
{ DEFTYPE_REAL32 , 0x20 , ACCESS_READ }, /* Subindex39 - OutF_psm_velocity_x_cur */
{ DEFTYPE_REAL32 , 0x20 , ACCESS_READ }, /* Subindex40 - OutF_psm_velocity_y_cur */
{ DEFTYPE_REAL32 , 0x20 , ACCESS_READ }, /* Subindex41 - OutF_psm_velocity_z_cur */
{ DEFTYPE_REAL32 , 0x20 , ACCESS_READ }, /* Subindex42 - OutF_psm_velocity_x_r_cur */
{ DEFTYPE_REAL32 , 0x20 , ACCESS_READ }, /* Subindex43 - OutF_psm_velocity_y_r_cur */
{ DEFTYPE_REAL32 , 0x20 , ACCESS_READ }}; /* Subindex44 - OutF_psm_velocity_z_r_cur */

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
"OutU8_ethercat_Link_state\000"
"OutU8_plc_info\000"
"OutU8_gmm_fsm_state_current\000"
"OutU8_gmm_ctrl_mode_cur\000"
"OutU16_reserved2\000"
"OutU8_gmm_beam_id\000"
"OutU8_reserved3\000"
"OutU16_gmm_radiation_index\000"
"OutU32_gmm_not_ready_event\000"
"OutU32_gmm_warning_interlock\000"
"OutU32_gmm_minor_interlock\000"
"OutU32_gmm_serious_interlock\000"
"OutU32_gmm_move_status\000"
"OutF_gmm_position_cur\000"
"OutF_gmm_velocity_cur\000"
"OutU8_psm_fsm_state_current\000"
"OutU8_psm_ctrl_mode_cur\000"
"OutU16_reserved4\000"
"OutU8_psm_beam_id\000"
"OutU8_reserved5\000"
"OutU16_psm_radiation_index\000"
"OutU32_psm_not_ready_event\000"
"OutU32_psm_warning_interlock\000"
"OutU32_psm_minor_interlock\000"
"OutU32_psm_serious_interlock\000"
"OutU32_psm_move_status\000"
"OutF_psm_position_x_cur\000"
"OutF_psm_position_y_cur\000"
"OutF_psm_position_z_cur\000"
"OutF_psm_position_x_r_cur\000"
"OutF_psm_position_y_r_cur\000"
"OutF_psm_position_z_r_cur\000"
"OutF_psm_velocity_x_cur\000"
"OutF_psm_velocity_y_cur\000"
"OutF_psm_velocity_z_cur\000"
"OutF_psm_velocity_x_r_cur\000"
"OutF_psm_velocity_y_r_cur\000"
"OutF_psm_velocity_z_r_cur\000\377";
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
UINT8 OutU8_ethercat_Link_state; /* Subindex7 - OutU8_ethercat_Link_state */
UINT8 OutU8_plc_info; /* Subindex8 - OutU8_plc_info */
UINT8 OutU8_gmm_fsm_state_current; /* Subindex9 - OutU8_gmm_fsm_state_current */
UINT8 OutU8_gmm_ctrl_mode_cur; /* Subindex10 - OutU8_gmm_ctrl_mode_cur */
UINT16 OutU16_reserved2; /* Subindex11 - OutU16_reserved2 */
UINT8 OutU8_gmm_beam_id; /* Subindex12 - OutU8_gmm_beam_id */
UINT8 OutU8_reserved3; /* Subindex13 - OutU8_reserved3 */
UINT16 OutU16_gmm_radiation_index; /* Subindex14 - OutU16_gmm_radiation_index */
UINT32 OutU32_gmm_not_ready_event; /* Subindex15 - OutU32_gmm_not_ready_event */
UINT32 OutU32_gmm_warning_interlock; /* Subindex16 - OutU32_gmm_warning_interlock */
UINT32 OutU32_gmm_minor_interlock; /* Subindex17 - OutU32_gmm_minor_interlock */
UINT32 OutU32_gmm_serious_interlock; /* Subindex18 - OutU32_gmm_serious_interlock */
UINT32 OutU32_gmm_move_status; /* Subindex19 - OutU32_gmm_move_status */
REAL32 OutF_gmm_position_cur; /* Subindex20 - OutF_gmm_position_cur */
REAL32 OutF_gmm_velocity_cur; /* Subindex21 - OutF_gmm_velocity_cur */
UINT8 OutU8_psm_fsm_state_current; /* Subindex22 - OutU8_psm_fsm_state_current */
UINT8 OutU8_psm_ctrl_mode_cur; /* Subindex23 - OutU8_psm_ctrl_mode_cur */
UINT16 OutU16_reserved4; /* Subindex24 - OutU16_reserved4 */
UINT8 OutU8_psm_beam_id; /* Subindex25 - OutU8_psm_beam_id */
UINT8 OutU8_reserved5; /* Subindex26 - OutU8_reserved5 */
UINT16 OutU16_psm_radiation_index; /* Subindex27 - OutU16_psm_radiation_index */
UINT32 OutU32_psm_not_ready_event; /* Subindex28 - OutU32_psm_not_ready_event */
UINT32 OutU32_psm_warning_interlock; /* Subindex29 - OutU32_psm_warning_interlock */
UINT32 OutU32_psm_minor_interlock; /* Subindex30 - OutU32_psm_minor_interlock */
UINT32 OutU32_psm_serious_interlock; /* Subindex31 - OutU32_psm_serious_interlock */
UINT32 OutU32_psm_move_status; /* Subindex32 - OutU32_psm_move_status */
REAL32 OutF_psm_position_x_cur; /* Subindex33 - OutF_psm_position_x_cur */
REAL32 OutF_psm_position_y_cur; /* Subindex34 - OutF_psm_position_y_cur */
REAL32 OutF_psm_position_z_cur; /* Subindex35 - OutF_psm_position_z_cur */
REAL32 OutF_psm_position_x_r_cur; /* Subindex36 - OutF_psm_position_x_r_cur */
REAL32 OutF_psm_position_y_r_cur; /* Subindex37 - OutF_psm_position_y_r_cur */
REAL32 OutF_psm_position_z_r_cur; /* Subindex38 - OutF_psm_position_z_r_cur */
REAL32 OutF_psm_velocity_x_cur; /* Subindex39 - OutF_psm_velocity_x_cur */
REAL32 OutF_psm_velocity_y_cur; /* Subindex40 - OutF_psm_velocity_y_cur */
REAL32 OutF_psm_velocity_z_cur; /* Subindex41 - OutF_psm_velocity_z_cur */
REAL32 OutF_psm_velocity_x_r_cur; /* Subindex42 - OutF_psm_velocity_x_r_cur */
REAL32 OutF_psm_velocity_y_r_cur; /* Subindex43 - OutF_psm_velocity_y_r_cur */
REAL32 OutF_psm_velocity_z_r_cur; /* Subindex44 - OutF_psm_velocity_z_r_cur */
} OBJ_STRUCT_PACKED_END
TOBJ7010;
#endif //#ifndef _LAN9252_APP_OBJECTS_H_

/**
* \brief Object variable
*/
PROTO TOBJ7010 OutputData0x7010
#if defined(_LAN9252_APP_) && (_LAN9252_APP_ == 1)
={44,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}
#endif
;
/** @}*/







#ifdef _OBJD_
TOBJECT    OBJMEM ApplicationObjDic[] = {
/* Object 0x1601 */
{NULL , NULL ,  0x1601 , {DEFTYPE_PDOMAPPING , 44 | (OBJCODE_REC << 8)} , asEntryDesc0x1601 , aName0x1601 , &OutputDataProcessDataMapping0x1601 , NULL , NULL , 0x0000 },
/* Object 0x1A00 */
{NULL , NULL ,  0x1A00 , {DEFTYPE_PDOMAPPING , 40 | (OBJCODE_REC << 8)} , asEntryDesc0x1A00 , aName0x1A00 , &InputDataProcessDataMapping0x1A00 , NULL , NULL , 0x0000 },
/* Object 0x1C12 */
{NULL , NULL ,  0x1C12 , {DEFTYPE_UNSIGNED16 , 1 | (OBJCODE_ARR << 8)} , asEntryDesc0x1C12 , aName0x1C12 , &sRxPDOassign , NULL , NULL , 0x0000 },
/* Object 0x1C13 */
{NULL , NULL ,  0x1C13 , {DEFTYPE_UNSIGNED16 , 1 | (OBJCODE_ARR << 8)} , asEntryDesc0x1C13 , aName0x1C13 , &sTxPDOassign , NULL , NULL , 0x0000 },
/* Object 0x6000 */
{NULL , NULL ,  0x6000 , {DEFTYPE_RECORD , 40 | (OBJCODE_REC << 8)} , asEntryDesc0x6000 , aName0x6000 , &InputData0x6000 , NULL , NULL , 0x0000 },
/* Object 0x7010 */
{NULL , NULL ,  0x7010 , {DEFTYPE_RECORD , 44 | (OBJCODE_REC << 8)} , asEntryDesc0x7010 , aName0x7010 , &OutputData0x7010 , NULL , NULL , 0x0000 },
{NULL,NULL, 0xFFFF, {0, 0}, NULL, NULL, NULL, NULL}};
#endif    //#ifdef _OBJD_

#undef PROTO

/** @}*/
#define _LAN9252_APP_OBJECTS_H_
