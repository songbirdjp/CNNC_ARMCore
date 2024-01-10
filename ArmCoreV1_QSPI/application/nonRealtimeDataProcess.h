#ifndef W5500H7_NONREALTIMEDATAPROCESS_H
#define W5500H7_NONREALTIMEDATAPROCESS_H

#include "cmsis_os.h"
#include "fpga_rw.h"
#include "tcp_config.h"

#define MAX_BEAM_NUM    30

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
    uint16_t faultInfo1;
    uint16_t faultInfo2;
    uint16_t rtPosUpload[RT_ARM_UPLOAD_POS_LEN/2];
}RT_BEAM_DATA;

typedef struct {
    uint16_t versionARM;
    uint16_t versionFPGA;
    uint16_t leafNcarInterlock[83];
   // uint16_t carrierInterlock;
    uint16_t jawInterlock[2];
    uint16_t powerInterlock;
    uint16_t fanInterlock;
    uint16_t boardLoss;
    uint16_t armStatus;
    uint16_t FPGAStatus;
}INTERLOCK_FEEDBACK;

typedef struct {
    uint16_t bankNo;
    uint16_t packIndexInOneBeam;
    uint16_t errorCode;
    uint16_t leafSecondPos[82];
    uint16_t carrierSecondPos;   // total RI in one beam, < 2048
    uint16_t jawSecondPos[2];
}SECOND_POS_FEEDBACK;

typedef struct {
    uint8_t carrierPosMaxL;//[MAX_CP_IN_BEAM];
    uint8_t carrierPosMaxH;
    uint8_t carrierPosMinL;//[MAX_CP_IN_BEAM];
    uint8_t carrierPosMinH;
    uint16_t carrierPos;//[MAX_CP_IN_BEAM];
    uint16_t pausePos;
}CARRIER_POS;

typedef struct {
    uint16_t ri;
    float speed;
    uint8_t retCode;
}CARRIER_PERIOD_INFO;

enum serverFsmStates {FSM_NOSTATE,FSM_INIT,FSM_IDLE,FSM_PREPARE,FSM_READY,FSM_SERVO,FSM_MANUAL,FSM_FAULT,FSM_SHUTDOWN};
enum planCommand {NO_USE,SEND_PLAN,CLOSE_PLAN};
enum carrierPosType {MAX,MIN};

extern RT_BEAM_DATA rtBeamData;
extern INTERLOCK_FEEDBACK interlockFeedback;
extern SECOND_POS_FEEDBACK secondPosFeedback;

void nrtDataMainLoop(void);
void ntrRecvParamAndPlan(TCP_DATA_t* info);
void nrtInit(void);
void sendCPtoFPGA(uint16_t beamIndex, uint16_t RIIndex);
void sendFeedback(void);
uint16_t beam_cmd_get(void);

int8_t non_realtime_fpga_data_process(uint8_t *recvBuf);
int8_t non_realtime_data_process_init(void);

#endif //W5500H7_NONREALTIMEDATAPROCESS_H
