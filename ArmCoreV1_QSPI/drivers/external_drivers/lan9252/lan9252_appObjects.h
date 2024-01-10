/**
* \addtogroup lan9252_app lan9252_app
* @{
*/

/**
\file lan9252_appObjects
\author ET9300Utilities.ApplicationHandler (Version 1.3.6.0) | EthercatSSC@beckhoff.com

\brief lan9252_app specific objects<br>
\brief NOTE : This file will be overwritten if a new object dictionary is generated!<br>
*/

#if defined(_LAN9252_APP_) && (_LAN9252_APP_ == 1)
#define PROTO
#else
#define PROTO extern
#endif
/******************************************************************************
*                    Object 0x1601 : DO Outputs process data mapping
******************************************************************************/
/**
* \addtogroup 0x1601 0x1601 | DO Outputs process data mapping
* @{
* \brief Object 0x1601 (DO Outputs process data mapping) definition
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
OBJCONST TSDOINFOENTRYDESC	OBJMEM asEntryDesc0x1601[] = {
   {DEFTYPE_UNSIGNED8, 0x8, ACCESS_READ },
   {DEFTYPE_UNSIGNED32, 0x20, ACCESS_READ},
   {DEFTYPE_UNSIGNED32, 0x20, ACCESS_READ},
   {DEFTYPE_UNSIGNED32, 0x20, ACCESS_READ},
   {DEFTYPE_UNSIGNED32, 0x20, ACCESS_READ},
   {DEFTYPE_UNSIGNED32, 0x20, ACCESS_READ},

   {DEFTYPE_UNSIGNED32, 0x20, ACCESS_READ},
   {DEFTYPE_UNSIGNED32, 0x20, ACCESS_READ},
   {DEFTYPE_UNSIGNED32, 0x20, ACCESS_READ},
   {DEFTYPE_UNSIGNED32, 0x20, ACCESS_READ},
   {DEFTYPE_UNSIGNED32, 0x20, ACCESS_READ},

   {DEFTYPE_UNSIGNED32, 0x20, ACCESS_READ},
   {DEFTYPE_UNSIGNED32, 0x20, ACCESS_READ},
   {DEFTYPE_UNSIGNED32, 0x20, ACCESS_READ}
};

/**
* \brief Object/Entry names
*/
OBJCONST UCHAR OBJMEM aName0x1601[] = "DO RxPDO-Map\000\377";
#endif //#ifdef _OBJD_

#ifndef _LAN9252_APP_OBJECTS_H_
/**
* \brief Object structure
*/
typedef struct OBJ_STRUCT_PACKED_START {
UINT16 u16SubIndex0;
UINT32 aEntries[13]; /**< \brief Entry buffer*/
} OBJ_STRUCT_PACKED_END
TOBJ1601;
#endif //#ifndef _LAN9252_APP_OBJECTS_H_

/**
* \brief Object variable
*/
PROTO TOBJ1601 DOOutputsProcessDataMapping0x1601
#if defined(_LAN9252_APP_) && (_LAN9252_APP_ == 1)
= {13, {0x70100180,0x70100280,0x70100380,0x70100480,0x70100580,0x70100680,0x70100780,0x70100880,0x70100980,0x70100A80,0x70100B80,0x70100C80,0x70100D80}}
#endif
;
/** @}*/


/******************************************************************************
*                    Object 0x1A00 : DI Inputs process data mapping
******************************************************************************/
/**
* \addtogroup 0x1A00 0x1A00 | DI Inputs process data mapping
* @{
* \brief Object 0x1A00 (DI Inputs process data mapping) definition
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
*/
OBJCONST TSDOINFOENTRYDESC    OBJMEM asEntryDesc0x1A00[] = {
   {DEFTYPE_UNSIGNED8, 0x8, ACCESS_READ },
   {DEFTYPE_UNSIGNED32, 0x20, ACCESS_READ},
   {DEFTYPE_UNSIGNED32, 0x20, ACCESS_READ},
   {DEFTYPE_UNSIGNED32, 0x20, ACCESS_READ},
   {DEFTYPE_UNSIGNED32, 0x20, ACCESS_READ},
   {DEFTYPE_UNSIGNED32, 0x20, ACCESS_READ},

   {DEFTYPE_UNSIGNED32, 0x20, ACCESS_READ},
   {DEFTYPE_UNSIGNED32, 0x20, ACCESS_READ},
   {DEFTYPE_UNSIGNED32, 0x20, ACCESS_READ},
   {DEFTYPE_UNSIGNED32, 0x20, ACCESS_READ},
   {DEFTYPE_UNSIGNED32, 0x20, ACCESS_READ},

   {DEFTYPE_UNSIGNED32, 0x20, ACCESS_READ},
   {DEFTYPE_UNSIGNED32, 0x20, ACCESS_READ},
   {DEFTYPE_UNSIGNED32, 0x20, ACCESS_READ}
};

/**
* \brief Object/Entry names
*/
OBJCONST UCHAR OBJMEM aName0x1A00[] = "DI TxPDO-Map\000\377";
#endif //#ifdef _OBJD_

#ifndef _LAN9252_APP_OBJECTS_H_
/**
* \brief Object structure
*/
typedef struct OBJ_STRUCT_PACKED_START {
UINT16 u16SubIndex0;
UINT32 aEntries[13]; /**< \brief Entry buffer*/
} OBJ_STRUCT_PACKED_END
TOBJ1A00;
#endif //#ifndef _LAN9252_APP_OBJECTS_H_

/**
* \brief Object variable
*/
PROTO TOBJ1A00 DIInputsProcessDataMapping0x1A00
#if defined(_LAN9252_APP_) && (_LAN9252_APP_ == 1)
= {13, {0x60000180,0x60000280,0x60000380,0x60000480,0x60000580,0x60000680,0x60000780,0x60000880,0x60000980,0x60000A80,0x60000B80,0x60000C80,0x60000D80}}
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
OBJCONST UCHAR OBJMEM aName0x1C12[] = "RxPDO assign";
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
OBJCONST UCHAR OBJMEM aName0x1C13[] = "TxPDO assign";
#endif //#ifdef _OBJD_

#ifndef _LAN9252_APP_OBJECTS_H_
/**
* \brief Object structure
*/
typedef struct OBJ_STRUCT_PACKED_START {
UINT16   u16SubIndex0;  /**< \brief Subindex 0 */
UINT16 aEntries[2];  /**< \brief Subindex 1 - 1 */
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
*                    Object 0x6000 : DI Inputs
******************************************************************************/
/**
* \addtogroup 0x6000 0x6000 | DI Inputs
* @{
* \brief Object 0x6000 (DI Inputs) definition
*/
#ifdef _OBJD_
/**
* \brief Object entry descriptions<br>
* <br>
* SubIndex 0<br>
* SubIndex 1 - InfoIn1<br>
* SubIndex 2 - InfoIn2<br>
* SubIndex 3 - InfoIn3<br>
* SubIndex 4 - InfoIn4<br>
* SubIndex 5 - InfoIn5<br>
* SubIndex 6 - InfoIn6<br>
* SubIndex 7 - InfoIn7<br>
* SubIndex 8 - InfoIn8<br>
*/
OBJCONST TSDOINFOENTRYDESC    OBJMEM asEntryDesc0x6000[] = {
{ DEFTYPE_UNSIGNED8 , 0x8 , ACCESS_READ },
{DEFTYPE_UNICODE_STRING, 0x80, ACCESS_READ | OBJACCESS_TXPDOMAPPING},//ref ETG.1020 page105 cjh tag
{DEFTYPE_UNICODE_STRING, 0x80, ACCESS_READ | OBJACCESS_TXPDOMAPPING},
{DEFTYPE_UNICODE_STRING, 0x80, ACCESS_READ | OBJACCESS_TXPDOMAPPING},
{DEFTYPE_UNICODE_STRING, 0x80, ACCESS_READ | OBJACCESS_TXPDOMAPPING},
{DEFTYPE_UNICODE_STRING, 0x80, ACCESS_READ | OBJACCESS_TXPDOMAPPING},
{DEFTYPE_UNICODE_STRING, 0x80, ACCESS_READ | OBJACCESS_TXPDOMAPPING},
{DEFTYPE_UNICODE_STRING, 0x80, ACCESS_READ | OBJACCESS_TXPDOMAPPING},
{DEFTYPE_UNICODE_STRING, 0x80, ACCESS_READ | OBJACCESS_TXPDOMAPPING},
{DEFTYPE_UNICODE_STRING, 0x80, ACCESS_READ | OBJACCESS_TXPDOMAPPING},
{DEFTYPE_UNICODE_STRING, 0x80, ACCESS_READ | OBJACCESS_TXPDOMAPPING},
{DEFTYPE_UNICODE_STRING, 0x80, ACCESS_READ | OBJACCESS_TXPDOMAPPING},
{DEFTYPE_UNICODE_STRING, 0x80, ACCESS_READ | OBJACCESS_TXPDOMAPPING},
{DEFTYPE_UNICODE_STRING, 0x80, ACCESS_READ | OBJACCESS_TXPDOMAPPING},
};

/**
* \brief Object/Entry names
*/
OBJCONST UCHAR OBJMEM aName0x6000[] = "DI Inputs\000InfoIn\000DataIn1\000DataIn2\000DataIn3\000DataIn4\000DataIn5\000DataIn6\000DataIn7\000DataIn8\000DataIn9\000DataIn10\000DataIn11\000DataIn12\000\377";
#endif //#ifdef _OBJD_

#ifndef _LAN9252_APP_OBJECTS_H_
/**
* \brief Object structure
*/
typedef struct OBJ_STRUCT_PACKED_START {
UINT16 u16SubIndex0;
UINT16 InfoIn[8];
UINT16 DataIn1[8];
UINT16 DataIn2[8];
UINT16 DataIn3[8];
UINT16 DataIn4[8];
UINT16 DataIn5[8];
UINT16 DataIn6[8];
UINT16 DataIn7[8];
UINT16 DataIn8[8];
UINT16 DataIn9[8];
UINT16 DataIn10[8];
UINT16 DataIn11[8];
UINT16 DataIn12[8];
} OBJ_STRUCT_PACKED_END
TOBJ6000;
#endif //#ifndef _LAN9252_APP_OBJECTS_H_

/**
* \brief Object variable
*/
PROTO TOBJ6000 sDIInputs
#if defined(_LAN9252_APP_) && (_LAN9252_APP_ == 1)
= {13,
   {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},
   {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},
   {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},
   {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},
   {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},

   {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},
   {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},
   {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},
   {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},
   {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},

   {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},
   {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},
   {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},
}
#endif
;
/** @}*/


/******************************************************************************
*                    Object 0x7010 : DO Outputs
******************************************************************************/
/**
* \addtogroup 0x7010 0x7010 | DO Outputs
* @{
* \brief Object 0x7010 (DO Outputs) definition
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
   {DEFTYPE_UNICODE_STRING, 0x80, ACCESS_READ | OBJACCESS_RXPDOMAPPING},/* SubIndex 001: LED 1 */
   {DEFTYPE_UNICODE_STRING, 0x80, ACCESS_READ | OBJACCESS_RXPDOMAPPING},
   {DEFTYPE_UNICODE_STRING, 0x80, ACCESS_READ | OBJACCESS_RXPDOMAPPING},
   {DEFTYPE_UNICODE_STRING, 0x80, ACCESS_READ | OBJACCESS_RXPDOMAPPING},
   {DEFTYPE_UNICODE_STRING, 0x80, ACCESS_READ | OBJACCESS_RXPDOMAPPING},

   {DEFTYPE_UNICODE_STRING, 0x80, ACCESS_READ | OBJACCESS_RXPDOMAPPING},
   {DEFTYPE_UNICODE_STRING, 0x80, ACCESS_READ | OBJACCESS_RXPDOMAPPING},
   {DEFTYPE_UNICODE_STRING, 0x80, ACCESS_READ | OBJACCESS_RXPDOMAPPING},
   {DEFTYPE_UNICODE_STRING, 0x80, ACCESS_READ | OBJACCESS_RXPDOMAPPING},
   {DEFTYPE_UNICODE_STRING, 0x80, ACCESS_READ | OBJACCESS_RXPDOMAPPING},

   {DEFTYPE_UNICODE_STRING, 0x80, ACCESS_READ | OBJACCESS_RXPDOMAPPING},
   {DEFTYPE_UNICODE_STRING, 0x80, ACCESS_READ | OBJACCESS_RXPDOMAPPING},
   {DEFTYPE_UNICODE_STRING, 0x80, ACCESS_READ | OBJACCESS_RXPDOMAPPING}
}; /* Subindex13 - DataOut12 */

/**
* \brief Object/Entry names
*/
OBJCONST UCHAR OBJMEM aName0x7010[] = "DO Outputs\000InfoOut\000DataOut1\000DataOut2\000DataOut3\000DataOut4\000DataOut5\000DataOut6\000DataOut7\000DataOut8\000DataOut9\000DataOut10\000DataOut11\000DataOut12\000DataOut13\000\377";
#endif //#ifdef _OBJD_

#ifndef _LAN9252_APP_OBJECTS_H_
/**
* \brief Object structure
*/
typedef struct OBJ_STRUCT_PACKED_START {
UINT16 u16SubIndex0;
UINT16 InfoOut[8];
UINT16 DataOut1[8];
UINT16 DataOut2[8];
UINT16 DataOut3[8];
UINT16 DataOut4[8];
UINT16 DataOut5[8];
UINT16 DataOut6[8];
UINT16 DataOut7[8];
UINT16 DataOut8[8];
UINT16 DataOut9[8];
UINT16 DataOut10[8];
UINT16 DataOut11[8];
UINT16 DataOut12[8];
} OBJ_STRUCT_PACKED_END
TOBJ7010;
#endif //#ifndef _LAN9252_APP_OBJECTS_H_

/**
* \brief Object variable
*/
PROTO TOBJ7010 sDOOutputs
#if defined(_LAN9252_APP_) && (_LAN9252_APP_ == 1)
= {13,
   {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},
   {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},
   {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},
   {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},
   {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},

   {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},
   {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},
   {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},
   {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},
   {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},

   {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},
   {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},
   {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},
}
#endif
;
/** @}*/


#ifdef _OBJD_
TOBJECT    OBJMEM ApplicationObjDic[] = {
   /* Object 0x1601 */
   {NULL,NULL,  0x1601, {DEFTYPE_PDOMAPPING, 13| (OBJCODE_REC << 8)}, asEntryDesc0x1601, aName0x1601, &DOOutputsProcessDataMapping0x1601, NULL, NULL, 0x0000 },
   /* Object 0x1A00 */
   {NULL,NULL,   0x1A00, {DEFTYPE_PDOMAPPING, 13 | (OBJCODE_REC << 8)}, asEntryDesc0x1A00, aName0x1A00, &DIInputsProcessDataMapping0x1A00, NULL, NULL, 0x0000 },
    /* Object 0x1C12 */
   {NULL,NULL,   0x1C12, {DEFTYPE_UNSIGNED16, 1 | (OBJCODE_ARR << 8)}, asEntryDesc0x1C12, aName0x1C12, &sRxPDOassign, NULL, NULL, 0x0000 },
   /* Object 0x1C13 */
   {NULL,NULL,   0x1C13, {DEFTYPE_UNSIGNED16, 2 | (OBJCODE_ARR << 8)}, asEntryDesc0x1C13, aName0x1C13, &sTxPDOassign, NULL, NULL, 0x0000 },
   /* Object 0x6000 */
   {NULL,NULL,   0x6000, {DEFTYPE_RECORD, 13 | (OBJCODE_REC << 8)}, asEntryDesc0x6000, aName0x6000, &sDIInputs, NULL, NULL, 0x0000 },   /* pdo input */
   /* Object 0x7010 */
   {NULL,NULL,   0x7010, {DEFTYPE_RECORD, 13 | (OBJCODE_REC << 8)}, asEntryDesc0x7010, aName0x7010, &sDOOutputs, NULL, NULL, 0x0000 },  /* pdo output */
   {NULL,NULL, 0xFFFF, {0, 0}, NULL, NULL, NULL, NULL}};
#endif    //#ifdef _OBJD_
#undef PROTO

/** @}*/
#define _LAN9252_APP_OBJECTS_H_
