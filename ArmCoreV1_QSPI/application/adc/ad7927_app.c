#include "ad7927_app.h"
#include "main_app.h"
#include "jaw_drv.h"
#include "jaw_control.h"
#include "planData.h"

static const uint8_t powerErrorBit[POWER_CHANNEL_NUM] = {0x8, 0x10, 0x1, 0x2, 0x4};

bool powerManage(uint8_t ch, uint16_t value)
{
    //Vref = 5V
    static uint8_t powerErrorCnt[POWER_CHANNEL_NUM] = {0};
    double theoryVoltage;
    double actualDiffRatio, Diff;
    uint8_t index;
   
    switch(ch)
    {
        case 0:
        case 2:
        case 3:
            index = ch;
            theoryVoltage = 3.75;
            break;
        case 1:
            index = ch;
            theoryVoltage = 4.0;
        break;
        case 7:
            index = 4;
            theoryVoltage = 3.75;
            break;
        default:
            printf("Invalid power channel\r\n");
            return 0;
    }

    double sampleVoltage = (double)value/4096*5.0;
    Diff = theoryVoltage - sampleVoltage;
    actualDiffRatio = fabs(Diff)/theoryVoltage;
    if(actualDiffRatio > HEALTHY_DIFF_RATIO)
    {
       // printf("channel %d power is too %s %fV\r\n", ch, Diff<1e-6?"high":"low", sampleVoltage);
        if(++powerErrorCnt[index] >= 3){//always error in 30s
            interlockFeedback.powerInterlock |= powerErrorBit[index];
            powerErrorCnt[index] = 0;
        }   
        return 0;
    }
    else    powerErrorCnt[index] = 0;

   // printf("adc:%d %d\r\n", ch, value);
    return 1;
}

void calculateDualChannelDiff(uint8_t ch, uint16_t value)
{
  //  uint16_t convertEnc;

    switch(ch)
    {
        case CARRIER_SECOND_FEEDBACK_CH: 
            secondPosFeedback.carrierSecondPos = value;   
            break;
        case YJAW_SECOND_FEEDBACK_CH:
            secondPosFeedback.jawSecondPos[Y] = value;
            break;
        case XJAW_SECOND_FEEDBACK_CH:   
            secondPosFeedback.jawSecondPos[X] = value;   
            break;
        default:    
            printf("Invalid second feedback channel\r\n");  
            return;
    }
#if 0
    if(value > jawParameterByAxes[axes].jawMaxADSetting)   interlockFeedback.jawInterlock[axes] |= 0x100;
    if(value > MAX_ADC_VALUE)   interlockFeedback.jawInterlock[axes] |= 0x20;
    convertEnc = jawParameterByAxes[axes].jaw2ndEncCalibrationPK*value
        +jawParameterByAxes[axes].jaw2ndEncCalibrationPB;
            
    if(abs(convertEnc - rtFeedback.jawRTPos[axes]) > jawParameterByAxes[axes].jawDualChTolerance){
        //    printf("Y Jaw Encoder untrue\r\n");
        interlockFeedback.jawInterlock[axes] |= 0x40;
    }
#endif
}

void ADCProcessTask(void *argument)
{
     uint8_t ch456 = CARRIER_SECOND_FEEDBACK_CH;
     uint32_t loopCnt = 0;
     uint8_t wch = 0, rch = ADC_CHANNEL_NUM;
     uint16_t adc = 0;

    AD7927_Init(&hspi4);
    osDelay(1000);
  /* Infinite loop */

  for(;;)
  {
    switch(loopCnt++ % 5000){//read power value every 10s
        case 0: wch = 0;    break;
        case 1: wch = 1;    break;
        case 2: wch = 2;    break;
        case 3: wch = 3;    break;
        case 4: wch = 7;    break;
        default:    wch = ch456;
            if(++ch456 > XJAW_SECOND_FEEDBACK_CH) ch456 = CARRIER_SECOND_FEEDBACK_CH; 
            break;
    }
    adc = ADCgetValue(&hspi4, wch);
    
    switch(rch)
    {
        case 0:
        case 1:
        case 2:
        case 3:
        case 7:
            powerManage(rch, adc);
            break;
        case 4:
        case 5:
        case 6:
           // if(rch == 5)    printf("4:%d\r\n",adc);
            calculateDualChannelDiff(rch, adc);
            break;
        default: break;
    }
 //   printf("%d %d\r\n",wch,rch);
    rch = wch;
   
    osDelay(2);
  }
}

static int8_t adc_thread_init(void)
{
    osThreadAttr_t adc_attributes = {
    .name = "ADC_value_process_thread",
    .stack_size = 512 * 4,
    .priority = (osPriority_t) osPriorityNormal,
    };

    osThreadId_t ADCHandle = osThreadNew(ADCProcessTask, NULL, &adc_attributes);
    if (ADCHandle == NULL)
    {
        printf("thread adc process create failed\r\n");
        return -1;
    }

    return 0;
}
INIT_APP_EXPORT(adc_thread_init);