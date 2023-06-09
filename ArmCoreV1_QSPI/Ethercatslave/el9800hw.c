/**
\addtogroup EL9800_HW EL9800 Platform (Serial ESC Access)
@{
*/

/**
\file    el9800hw.c
\author EthercatSSC@beckhoff.com
\brief Implementation
Hardware access implementation for EL9800 onboard PIC18/PIC24 connected via SPI to ESC

\version 5.11

<br>Changes to version V5.10:<br>
V5.11 ECAT10: change PROTO handling to prevent compiler errors<br>
V5.11 EL9800 2: change PDI access test to 32Bit ESC access and reset AL Event mask after test even if AL Event is not enabled<br>
<br>Changes to version V5.01:<br>
V5.10 ESC5: Add missing swapping<br>
V5.10 HW3: Sync1 Isr added<br>
V5.10 HW4: Add volatile directive for direct ESC DWORD/WORD/BYTE access<br>
           Add missing swapping in mcihw.c<br>
           Add "volatile" directive vor dummy variables in enable and disable SyncManger functions<br>
           Add missing swapping in EL9800hw files<br>
<br>Changes to version V5.0:<br>
V5.01 HW1: Invalid ESC access function was used<br>
<br>Changes to version V4.40:<br>
V5.0 ESC4: Save SM disable/Enable. Operation may be pending due to frame handling.<br>
<br>Changes to version V4.30:<br>
V4.40 : File renamed from spihw.c to el9800hw.c<br>
<br>Changes to version V4.20:<br>
V4.30 ESM: if mailbox Syncmanger is disabled and bMbxRunning is true the SyncManger settings need to be revalidate<br>
V4.30 EL9800: EL9800_x hardware initialization is moved to el9800.c<br>
V4.30 SYNC: change synchronisation control function. Add usage of 0x1C32:12 [SM missed counter].<br>
Calculate bus cycle time (0x1C32:02 ; 0x1C33:02) CalcSMCycleTime()<br>
V4.30 PDO: rename PDO specific functions (COE_xxMapping -> PDO_xxMapping and COE_Application -> ECAT_Application)<br>
V4.30 ESC: change requested address in GetInterruptRegister() to prevent acknowledge events.<br>
(e.g. reading an SM config register acknowledge SM change event)<br>
GENERIC: renamed several variables to identify used SPI if multiple interfaces are available<br>
V4.20 MBX 1: Add Mailbox queue support<br>
V4.20 SPI 1: include SPI RxBuffer dummy read<br>
V4.20 DC 1: Add Sync0 Handling<br>
V4.20 PIC24: Add EL9800_4 (PIC24) required source code<br>
V4.08 ECAT 3: The AlStatusCode is changed as parameter of the function AL_ControlInd<br>
<br>Changes to version V4.02:<br>
V4.03 SPI 1: In ISR_GetInterruptRegister the NOP-command should be used.<br>
<br>Changes to version V4.01:<br>
V4.02 SPI 1: In HW_OutputMapping the variable u16OldTimer shall not be set,<br>
             otherwise the watchdog might exceed too early.<br>
<br>Changes to version V4.00:<br>
V4.01 SPI 1: DI and DO were changed (DI is now an input for the uC, DO is now an output for the uC)<br>
V4.01 SPI 2: The SPI has to operate with Late-Sample = FALSE on the Eva-Board<br>
<br>Changes to version V3.20:<br>
V4.00 ECAT 1: The handling of the Sync Manager Parameter was included according to<br>
              the EtherCAT Guidelines and Protocol Enhancements Specification<br>
V4.00 APPL 1: The watchdog checking should be done by a microcontroller<br>
                 timer because the watchdog trigger of the ESC will be reset too<br>
                 if only a part of the sync manager data is written<br>
V4.00 APPL 4: The EEPROM access through the ESC is added

*/
#include "gpio.h"
#include "tim.h"
#include "octospi.h"
#include "SPIDriver.h"
#include "main.h"/*--------------------------------------------------------------------------------------
------
------    Includes
------
--------------------------------------------------------------------------------------*/
#include "ecat_def.h"

#include "ecatslv.h"

#define    _EL9800HW_ 1

#include "el9800hw.h"

#undef    _EL9800HW_
/* ECATCHANGE_START(V5.11) ECAT10*/
/*remove definition of _EL9800HW_ (#ifdef is used in el9800hw.h)*/
/* ECATCHANGE_END(V5.11) ECAT10*/
#include "stdio.h"
#include "ecatappl.h"
#define SPI_Connect 1//�궨��ON--->�ӿ�ģʽѡ�� SPI
//�궨��OFF-->�ӿ�ģʽѡ�� 16bit_HBI

/*--------------------------------------------------------------------------------------
------
------    internal Types and Defines
------
--------------------------------------------------------------------------------------*/

typedef union
{
    unsigned short Word;
    unsigned char Byte[2];
} UBYTETOWORD;

typedef union
{
    UINT8 Byte[2];
    UINT16 Word;
}
        UALEVENT;

/*-----------------------------------------------------------------------------------------
------
------    SPI defines/macros
------
-----------------------------------------------------------------------------------------*/

//#define SPI1_SEL                        _LATB2
//#define SPI1_IF                            _SPI1IF
//#define SPI1_BUF                        SPI1BUF
//#define SPI1_CON1                        SPI1CON1
//#define SPI1_STAT                        SPI1STAT
//#define    WAIT_SPI_IF                        while( !SPI1_IF);
//#define    SELECT_SPI                        {(SPI1_SEL) = (SPI_ACTIVE);}
//#define    DESELECT_SPI                    {(SPI1_SEL) = (SPI_DEACTIVE);}
//#define    INIT_SSPIF                        {(SPI1_IF)=0;}
//#define SPI1_STAT_VALUE                    0x8000
//#define SPI1_CON1_VALUE                    0x027E
//#define SPI1_CON1_VALUE_16BIT            0x047E
//#define SPI_DEACTIVE                    1
//#define SPI_ACTIVE                        0


/*-----------------------------------------------------------------------------------------
------
------    Global Interrupt setting
------
-----------------------------------------------------------------------------------------*/
#define DISABLE_GLOBAL_INT                      {__set_PRIMASK(1);} //__disable_irq() �����ʹ�������������ֹ�Ϳ���ȫ���жϣ����벻��preop����ʱ�����Ϊʲô
#define ENABLE_GLOBAL_INT                    {__set_PRIMASK(0);} //__enable_irq()
//#define DISABLE_GLOBAL_INT                      Disable_all_int(); //__disable_irq()
//#define ENABLE_GLOBAL_INT                    Enable_all_int(); //__enable_irq()
#define    DISABLE_AL_EVENT_INT          DISABLE_GLOBAL_INT
#define    ENABLE_AL_EVENT_INT           ENABLE_GLOBAL_INT


/*-----------------------------------------------------------------------------------------
------
------    ESC Interrupt
------
-----------------------------------------------------------------------------------------*/


//#define    INIT_ESC_INT               MX_GPIO_Init();//Init_ESC();//EXTI0_Configuration(); //_INT1EP = 1:  negative edge ; _INT1IP = 1; //highest priority
#define    INIT_ESC_INT               {Init_ESC();}//EXTI0_Configuration(); //_INT1EP = 1:  negative edge ; _INT1IP = 1; //highest priority
#define    ESC_INT_REQ                (_INT1IF) //ESC Interrupt (INT1) state
#define    INT_EL                    (_RD8) //ESC Interrupt input port
#define    EscIsr                    EXTI3_IRQHandler // primary interrupt vector name//changed by yh
#define    ACK_ESC_INT               {__HAL_GPIO_EXTI_CLEAR_IT(GPIO_PIN_3);}//changed by yh




/*-----------------------------------------------------------------------------------------
------
------    SYNC0 Interrupt
------
-----------------------------------------------------------------------------------------*/

//#define    INIT_SYNC0_INT                   MX_GPIO_Init();//Init_SYNC0();//EXTI1_Configuration();//_INT3EP = 1:  negative edge ; _INT3IP = 1; //highest priority
#define    INIT_SYNC0_INT                   Init_SYNC0();//EXTI1_Configuration();//_INT3EP = 1:  negative edge ; _INT3IP = 1; //highest priority
#define    SYNC0_INT_REQ                    (_INT3IF) //Sync0 Interrupt (INT3) state
#define    INT_SYNC0                        (_RD10) //Sync1 Interrupt input port
#define    Sync0Isr                        EXTI15_10_IRQHandler// primary interrupt vector name//changed by yh
#define    DISABLE_SYNC0_INT               HAL_NVIC_DisableIRQ(EXTI15_10_IRQn);//disable interrupt source INT3//changed by yh
#define    ENABLE_SYNC0_INT                HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);     //enable interrupt source INT3//changed by yh
#define    ACK_SYNC0_INT                   __HAL_GPIO_EXTI_CLEAR_IT(GPIO_PIN_13);//changed by yh
#define    SET_SYNC0_INT                    {(SYNC0_INT_REQ) = 1;}
#define    SYNC0_INT_PORT_IS_ACTIVE        {(INT_EL) == 0;}


//sync1 not used
//#define    INIT_SYNC1_INT                   MX_GPIO_Init();//Init_SYNC1();//EXTI2_Configuration();//_INT4EP = 1:  negative edge ; _INT4IP = 1; //highest priority
#define    INIT_SYNC1_INT                   Init_SYNC1();//EXTI2_Configuration();//_INT4EP = 1:  negative edge ; _INT4IP = 1; //highest priority
#define    SYNC1_INT_REQ                    (_INT4IF) //Sync1 Interrupt (INT4) state
#define    INT_SYNC1                        (_RD11) //Sync1 Interrupt input port
#define    Sync1Isr                        EXTI2_IRQHandler // primary interrupt vector name//changed by yh
#define    DISABLE_SYNC1_INT               HAL_NVIC_DisableIRQ(EXTI15_10_IRQn);//disable interrupt source INT4//changed by yh
#define    ENABLE_SYNC1_INT                HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);  //enable interrupt source INT4 //changed by yh
#define    ACK_SYNC1_INT                   __HAL_GPIO_EXTI_CLEAR_IT(GPIO_PIN_2);//changed by yh
#define    SET_SYNC1_INT                    {(SYNC1_INT_REQ) = 1;}
#define    SYNC1_INT_PORT_IS_ACTIVE        {(INT_EL) == 0;}


/*-----------------------------------------------------------------------------------------
------
------    Hardware timer
------
-----------------------------------------------------------------------------------------*/


#define ECAT_TIMER_INT_STATE
#define ECAT_TIMER_ACK_INT             __HAL_TIM_CLEAR_IT(&htim2, TIM_IT_UPDATE)
#define    TimerIsr                    TIM2_IRQHandler //changed by yh
#define    ENABLE_ECAT_TIMER_INT       {HAL_NVIC_EnableIRQ(TIM2_IRQn) ;HAL_TIM_Base_Start_IT(&htim2);}
#define    DISABLE_ECAT_TIMER_INT      {HAL_NVIC_DisableIRQ(TIM2_IRQn);HAL_TIM_Base_Stop_IT(&htim2);}
//desired period 1ms (625 counts at 40 MHz and prescale 1:64)
#define INIT_ECAT_TIMER                MX_TIM2_Init();
#define STOP_ECAT_TIMER                {DISABLE_ECAT_TIMER_INT;/*disable timer interrupt*/  /*disable timer*/}

#define START_ECAT_TIMER            {ENABLE_ECAT_TIMER_INT; /*enable timer interrupt*/  /*enable timer*/}

/*-----------------------------------------------------------------------------------------
------
------    Configuration Bits
------
-----------------------------------------------------------------------------------------*/
//_FGS(GCP_OFF);
//_FOSCSEL(FNOSC_PRI & IESO_OFF);
//_FOSC(FCKSM_CSECMD & OSCIOFNC_ON & POSCMD_HS);
//_FWDT(FWDTEN_OFF);
//_FPOR(FPWRT_PWR1);
//_FICD(ICS_PGD3 & JTAGEN_OFF);

/*-----------------------------------------------------------------------------------------
------
------    LED defines
------
-----------------------------------------------------------------------------------------*/
// EtherCAT Status LEDs -> StateMachine
#define LED_ECATGREEN
#define LED_ECATRED

/*--------------------------------------------------------------------------------------
------
------    internal Variables
------
--------------------------------------------------------------------------------------*/
UALEVENT EscALEvent;            //contains the content of the ALEvent register (0x220), this variable is updated on each Access to the Esc

/*--------------------------------------------------------------------------------------
------
------    internal functions
------
--------------------------------------------------------------------------------------*/

/////////////////////////////////////////////////////////////////////////////////////////
/**
 \brief  The function operates a SPI access without addressing.

        The first two bytes of an access to the EtherCAT ASIC always deliver the AL_Event register (0x220).
        It will be saved in the global "EscALEvent"
*////////////////////////////////////////////////////////////////////////////////////////
static void GetInterruptRegister(void)
{
    DISABLE_AL_EVENT_INT;
    HW_EscReadIsr((MEM_ADDR *) &EscALEvent.Word, 0x220, 2);
    ENABLE_AL_EVENT_INT;

}

/////////////////////////////////////////////////////////////////////////////////////////
/**
 \brief  The function operates a SPI access without addressing.
        Shall be implemented if interrupts are supported else this function is equal to "GetInterruptRegsiter()"

        The first two bytes of an access to the EtherCAT ASIC always deliver the AL_Event register (0x220).
        It will be saved in the global "EscALEvent"
*////////////////////////////////////////////////////////////////////////////////////////
static void ISR_GetInterruptRegister(void)
{
    HW_EscReadIsr((MEM_ADDR *) &EscALEvent.Word, 0x220, 2);
}

///////////////////////////////////////////////////////////////////////////////////////////
///**
// \param Address     EtherCAT ASIC address ( upper limit is 0x1FFF )    for access.
// \param Command    ESC_WR performs a write access; ESC_RD performs a read access.

// \brief The function addresses the EtherCAT ASIC via SPI for a following SPI access.
//*////////////////////////////////////////////////////////////////////////////////////////
//static void AddressingEsc( UINT16 Address, UINT8 Command )
//{
//    UBYTETOWORD tmp;
//    VARVOLATILE UINT8 dummy;
//    tmp.Word = ( Address << 3 ) | Command;
//    /* select the SPI */
//    SELECT_SPI;

//    /* reset transmission flag */
//    SPI1_IF=0;
//    dummy = SPI1_BUF;
//    /* there have to be at least 15 ns after the SPI1_SEL signal was active (0) before
//       the transmission shall be started */
//    /* send the first address/command byte to the ESC */
//    SPI1_BUF = tmp.Byte[1];
//    /* wait until the transmission of the byte is finished */
//    WAIT_SPI_IF
//    /* get first byte of AL Event register */

//    EscALEvent.Byte[0] = SPI1_BUF;

//    /* reset transmission flag */
//    SPI1_IF=0;
//    dummy = SPI1_BUF;
//    /* send the second address/command byte to the ESC */
//    SPI1_BUF = tmp.Byte[0];
//    /* wait until the transmission of the byte is finished */
//    WAIT_SPI_IF
//    /* get second byte of AL Event register */

//    EscALEvent.Byte[1] = SPI1_BUF;

//    /* reset transmission flag */
//    SPI1_IF = 0;

//    /* if the SPI transmission rate is higher than 15 MBaud, the Busy detection shall be
//       done here */
//}

///////////////////////////////////////////////////////////////////////////////////////////
///**
// \param Address     EtherCAT ASIC address ( upper limit is 0x1FFF )    for access.
// \param Command    ESC_WR performs a write access; ESC_RD performs a read access.

// \brief The function addresses the EtherCAT ASIC via SPI for a following SPI access.
//        Shall be implemented if interrupts are supported else this function is equal to "AddressingEsc()"
//*////////////////////////////////////////////////////////////////////////////////////////
//static void ISR_AddressingEsc( UINT16 Address, UINT8 Command )
//{
//    VARVOLATILE UINT8 dummy;
//    UBYTETOWORD tmp;
//    tmp.Word = ( Address << 3 ) | Command;

//    /* select the SPI */
//    SELECT_SPI;

//    /* reset transmission flag */
//    SPI1_IF=0;

//    /* there have to be at least 15 ns after the SPI1_SEL signal was active (0) before
//       the transmission shall be started */
//    /* send the first address/command byte to the ESC */
//    SPI1_BUF = tmp.Byte[1];
//    /* wait until the transmission of the byte is finished */
//    WAIT_SPI_IF
//    dummy = SPI1_BUF;
//    /* reset transmission flag */
//    SPI1_IF=0;

//    /* send the second address/command byte to the ESC */
//    SPI1_BUF = tmp.Byte[0];
//    /* wait until the transmission of the byte is finished */
//    WAIT_SPI_IF
//    dummy = SPI1_BUF;

//    /* reset transmission flag */
//    SPI1_IF = 0;

//    /* if the SPI transmission rate is higher than 15 MBaud, the Busy detection shall be
//       done here */
//}

/*--------------------------------------------------------------------------------------
------
------    exported hardware access functions
------
--------------------------------------------------------------------------------------*/


/////////////////////////////////////////////////////////////////////////////////////////
/**
\return     0 if initialization was successful

 \brief    This function intialize the Process Data Interface (PDI) and the host controller.
*////////////////////////////////////////////////////////////////////////////////////////
UINT8 HW_Init(void)
{
    /*ECATCHANGE_START(V5.11) EL9800 2*/
    UINT32 intMask;
    UINT32 data;
    uint32_t readData=0;//test by yh
    uint32_t temp_data = 0xff;
    uint32_t sysclock = 0;
    /*ECATCHANGE_END(V5.11) EL9800 2*/
//    HAL_Init();

//		/* USER CODE BEGIN Init */

//		/* USER CODE END Init */

//		/* Configure the system clock */
//    SystemClock_Config();
//    sysclock = HAL_RCC_GetSysClockFreq();//SYS
//    sysclock = HAL_RCC_GetHCLKFreq();//HCLK
//    sysclock = HAL_RCC_GetPCLK1Freq();
//    sysclock = HAL_RCC_GetPCLK2Freq();
//		MX_GPIO_Init();
//    MX_DMA_Init();
//    MX_SPI5_Init();
//    MX_ADC1_Init();
//		HAL_ADC_Start_DMA(&hadc1,(uint32_t *)&uhADCxConvertedValue,1);
    //MX_FMC_Init();
//		MX_TIM2_Init();	
//		HAL_TIM_Base_Start_IT(&htim2);
//		intMask=SPIReadDWord_test(0x64);
    do {

#ifdef SPI_Connect    //�������ԣ���ȡ����ֵ0x87654321
        readData = 0x55aa55aa;
        readData = SPIReadDWord(0x64);
//        printf("read test byte reg (0x64), readData is  : %0x, \r\n", readData);
        HAL_Delay(1);
    }
        while (!(readData==0x87654321));
#else
#if defined(HBI_INDEXED_16BIT) | defined(HBI_INDEXED_8BIT)
    PMPWriteDWord(0, 0x64);
    intMask = PMPReadDWord(0x64);
#else
    intMask=PMPReadDWord(0x64);
#endif

#endif


/*ECATCHANGE_START(V5.11) EL9800 2*/
    do
    {
        intMask = 0x93;
        HW_EscWriteDWord(intMask, ESC_AL_EVENTMASK_OFFSET);
        intMask = 0;
        HW_EscReadDWord(intMask, ESC_AL_EVENTMASK_OFFSET);
//        printf("read ESC_AL_EVENTMASK (0x204), intMask is  : %0x, \r\n", intMask);
    } while (intMask != 0x93);


    intMask = 0x00;
    HW_EscWriteDWord(intMask, ESC_AL_EVENTMASK_OFFSET);
//    printf("read ESC_AL_EVENTMASK (0x204), intMask is  : %0x, \r\n", intMask);


    //IRQ enable,IRQ polarity, IRQ buffer type in Interrupt Configuration register.
    //Wrte 0x54 - 0x00000101
    data = 0x00000101;
#ifdef SPI_Connect
    SPIWriteDWord (0x54,data);
//    printf("SPIWriteDWord (0x54), data is  : %0x, \r\n", data);

//    temp_data = SPIReadDWord(0x54);
//    printf("SPIReadDWord (0x54), data is  : %0x, \r\n", temp_data);


#else
#if defined(HBI_INDEXED_16BIT) | defined(HBI_INDEXED_8BIT)
    PMPWriteDWord(0, 0x54);
    PMPWriteDWord(4, data);
#else
    PMPWriteDWord(0x54,data);
#endif
#endif
    //Write in Interrupt Enable register -->
    //Write 0x5c - 0x00000001
    data = 0x00000001;
#ifdef SPI_Connect
    SPIWriteDWord (0x5C, data);
//    temp_data = SPIReadDWord(0x5C);
//    printf("SPIReadDWord (0x5C), data is  : %0x, \r\n", temp_data);
//    temp_data = SPIReadDWord(0x58);
//    printf("SPIReadDWord (0x58), data is  : %0x, \r\n", temp_data);
#else
#if defined(HBI_INDEXED_16BIT) | defined(HBI_INDEXED_8BIT)
    PMPWriteDWord(0, 0x5C);
    PMPWriteDWord(4, data);
    PMPReadDWord(0x58);
#else
    PMPWriteDWord(0x5C, data);
    PMPReadDWord(0x58);
#endif

#endif

/*ECATCHANGE_END(V5.11) EL9800 2*/
    INIT_ESC_INT
//    ENABLE_ESC_INT();
    HAL_NVIC_EnableIRQ(EXTI3_IRQn);//changed by yh


    INIT_SYNC0_INT
    INIT_SYNC1_INT

//    ENABLE_SYNC0_INT;
//    ENABLE_SYNC1_INT;
    HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);
//    HAL_NVIC_EnableIRQ(EXTI2_IRQn);

    INIT_ECAT_TIMER
    START_ECAT_TIMER

    /* enable all interrupts */
    ENABLE_GLOBAL_INT

    return 0;
}


/////////////////////////////////////////////////////////////////////////////////////////
/**
 \brief    This function shall be implemented if hardware resources need to be release
        when the sample application stops
*////////////////////////////////////////////////////////////////////////////////////////
void HW_Release(void)
{

}

/////////////////////////////////////////////////////////////////////////////////////////
/**
 \return    first two Bytes of ALEvent register (0x220)

 \brief  This function gets the current content of ALEvent register
*////////////////////////////////////////////////////////////////////////////////////////
UINT16 HW_GetALEventRegister(void)
{
    GetInterruptRegister();
    return EscALEvent.Word;
}

/////////////////////////////////////////////////////////////////////////////////////////
/**
 \return    first two Bytes of ALEvent register (0x220)

 \brief  The SPI PDI requires an extra ESC read access functions from interrupts service routines.
        The behaviour is equal to "HW_GetALEventRegister()"
*////////////////////////////////////////////////////////////////////////////////////////
UINT16 HW_GetALEventRegister_Isr(void)
{
    ISR_GetInterruptRegister();
    return EscALEvent.Word;
}


/////////////////////////////////////////////////////////////////////////////////////////
/**
 \param RunLed            desired EtherCAT Run led state
 \param ErrLed            desired EtherCAT Error led state

  \brief    This function updates the EtherCAT run and error led
*////////////////////////////////////////////////////////////////////////////////////////
void HW_SetLed(UINT8 RunLed, UINT8 ErrLed)
{
//      LED_ECATGREEN = RunLed;
//      LED_ECATRED   = ErrLed;
}
/////////////////////////////////////////////////////////////////////////////////////////
/**
 \param pData        Pointer to a byte array which holds data to write or saves read data.
 \param Address     EtherCAT ASIC address ( upper limit is 0x1FFF )    for access.
 \param Len            Access size in Bytes.

 \brief  This function operates the SPI read access to the EtherCAT ASIC.
*////////////////////////////////////////////////////////////////////////////////////////
void HW_EscRead(MEM_ADDR *pData, UINT16 Address, UINT16 Len)
{
    /* HBu 24.01.06: if the SPI will be read by an interrupt routine too the
                     mailbox reading may be interrupted but an interrupted
                     reading will remain in a SPI transmission fault that will
                     reset the internal Sync Manager status. Therefore the reading
                     will be divided in 1-byte reads with disabled interrupt */
    UINT16 i;
    UINT8 *pTmpData = (UINT8 *) pData;

    /* loop for all bytes to be read */
    while (Len > 0)
    {
        if (Address >= 0x1000)
        {
            i = Len;
        }
        else
        {
            i = (Len > 4) ? 4 : Len;

            if (Address & 01)
            {
                i = 1;
            }
            else if (Address & 02)
            {
                i = (i & 1) ? 1 : 2;
            }
            else if (i == 03)
            {
                i = 1;
            }
        }

        DISABLE_AL_EVENT_INT;
//       SPIReadDRegister(pTmpData,Address,i);
//       PMPReadDRegister(pTmpData,Address,i);
#ifdef SPI_Connect
        SPIReadDRegister(pTmpData, Address,i);
#else
        PMPReadDRegister(pTmpData, Address, i);
#endif

        ENABLE_AL_EVENT_INT;

        Len -= i;
        pTmpData += i;
        Address += i;
    }


}

/////////////////////////////////////////////////////////////////////////////////////////
/**
 \param pData        Pointer to a byte array which holds data to write or saves read data.
 \param Address     EtherCAT ASIC address ( upper limit is 0x1FFF )    for access.
 \param Len            Access size in Bytes.

\brief  The SPI PDI requires an extra ESC read access functions from interrupts service routines.
        The behaviour is equal to "HW_EscRead()"
*////////////////////////////////////////////////////////////////////////////////////////
void HW_EscReadIsr(MEM_ADDR *pData, UINT16 Address, UINT16 Len)
{

    UINT16 i;
    UINT8 *pTmpData = (UINT8 *) pData;

    /* send the address and command to the ESC */

    /* loop for all bytes to be read */
    while (Len > 0)
    {

        if (Address >= 0x1000)
        {
            i = Len;
        }
        else
        {
            i = (Len > 4) ? 4 : Len;

            if (Address & 01)
            {
                i = 1;
            }
            else if (Address & 02)
            {
                i = (i & 1) ? 1 : 2;
            }
            else if (i == 03)
            {
                i = 1;
            }
        }

#ifdef SPI_Connect
        SPIReadDRegister(pTmpData, Address,i);
#else
        PMPReadDRegister(pTmpData, Address, i);
#endif
        Len -= i;
        pTmpData += i;
        Address += i;
    }

}
/////////////////////////////////////////////////////////////////////////////////////////
/**
 \param pData        Pointer to a byte array which holds data to write or saves write data.
 \param Address     EtherCAT ASIC address ( upper limit is 0x1FFF )    for access.
 \param Len            Access size in Bytes.

  \brief  This function operates the SPI write access to the EtherCAT ASIC.
*////////////////////////////////////////////////////////////////////////////////////////
void HW_EscWrite(MEM_ADDR *pData, UINT16 Address, UINT16 Len)
{

    UINT16 i;
    UINT8 *pTmpData = (UINT8 *) pData;

    /* loop for all bytes to be written */
    while (Len)
    {

        if (Address >= 0x1000)
        {
            i = Len;
        }
        else
        {
            i = (Len > 4) ? 4 : Len;

            if (Address & 01)
            {
                i = 1;
            }
            else if (Address & 02)
            {
                i = (i & 1) ? 1 : 2;
            }
            else if (i == 03)
            {
                i = 1;
            }
        }
//        printf("HW_EscWrite\r\n");
        DISABLE_AL_EVENT_INT;

        /* start transmission */
//        SPIWriteRegister(pTmpData, Address, i);
//        PMPWriteRegister(pTmpData, Address, i);
#ifdef SPI_Connect
        SPIWriteRegister(pTmpData, Address,i);
#else
        PMPWriteRegister(pTmpData, Address, i);
#endif

        ENABLE_AL_EVENT_INT;



        /* next address */
        Len -= i;
        pTmpData += i;
        Address += i;

    }

}

/////////////////////////////////////////////////////////////////////////////////////////
/**
 \param pData        Pointer to a byte array which holds data to write or saves write data.
 \param Address     EtherCAT ASIC address ( upper limit is 0x1FFF )    for access.
 \param Len            Access size in Bytes.

 \brief  The SPI PDI requires an extra ESC write access functions from interrupts service routines.
        The behaviour is equal to "HW_EscWrite()"
*////////////////////////////////////////////////////////////////////////////////////////
void HW_EscWriteIsr(MEM_ADDR *pData, UINT16 Address, UINT16 Len)
{

    UINT16 i;
    UINT8 *pTmpData = (UINT8 *) pData;


    /* loop for all bytes to be written */
    while (Len)
    {

        if (Address >= 0x1000)
        {
            i = Len;
//            printf("HW_EscWriteIsr\r\n");
        }
        else
        {
            i = (Len > 4) ? 4 : Len;

            if (Address & 01)
            {
                i = 1;
            }
            else if (Address & 02)
            {
                i = (i & 1) ? 1 : 2;
            }
            else if (i == 03)
            {
                i = 1;
            }
        }

        /* start transmission */

//       SPIWriteRegister(pTmpData, Address, i);
//       PMPWriteRegister(pTmpData, Address, i);
#ifdef SPI_Connect
        SPIWriteRegister(pTmpData, Address,i);
#else
        PMPWriteRegister(pTmpData, Address, i);
#endif
        /* next address */
        Len -= i;
        pTmpData += i;
        Address += i;
    }

}



/////////////////////////////////////////////////////////////////////////////////////////
/**
 \brief    Interrupt service routine for the PDI interrupt from the EtherCAT Slave Controller
*////////////////////////////////////////////////////////////////////////////////////////

void  EscIsr(void)
{
//    printf("ESCIsr in \r\n");//test by yh
    HAL_GPIO_TogglePin(GPIOG, GPIO_PIN_10);
    HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_3);

    PDI_Isr();
    /* reset the interrupt flag */
    ACK_ESC_INT;
//    printf("ESCIsr exit \r\n");//test by yh
}



/////////////////////////////////////////////////////////////////////////////////////////
/**
 \brief    Interrupt service routine for the interrupts from SYNC0
*////////////////////////////////////////////////////////////////////////////////////////
void Sync0Isr(void)
{
//    printf("Sync0Isr in \r\n");//test by yh
//    DISABLE_ESC_INT();
    HAL_NVIC_DisableIRQ(EXTI3_IRQn);//changed by yh
    HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_13);

    Sync0_Isr();
    /* reset the interrupt flag */
    ACK_SYNC0_INT;
//    ENABLE_ESC_INT();
    HAL_NVIC_EnableIRQ(EXTI3_IRQn);//changed by yh
}
/////////////////////////////////////////////////////////////////////////////////////////
/**
 \brief    Interrupt service routine for the interrupts from SYNC1
*////////////////////////////////////////////////////////////////////////////////////////
UINT16 SYSruncon;//���м���
void Sync1Isr(void)
{
    HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_2);
    Sync1_Isr();
    /* reset the interrupt flag */
    ACK_SYNC1_INT;
}

// Timer 7 ISR (0.1ms)
void  TimerIsr(void)
{
    HAL_TIM_IRQHandler(&htim2);
//    printf("SYSruncon , timer INT in ... : %d,%d \r\n ",SYSruncon,uwTick);//test by yh
//    printf("timer isr in and checktimerI() \r \n");
    ECAT_CheckTimer();
    ECAT_TIMER_ACK_INT;
    SYSruncon++;
    if (SYSruncon >= 500)//500ms--ϵͳ����ָʾ��
    {
//		LEDtus=!LEDtus;
        SYSruncon = 0;
//		HAL_GPIO_WritePin(GPIOC,GPIO_PIN_14, LEDtus);
        HAL_GPIO_TogglePin(GPIOG, GPIO_PIN_9);
    }
}

/** @} */



