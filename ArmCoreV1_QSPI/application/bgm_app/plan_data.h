#ifndef __PLAN_DATA_H__
#define __PLAN_DATA_H__

#include <stdint.h>
#include "websocket.h"

#ifdef __cplusplus
extern "C" {
#endif

#define MAX_BEAM_NUM    30
#define MAX_CP_IN_BEAM 1280
#define CP_IN_PACK 32

typedef enum{
    RADIATION_TYPE_NONE = 0,
    RADIATION_TYPE_KV,
    RADIATION_TYPE_MV
}RADIATION_TYPE;

typedef enum{
    DELIVER_TYPE_NONE = 0,
    DELIVER_TYPE_VMAT,
    DELIVER_TYPE_SWIMRT,
    DELIVER_TYPE_SSIMRT,
    DELIVER_TYPE_CRT,
    DELIVER_TYPE_HiMAT,
    DELIVER_TYPE_SURVIEW,
    DELIVER_TYPE_CT
}DELIVER_TYPE;

typedef struct {
    uint16_t frmTag;
    uint16_t frmType;
    uint16_t frmLength;
    uint16_t totalPackInOneBeam;
    uint16_t packIndexInOneBeam;
    uint8_t beamType;       /* 摄野类型：0：none, 1: static, 2: dynamic */
    uint8_t radiationType;  /* 束流类型： 0：none, 1: KV, 2: MV */
    uint8_t deliveryType;   /* 治疗类型： 0：none, 1: VMAT, 2: SWIMRT, 3: SSIMRT, 4: CRT, 5: HiMAT, 6: SURVIEW, 7: CT */
    uint8_t reserved;
    uint16_t CPQuantityInPack;
    uint16_t RIQuantityInPack;
}FRAME_HEAD;

typedef struct {
    uint16_t crcHigh;
    uint16_t crcLow;
}FRAME_END;

typedef struct {
    uint8_t totalBeam; // < 30
    uint16_t totalCPInBeam[MAX_BEAM_NUM];
    uint16_t totalRIInBeam[MAX_BEAM_NUM];   // total RI in one beam, < 4096
    uint32_t oneBeamSize[MAX_BEAM_NUM];
    uint16_t beamIndex;
    uint16_t packIndexInOneBeam;
    uint16_t errorCode;
    uint8_t *pCPData;
    uint8_t *pRIData;
}BEAM_DATA;

typedef struct {
    uint32_t RadiationPointIndex;
    float DeliveryTime;
    float fCumulativeDose;
    float fDoseRate;
}RADIATION_POINT_DATA;

typedef struct {
    uint16_t beamID;
    uint8_t radiationType;
    uint8_t deliveryType;
    float doseRateSet;
    float beamMeterSet;
    uint16_t CPQuantityInBeam;
    uint16_t RIQuantityInBeam;
}SDRAM_DATA;

struct one_beam_order
{
    SDRAM_DATA *info;
    uint16_t *cp_ri_map;
    RADIATION_POINT_DATA *ri_data;
};

void setTCPSendControlSignal(uint8_t itemIndex, int32_t setVal);
int8_t clearPlan(void);
int8_t nrtRecvPlan(APP_DATA_RECV *info);
int32_t planFeedback(uint8_t sn, uint8_t tag_fb);

int8_t checkPlanRecvStatus(void);
int8_t getPlanBeamData(uint16_t beamIndex, struct one_beam_order *beam_info);

#ifdef __cplusplus
}
#endif
#endif