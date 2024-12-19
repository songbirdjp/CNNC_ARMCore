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
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }}; /* Subindex18 - Reference to 0x6000.18 */

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
"SubIndex 018\000\377";
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
} OBJ_STRUCT_PACKED_END
TOBJ1A00;
#endif //#ifndef _LAN9252_APP_OBJECTS_H_

/**
* \brief Object variable
*/
PROTO TOBJ1A00 InputDataProcessDataMapping0x1A00
#if defined(_LAN9252_APP_) && (_LAN9252_APP_ == 1)
={18,0x60000108,0x60000208,0x60000310,0x60000410,0x60000520,0x60000620,0x60000720,0x60000820,0x60000920,0x60000A20,0x60000B20,0x60000C20,0x60000D08,0x60000E08,0x60000F20,0x60001020,0x60001120,0x60001220}
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
* SubIndex 17 - InU32_AfcState<br>
* SubIndex 18 - InF_AfcPositionCurrent<br>
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
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex17 - InU32_AfcState */
{ DEFTYPE_REAL32 , 0x20 , ACCESS_READ }}; /* Subindex18 - InF_AfcPositionCurrent */

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
"InU32_AfcState\000"
"InF_AfcPositionCurrent\000\377";
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
UINT32 InU32_AfcState; /* Subindex17 - InU32_AfcState */
REAL32 InF_AfcPositionCurrent; /* Subindex18 - InF_AfcPositionCurrent */
} OBJ_STRUCT_PACKED_END
__attribute__((aligned(1), packed))TOBJ6000;
#endif //#ifndef _LAN9252_APP_OBJECTS_H_

/**
* \brief Object variable
*/
PROTO TOBJ6000 InputData0x6000
#if defined(_LAN9252_APP_) && (_LAN9252_APP_ == 1)
={18,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}
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
{NULL , NULL ,  0x1A00 , {DEFTYPE_PDOMAPPING , 18 | (OBJCODE_REC << 8)} , asEntryDesc0x1A00 , aName0x1A00 , &InputDataProcessDataMapping0x1A00 , NULL , NULL , 0x0000 },
/* Object 0x1C12 */
{NULL , NULL ,  0x1C12 , {DEFTYPE_UNSIGNED16 , 1 | (OBJCODE_ARR << 8)} , asEntryDesc0x1C12 , aName0x1C12 , &sRxPDOassign , NULL , NULL , 0x0000 },
/* Object 0x1C13 */
{NULL , NULL ,  0x1C13 , {DEFTYPE_UNSIGNED16 , 1 | (OBJCODE_ARR << 8)} , asEntryDesc0x1C13 , aName0x1C13 , &sTxPDOassign , NULL , NULL , 0x0000 },
/* Object 0x6000 */
{NULL , NULL ,  0x6000 , {DEFTYPE_RECORD , 18 | (OBJCODE_REC << 8)} , asEntryDesc0x6000 , aName0x6000 , &InputData0x6000 , NULL , NULL , 0x0000 },
/* Object 0x7010 */
{NULL , NULL ,  0x7010 , {DEFTYPE_RECORD , 6 | (OBJCODE_REC << 8)} , asEntryDesc0x7010 , aName0x7010 , &OutputData0x7010 , NULL , NULL , 0x0000 },
{NULL,NULL, 0xFFFF, {0, 0}, NULL, NULL, NULL, NULL}};
#endif    //#ifdef _OBJD_

#undef PROTO

/** @}*/
#define _LAN9252_APP_OBJECTS_H_
