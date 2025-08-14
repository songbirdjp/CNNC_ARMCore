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
*/
OBJCONST TSDOINFOENTRYDESC    OBJMEM asEntryDesc0x1601[] = {
{ DEFTYPE_UNSIGNED8 , 0x8 , ACCESS_READ },
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex1 - Reference to 0x7010.1 */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex2 - Reference to 0x7010.2 */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex3 - Reference to 0x7010.3 */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex4 - Reference to 0x7010.4 */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex5 - Reference to 0x7010.5 */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }}; /* Subindex6 - Reference to 0x7010.6 */

/**
* \brief Object/Entry names
*/
OBJCONST UCHAR OBJMEM aName0x1601[] = "OutputData process data mapping\000"
"SubIndex 001\000"
"SubIndex 002\000"
"SubIndex 003\000"
"SubIndex 004\000"
"SubIndex 005\000"
"SubIndex 006\000\377";
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
} OBJ_STRUCT_PACKED_END
TOBJ1601;
#endif //#ifndef _LAN9252_APP_OBJECTS_H_

/**
* \brief Object variable
*/
PROTO TOBJ1601 OutputDataProcessDataMapping0x1601
#if defined(_LAN9252_APP_) && (_LAN9252_APP_ == 1)
={6,0x70100108,0x70100208,0x70100310,0x70100420,0x70100520,0x70100620}
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
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }}; /* Subindex35 - Reference to 0x6000.35 */

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
"SubIndex 035\000\377";
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
} OBJ_STRUCT_PACKED_END
TOBJ1A00;
#endif //#ifndef _LAN9252_APP_OBJECTS_H_

/**
* \brief Object variable
*/
PROTO TOBJ1A00 InputDataProcessDataMapping0x1A00
#if defined(_LAN9252_APP_) && (_LAN9252_APP_ == 1)
={35,0x60000108,0x60000208,0x60000310,0x60000410,0x60000520,0x60000620,0x60000720,0x60000820,0x60000920,0x60000A20,0x60000B20,0x60000C20,0x60000D08,0x60000E08,0x60000F20,0x60001020,0x60001120,0x60001220,0x60001320,0x60001420,0x60001520,0x60001608,0x60001708,0x60001820,0x60001920,0x60001A20,0x60001B20,0x60001C20,0x60001D08,0x60001E08,0x60001F20,0x60002020,0x60002120,0x60002220,0x60002320}
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
* SubIndex 1 - InU8_FsmState<br>
* SubIndex 2 - InU8_BeamId<br>
* SubIndex 3 - InU16_RadiationIndex<br>
* SubIndex 4 - InU16_NotReadyEvent<br>
* SubIndex 5 - InU32_WaringInterlock<br>
* SubIndex 6 - InU32_MinorInterlock<br>
* SubIndex 7 - InU32_SeriousInterlock<br>
* SubIndex 8 - InF_BeamOnTime<br>
* SubIndex 9 - InF_PrimaryDoseTotalActual<br>
* SubIndex 10 - InF_SecondaryDoseTotalActual<br>
* SubIndex 11 - InF_PrimaryDoseRateActual<br>
* SubIndex 12 - InF_SecondaryDoseRateActual<br>
* SubIndex 13 - InU8_DoseAFsmState<br>
* SubIndex 14 - InU8_DoseBFsmState<br>
* SubIndex 15 - InU32_DoseAInterlock<br>
* SubIndex 16 - InU32_DoseBInterlock<br>
* SubIndex 17 - InU32_DoseAErrorCode<br>
* SubIndex 18 - InU32_DoseBErrorCode<br>
* SubIndex 19 - InU32_AfcState<br>
* SubIndex 20 - InF_AfcPositionCurrent<br>
* SubIndex 21 - InF_EpsVersion<br>
* SubIndex 22 - InU8_EpsRunStatus<br>
* SubIndex 23 - InU8_EpsFaultStop<br>
* SubIndex 24 - InU32_EpsFaultCode<br>
* SubIndex 25 - InF_EpsVoltageOutput<br>
* SubIndex 26 - InF_EpsCurrentOutput<br>
* SubIndex 27 - InF_EpsPowerOutput<br>
* SubIndex 28 - InF_VpsVersion<br>
* SubIndex 29 - InU8_VpsRunStatus<br>
* SubIndex 30 - InU8_VpsFaultStop<br>
* SubIndex 31 - InU32_VpsFaultCode<br>
* SubIndex 32 - InF_VpsVoltageOutput<br>
* SubIndex 33 - InF_VpsCurrentOutput<br>
* SubIndex 34 - InF_VpsPowerVoltage<br>
* SubIndex 35 - InF_SF6Pressure<br>
*/
OBJCONST TSDOINFOENTRYDESC    OBJMEM asEntryDesc0x6000[] = {
{ DEFTYPE_UNSIGNED8 , 0x8 , ACCESS_READ },
{ DEFTYPE_UNSIGNED8 , 0x08 , ACCESS_READ }, /* Subindex1 - InU8_FsmState */
{ DEFTYPE_UNSIGNED8 , 0x08 , ACCESS_READ }, /* Subindex2 - InU8_BeamId */
{ DEFTYPE_UNSIGNED16 , 0x10 , ACCESS_READ }, /* Subindex3 - InU16_RadiationIndex */
{ DEFTYPE_UNSIGNED16 , 0x10 , ACCESS_READ }, /* Subindex4 - InU16_NotReadyEvent */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex5 - InU32_WaringInterlock */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex6 - InU32_MinorInterlock */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex7 - InU32_SeriousInterlock */
{ DEFTYPE_REAL32 , 0x20 , ACCESS_READ }, /* Subindex8 - InF_BeamOnTime */
{ DEFTYPE_REAL32 , 0x20 , ACCESS_READ }, /* Subindex9 - InF_PrimaryDoseTotalActual */
{ DEFTYPE_REAL32 , 0x20 , ACCESS_READ }, /* Subindex10 - InF_SecondaryDoseTotalActual */
{ DEFTYPE_REAL32 , 0x20 , ACCESS_READ }, /* Subindex11 - InF_PrimaryDoseRateActual */
{ DEFTYPE_REAL32 , 0x20 , ACCESS_READ }, /* Subindex12 - InF_SecondaryDoseRateActual */
{ DEFTYPE_UNSIGNED8 , 0x08 , ACCESS_READ }, /* Subindex13 - InU8_DoseAFsmState */
{ DEFTYPE_UNSIGNED8 , 0x08 , ACCESS_READ }, /* Subindex14 - InU8_DoseBFsmState */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex15 - InU32_DoseAInterlock */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex16 - InU32_DoseBInterlock */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex17 - InU32_DoseAErrorCode */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex18 - InU32_DoseBErrorCode */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex19 - InU32_AfcState */
{ DEFTYPE_REAL32 , 0x20 , ACCESS_READ }, /* Subindex20 - InF_AfcPositionCurrent */
{ DEFTYPE_REAL32 , 0x20 , ACCESS_READ }, /* Subindex21 - InF_EpsVersion */
{ DEFTYPE_UNSIGNED8 , 0x08 , ACCESS_READ }, /* Subindex22 - InU8_EpsRunStatus */
{ DEFTYPE_UNSIGNED8 , 0x08 , ACCESS_READ }, /* Subindex23 - InU8_EpsFaultStop */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex24 - InU32_EpsFaultCode */
{ DEFTYPE_REAL32 , 0x20 , ACCESS_READ }, /* Subindex25 - InF_EpsVoltageOutput */
{ DEFTYPE_REAL32 , 0x20 , ACCESS_READ }, /* Subindex26 - InF_EpsCurrentOutput */
{ DEFTYPE_REAL32 , 0x20 , ACCESS_READ }, /* Subindex27 - InF_EpsPowerOutput */
{ DEFTYPE_REAL32 , 0x20 , ACCESS_READ }, /* Subindex28 - InF_VpsVersion */
{ DEFTYPE_UNSIGNED8 , 0x08 , ACCESS_READ }, /* Subindex29 - InU8_VpsRunStatus */
{ DEFTYPE_UNSIGNED8 , 0x08 , ACCESS_READ }, /* Subindex30 - InU8_VpsFaultStop */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex31 - InU32_VpsFaultCode */
{ DEFTYPE_REAL32 , 0x20 , ACCESS_READ }, /* Subindex32 - InF_VpsVoltageOutput */
{ DEFTYPE_REAL32 , 0x20 , ACCESS_READ }, /* Subindex33 - InF_VpsCurrentOutput */
{ DEFTYPE_REAL32 , 0x20 , ACCESS_READ }, /* Subindex34 - InF_VpsPowerVoltage */
{ DEFTYPE_REAL32 , 0x20 , ACCESS_READ }}; /* Subindex35 - InF_SF6Pressure */

/**
* \brief Object/Entry names
*/
OBJCONST UCHAR OBJMEM aName0x6000[] = "InputData\000"
"InU8_FsmState\000"
"InU8_BeamId\000"
"InU16_RadiationIndex\000"
"InU16_NotReadyEvent\000"
"InU32_WaringInterlock\000"
"InU32_MinorInterlock\000"
"InU32_SeriousInterlock\000"
"InF_BeamOnTime\000"
"InF_PrimaryDoseTotalActual\000"
"InF_SecondaryDoseTotalActual\000"
"InF_PrimaryDoseRateActual\000"
"InF_SecondaryDoseRateActual\000"
"InU8_DoseAFsmState\000"
"InU8_DoseBFsmState\000"
"InU32_DoseAInterlock\000"
"InU32_DoseBInterlock\000"
"InU32_DoseAErrorCode\000"
"InU32_DoseBErrorCode\000"
"InU32_AfcState\000"
"InF_AfcPositionCurrent\000"
"InF_EpsVersion\000"
"InU8_EpsRunStatus\000"
"InU8_EpsFaultStop\000"
"InU32_EpsFaultCode\000"
"InF_EpsVoltageOutput\000"
"InF_EpsCurrentOutput\000"
"InF_EpsPowerOutput\000"
"InF_VpsVersion\000"
"InU8_VpsRunStatus\000"
"InU8_VpsFaultStop\000"
"InU32_VpsFaultCode\000"
"InF_VpsVoltageOutput\000"
"InF_VpsCurrentOutput\000"
"InF_VpsPowerVoltage\000"
"InF_SF6Pressure\000\377";
#endif //#ifdef _OBJD_

#ifndef _LAN9252_APP_OBJECTS_H_
/**
* \brief Object structure
*/
typedef struct OBJ_STRUCT_PACKED_START {
UINT16 u16SubIndex0;
UINT8 InU8_FsmState; /* Subindex1 - InU8_FsmState */
UINT8 InU8_BeamId; /* Subindex2 - InU8_BeamId */
UINT16 InU16_RadiationIndex; /* Subindex3 - InU16_RadiationIndex */
UINT16 InU16_NotReadyEvent; /* Subindex4 - InU16_NotReadyEvent */
UINT32 InU32_WaringInterlock; /* Subindex5 - InU32_WaringInterlock */
UINT32 InU32_MinorInterlock; /* Subindex6 - InU32_MinorInterlock */
UINT32 InU32_SeriousInterlock; /* Subindex7 - InU32_SeriousInterlock */
REAL32 InF_BeamOnTime; /* Subindex8 - InF_BeamOnTime */
REAL32 InF_PrimaryDoseTotalActual; /* Subindex9 - InF_PrimaryDoseTotalActual */
REAL32 InF_SecondaryDoseTotalActual; /* Subindex10 - InF_SecondaryDoseTotalActual */
REAL32 InF_PrimaryDoseRateActual; /* Subindex11 - InF_PrimaryDoseRateActual */
REAL32 InF_SecondaryDoseRateActual; /* Subindex12 - InF_SecondaryDoseRateActual */
UINT8 InU8_DoseAFsmState; /* Subindex13 - InU8_DoseAFsmState */
UINT8 InU8_DoseBFsmState; /* Subindex14 - InU8_DoseBFsmState */
UINT32 InU32_DoseAInterlock; /* Subindex15 - InU32_DoseAInterlock */
UINT32 InU32_DoseBInterlock; /* Subindex16 - InU32_DoseBInterlock */
UINT32 InU32_DoseAErrorCode; /* Subindex17 - InU32_DoseAErrorCode */
UINT32 InU32_DoseBErrorCode; /* Subindex18 - InU32_DoseBErrorCode */
UINT32 InU32_AfcState; /* Subindex19 - InU32_AfcState */
REAL32 InF_AfcPositionCurrent; /* Subindex20 - InF_AfcPositionCurrent */
REAL32 InF_EpsVersion; /* Subindex21 - InF_EpsVersion */
UINT8 InU8_EpsRunStatus; /* Subindex22 - InU8_EpsRunStatus */
UINT8 InU8_EpsFaultStop; /* Subindex23 - InU8_EpsFaultStop */
UINT32 InU32_EpsFaultCode; /* Subindex24 - InU32_EpsFaultCode */
REAL32 InF_EpsVoltageOutput; /* Subindex25 - InF_EpsVoltageOutput */
REAL32 InF_EpsCurrentOutput; /* Subindex26 - InF_EpsCurrentOutput */
REAL32 InF_EpsPowerOutput; /* Subindex27 - InF_EpsPowerOutput */
REAL32 InF_VpsVersion; /* Subindex28 - InF_VpsVersion */
UINT8 InU8_VpsRunStatus; /* Subindex29 - InU8_VpsRunStatus */
UINT8 InU8_VpsFaultStop; /* Subindex30 - InU8_VpsFaultStop */
UINT32 InU32_VpsFaultCode; /* Subindex31 - InU32_VpsFaultCode */
REAL32 InF_VpsVoltageOutput; /* Subindex32 - InF_VpsVoltageOutput */
REAL32 InF_VpsCurrentOutput; /* Subindex33 - InF_VpsCurrentOutput */
REAL32 InF_VpsPowerVoltage; /* Subindex34 - InF_VpsPowerVoltage */
REAL32 InF_SF6Pressure; /* Subindex35 - InF_SF6Pressure */
} OBJ_STRUCT_PACKED_END
__attribute__((aligned(1), packed))TOBJ6000;
#endif //#ifndef _LAN9252_APP_OBJECTS_H_

/**
* \brief Object variable
*/
PROTO TOBJ6000 InputData0x6000
#if defined(_LAN9252_APP_) && (_LAN9252_APP_ == 1)
={35,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}
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
* SubIndex 1 - OutU8_RequireState<br>
* SubIndex 2 - OutU8_BeamId<br>
* SubIndex 3 - OutU16_RadiationIndex<br>
* SubIndex 4 - OutU32_InterlockOverride<br>
* SubIndex 5 - OutU32_UnreadyOveride<br>
* SubIndex 6 - OutF_GantryPosition<br>
*/
OBJCONST TSDOINFOENTRYDESC    OBJMEM asEntryDesc0x7010[] = {
{ DEFTYPE_UNSIGNED8 , 0x8 , ACCESS_READ },
{ DEFTYPE_UNSIGNED8 , 0x08 , ACCESS_READ }, /* Subindex1 - OutU8_RequireState */
{ DEFTYPE_UNSIGNED8 , 0x08 , ACCESS_READ }, /* Subindex2 - OutU8_BeamId */
{ DEFTYPE_UNSIGNED16 , 0x10 , ACCESS_READ }, /* Subindex3 - OutU16_RadiationIndex */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex4 - OutU32_InterlockOverride */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex5 - OutU32_UnreadyOveride */
{ DEFTYPE_REAL32 , 0x20 , ACCESS_READ }}; /* Subindex6 - OutF_GantryPosition */

/**
* \brief Object/Entry names
*/
OBJCONST UCHAR OBJMEM aName0x7010[] = "OutputData\000"
"OutU8_RequireState\000"
"OutU8_BeamId\000"
"OutU16_RadiationIndex\000"
"OutU32_InterlockOverride\000"
"OutU32_UnreadyOveride\000"
"OutF_GantryPosition\000\377";
#endif //#ifdef _OBJD_

#ifndef _LAN9252_APP_OBJECTS_H_
/**
* \brief Object structure
*/
typedef struct OBJ_STRUCT_PACKED_START {
UINT16 u16SubIndex0;
UINT8 OutU8_RequireState; /* Subindex1 - OutU8_RequireState */
UINT8 OutU8_BeamId; /* Subindex2 - OutU8_BeamId */
UINT16 OutU16_RadiationIndex; /* Subindex3 - OutU16_RadiationIndex */
UINT32 OutU32_InterlockOverride; /* Subindex4 - OutU32_InterlockOverride */
UINT32 OutU32_UnreadyOveride; /* Subindex5 - OutU32_UnreadyOveride */
REAL32 OutF_GantryPosition; /* Subindex6 - OutF_GantryPosition */
} OBJ_STRUCT_PACKED_END
__attribute__((aligned(1), packed))TOBJ7010;
#endif //#ifndef _LAN9252_APP_OBJECTS_H_

/**
* \brief Object variable
*/
PROTO TOBJ7010 OutputData0x7010
#if defined(_LAN9252_APP_) && (_LAN9252_APP_ == 1)
={6,0,0,0,0,0,0}
#endif
;
/** @}*/







#ifdef _OBJD_
TOBJECT    OBJMEM ApplicationObjDic[] = {
/* Object 0x1601 */
{NULL , NULL ,  0x1601 , {DEFTYPE_PDOMAPPING , 6 | (OBJCODE_REC << 8)} , asEntryDesc0x1601 , aName0x1601 , &OutputDataProcessDataMapping0x1601 , NULL , NULL , 0x0000 },
/* Object 0x1A00 */
{NULL , NULL ,  0x1A00 , {DEFTYPE_PDOMAPPING , 35 | (OBJCODE_REC << 8)} , asEntryDesc0x1A00 , aName0x1A00 , &InputDataProcessDataMapping0x1A00 , NULL , NULL , 0x0000 },
/* Object 0x1C12 */
{NULL , NULL ,  0x1C12 , {DEFTYPE_UNSIGNED16 , 1 | (OBJCODE_ARR << 8)} , asEntryDesc0x1C12 , aName0x1C12 , &sRxPDOassign , NULL , NULL , 0x0000 },
/* Object 0x1C13 */
{NULL , NULL ,  0x1C13 , {DEFTYPE_UNSIGNED16 , 1 | (OBJCODE_ARR << 8)} , asEntryDesc0x1C13 , aName0x1C13 , &sTxPDOassign , NULL , NULL , 0x0000 },
/* Object 0x6000 */
{NULL , NULL ,  0x6000 , {DEFTYPE_RECORD , 35 | (OBJCODE_REC << 8)} , asEntryDesc0x6000 , aName0x6000 , &InputData0x6000 , NULL , NULL , 0x0000 },
/* Object 0x7010 */
{NULL , NULL ,  0x7010 , {DEFTYPE_RECORD , 6 | (OBJCODE_REC << 8)} , asEntryDesc0x7010 , aName0x7010 , &OutputData0x7010 , NULL , NULL , 0x0000 },
{NULL,NULL, 0xFFFF, {0, 0}, NULL, NULL, NULL, NULL}};
#endif    //#ifdef _OBJD_

#undef PROTO

/** @}*/
#define _LAN9252_APP_OBJECTS_H_
