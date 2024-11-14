#include "ad7927_app.h"
#include "main_app.h"
#include "jaw_drv.h"
#include "jaw_control.h"
#include "planData.h"

#define CARRIER_SECOND_FEEDBACK_CH   4
#define YJAW_SECOND_FEEDBACK_CH   5
#define XJAW_SECOND_FEEDBACK_CH   6
#define HEALTHY_DIFF_RATIO   0.05
#define MAX_ADC_VALUE   4096 //12 bits ADC

bool powerManage(uint8_t ch)
{
    //Vref = 5V
    double theoryVoltage;
    double actualDiffRatio, Diff;

    double sampleVoltage = (double)ADCgetValue(&hspi4, ch)/4096*5.0;
    if(ch == 1) theoryVoltage = 4.0;
    else    theoryVoltage = 3.75;

    Diff = theoryVoltage - sampleVoltage;
    actualDiffRatio = fabs(Diff)/theoryVoltage;
    if(actualDiffRatio > HEALTHY_DIFF_RATIO){
      //  printf("channel %d power is too %s %fV\r\n", ch, Diff<1e-6?"high":"low", sampleVoltage);
        return 0;
    }
    return 1;
}

uint16_t getSecondFeedBack(uint8_t ch) //ch should be 4,5,6
{
    if((ch < 4 ) || (ch > 6)){
        printf("Invalid second feedback channel\r\n");
        return 0;
    }

    return ADCgetValue(&hspi4, ch);
}

void calculateDualChannelDiff(uint8_t ch)
{
    uint16_t adcResult;
    uint16_t convertEnc;

    adcResult = getSecondFeedBack(ch);

    switch(ch)
    {
        case YJAW_SECOND_FEEDBACK_CH:
            if(adcResult > jawParameterByAxes[Y].jawMaxADSetting)   interlockFeedback.jawInterlock[Y] |= 0x100;
            if(adcResult > MAX_ADC_VALUE)   interlockFeedback.jawInterlock[Y] |= 0x20;
            convertEnc = jawParameterByAxes[Y].jaw2ndEncCalibrationPK*adcResult
                            +jawParameterByAxes[Y].jaw2ndEncCalibrationPB;
            
            if(abs(convertEnc - jawFeedbackByAxes[Y].jawCurrentPos) > jawParameterByAxes[Y].jawDualChTolerance){
            //    printf("Y Jaw Encoder untrue\r\n");
                interlockFeedback.jawInterlock[Y] |= 0x40;
            }
            secondPosFeedback.jawSecondPos[Y] = adcResult;
            break;
        case XJAW_SECOND_FEEDBACK_CH:
            if(adcResult > jawParameterByAxes[X].jawMaxADSetting)   interlockFeedback.jawInterlock[X] |= 0x100;
            if(adcResult > MAX_ADC_VALUE)   interlockFeedback.jawInterlock[X] |= 0x20;
            convertEnc = jawParameterByAxes[X].jaw2ndEncCalibrationPK*adcResult
                         +jawParameterByAxes[X].jaw2ndEncCalibrationPB;
            if(abs(convertEnc - jawFeedbackByAxes[X].jawCurrentPos) > jawParameterByAxes[X].jawDualChTolerance){
             //   printf("X Jaw Encoder untrue\r\n");
                interlockFeedback.jawInterlock[X] |= 0x40;
            }
            secondPosFeedback.jawSecondPos[X] = adcResult;
            break;
        default:    break;
    }
}

void ADCProcessTask(void *argument)
{
     uint8_t ch456 = CARRIER_SECOND_FEEDBACK_CH;
     uint32_t loopCnt;

    AD7927_Init(&hspi4);
  /* Infinite loop */
  for(;;)
  {
      switch(loopCnt++ % 5000){//read power channels, each channel sample period = 5s
          case 1:
              if(!powerManage(0))   interlockFeedback.powerInterlock |= 0x8;//bit 3
              osDelay(1);
              break;
          case 2:
              if(!powerManage(1))   interlockFeedback.powerInterlock |= 0x10;//bit 4
              osDelay(1);
              break;
          case 3:
              if(!powerManage(2))   interlockFeedback.powerInterlock |= 0x1;//bit 0
              osDelay(1);
              break;
          case 4:
              if(!powerManage(3))   interlockFeedback.powerInterlock |= 0x2;//bit 1
              osDelay(1);
              break;
          case 5:
              if(!powerManage(7))   interlockFeedback.powerInterlock |= 0x4;//bit 2
              osDelay(1);
              break;
          default:  break;
      }
      calculateDualChannelDiff(ch456);
      if(++ch456 > XJAW_SECOND_FEEDBACK_CH) ch456 = CARRIER_SECOND_FEEDBACK_CH;

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