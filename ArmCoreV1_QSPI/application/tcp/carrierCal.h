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

void calCarrierTrajectory(uint8_t *pSDStart, uint8_t totalBeam);

#ifdef __cplusplus
}
#endif
#endif