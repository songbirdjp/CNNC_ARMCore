/**
\addtogroup EL9800Appl EL9800 application
@{
*/

/**
\file el9800appl.c
\author EthercatSSC@beckhoff.com
\brief Implementation

\version 5.11

<br>Changes to version V5.10:<br>
V5.11 ECAT11: create application interface function pointer, add eeprom emulation interface functions<br>
V5.11 EL9800 1: reset outputs on fallback from OP state<br>
<br>Changes to version V5.01:<br>
V5.10 ECAT6: Add "USE_DEFAULT_MAIN" to enable or disable the main function<br>
<br>Changes to version V5.0:<br>
V5.01 EL9800 2: Add TxPdo Parameter object 0x1802<br>
<br>Changes to version V4.30:<br>
V4.50 ECAT2: Create generic application interface functions. Documentation in Application Note ET9300.<br>
V4.50 COE2: Handle invalid PDO assign values.<br>
V4.30 : create file
*/


/*-----------------------------------------------------------------------------------------
------
------    Includes
------
-----------------------------------------------------------------------------------------*/
#include "ecat_def.h"

/* ECATCHANGE_START(V5.11) ECAT11*/
#include "applInterface.h"
/* ECATCHANGE_END(V5.11) ECAT11*/

#include "el9800hw.h"

#define _EVALBOARD_

#include "el9800appl.h"
#include "retarget.h"

#undef _EVALBOARD_
/*--------------------------------------------------------------------------------------
------
------    local types and defines
------
--------------------------------------------------------------------------------------*/
/*-----------------------------------------------------------------------------------------
------
------    local variables and constants
------
-----------------------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------------------
------
------    application specific functions
------
-----------------------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------------------
------
------    generic functions
------
-----------------------------------------------------------------------------------------*/

/////////////////////////////////////////////////////////////////////////////////////////
/**
 \brief    The function is called when an error state was acknowledged by the master

*////////////////////////////////////////////////////////////////////////////////////////

void APPL_AckErrorInd(UINT16 stateTrans)
{
}

/////////////////////////////////////////////////////////////////////////////////////////
/**
 \return    AL Status Code (see ecatslv.h ALSTATUSCODE_....)

 \brief    The function is called in the state transition from INIT to PREOP when
           all general settings were checked to start the mailbox handler. This function
           informs the application about the state transition, the application can refuse
           the state transition when returning an AL Status error code.
           The return code NOERROR_INWORK can be used, if the application cannot confirm
           the state transition immediately, in that case the application need to be complete 
           the transition by calling ECAT_StateChange.

*////////////////////////////////////////////////////////////////////////////////////////

UINT16 APPL_StartMailboxHandler(void)
{
    return ALSTATUSCODE_NOERROR;
}

/////////////////////////////////////////////////////////////////////////////////////////
/**
 \return     0, NOERROR_INWORK

 \brief    The function is called in the state transition from PREEOP to INIT
             to stop the mailbox handler. This functions informs the application
             about the state transition, the application cannot refuse
             the state transition.

*////////////////////////////////////////////////////////////////////////////////////////

UINT16 APPL_StopMailboxHandler(void)
{
    return ALSTATUSCODE_NOERROR;
}

/////////////////////////////////////////////////////////////////////////////////////////
/**
 \param    pIntMask    pointer to the AL Event Mask which will be written to the AL event Mask
                        register (0x204) when this function is succeeded. The event mask can be adapted
                        in this function
 \return    AL Status Code (see ecatslv.h ALSTATUSCODE_....)

 \brief    The function is called in the state transition from PREOP to SAFEOP when
             all general settings were checked to start the input handler. This function
             informs the application about the state transition, the application can refuse
             the state transition when returning an AL Status error code.
            The return code NOERROR_INWORK can be used, if the application cannot confirm
            the state transition immediately, in that case the application need to be complete 
            the transition by calling ECAT_StateChange.
*////////////////////////////////////////////////////////////////////////////////////////

UINT16 APPL_StartInputHandler(UINT16 *pIntMask)
{
    return ALSTATUSCODE_NOERROR;
}

/////////////////////////////////////////////////////////////////////////////////////////
/**
 \return     0, NOERROR_INWORK

 \brief    The function is called in the state transition from SAFEOP to PREEOP
             to stop the input handler. This functions informs the application
             about the state transition, the application cannot refuse
             the state transition.

*////////////////////////////////////////////////////////////////////////////////////////

UINT16 APPL_StopInputHandler(void)
{
    return ALSTATUSCODE_NOERROR;
}

/////////////////////////////////////////////////////////////////////////////////////////
/**
 \return    AL Status Code (see ecatslv.h ALSTATUSCODE_....)

 \brief    The function is called in the state transition from SAFEOP to OP when
             all general settings were checked to start the output handler. This function
             informs the application about the state transition, the application can refuse
             the state transition when returning an AL Status error code.
           The return code NOERROR_INWORK can be used, if the application cannot confirm
           the state transition immediately, in that case the application need to be complete 
           the transition by calling ECAT_StateChange.
*////////////////////////////////////////////////////////////////////////////////////////

UINT16 APPL_StartOutputHandler(void)
{
    return ALSTATUSCODE_NOERROR;
}

/////////////////////////////////////////////////////////////////////////////////////////
/**
 \return     0, NOERROR_INWORK

 \brief    The function is called in the state transition from OP to SAFEOP
             to stop the output handler. This functions informs the application
             about the state transition, the application cannot refuse
             the state transition.

*////////////////////////////////////////////////////////////////////////////////////////

UINT16 APPL_StopOutputHandler(void)
{
/*ECATCHANGE_START(V5.11) EL9800 1*/
//    sDOOutputs.bLED1 = 0;
//    sDOOutputs.bLED2 = 1;
//    sDOOutputs.bLED3 = 0;
//    sDOOutputs.bLED4 = 0;
//    sDOOutputs.bLED5 = 0;
//    sDOOutputs.bLED7 = 0;
//    sDOOutputs.bLED6 = 1;
//    sDOOutputs.bLED8 = 0;
//    sDOOutputs.bLED9 = 0;
//    sDOOutputs.bLED10 = 1;
//    sDOOutputs.bLED11 = 0;
//    sDOOutputs.bLED12 = 0;
//    sDOOutputs.bLED13 = 0;
//    sDOOutputs.bLED14 = 0;
//    sDOOutputs.bLED15 = 1;
//    sDOOutputs.bLED16 = 0;
//    sDOOutputs.bLED17 = 0x10;
//    sDOOutputs.bLED18 = 0x10;
//    sDOOutputs.bLED19 = 0x10;
//    sDOOutputs.bLED20 = 0x10;
//    LED_1(sDOOutputs.bLED1);
//    LED_2(sDOOutputs.bLED2);
//    LED_3(sDOOutputs.bLED3);
//    LED_4(sDOOutputs.bLED4);
//    LED_5(sDOOutputs.bLED5);
//    LED_6(sDOOutputs.bLED6); sDIInputs.InfoIn[0] = 0x2345;
////    sDIInputs.InfoIn[1] = 0x0000;
////    sDIInputs.InfoIn[2] = 0xffff;
////    sDIInputs.InfoIn[3] = 0x0000;
////    sDIInputs.InfoIn[4] = 0xffff;
////    sDIInputs.InfoIn[5] = 0x0000;
////    sDIInputs.InfoIn[6] = 0xffff;
////    sDIInputs.InfoIn[7] = 0x0000;
////
////    sDIInputs.DataIn1[0] = 0xffff;
////    sDIInputs.DataIn1[1] = 0xffff;
////    sDIInputs.DataIn1[2] = 0x0101;
////    sDIInputs.DataIn1[3] = 0xffff;
////    sDIInputs.DataIn1[4] = 0x0101;
//    LED_7(sDOOutputs.bLED7);
//    LED_8(sDOOutputs.bLED8);
//

    // printf("sDOOutputs.bLED9 = %xr\n",sDOOutputs.bLED9);
//    LED_1                        = sDOOutputs.bLED1;
//    LED_2                        = sDOOutputs.bLED2;
//    LED_3                        = sDOOutputs.bLED3;
//    LED_4                        = sDOOutputs.bLED4;
//    LED_5                        = sDOOutputs.bLED5;
//    LED_7                        = sDOOutputs.bLED7;
//    LED_6                        = sDOOutputs.bLED6;
//    LED_8                        = sDOOutputs.bLED8;
/*ECATCHANGE_END(V5.11) EL9800 1*/
    return ALSTATUSCODE_NOERROR;
}

/////////////////////////////////////////////////////////////////////////////////////////
/**
\return     0(ALSTATUSCODE_NOERROR), NOERROR_INWORK
\param      pInputSize  pointer to save the input process data length
\param      pOutputSize  pointer to save the output process data length

\brief    This function calculates the process data sizes from the actual SM-PDO-Assign
            and PDO mapping
*////////////////////////////////////////////////////////////////////////////////////////
UINT16 APPL_GenerateMapping(UINT16 *pInputSize, UINT16 *pOutputSize)
{
    UINT16 result = ALSTATUSCODE_NOERROR;
    UINT16 PDOAssignEntryCnt = 0;
    OBJCONST TOBJECT OBJMEM *pPDO = NULL;
    UINT16 PDOSubindex0 = 0;
    UINT32 *pPDOEntry = NULL;
    UINT16 PDOEntryCnt = 0;
    UINT16 InputSize = 0;
    UINT16 OutputSize = 0;

    /*Scan object 0x1C12 RXPDO assign*/
    for (PDOAssignEntryCnt = 0; PDOAssignEntryCnt < sRxPDOassign.u16SubIndex0; PDOAssignEntryCnt++)
    {
        pPDO = OBJ_GetObjectHandle(sRxPDOassign.aEntries[PDOAssignEntryCnt]);
//        printf("sRxPDOassign.aEntries[PDOAssignEntryCnt] = %x\r\n",sRxPDOassign.aEntries[PDOAssignEntryCnt]);//1601
        if (pPDO != NULL)
        {
            PDOSubindex0 = *((UINT16 *) pPDO->pVarPtr);
//            printf("PDOIdx  = %x\r\n",PDOSubindex0);
            for (PDOEntryCnt = 0; PDOEntryCnt < PDOSubindex0; PDOEntryCnt++)
            {
                uint32_t tmp = OBJ_GetEntryOffset((PDOEntryCnt + 1), pPDO);
//                printf("(OBJ_GetEntryOffset((PDOEntryCnt + 1), pPDO) = %ld\r\n",tmp);
                pPDOEntry = (UINT32 *) ((UINT8 *) pPDO->pVarPtr +
                                        (OBJ_GetEntryOffset((PDOEntryCnt + 1), pPDO) >> 3));    //goto PDO entry
                // we increment the expected output size depending on the mapped Entry
//                printf("PDOIdx  = %x\r\n",PDOSubindex0);
//                printf("pPDOEntry = %ld\r\n",*pPDOEntry);
                OutputSize += (UINT16) ((*pPDOEntry) & 0xFF);
//                printf("OutputSize = %d\r\n",OutputSize);
            }
        }
        else
        {
            /*assigned PDO was not found in object dictionary. return invalid mapping*/
            OutputSize = 0;
            result = ALSTATUSCODE_INVALIDOUTPUTMAPPING;
            break;
        }
    }

    OutputSize = (OutputSize + 7) >> 3;

    if (result == 0)
    {
        /*Scan Object 0x1C13 TXPDO assign*/
        for (PDOAssignEntryCnt = 0; PDOAssignEntryCnt < sTxPDOassign.u16SubIndex0; PDOAssignEntryCnt++)
        {
            pPDO = OBJ_GetObjectHandle(sTxPDOassign.aEntries[PDOAssignEntryCnt]);
            if (pPDO != NULL)
            {
                PDOSubindex0 = *((UINT16 *) pPDO->pVarPtr);
                for (PDOEntryCnt = 0; PDOEntryCnt < PDOSubindex0; PDOEntryCnt++)
                {
                    pPDOEntry = (UINT32 *) ((UINT8 *) pPDO->pVarPtr +
                                            (OBJ_GetEntryOffset((PDOEntryCnt + 1), pPDO) >> 3));    //goto PDO entry
                    // we increment the expected output size depending on the mapped Entry
                    InputSize += (UINT16) ((*pPDOEntry) & 0xFF);
                }
            }
            else
            {
                /*assigned PDO was not found in object dictionary. return invalid mapping*/
                InputSize = 0;
                result = ALSTATUSCODE_INVALIDINPUTMAPPING;
                break;
            }
        }
    }
    InputSize = (InputSize + 7) >> 3;

    *pInputSize = InputSize;
    *pOutputSize = OutputSize;
    return result;

}


/////////////////////////////////////////////////////////////////////////////////////////
/**
\param      pData  pointer to input process data
\brief      This function will copies the inputs from the local memory to the ESC memory
            to the hardware
*////////////////////////////////////////////////////////////////////////////////////////
void APPL_InputMapping(UINT16 *pData)
{
    UINT16 j = 0;
    UINT16 *pTmpData = (UINT16 *) pData;

    /* we go through all entries of the TxPDO Assign object to get the assigned TxPDOs */
    for (j = 0; j < sTxPDOassign.u16SubIndex0; j++)
    {
        switch (sTxPDOassign.aEntries[j])
        {
            /* TxPDO 1 */
            case 0x1A00:
//                *pTmpData++ = SWAPWORD(((UINT16 * ) & sDIInputs)[1]);
////                printf("*pTmpData++ = %x \r\n",*pTm/pData);
//                *pTmpData++ = SWAPWORD(((UINT16 * ) & sDIInputs)[2]);
////                printf("*pTmpData++ = %x \r\n",*pTmpData);
//                *pTmpData++ = SWAPWORD(((UINT16 * ) & sDIInputs)[3]);
//                *pTmpData++ = SWAPWORD(((UINT16 * ) & sDIInputs)[4]);
//                *pTmpData++ = SWAPWORD(((UINT16 * ) & sDIInputs)[5]);
//                *pTmpData++ = SWAPWORD(((UINT16 * ) & sDIInputs)[6]);
//                *pTmpData++ = SWAPWORD(((UINT16 * ) & sDIInputs)[7]);
//                *pTmpData++ = SWAPWORD(((UINT16 * ) & sDIInputs)[8]);
//                *pTmpData++ = SWAPWORD(((UINT16 * ) & sDIInputs)[9]);
//                *pTmpData++ = SWAPWORD(((UINT16 * ) & sDIInputs)[10]);
//                *pTmpData++ = SWAPWORD(((UINT16 * ) & sDIInputs)[11]);
//                *pTmpData++ = SWAPWORD(((UINT16 * ) & sDIInputs)[12]);
//                *pTmpData++ = SWAPWORD(((UINT16 * ) & sDIInputs)[13]);
                for (int i = 1; i < 105; i++)
                {
                    *pTmpData++ = SWAPWORD(((UINT16 * ) & sDIInputs)[i]);


                }

//            printf("case 0x1A00 \r\n");
                break;
                /* TxPDO 3 */
//            case 0x1A02:
//                *pTmpData++ = SWAPWORD(((UINT16 * ) & sAIInputs)[1]);
//                *pTmpData++ = SWAPWORD(((UINT16 * ) & sAIInputs)[2]);
////                printf("case 0x1A02\r\n");
//                break;
        }
    }
}

/////////////////////////////////////////////////////////////////////////////////////////
/**
\param      pData  pointer to output process data

\brief    This function will copies the outputs from the ESC memory to the local memory
            to the hardware
*////////////////////////////////////////////////////////////////////////////////////////
void APPL_OutputMapping(UINT16 *pData)
{
    UINT16 j = 0;
    UINT16 *pTmpData = (UINT16 *) pData;

    /* we go through all entries of the RxPDO Assign object to get the assigned RxPDOs */
    for (j = 0; j < sRxPDOassign.u16SubIndex0; j++)
    {
        switch (sRxPDOassign.aEntries[j])
        {
            /* RxPDO 2 */
            case 0x1601:
                for (int i = 1; i < 105; i++)
                {
                    ((UINT16 *) &sDOOutputs)[i] = SWAPWORD(*pTmpData++);
                }
//                ((UINT16 *) &sDOOutputs)[1] = SWAPWORD(*pTmpData++);
//                ((UINT16 *) &sDOOutputs)[2] = SWAPWORD(*pTmpData++);
//                ((UINT16 *) &sDOOutputs)[3] = SWAPWORD(*pTmpData++);
//                ((UINT16 *) &sDOOutputs)[4] = SWAPWORD(*pTmpData++);
//                printf("sDOOutputs.u16SubIndex0 =0x%x\r\n",sDOOutputs.u16SubIndex0);
                break;
        }
    }
}

/////////////////////////////////////////////////////////////////////////////////////////
/**
\brief    This function will called from the synchronisation ISR 
            or from the mainloop if no synchronisation is supported
*////////////////////////////////////////////////////////////////////////////////////////
void APPL_Application(void)
{
//    printf("APP data change \r\n  ");
    UINT32 uhADCxConvertedValue;

//    memccpy(((UINT16 * ) & sDIInputs)[1],tempBuffer,104);
//    sDIInputs.InfoIn[0] = 0x0001;
//    sDIInputs.InfoIn[1] = 0x0002;
//    sDIInputs.InfoIn[2] = 0x0003;
//    sDIInputs.InfoIn[3] = 0x0004;
//    sDIInputs.InfoIn[4] = 0x0005;
//    sDIInputs.InfoIn[5] = 0x0006;
//    sDIInputs.InfoIn[6] = 0x0007;
//    sDIInputs.InfoIn[7] = 0x0008;
//    sDIInputs.DataIn1[1] = 0x1fff;
//    sDIInputs.DataIn12[4] = 0x205;
//    sDIInputs.DataIn12[5] = 0x206;
//    sDIInputs.DataIn12[6] = 0x207;
//    sDIInputs.DataIn12[7] = 0x208;


    UINT16 *InfoIn1PTR = sDIInputs.InfoIn;
    UINT16 *DataIn1PTR = sDIInputs.DataIn1;
    UINT16 *DataIn2PTR = sDIInputs.DataIn2;
    UINT16 *DataIn3PTR = sDIInputs.DataIn3;
    UINT16 *DataIn4PTR = sDIInputs.DataIn4;
    UINT16 *DataIn5PTR = sDIInputs.DataIn5;
    UINT16 *DataIn6PTR = sDIInputs.DataIn6;
    UINT16 *DataIn7PTR = sDIInputs.DataIn7;
    UINT16 *DataIn8PTR = sDIInputs.DataIn8;
    UINT16 *DataIn9PTR = sDIInputs.DataIn9;
    UINT16 *DataIn10PTR = sDIInputs.DataIn10;
    UINT16 *DataIn11PTR = sDIInputs.DataIn11;
    UINT16 *DataIn12PTR = sDIInputs.DataIn12;

    UINT16 *InfoOutPTR = sDOOutputs.InfoOut;
    UINT16 *DataOut1PTR = sDOOutputs.DataOut1;
    UINT16 *DataOut2PTR = sDOOutputs.DataOut2;
    UINT16 *DataOut3PTR = sDOOutputs.DataOut3;
    UINT16 *DataOut4PTR = sDOOutputs.DataOut4;
    UINT16 *DataOut5PTR = sDOOutputs.DataOut5;
    UINT16 *DataOut6PTR = sDOOutputs.DataOut6;
    UINT16 *DataOut7PTR = sDOOutputs.DataOut7;
    UINT16 *DataOut8PTR = sDOOutputs.DataOut8;
    UINT16 *DataOut9PTR = sDOOutputs.DataOut9;
    UINT16 *DataOut10PTR = sDOOutputs.DataOut10;
    UINT16 *DataOut11PTR = sDOOutputs.DataOut11;
    UINT16 *DataOut12PTR = sDOOutputs.DataOut12;


    memcpy(InfoIn1PTR, InfoOutPTR, sizeof(UINT16) * 8);
    memcpy(DataIn1PTR, DataOut1PTR, sizeof(UINT16) * 8);
    memcpy(DataIn2PTR, DataOut2PTR, sizeof(UINT16) * 8);
    memcpy(DataIn3PTR, DataOut3PTR, sizeof(UINT16) * 8);
    memcpy(DataIn4PTR, DataOut4PTR, sizeof(UINT16) * 8);
    memcpy(DataIn5PTR, DataOut5PTR, sizeof(UINT16) * 8);
    memcpy(DataIn6PTR, DataOut6PTR, sizeof(UINT16) * 8);
    memcpy(DataIn7PTR, DataOut7PTR, sizeof(UINT16) * 8);
    memcpy(DataIn8PTR, DataOut8PTR, sizeof(UINT16) * 8);
    memcpy(DataIn9PTR, DataOut9PTR, sizeof(UINT16) * 8);
    memcpy(DataIn10PTR, DataOut10PTR, sizeof(UINT16) * 8);
    memcpy(DataIn11PTR, DataOut11PTR, sizeof(UINT16) * 8);
    memcpy(DataIn12PTR, DataOut12PTR, sizeof(UINT16) * 8);

//    printf("InfoIN contents: \r\n");
//    for (int i = 0; i < 8; i++) {
//        printf("%x\r\n", sDIInputs.InfoIn[i]);
//    }
//    printf("InfoOut contents: \r\n");
//    for (int i = 0; i < 8; i++) {
//        printf("%x\r\n", sDOOutputs.InfoOut[i]);
//    }
}

/////////////////////////////////////////////////////////////////////////////////////////
/**
 \param     index               index of the requested object.
 \param     subindex            subindex of the requested object.
 \param     objSize             size of the requested object data, calculated with OBJ_GetObjectLength
 \param     pData               Pointer to the buffer where the data can be copied to
 \param     bCompleteAccess     Indicates if a complete read of all subindices of the
                                object shall be done or not

 \return    ABORTIDX_XXX

 \brief     Handles SDO read requests to TxPDO Parameter
*////////////////////////////////////////////////////////////////////////////////////////
UINT8 ReadObject0x1802(UINT16 index, UINT8 subindex, UINT32 dataSize, UINT16 MBXMEM *pData, UINT8 bCompleteAccess)
{

    if (bCompleteAccess)
        return ABORTIDX_UNSUPPORTED_ACCESS;

    if (subindex == 0)
    {
        *pData = TxPDO1802Subindex0;
    }
    else if (subindex == 6)
    {
        /*clear destination buffer (no excluded TxPDO set)*/
        if (dataSize > 0)
            MBXMEMSET(pData, 0x00, dataSize);
    }
    else if (subindex == 7)
    {
        /*min size is one Byte*/
        UINT8 *pu8Data = (UINT8 *) pData;

        //Reset Buffer
        *pu8Data = 0;

        //*pu8Data = sAIInputs.bTxPDOState;
    }
    else if (subindex == 9)
    {
        /*min size is one Byte*/
        UINT8 *pu8Data = (UINT8 *) pData;

        //Reset Buffer
        *pu8Data = 0;

        //*pu8Data = sAIInputs.bTxPDOToggle;
    }
    else
        return ABORTIDX_SUBINDEX_NOT_EXISTING;

    return 0;
}


/////////////////////////////////////////////////////////////////////////////////////////
/**

 \brief    This is the main function

*////////////////////////////////////////////////////////////////////////////////////////
int ECT_main(void)
{
    /* initialize the Hardware and the EtherCAT Slave Controller */
//    HW_Init();
    MainInit();


    HW_Release();
    return 0;
}
uint16_t GPIO_ChangeAuto(void)
{
    uint16_t bitstatus;
    uint32_t curtime = HAL_GetTick();
    bitstatus = (uint16_t) (curtime % 3);
//    printf("bitstatus = %d\r\n",bitstatus);
    return bitstatus;
}
