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
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }}; /* Subindex13 - Reference to 0x7010.13 */

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
"SubIndex 013\000\377";
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
} OBJ_STRUCT_PACKED_END
TOBJ1601;
#endif //#ifndef _LAN9252_APP_OBJECTS_H_

/**
* \brief Object variable
*/
PROTO TOBJ1601 OutputDataProcessDataMapping0x1601
#if defined(_LAN9252_APP_) && (_LAN9252_APP_ == 1)
={13,0x70100110,0x70100210,0x70100310,0x70100410,0x70100510,0x70100610,0x70100710,0x70100810,0x70100910,0x70100A10,0x70100B10,0x70100C10,0x70100D10}
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
* SubIndex 15 - Following bits for 0x6000.14<br>
* SubIndex 16 - Following bits for 0x6000.14<br>
* SubIndex 17 - Following bits for 0x6000.14<br>
* SubIndex 18 - Following bits for 0x6000.14<br>
* SubIndex 19 - Following bits for 0x6000.14<br>
* SubIndex 20 - Reference to 0x6000.15<br>
* SubIndex 21 - Reference to 0x6000.16<br>
* SubIndex 22 - Reference to 0x6000.17<br>
* SubIndex 23 - Reference to 0x6000.18<br>
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
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex15 - Following bits for 0x6000.14 */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex16 - Following bits for 0x6000.14 */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex17 - Following bits for 0x6000.14 */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex18 - Following bits for 0x6000.14 */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex19 - Following bits for 0x6000.14 */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex20 - Reference to 0x6000.15 */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex21 - Reference to 0x6000.16 */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex22 - Reference to 0x6000.17 */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }}; /* Subindex23 - Reference to 0x6000.18 */

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
"SubIndex 023\000\377";
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
UINT32 SI15; /* Subindex15 - Following bits for 0x6000.14 */
UINT32 SI16; /* Subindex16 - Following bits for 0x6000.14 */
UINT32 SI17; /* Subindex17 - Following bits for 0x6000.14 */
UINT32 SI18; /* Subindex18 - Following bits for 0x6000.14 */
UINT32 SI19; /* Subindex19 - Following bits for 0x6000.14 */
UINT32 SI20; /* Subindex20 - Reference to 0x6000.15 */
UINT32 SI21; /* Subindex21 - Reference to 0x6000.16 */
UINT32 SI22; /* Subindex22 - Reference to 0x6000.17 */
UINT32 SI23; /* Subindex23 - Reference to 0x6000.18 */
} OBJ_STRUCT_PACKED_END
TOBJ1A00;
#endif //#ifndef _LAN9252_APP_OBJECTS_H_

/**
* \brief Object variable
*/
PROTO TOBJ1A00 InputDataProcessDataMapping0x1A00
#if defined(_LAN9252_APP_) && (_LAN9252_APP_ == 1)
={23,0x60000110,0x60000210,0x60000310,0x60000410,0x60000510,0x60000610,0x60000710,0x60000810,0x60000910,0x60000A10,0x60000B10,0x60000C10,0x60000D40,0x60000EF0,0x000000F0,0x000000F0,0x000000F0,0x000000F0,0x00000070,0x60000F10,0x60001020,0x60001110,0x60001210}
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
* SubIndex 1 - InU16_MlcFsmState<br>
* SubIndex 2 - InU16_BeamIndexFB<br>
* SubIndex 3 - InU16_BankIndexFB<br>
* SubIndex 4 - InU16_RidiationIndexFB<br>
* SubIndex 5 - InU16_LeafCrtControlMode<br>
* SubIndex 6 - InU16_CarCrtControlMode<br>
* SubIndex 7 - InU16_JawYCrtControlMode<br>
* SubIndex 8 - InU16_JawXCrtControlMode<br>
* SubIndex 9 - InU16_PlanCmdFB<br>
* SubIndex 10 - InU16_FaultInfo1<br>
* SubIndex 11 - InU16_FaultInfo2<br>
* SubIndex 12 - InU16_FaultCode<br>
* SubIndex 13 - InAU16_Reserve1<br>
* SubIndex 14 - InAU16_LeafCrtPos<br>
* SubIndex 15 - InU16_CarCrtPos<br>
* SubIndex 16 - InAU16_JawCrtPos<br>
* SubIndex 17 - InU16_JawInfo<br>
* SubIndex 18 - InU16_JawFsmState<br>
*/
OBJCONST TSDOINFOENTRYDESC    OBJMEM asEntryDesc0x6000[] = {
{ DEFTYPE_UNSIGNED8 , 0x8 , ACCESS_READ },
{ DEFTYPE_UNSIGNED16 , 0x10 , ACCESS_READ }, /* Subindex1 - InU16_MlcFsmState */
{ DEFTYPE_UNSIGNED16 , 0x10 , ACCESS_READ }, /* Subindex2 - InU16_BeamIndexFB */
{ DEFTYPE_UNSIGNED16 , 0x10 , ACCESS_READ }, /* Subindex3 - InU16_BankIndexFB */
{ DEFTYPE_UNSIGNED16 , 0x10 , ACCESS_READ }, /* Subindex4 - InU16_RidiationIndexFB */
{ DEFTYPE_UNSIGNED16 , 0x10 , ACCESS_READ }, /* Subindex5 - InU16_LeafCrtControlMode */
{ DEFTYPE_UNSIGNED16 , 0x10 , ACCESS_READ }, /* Subindex6 - InU16_CarCrtControlMode */
{ DEFTYPE_UNSIGNED16 , 0x10 , ACCESS_READ }, /* Subindex7 - InU16_JawYCrtControlMode */
{ DEFTYPE_UNSIGNED16 , 0x10 , ACCESS_READ }, /* Subindex8 - InU16_JawXCrtControlMode */
{ DEFTYPE_UNSIGNED16 , 0x10 , ACCESS_READ }, /* Subindex9 - InU16_PlanCmdFB */
{ DEFTYPE_UNSIGNED16 , 0x10 , ACCESS_READ }, /* Subindex10 - InU16_FaultInfo1 */
{ DEFTYPE_UNSIGNED16 , 0x10 , ACCESS_READ }, /* Subindex11 - InU16_FaultInfo2 */
{ DEFTYPE_UNSIGNED16 , 0x10 , ACCESS_READ }, /* Subindex12 - InU16_FaultCode */
{ DEFTYPE_UNICODE_STRING , 0x40 , ACCESS_READ }, /* Subindex13 - InAU16_Reserve1 */
{ DEFTYPE_UNICODE_STRING , 0x520 , ACCESS_READ }, /* Subindex14 - InAU16_LeafCrtPos */
{ DEFTYPE_UNSIGNED16 , 0x10 , ACCESS_READ }, /* Subindex15 - InU16_CarCrtPos */
{ DEFTYPE_UNICODE_STRING , 0x20 , ACCESS_READ }, /* Subindex16 - InAU16_JawCrtPos */
{ DEFTYPE_UNSIGNED16 , 0x10 , ACCESS_READ }, /* Subindex17 - InU16_JawInfo */
{ DEFTYPE_UNSIGNED16 , 0x10 , ACCESS_READ }}; /* Subindex18 - InU16_JawFsmState */

/**
* \brief Object/Entry names
*/
OBJCONST UCHAR OBJMEM aName0x6000[] = "InputData\000"
"InU16_MlcFsmState\000"
"InU16_BeamIndexFB\000"
"InU16_BankIndexFB\000"
"InU16_RidiationIndexFB\000"
"InU16_LeafCrtControlMode\000"
"InU16_CarCrtControlMode\000"
"InU16_JawYCrtControlMode\000"
"InU16_JawXCrtControlMode\000"
"InU16_PlanCmdFB\000"
"InU16_FaultInfo1\000"
"InU16_FaultInfo2\000"
"InU16_FaultCode\000"
"InAU16_Reserve1\000"
"InAU16_LeafCrtPos\000"
"InU16_CarCrtPos\000"
"InAU16_JawCrtPos\000"
"InU16_JawInfo\000"
"InU16_JawFsmState\000\377";
#endif //#ifdef _OBJD_

#ifndef _LAN9252_APP_OBJECTS_H_
/**
* \brief Object structure
*/
typedef struct OBJ_STRUCT_PACKED_START {
UINT16 u16SubIndex0;
UINT16 InU16_MlcFsmState; /* Subindex1 - InU16_MlcFsmState */
UINT16 InU16_BeamIndexFB; /* Subindex2 - InU16_BeamIndexFB */
UINT16 InU16_BankIndexFB; /* Subindex3 - InU16_BankIndexFB */
UINT16 InU16_RidiationIndexFB; /* Subindex4 - InU16_RidiationIndexFB */
UINT16 InU16_LeafCrtControlMode; /* Subindex5 - InU16_LeafCrtControlMode */
UINT16 InU16_CarCrtControlMode; /* Subindex6 - InU16_CarCrtControlMode */
UINT16 InU16_JawYCrtControlMode; /* Subindex7 - InU16_JawYCrtControlMode */
UINT16 InU16_JawXCrtControlMode; /* Subindex8 - InU16_JawXCrtControlMode */
UINT16 InU16_PlanCmdFB; /* Subindex9 - InU16_PlanCmdFB */
UINT16 InU16_FaultInfo1; /* Subindex10 - InU16_FaultInfo1 */
UINT16 InU16_FaultInfo2; /* Subindex11 - InU16_FaultInfo2 */
UINT16 InU16_FaultCode; /* Subindex12 - InU16_FaultCode */
UINT16 InAU16_Reserve1[4]; /* Subindex13 - InAU16_Reserve1 */
UINT16 InAU16_LeafCrtPos[82]; /* Subindex14 - InAU16_LeafCrtPos */
UINT16 InU16_CarCrtPos; /* Subindex15 - InU16_CarCrtPos */
UINT16 InAU16_JawCrtPos[2]; /* Subindex16 - InAU16_JawCrtPos */
UINT16 InU16_JawInfo; /* Subindex17 - InU16_JawInfo */
UINT16 InU16_JawFsmState; /* Subindex18 - InU16_JawFsmState */
} OBJ_STRUCT_PACKED_END
__attribute__((aligned(1), packed))TOBJ6000;
#endif //#ifndef _LAN9252_APP_OBJECTS_H_

/**
* \brief Object variable
*/
PROTO TOBJ6000 InputData0x6000
#if defined(_LAN9252_APP_) && (_LAN9252_APP_ == 1)
={18,0,0,0,0,0,0,0,0,0,0,0,0,{0x0000,0x0000,0x0000,0x0000},{0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000},0,{0x0000,0x0000},0,0}
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
* SubIndex 1 - OutU16_MlcFsmSetting<br>
* SubIndex 2 - OutU16_BeamIndex<br>
* SubIndex 3 - OutU16_BankIndex<br>
* SubIndex 4 - OutU16_RidiationIndex<br>
* SubIndex 5 - OutU16_LeafControlModeSetting<br>
* SubIndex 6 - OutU16_CarControlModeSetting<br>
* SubIndex 7 - OutU16_JawYControlModeSetting<br>
* SubIndex 8 - OutU16_JawXControlModeSetting<br>
* SubIndex 9 - OutU16_PlanCmd<br>
* SubIndex 10 - OutU16_JawXPositionSetting<br>
* SubIndex 11 - OutU16_JawYPositionSetting<br>
* SubIndex 12 - OutU16_ErrReset<br>
* SubIndex 13 - OutU16_JawFsmSetting<br>
*/
OBJCONST TSDOINFOENTRYDESC    OBJMEM asEntryDesc0x7010[] = {
{ DEFTYPE_UNSIGNED8 , 0x8 , ACCESS_READ },
{ DEFTYPE_UNSIGNED16 , 0x10 , ACCESS_READ }, /* Subindex1 - OutU16_MlcFsmSetting */
{ DEFTYPE_UNSIGNED16 , 0x10 , ACCESS_READ }, /* Subindex2 - OutU16_BeamIndex */
{ DEFTYPE_UNSIGNED16 , 0x10 , ACCESS_READ }, /* Subindex3 - OutU16_BankIndex */
{ DEFTYPE_UNSIGNED16 , 0x10 , ACCESS_READ }, /* Subindex4 - OutU16_RidiationIndex */
{ DEFTYPE_UNSIGNED16 , 0x10 , ACCESS_READ }, /* Subindex5 - OutU16_LeafControlModeSetting */
{ DEFTYPE_UNSIGNED16 , 0x10 , ACCESS_READ }, /* Subindex6 - OutU16_CarControlModeSetting */
{ DEFTYPE_UNSIGNED16 , 0x10 , ACCESS_READ }, /* Subindex7 - OutU16_JawYControlModeSetting */
{ DEFTYPE_UNSIGNED16 , 0x10 , ACCESS_READ }, /* Subindex8 - OutU16_JawXControlModeSetting */
{ DEFTYPE_UNSIGNED16 , 0x10 , ACCESS_READ }, /* Subindex9 - OutU16_PlanCmd */
{ DEFTYPE_UNSIGNED16 , 0x10 , ACCESS_READ }, /* Subindex10 - OutU16_JawXPositionSetting */
{ DEFTYPE_UNSIGNED16 , 0x10 , ACCESS_READ }, /* Subindex11 - OutU16_JawYPositionSetting */
{ DEFTYPE_UNSIGNED16 , 0x10 , ACCESS_READ }, /* Subindex12 - OutU16_ErrReset */
{ DEFTYPE_UNSIGNED16 , 0x10 , ACCESS_READ }}; /* Subindex13 - OutU16_JawFsmSetting */

/**
* \brief Object/Entry names
*/
OBJCONST UCHAR OBJMEM aName0x7010[] = "OutputData\000"
"OutU16_MlcFsmSetting\000"
"OutU16_BeamIndex\000"
"OutU16_BankIndex\000"
"OutU16_RidiationIndex\000"
"OutU16_LeafControlModeSetting\000"
"OutU16_CarControlModeSetting\000"
"OutU16_JawYControlModeSetting\000"
"OutU16_JawXControlModeSetting\000"
"OutU16_PlanCmd\000"
"OutU16_JawXPositionSetting\000"
"OutU16_JawYPositionSetting\000"
"OutU16_ErrReset\000"
"OutU16_JawFsmSetting\000\377";
#endif //#ifdef _OBJD_

#ifndef _LAN9252_APP_OBJECTS_H_
/**
* \brief Object structure
*/
typedef struct OBJ_STRUCT_PACKED_START {
UINT16 u16SubIndex0;
UINT16 OutU16_MlcFsmSetting; /* Subindex1 - OutU16_MlcFsmSetting */
UINT16 OutU16_BeamIndex; /* Subindex2 - OutU16_BeamIndex */
UINT16 OutU16_BankIndex; /* Subindex3 - OutU16_BankIndex */
UINT16 OutU16_RidiationIndex; /* Subindex4 - OutU16_RidiationIndex */
UINT16 OutU16_LeafControlModeSetting; /* Subindex5 - OutU16_LeafControlModeSetting */
UINT16 OutU16_CarControlModeSetting; /* Subindex6 - OutU16_CarControlModeSetting */
UINT16 OutU16_JawYControlModeSetting; /* Subindex7 - OutU16_JawYControlModeSetting */
UINT16 OutU16_JawXControlModeSetting; /* Subindex8 - OutU16_JawXControlModeSetting */
UINT16 OutU16_PlanCmd; /* Subindex9 - OutU16_PlanCmd */
UINT16 OutU16_JawXPositionSetting; /* Subindex10 - OutU16_JawXPositionSetting */
UINT16 OutU16_JawYPositionSetting; /* Subindex11 - OutU16_JawYPositionSetting */
UINT16 OutU16_ErrReset; /* Subindex12 - OutU16_ErrReset */
UINT16 OutU16_JawFsmSetting; /* Subindex13 - OutU16_JawFsmSetting */
} OBJ_STRUCT_PACKED_END
__attribute__((aligned(1), packed))TOBJ7010;
#endif //#ifndef _LAN9252_APP_OBJECTS_H_

/**
* \brief Object variable
*/
PROTO TOBJ7010 OutputData0x7010
#if defined(_LAN9252_APP_) && (_LAN9252_APP_ == 1)
={13,0,0,0,0,0,0,0,0,0,0,0,0,0}
#endif
;
/** @}*/







#ifdef _OBJD_
TOBJECT    OBJMEM ApplicationObjDic[] = {
/* Object 0x1601 */
{NULL , NULL ,  0x1601 , {DEFTYPE_PDOMAPPING , 13 | (OBJCODE_REC << 8)} , asEntryDesc0x1601 , aName0x1601 , &OutputDataProcessDataMapping0x1601 , NULL , NULL , 0x0000 },
/* Object 0x1A00 */
{NULL , NULL ,  0x1A00 , {DEFTYPE_PDOMAPPING , 23 | (OBJCODE_REC << 8)} , asEntryDesc0x1A00 , aName0x1A00 , &InputDataProcessDataMapping0x1A00 , NULL , NULL , 0x0000 },
/* Object 0x1C12 */
{NULL , NULL ,  0x1C12 , {DEFTYPE_UNSIGNED16 , 1 | (OBJCODE_ARR << 8)} , asEntryDesc0x1C12 , aName0x1C12 , &sRxPDOassign , NULL , NULL , 0x0000 },
/* Object 0x1C13 */
{NULL , NULL ,  0x1C13 , {DEFTYPE_UNSIGNED16 , 1 | (OBJCODE_ARR << 8)} , asEntryDesc0x1C13 , aName0x1C13 , &sTxPDOassign , NULL , NULL , 0x0000 },
/* Object 0x6000 */
{NULL , NULL ,  0x6000 , {DEFTYPE_RECORD , 18 | (OBJCODE_REC << 8)} , asEntryDesc0x6000 , aName0x6000 , &InputData0x6000 , NULL , NULL , 0x0000 },
/* Object 0x7010 */
{NULL , NULL ,  0x7010 , {DEFTYPE_RECORD , 13 | (OBJCODE_REC << 8)} , asEntryDesc0x7010 , aName0x7010 , &OutputData0x7010 , NULL , NULL , 0x0000 },
{NULL,NULL, 0xFFFF, {0, 0}, NULL, NULL, NULL, NULL}};
#endif    //#ifdef _OBJD_

#undef PROTO

/** @}*/
#define _LAN9252_APP_OBJECTS_H_
