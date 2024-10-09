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
={13,0x70100180,0x70100280,0x70100380,0x70100480,0x70100580,0x70100680,0x70100780,0x70100880,0x70100980,0x70100A80,0x70100B80,0x70100C80,0x70100D80}
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
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }}; /* Subindex13 - Reference to 0x6000.13 */

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
"SubIndex 013\000\377";
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
} OBJ_STRUCT_PACKED_END
TOBJ1A00;
#endif //#ifndef _LAN9252_APP_OBJECTS_H_

/**
* \brief Object variable
*/
PROTO TOBJ1A00 InputDataProcessDataMapping0x1A00
#if defined(_LAN9252_APP_) && (_LAN9252_APP_ == 1)
={13,0x60000180,0x60000280,0x60000380,0x60000480,0x60000580,0x60000680,0x60000780,0x60000880,0x60000980,0x60000A80,0x60000B80,0x60000C80,0x60000D80}
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
* SubIndex 1 - InfoIn<br>
* SubIndex 2 - DataIn1<br>
* SubIndex 3 - DataIn2<br>
* SubIndex 4 - DataIn3<br>
* SubIndex 5 - DataIn4<br>
* SubIndex 6 - DataIn5<br>
* SubIndex 7 - DataIn6<br>
* SubIndex 8 - DataIn7<br>
* SubIndex 9 - DataIn8<br>
* SubIndex 10 - DataIn9<br>
* SubIndex 11 - DataIn10<br>
* SubIndex 12 - DataIn11<br>
* SubIndex 13 - DataIn12<br>
*/
OBJCONST TSDOINFOENTRYDESC    OBJMEM asEntryDesc0x6000[] = {
{ DEFTYPE_UNSIGNED8 , 0x8 , ACCESS_READ },
{ DEFTYPE_UNICODE_STRING , 0x80 , ACCESS_READ }, /* Subindex1 - InfoIn */
{ DEFTYPE_UNICODE_STRING , 0x80 , ACCESS_READ }, /* Subindex2 - DataIn1 */
{ DEFTYPE_UNICODE_STRING , 0x80 , ACCESS_READ }, /* Subindex3 - DataIn2 */
{ DEFTYPE_UNICODE_STRING , 0x80 , ACCESS_READ }, /* Subindex4 - DataIn3 */
{ DEFTYPE_UNICODE_STRING , 0x80 , ACCESS_READ }, /* Subindex5 - DataIn4 */
{ DEFTYPE_UNICODE_STRING , 0x80 , ACCESS_READ }, /* Subindex6 - DataIn5 */
{ DEFTYPE_UNICODE_STRING , 0x80 , ACCESS_READ }, /* Subindex7 - DataIn6 */
{ DEFTYPE_UNICODE_STRING , 0x80 , ACCESS_READ }, /* Subindex8 - DataIn7 */
{ DEFTYPE_UNICODE_STRING , 0x80 , ACCESS_READ }, /* Subindex9 - DataIn8 */
{ DEFTYPE_UNICODE_STRING , 0x80 , ACCESS_READ }, /* Subindex10 - DataIn9 */
{ DEFTYPE_UNICODE_STRING , 0x80 , ACCESS_READ }, /* Subindex11 - DataIn10 */
{ DEFTYPE_UNICODE_STRING , 0x80 , ACCESS_READ }, /* Subindex12 - DataIn11 */
{ DEFTYPE_UNICODE_STRING , 0x80 , ACCESS_READ }}; /* Subindex13 - DataIn12 */

/**
* \brief Object/Entry names
*/
OBJCONST UCHAR OBJMEM aName0x6000[] = "InputData\000"
"InfoIn\000"
"DataIn1\000"
"DataIn2\000"
"DataIn3\000"
"DataIn4\000"
"DataIn5\000"
"DataIn6\000"
"DataIn7\000"
"DataIn8\000"
"DataIn9\000"
"DataIn10\000"
"DataIn11\000"
"DataIn12\000\377";
#endif //#ifdef _OBJD_

#ifndef _LAN9252_APP_OBJECTS_H_
/**
* \brief Object structure
*/
typedef struct OBJ_STRUCT_PACKED_START {
UINT16 u16SubIndex0;
UINT16 InfoIn[8]; /* Subindex1 - InfoIn */
UINT16 DataIn1[8]; /* Subindex2 - DataIn1 */
UINT16 DataIn2[8]; /* Subindex3 - DataIn2 */
UINT16 DataIn3[8]; /* Subindex4 - DataIn3 */
UINT16 DataIn4[8]; /* Subindex5 - DataIn4 */
UINT16 DataIn5[8]; /* Subindex6 - DataIn5 */
UINT16 DataIn6[8]; /* Subindex7 - DataIn6 */
UINT16 DataIn7[8]; /* Subindex8 - DataIn7 */
UINT16 DataIn8[8]; /* Subindex9 - DataIn8 */
UINT16 DataIn9[8]; /* Subindex10 - DataIn9 */
UINT16 DataIn10[8]; /* Subindex11 - DataIn10 */
UINT16 DataIn11[8]; /* Subindex12 - DataIn11 */
UINT16 DataIn12[8]; /* Subindex13 - DataIn12 */
} OBJ_STRUCT_PACKED_END
__attribute__((aligned(1), packed))TOBJ6000;
#endif //#ifndef _LAN9252_APP_OBJECTS_H_

/**
* \brief Object variable
*/
PROTO TOBJ6000 InputData0x6000
#if defined(_LAN9252_APP_) && (_LAN9252_APP_ == 1)
={13,{0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000},{0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000},{0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000},{0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000},{0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000},{0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000},{0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000},{0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000},{0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000},{0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000},{0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000},{0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000},{0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000}}
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
* SubIndex 1 - InfoOut<br>
* SubIndex 2 - DataOut1<br>
* SubIndex 3 - DataOut2<br>
* SubIndex 4 - DataOut3<br>
* SubIndex 5 - DataOut4<br>
* SubIndex 6 - DataOut5<br>
* SubIndex 7 - DataOut6<br>
* SubIndex 8 - DataOut7<br>
* SubIndex 9 - DataOut8<br>
* SubIndex 10 - DataOut9<br>
* SubIndex 11 - DataOut10<br>
* SubIndex 12 - DataOut11<br>
* SubIndex 13 - DataOut12<br>
*/
OBJCONST TSDOINFOENTRYDESC    OBJMEM asEntryDesc0x7010[] = {
{ DEFTYPE_UNSIGNED8 , 0x8 , ACCESS_READ },
{ DEFTYPE_UNICODE_STRING , 0x80 , ACCESS_READ }, /* Subindex1 - InfoOut */
{ DEFTYPE_UNICODE_STRING , 0x80 , ACCESS_READ }, /* Subindex2 - DataOut1 */
{ DEFTYPE_UNICODE_STRING , 0x80 , ACCESS_READ }, /* Subindex3 - DataOut2 */
{ DEFTYPE_UNICODE_STRING , 0x80 , ACCESS_READ }, /* Subindex4 - DataOut3 */
{ DEFTYPE_UNICODE_STRING , 0x80 , ACCESS_READ }, /* Subindex5 - DataOut4 */
{ DEFTYPE_UNICODE_STRING , 0x80 , ACCESS_READ }, /* Subindex6 - DataOut5 */
{ DEFTYPE_UNICODE_STRING , 0x80 , ACCESS_READ }, /* Subindex7 - DataOut6 */
{ DEFTYPE_UNICODE_STRING , 0x80 , ACCESS_READ }, /* Subindex8 - DataOut7 */
{ DEFTYPE_UNICODE_STRING , 0x80 , ACCESS_READ }, /* Subindex9 - DataOut8 */
{ DEFTYPE_UNICODE_STRING , 0x80 , ACCESS_READ }, /* Subindex10 - DataOut9 */
{ DEFTYPE_UNICODE_STRING , 0x80 , ACCESS_READ }, /* Subindex11 - DataOut10 */
{ DEFTYPE_UNICODE_STRING , 0x80 , ACCESS_READ }, /* Subindex12 - DataOut11 */
{ DEFTYPE_UNICODE_STRING , 0x80 , ACCESS_READ }}; /* Subindex13 - DataOut12 */

/**
* \brief Object/Entry names
*/
OBJCONST UCHAR OBJMEM aName0x7010[] = "OutputData\000"
"InfoOut\000"
"DataOut1\000"
"DataOut2\000"
"DataOut3\000"
"DataOut4\000"
"DataOut5\000"
"DataOut6\000"
"DataOut7\000"
"DataOut8\000"
"DataOut9\000"
"DataOut10\000"
"DataOut11\000"
"DataOut12\000\377";
#endif //#ifdef _OBJD_

#ifndef _LAN9252_APP_OBJECTS_H_
/**
* \brief Object structure
*/
typedef struct OBJ_STRUCT_PACKED_START {
UINT16 u16SubIndex0;
UINT16 InfoOut[8]; /* Subindex1 - InfoOut */
UINT16 DataOut1[8]; /* Subindex2 - DataOut1 */
UINT16 DataOut2[8]; /* Subindex3 - DataOut2 */
UINT16 DataOut3[8]; /* Subindex4 - DataOut3 */
UINT16 DataOut4[8]; /* Subindex5 - DataOut4 */
UINT16 DataOut5[8]; /* Subindex6 - DataOut5 */
UINT16 DataOut6[8]; /* Subindex7 - DataOut6 */
UINT16 DataOut7[8]; /* Subindex8 - DataOut7 */
UINT16 DataOut8[8]; /* Subindex9 - DataOut8 */
UINT16 DataOut9[8]; /* Subindex10 - DataOut9 */
UINT16 DataOut10[8]; /* Subindex11 - DataOut10 */
UINT16 DataOut11[8]; /* Subindex12 - DataOut11 */
UINT16 DataOut12[8]; /* Subindex13 - DataOut12 */
} OBJ_STRUCT_PACKED_END
__attribute__((aligned(1), packed))TOBJ7010;
#endif //#ifndef _LAN9252_APP_OBJECTS_H_

/**
* \brief Object variable
*/
PROTO TOBJ7010 OutputData0x7010
#if defined(_LAN9252_APP_) && (_LAN9252_APP_ == 1)
={13,{0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000},{0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000},{0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000},{0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000},{0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000},{0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000},{0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000},{0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000},{0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000},{0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000},{0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000},{0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000},{0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000}}
#endif
;
/** @}*/







#ifdef _OBJD_
TOBJECT    OBJMEM ApplicationObjDic[] = {
/* Object 0x1601 */
{NULL , NULL ,  0x1601 , {DEFTYPE_PDOMAPPING , 13 | (OBJCODE_REC << 8)} , asEntryDesc0x1601 , aName0x1601 , &OutputDataProcessDataMapping0x1601 , NULL , NULL , 0x0000 },
/* Object 0x1A00 */
{NULL , NULL ,  0x1A00 , {DEFTYPE_PDOMAPPING , 13 | (OBJCODE_REC << 8)} , asEntryDesc0x1A00 , aName0x1A00 , &InputDataProcessDataMapping0x1A00 , NULL , NULL , 0x0000 },
/* Object 0x1C12 */
{NULL , NULL ,  0x1C12 , {DEFTYPE_UNSIGNED16 , 1 | (OBJCODE_ARR << 8)} , asEntryDesc0x1C12 , aName0x1C12 , &sRxPDOassign , NULL , NULL , 0x0000 },
/* Object 0x1C13 */
{NULL , NULL ,  0x1C13 , {DEFTYPE_UNSIGNED16 , 1 | (OBJCODE_ARR << 8)} , asEntryDesc0x1C13 , aName0x1C13 , &sTxPDOassign , NULL , NULL , 0x0000 },
/* Object 0x6000 */
{NULL , NULL ,  0x6000 , {DEFTYPE_RECORD , 13 | (OBJCODE_REC << 8)} , asEntryDesc0x6000 , aName0x6000 , &InputData0x6000 , NULL , NULL , 0x0000 },
/* Object 0x7010 */
{NULL , NULL ,  0x7010 , {DEFTYPE_RECORD , 13 | (OBJCODE_REC << 8)} , asEntryDesc0x7010 , aName0x7010 , &OutputData0x7010 , NULL , NULL , 0x0000 },
{NULL,NULL, 0xFFFF, {0, 0}, NULL, NULL, NULL, NULL}};
#endif    //#ifdef _OBJD_

#undef PROTO

/** @}*/
#define _LAN9252_APP_OBJECTS_H_
