#ifndef __PLAN_DATA_H__
#define __PLAN_DATA_H__

#include "stdint.h"
#include "websocket.h"

#ifdef __cplusplus
extern "C" {
#endif

#define PLAN_DATA_SETTING_TAG 2

#define BGM_NRT_COMMAND_TAG 30
#define AFC_NRT_COMMAND_TAG 31
#define DOSE1_NRT_COMMAND_TAG 32
#define DOSE2_NRT_COMMAND_TAG 33

#define MAX_BEAM_NUM    30
#define MAX_CP_IN_BEAM 256
#define TCP_SEND_PERIOD 1000 //100*1 = 100ms

enum planCommand {NO_USE,SEND_PLAN,CLOSE_PLAN};
typedef struct {
    uint16_t frmTag;
    uint16_t frmType;
    uint16_t frmLength;
    uint16_t totalPackInOneBeam;
    uint16_t packIndexInOneBeam;
    uint8_t beamType;
    uint8_t radiationType;
    uint8_t deliveryType;
    uint16_t CPQuantityInPack;
    uint16_t RIQuantityInPack;
}__attribute__((aligned(1), packed))FRAME_HEAD;

typedef struct {
    uint16_t crcHigh;
    uint16_t crcLow;
}FRAME_END;

typedef struct {
  //  uint16_t fsmState;
  //  uint16_t planCmd;
    uint8_t totalBeam; // < 30
    uint16_t totalCPInBeam[MAX_BEAM_NUM];
    uint16_t totalRIInBeam[MAX_BEAM_NUM];   // total RI in one beam, < 4096
    uint32_t oneBeamSize[MAX_BEAM_NUM];
    uint16_t beamIndex;
   // uint16_t radiationIndex;
    uint16_t packIndexInOneBeam;
    uint16_t errorCode;
    uint8_t *pCPData;
    uint8_t *pRIData;
}BEAM_DATA;

typedef struct {
    uint16_t RadiationPointIndex;
    float DeliveryTime;
    float fCumulativeDose;
    float fDoseRate;
}RADIATION_POINT_DATA;

typedef struct {
    __IO uint8_t *pBeamData;
    uint16_t length;
}BEAM_DATA_ADDR;

void planDataInit(void);
uint8_t clearPlan(void);
void setTCPSendControlSignal(uint8_t itemIndex, int32_t setVal);
int8_t nrtRecvPlan(APP_DATA_RECV* info);
void planFeedback(uint8_t sn);

#ifdef __cplusplus
}
#endif
#endif