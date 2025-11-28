#ifndef __CARRIER_CALCULATE_H__
#define __CARRIER_CALCULATE_H__

#include "main.h"
#include <stdint.h>
#include "cmsis_os2.h"
#include <stdbool.h>
#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

enum carrierPosType {MAX,MIN,CAL_RESULT};
enum deviceType {MLC,XJAW,YJAW};

typedef struct {
    uint8_t carrierPosMaxL;//[MAX_CP_IN_BEAM];
    uint8_t carrierPosMaxH;
    uint8_t carrierPosMinL;//[MAX_CP_IN_BEAM];
    uint8_t carrierPosMinH;
    uint16_t carrierPos;//[MAX_CP_IN_BEAM];
    uint16_t preparePos;
}CARRIER_POS;

typedef struct {
    uint16_t ri;
    float_t speed;
    uint8_t retCode;
}CARRIER_PERIOD_INFO;

typedef struct {
    uint16_t encodeLeafRange;
    uint16_t carrierNegLim;
    uint16_t leafNCarMinDist;
    uint16_t leafRangeOffset;
    uint16_t encodeLeafMax;
    float calibrationCoef[9];
}CARRIER_PARAM_SETTING;

extern osEventFlagsId_t carrier_cal_eventHandle;

void calCarrierTrajectory(uint8_t *pSDStart, uint8_t totalBeam);
void plcSetCarrierParam(uint8_t *pData);

#ifdef __cplusplus
}
#endif
#endif