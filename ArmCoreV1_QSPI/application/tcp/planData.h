#ifndef __PLAN_DATA_H__
#define __PLAN_DATA_H__

#include "stdint.h"
#include "websocket.h"
#include "jaw_control.h"
#include "fpga_rw.h"

#ifdef __cplusplus
extern "C" {
#endif

#define MAX_BEAM_NUM    30
#define MAX_CP_IN_BEAM 8//3601
#define TCP_SEND_PERIOD 1000 //100*1 = 100ms

enum planCommand {NO_USE,SEND_PLAN,CLOSE_PLAN};
typedef struct {
    uint16_t frmTag;
    uint16_t frmType;
    uint16_t frmLength;
    uint16_t totalPackInOneBeam;
    uint16_t bankNo;
    uint16_t packIndexInOneBeam;
}FRAME_HEAD;

typedef struct {
    uint16_t CPLimitPos[4];
    uint16_t crcHigh;
    uint16_t crcLow;
}FRAME_END;

typedef struct {
    uint16_t fsmState;
    uint16_t planCmd;
    uint8_t totalBeam; // < 30
    uint16_t totalRIInBeam[MAX_BEAM_NUM];   // total RI in one beam, < 4096
    uint32_t oneBeamSize[MAX_BEAM_NUM];
    uint16_t beamIndex;
    uint16_t radiationIndex;
}BEAM_DATA;

typedef struct {
    uint16_t faultInfo1;
    uint16_t faultInfo2;
    uint16_t rtPosUpload[RT_FPGA_UPLOAD_POS_LEN/2];
    uint16_t MlcCurFsm;
    uint16_t jawRTPos[2];
    uint16_t jawInfo[2];
    uint16_t jawTowardPos[2];
}REALTIME_FEEDBACK;

typedef struct {
    uint16_t versionARM;
    uint32_t versionFPGA;
    uint16_t leafNcarInterlock[83];
    uint16_t jawInterlock[2];
    uint16_t powerInterlock;
    uint16_t fanInterlock;
   // uint16_t boardLoss;
    uint16_t armStatus;
    uint16_t FPGAStatus;
}__attribute__((aligned(1), packed))INTERLOCK_FEEDBACK;

typedef struct {
    uint16_t bankNo;
    uint16_t packIndexInOneBeam;
    uint16_t errorCode;
    uint16_t leafSecondPos[82];
    uint16_t carrierSecondPos;   // total RI in one beam, < 2048
    uint16_t jawSecondPos[2];
}__attribute__((aligned(1), packed))SECOND_POS_FEEDBACK;

extern BEAM_DATA rtBeamData;
extern REALTIME_FEEDBACK rtFeedback;
extern INTERLOCK_FEEDBACK interlockFeedback;
extern SECOND_POS_FEEDBACK secondPosFeedback;

void planDataInit(void);
void clearPlan(void);
void setTCPSendControlSignal(uint8_t itemIndex, int32_t setVal);
void sendCPtoDevice(uint16_t beamIndex, uint16_t RIIndex, struct JawFlagType JawPos);
void updateNRTFeedback(void);

#ifdef __cplusplus
}
#endif
#endif