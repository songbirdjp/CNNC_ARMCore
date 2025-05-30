#ifndef __AFC_APP_H__
#define __AFC_APP_H__

typedef struct
{
    float A1In_Para;
    float A2In_Para;
    float B1In_Para;//define CIn_Para = B1-B2
    float B2In_Para;//define DIn_Para = B1+B2
    uint16_t positionCurrent;
    uint16_t positionCalculated;
    uint16_t positionStep;
    uint16_t positionDeadzone;
    uint16_t Sample_Delay;
    uint16_t whichData;//0-8
    uint16_t positionUpperLimit;
    uint16_t positionLowerLimit;
} AFCApplicationParam_t;
uint16_t *AFCApplicationParamGet(void);
#endif
