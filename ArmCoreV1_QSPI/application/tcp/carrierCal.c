#include "carrierCal.h"
#include "fmc_sdram_port.h"
#include "fpga_rw.h"
#include "planData.h"
#include "ulog.h"

#define ENCODE_CNT_PER_MM   325
#define ENCODE_LEAF_RANGE   (80*ENCODE_CNT_PER_MM)//26000,leaf start from 10mm, range is 80mm
#define MM_CARRIER_40P_ORIG 7
#define ENCODE_CARRIER_40P_ORIG (uint16_t)(MM_CARRIER_40P_ORIG*ENCODE_CNT_PER_MM) //4387
//#define MM_LEAF_40P_ORIG (MM_CARRIER_40P_ORIG+10)//23.5
#define ENCODE_LEAF_CARRIER_MIN_DIST    (uint16_t)(8.5*ENCODE_CNT_PER_MM)//3250
//#define ENCODE_LEAF_40P_ORIG  (ENCODE_CARRIER_40P_ORIG + ENCODE_LEAF_CARRIER_MIN_DIST)//7637
#define ENCODE_LEAF_MAX_POS (ENCODE_LEAF_RANGE+ENCODE_LEAF_CARRIER_MIN_DIST)   //29250y
#define ENCODE_LEAF_RANGE_ALLOWANCE (0.5*ENCODE_CNT_PER_MM)//162.5,normally leaf range is 75mm，but we have 0.5mm allowance
//max leaf speed about 20mm/s

CARRIER_POS carrierPosCal;
static uint8_t *pSDBeamStart;

uint16_t calMaxMinPos(uint16_t *pos)
{
    uint8_t i;
    uint16_t max,min,carMin,carMax;

    max = min = pos[0];
    for (i = 1; i < 80; i++) {
        if (max < pos[i]) max = pos[i];
        if (min > pos[i]) min = pos[i];
    }

    if ((max - min) > (ENCODE_LEAF_RANGE + ENCODE_LEAF_RANGE_ALLOWANCE)) {
        LOG_E("ERROR: Leaf position difference is too big (%d,%d)(%fmm)!\r\n", min,max,(float)(max - min) / ENCODE_CNT_PER_MM);
        return 0;
    }

    int32_t possibleMin = max - ENCODE_LEAF_MAX_POS;
    if(possibleMin >= ENCODE_CARRIER_40P_ORIG)   carMin = possibleMin;//unit:mm
    else  carMin = ENCODE_CARRIER_40P_ORIG;
    int32_t possibleMax = min - ENCODE_LEAF_CARRIER_MIN_DIST;
    if(possibleMax >= ENCODE_CARRIER_40P_ORIG)   carMax = possibleMax;//unit:mm
    else  carMax = ENCODE_CARRIER_40P_ORIG;

    if (carMax < carMin) { // diff should not more than 1.5mm
        LOG_E("WARN: carrier min > carrier max, diff is %fmm\r\n", (float)(carMin - carMax)/ ENCODE_CNT_PER_MM);
        carMin = carMax;
    }

    carrierPosCal.carrierPosMinL = carMin&0x00ff;
    carrierPosCal.carrierPosMinH = (carMin&0xff00) >> 8;
    carrierPosCal.carrierPosMaxL = carMax&0x00ff;
    carrierPosCal.carrierPosMaxH = (carMax&0xff00) >> 8;

 //   LOG_I("%u,%u\r\n",carMin, carMax);
    //  LOG_I("c(Min:%u Max:%u)\r\n", carMin, carMax);
    return max; //for x jaw pos cal
}

uint16_t rdCarrierPosFromSDRAM(uint16_t ri, uint8_t type) //0: max pos 1: min pos 2:result pos
{
    uint16_t pos;
    uint8_t *pBeamData = pSDBeamStart;

    pBeamData += sizeof(SD_RI_DATA)*(ri - 1);//nRI
    switch(type)
    {
        case 0: pBeamData += (offsetof(SD_RI_DATA, bigLeafTarget2) - offsetof(SD_RI_DATA, RI));
        break;
        case 1: pBeamData += (offsetof(SD_RI_DATA, bigLeafTarget1) - offsetof(SD_RI_DATA, RI));
        break;
        case 2: pBeamData += (offsetof(SD_RI_DATA, carrierTarget) - offsetof(SD_RI_DATA, RI));
        break;
        default: break;
    }

   // if(type)  pBeamData += 168; //skip RI + 82 leaf pos + carrier target pos
   // else pBeamData += 170;//skip RI + 81 leaf pos = 82*2B
    pos = (*pBeamData++) + (*pBeamData << 8);

    return pos;
}

void wrCarrierPos2SDRAM(uint16_t ri, uint16_t pos)
{
    uint8_t *pBeamData = pSDBeamStart;
    // for(uint8_t i = 1; i < rtBeamData.totalBeam; i++)
    // {
    //     pBeamData += rtBeamData.oneBeamSize[i];
    // }   //skip front beams
    pBeamData += sizeof(SD_RI_DATA)*(ri - 1);//nRI
   // LOG_I("sd %d %u\r\n",ri, pos);
    pBeamData += (offsetof(SD_RI_DATA, carrierTarget) - offsetof(SD_RI_DATA, RI));
    *pBeamData++ = pos&0x00ff;
    *pBeamData = (pos&0xff00) >> 8;
}
#if 1
uint8_t calEachCarrierPosBtw(uint16_t changeSpeedRI, uint16_t farmostRI,int8_t dir)
{
    //  LOG_I("==%d\r\n", i);
    float_t startPos,middlePos;
    float_t speed = 0;
    uint16_t middlePosMin, middlePosMax, middlePosU16, i = changeSpeedRI;

    startPos = (float_t)rdCarrierPosFromSDRAM(i, CAL_RESULT);
    if (dir > 0) {
        middlePos = (float_t) rdCarrierPosFromSDRAM(farmostRI, MIN);//calculate each pos between start and farmost loop
        speed = (middlePos - startPos) / (float_t) (farmostRI - changeSpeedRI);
    } else if (dir < 0) {
        middlePos = (float_t) rdCarrierPosFromSDRAM(farmostRI, MAX);
        speed = (startPos - middlePos) / (float_t) (farmostRI - changeSpeedRI);
    }

    LOG_I("RI: %d - %d: speed: %f pos: %f - %f\r\n", changeSpeedRI,farmostRI,speed,startPos,middlePos);
    i++;
    for (; i <= farmostRI; i++) {
        startPos += dir * speed;
        middlePosU16 = (uint16_t) (startPos + 0.5);//rounding off
        middlePosMin = rdCarrierPosFromSDRAM(i, MIN);
        middlePosMax = rdCarrierPosFromSDRAM(i, MAX);
        if ((middlePosU16 < middlePosMin) || (middlePosU16 > middlePosMax)) {
            LOG_I("WARN:%d %u not in[%u %u],", i, middlePosU16, middlePosMin, middlePosMax);
            if (dir > 0) middlePosU16 = middlePosMax;
            else if (dir < 0) middlePosU16 = middlePosMin;
            LOG_I("constraint to %u\r\n", middlePosU16);
           // break;
        }
        wrCarrierPos2SDRAM(i,middlePosU16);
      //  LOG_I("%u,%u\r\n",i, middlePosU16);
        // LOG_I("middle pos %d:%u\r\n", i, middlePos0);
        //  LOG_I("fast %f carrierPosCal.carrierPos[%d]:%u\r\n", speed, i, carrierPosCal.carrierPos[i]);
    }
    return 1;
}

static uint16_t stageStartRI;//if ( formost speed > current speed > last slow speed ), then start a new stage calculation
static float_t stageStartPosF;

CARRIER_PERIOD_INFO findConstraintPos(uint16_t startRI, float_t lastSlowSpeed, uint16_t changeSpeedRI, float_t farmostSpeed, uint16_t farmostRI, int8_t dir)
{
    uint16_t /*calRI = MAX_CP_IN_BEAM - changeSpeedRI,*/ minSpeedRI = 0,i = changeSpeedRI,j,carrierPos;
  //  uint16_t totalRI = rtBeamData.totalRIInBeam[rtBeamData.totalBeam];
 // LOG_I("start ri %d puase ri %d\r\n", changeSpeedRI,farmostRI);
    float_t speedArry[farmostRI];//speedArry[calRI + 1];
    float_t startPosF, middlePos, localStageStartPos;
    CARRIER_PERIOD_INFO slow;

 //   for(i = changeSpeedRI; i <= totalRI; i++)
    if (dir > 0) {
        startPosF = (float_t)rdCarrierPosFromSDRAM(startRI, MAX);//constraint
    }
    else if (dir < 0) {
        startPosF = (float_t)rdCarrierPosFromSDRAM(startRI, MIN);//constraint
    }
   LOG_I("start ri: %u, pause ri: %u, startPosF: %f farmost speed %f\r\n",i, farmostRI, startPosF, farmostSpeed);
//  minSpeedRI = 1 + changeSpeedRI;
    slow.ri = changeSpeedRI;
    speedArry[0] = 0;
  
    i++;
    for(; i <= farmostRI; i++)
    {
        uint16_t iArry = i - startRI;// >=1
        if (dir > 0) {
            middlePos = (float_t)rdCarrierPosFromSDRAM(i, MAX);
            speedArry[i] = (middlePos - startPosF) / (float_t)iArry;//constraint
        }
        else if (dir < 0) {
            middlePos = (float_t)rdCarrierPosFromSDRAM(i, MIN);
            speedArry[i] = (startPosF - middlePos) / (float_t)iArry;//constraint
        }
//LOG_I("speed %u: %u %f %f %f\r\n", i, iArry, speedArry[i], startPosF, middlePos);
        if (speedArry[i] < -1e-6) {
            slow.ri = i;
            slow.retCode = 1;
            LOG_I("change direction, need re-calculate.Speed[%d]:%f\r\n", i, speedArry[i]);
            return slow;
        }
        if (farmostSpeed >= speedArry[i])
        {
            if (lastSlowSpeed < speedArry[i]){// a new stage 
                stageStartRI = changeSpeedRI;
                stageStartPosF = (float_t)rdCarrierPosFromSDRAM(stageStartRI, CAL_RESULT);
                LOG_I("higher speed than before, new stage %d %f!\r\n", stageStartRI, stageStartPosF);
            }
            minSpeedRI = i;
            localStageStartPos = stageStartPosF;
            for (j = (stageStartRI + 1); j <= minSpeedRI; j++)//calculate each pos between start and slowest loop
            {
                localStageStartPos += dir * speedArry[minSpeedRI];
                carrierPos = (uint16_t)(localStageStartPos + 0.5);
         //  wrCarrierPos2SDRAM(i, carrierPos);
                carrierPosCal.carrierPos = carrierPos;
                wrCarrierPos2SDRAM(j, carrierPosCal.carrierPos);
          // LOG_I("%d slow speed[%d]:%f carrierPos[%d]:%u\n", j,minSpeedRI,speedArry[minSpeedRI], j, carrierPos);
              //  LOG_I("%d,%u\r\n", j, carrierPos);
            }

            slow.speed = speedArry[i];
            slow.ri = minSpeedRI;
            slow.retCode = 2;

            return slow;
        }
    }

    slow.retCode = 3;
    return slow;
}

CARRIER_PERIOD_INFO findFarmostPos(uint16_t startRI, uint16_t pauseRI, int8_t dir)
{
    uint16_t i = startRI;
   // uint16_t totalRI = rtBeamData.totalRIInBeam[rtBeamData.totalBeam],i;
    float_t startPosF, middlePosF;
    float_t speedArry[pauseRI]/*, speedMax*/;
    CARRIER_PERIOD_INFO fastest;

  //  LOG_I("start ri:%u, pause ri:%u\r\n", startRI, pauseRI);
    if (dir > 0) {
        startPosF = (float_t)rdCarrierPosFromSDRAM(i, MAX);
    }
    else if (dir < 0) {
        startPosF = (float_t)rdCarrierPosFromSDRAM(i, MIN);
    }
  //  LOG_I("startPosF: %f\r\n", startPosF);
    fastest.speed = 0;
    fastest.ri = ++i;
    speedArry[0] = 0;

    for(; i <= pauseRI; i++)  //find out the fastest period, and its speed should satisfy all previous period
    {
        uint16_t iArry = i - startRI;// >=1
        if (dir > 0) {
            middlePosF = (float_t)rdCarrierPosFromSDRAM(i, MIN);
            speedArry[iArry] = (middlePosF - startPosF) / (float_t)iArry;
        }
        else if (dir < 0) {
            middlePosF = (float_t)rdCarrierPosFromSDRAM(i, MAX);
            speedArry[iArry] = (startPosF - middlePosF) / (float_t)iArry;//should reach
        }
// LOG_I("%d %f %f\r\n", i, speedArry[iArry], fastest.speed);
        if ((speedArry[iArry] - fastest.speed) > 1e-6)//find the farmost period in this direction
        {
            fastest.speed = speedArry[iArry];
            fastest.ri = i;
            if (dir > 0)    LOG_I("max speed %d: %f\r\n", fastest.ri, fastest.speed);
            else if (dir < 0)   LOG_I("max speed %d: %f\r\n", fastest.ri, -fastest.speed);
     //  if(dir > 0) startPosF = (float_t)rdCarrierPosFromSDRAM(startRI, MAX);
      // else if(dir < 0) startPosF = (float_t)rdCarrierPosFromSDRAM(startRI, MIN);
        }
    }

    return fastest;
}

uint16_t calculateOneMovement(uint16_t startRI, int8_t dir, uint16_t totalRI)//calculate carrier pos from start to pause
{
    uint8_t state = 1;
    CARRIER_PERIOD_INFO maxSpeedPeriod, constraintPeriod;
   // uint16_t totalRI = rtBeamData.totalRIInBeam[rtBeamData.beamIndex];
	//LOG_I("startRI %d\r\n", startRI);
    constraintPeriod.ri = startRI;
    while(1)
    {
        switch(state)
        {
            case 1:
				LOG_I("step 1:\r\n");
                maxSpeedPeriod = findFarmostPos(startRI, totalRI, dir);
                constraintPeriod.ri = startRI;
                constraintPeriod.speed = maxSpeedPeriod.speed;
                stageStartRI = startRI;
                if (dir > 0) {
                    stageStartPosF = (float_t)rdCarrierPosFromSDRAM(stageStartRI, MAX);//constraint
                }
                else if (dir < 0) {
                    stageStartPosF = (float_t)rdCarrierPosFromSDRAM(stageStartRI, MIN);//constraint
                }
                state = 2;
                break;
            case 2:
				LOG_I("step 2:\r\n");
                constraintPeriod =
                        findConstraintPos(startRI, constraintPeriod.speed, constraintPeriod.ri, maxSpeedPeriod.speed,maxSpeedPeriod.ri, dir);
                state = constraintPeriod.retCode;
                if(state == 1)  totalRI = constraintPeriod.ri;
                break;
            case 3:
				LOG_I("step 3:\r\n");
                calEachCarrierPosBtw(constraintPeriod.ri, maxSpeedPeriod.ri,dir);
//LOG_I("maxSpeed %d: %f\r\n", maxSpeedPeriod.ri, maxSpeedPeriod.speed);
                return maxSpeedPeriod.ri;
            default:break;
        }
    }
}

uint16_t findDirection(uint16_t startRI, uint16_t totalRI)
{
    uint16_t intersectMin, minInterRI = startRI, intersectMax, maxInterRI = startRI, pauseRI = 0,i;
    uint16_t posMin, posMax, carrierPos = 0;
    float speed = 0.0;

    intersectMin =  rdCarrierPosFromSDRAM(startRI, MIN);
    intersectMax =  rdCarrierPosFromSDRAM(startRI, MAX);
    LOG_I("total ri %u start ri %u min %u max %u\r\n", totalRI, startRI, intersectMin, intersectMax);
    for(uint16_t ri = startRI; ri <= totalRI; ri++) 
    {
        //the first stage - find out the carrier prepare pos and fix move direction
        // we need to calculate public intersection, choose intersection edge as carrier prepare pos
        //LOG_I("%d ", ri);
        posMin =  rdCarrierPosFromSDRAM(ri, MIN);
        posMax =  rdCarrierPosFromSDRAM(ri, MAX);
        if (intersectMax < posMin)
        {
            //determine the init pos for the first stage, choose right edge which is closest
            //  carrierInitPos = intersectMax;
            if (startRI == 1)   //first stage
            {
                for (i = startRI; i <= maxInterRI; i++) {
                    if(carrierPosCal.preparePos == 0)  carrierPosCal.preparePos = intersectMax;
                    carrierPosCal.carrierPos = carrierPosCal.preparePos;
                    wrCarrierPos2SDRAM(i, carrierPosCal.carrierPos);
      //  LOG_I("init carrierPosCal.carrierPos[%d]:%u\n", i, carrierPosCal.carrierPos[i]);
                  //  LOG_I("%d,%u\r\n", i, carrierPosCal.carrierPos);
                }
                carrierPosCal.preparePos = 0;
            }
            else {//later stages
                uint16_t startPos = rdCarrierPosFromSDRAM(startRI, CAL_RESULT);
                LOG_I("run to next stage start pos %d[%d] -> %d[%d]\r\n", startPos, startRI, intersectMax, maxInterRI);
                speed = (float)(intersectMax - startPos) / (maxInterRI - startRI);
                for (i = startRI+1; i <= maxInterRI; i++) {
                    carrierPos = rdCarrierPosFromSDRAM(i, MAX);
                    if (carrierPos == intersectMax) wrCarrierPos2SDRAM(i, intersectMax);
                    else{
                        carrierPos = startPos + speed*(i - startRI) + 0.5;
                        wrCarrierPos2SDRAM(i, carrierPos);
                    }   
                    //carrierPosCal.carrierPos[i] = carrierPosCal.carrierPos[startRI] + speed*(i - startRI) + 0.5;
       // LOG_I("init carrierPosCal.carrierPos[%d]:%u\n", i, carrierPosCal.carrierPos[i]);
                  //  LOG_I("%d,%u\r\n", i, carrierPos);
                }
            }
            LOG_I("Forward out-of-range ri %d min pos %u, start ri %d init pos %u\r\n", ri, posMin, maxInterRI, intersectMax);
            pauseRI = calculateOneMovement(maxInterRI, 1, totalRI);//move forward, calculate next movement
   // carrierPosCal.pausePos = rdCarrierPosFromSDRAM(pauseRI, MIN);
        //    LOG_I("pause ri %d\r\n", pauseRI);
            break;
        } 
        else if (intersectMin > posMax) {
            if (startRI == 1)//first stage
            {
                for (i = startRI; i <= minInterRI; i++) {
                    if(carrierPosCal.preparePos == 0)  carrierPosCal.preparePos = intersectMin;
                    carrierPosCal.carrierPos = carrierPosCal.preparePos;
                    wrCarrierPos2SDRAM(i, carrierPosCal.carrierPos);
       // LOG_I("init carrierPosCal.carrierPos[%d]:%u\n", i, carrierPosCal.carrierPos);
                 //   LOG_I("%d,%u\r\n", i, carrierPosCal.carrierPos);
                }
                carrierPosCal.preparePos = 0;
            }
            else {//later stages
                uint16_t startPos = rdCarrierPosFromSDRAM(startRI, CAL_RESULT);
                LOG_I("run to next stage start pos %d[%d] -> %d[%d]\r\n", startPos, startRI, intersectMin, minInterRI);
                speed = (float)(intersectMin - startPos) / (minInterRI - startRI);
                for (i = startRI + 1; i <= minInterRI; i++) {
                    carrierPos = rdCarrierPosFromSDRAM(i, MIN);
                  //  LOG_I("carrierPos 111 %d\r\n", carrierPos);
                    if(carrierPos == intersectMin) wrCarrierPos2SDRAM(i, intersectMin);
                    else{
                        carrierPos = startPos + speed*(i - startRI) + 0.5;
                        wrCarrierPos2SDRAM(i, carrierPos);
                       // LOG_I("carrierPos 222 %d\r\n", carrierPos);
                    }   
      //  LOG_I("init carrierPosCal.carrierPos[%d]:%u\n", i, carrierPosCal.carrierPos[i]);
                  //  LOG_I("%d,%u\r\n", i, carrierPos);
                }
            }
            LOG_I("Backward out-of-range ri %d max pos %u, start ri %d init pos %u\r\n", ri, posMax, minInterRI, intersectMin);
            pauseRI = calculateOneMovement(minInterRI, -1, totalRI);//move backward
           // carrierPosCal.pausePos = rdCarrierPosFromSDRAM(pauseRI, MAX);
       //     LOG_I("pause ri %d\r\n", pauseRI);
            break;
        }else //calculate carrier pos intersection for each RI
        {
            if (intersectMin <= posMin) {
                intersectMin = posMin;
                minInterRI = ri;
            }

            if (intersectMax >= posMax) {
                intersectMax = posMax;
                maxInterRI = ri;
            }
          //  LOG_I("%d (%u, %u)\r\n", ri, intersectMin, intersectMax);

            if (ri == totalRI) 
            { //can't find no-intersection period
                uint16_t startPos = rdCarrierPosFromSDRAM(startRI, CAL_RESULT);
                int16_t diff = abs(intersectMin - startPos) - abs(intersectMax - startPos);
                if (startRI == 1)//can't find no-intersection period in whole beam, directly decide prepare pos 
                {
                  //  LOG_I("init prepare pos %u\r\n", carrierPosCal.preparePos);
                    if (carrierPosCal.preparePos == 0) {
                        pauseRI = totalRI;
                        for (i = startRI; i <= pauseRI; i++) {
                            if (diff <= 0)  carrierPosCal.preparePos = intersectMin;
                            else   carrierPosCal.preparePos = intersectMax;
                            wrCarrierPos2SDRAM(i, carrierPosCal.preparePos);
                            if(i == startRI)  LOG_I("the prepare pos - carrierPos[%d]:%u\r\n", i, carrierPosCal.preparePos);
                        }
                        carrierPosCal.preparePos = 0;
                    }
                }
                else 
                {
                    if (diff <= 0) 
                    {
                        if (minInterRI != startRI)
                        {
                            pauseRI = minInterRI;
                            speed = (float)(intersectMin - startPos) / (pauseRI - startRI);
                            for (i = startRI + 1; i <= pauseRI; i++) {//whole beam end stage 
                                carrierPos = rdCarrierPosFromSDRAM(i, MIN);
                                if(carrierPos == intersectMin) wrCarrierPos2SDRAM(i, intersectMin);
                                else{   
                                    carrierPos = startPos + speed*(i - startRI) + 0.5;
                                    wrCarrierPos2SDRAM(i, carrierPos);
                                }
                              //  LOG_I("%d,%u\r\n", i, carrierPosCal.carrierPos);
                            }
                            LOG_I("end stage go to min: %d,%u-%d,%u %f\r\n", startRI,startPos,minInterRI,intersectMin,speed);
                        }
                        if (minInterRI != totalRI)
                        {
                            pauseRI = totalRI;
                            for (i = minInterRI + 1; i <= pauseRI; i++) {//whole beam end stage 
                                wrCarrierPos2SDRAM(i, intersectMin);
                               // LOG_I("%d,%u\r\n", i, intersectMin);
                            }
                            LOG_I("end stage go to min: %d-%d\r\n", minInterRI,totalRI);
                        }
                    }
                    else 
                    {
                        if (maxInterRI != startRI)
                        {
                            pauseRI = maxInterRI;
                            speed = (float)(intersectMax - startPos) / (pauseRI - startRI);
                            for (i = startRI + 1; i <= pauseRI; i++) {//whole beam end stage 
                                carrierPos = rdCarrierPosFromSDRAM(i, MAX);
                                if (carrierPos == intersectMax) carrierPos = intersectMax;
                                else  {   
                                    carrierPos = startPos + speed*(i - startRI) + 0.5;
                                    wrCarrierPos2SDRAM(i, carrierPos);
                                }
                               // LOG_I("%d,%u\r\n", i, carrierPos);
                            }
                            LOG_I("end stage go to max: %d,%u-%d,%u %f\r\n", startRI,startPos,maxInterRI,intersectMax,speed);
                        }
                        if (maxInterRI != totalRI)
                        {
                            pauseRI = totalRI;
                            for (i = maxInterRI + 1; i <= pauseRI; i++) {//whole beam end stage 
                                wrCarrierPos2SDRAM(i, intersectMax);
                              //  LOG_I("%d,%u\r\n", i, intersectMax);
                            }
                            LOG_I("end stage go to max: %d-%d\r\n", maxInterRI,totalRI);
                        }
                    } 
                }
                LOG_I("can't find no-intersection period\r\n");
            }
        }
    }

    return pauseRI;
}
#endif

void calCarrierTrajectory(uint8_t *pSDStart, uint8_t totalBeam)
{
    uint8_t *pBeamData = NULL, diff;
    uint16_t leafPos[80],xJawTarget;
    uint16_t startRI, pauseRI;
    uint16_t totalRI, beamID,maxLeafPos;
    SD_BEAM_DATA beamStruct= {0};
    uint8_t beamBufIndex = 0;
   // totalBeam = 2;
 
    memset(&carrierPosCal, 0, sizeof(carrierPosCal));
    pBeamData = pSDStart;
    while(totalBeam--)
    {
        totalRI = (pBeamData[1] << 8) + pBeamData[0];
        beamID = (pBeamData[3] << 8) + pBeamData[2];
        LOG_I("beam cnt %d %d\r\n", beamBufIndex, beamID);
      //  diff = offsetof(SD_BEAM_DATA, riStruct) - offsetof(SD_BEAM_DATA, totalRI);
        pBeamData += (offsetof(SD_BEAM_DATA, riStruct) - offsetof(SD_BEAM_DATA, totalRI)); //totalRI + Beam index
        pSDBeamStart = pBeamData;
        for(uint16_t ri = 1; ri <= totalRI; ri++)
        {
            pBeamData += sizeof(beamStruct.riStruct.RI);
            for(uint8_t i = 0; i < 80; i++) 
            {
                leafPos[i] = (pBeamData[2*(i+1)+1] << 8) + pBeamData[2*(i+1)];
             //   LOG_I("%d,", leafPos[i]);
            }
           // LOG_I("\r\n");
            maxLeafPos = calMaxMinPos(leafPos);//calculate carrier pos range
            if(maxLeafPos == 0) return;
            *pBeamData++ = carrierPosCal.carrierPosMinL;//write carrier min to 1st big leaf pos
            *pBeamData++ = carrierPosCal.carrierPosMinH;
             pBeamData += sizeof(beamStruct.riStruct.leafTarget);//skip 80 leaf pos
            *pBeamData++ = carrierPosCal.carrierPosMaxL;//write carrier max to second insert leaf pos
            *pBeamData++ = carrierPosCal.carrierPosMaxH;
            pBeamData += sizeof(beamStruct.riStruct.carrierTarget);//skip carrier pos
            xJawTarget = (pBeamData[1] << 8) + pBeamData[0];
            if((maxLeafPos > 0)&&(xJawTarget > 0))
            {
                if((maxLeafPos/325/0.44*0.213 - xJawTarget/2.5/ENCODER_CNT_PER_MM) > 59)
                {
                    LOG_I("Xjaw can't mask leaf end %d %d %d %d!\r\n",beamBufIndex,ri, xJawTarget, maxLeafPos);
                    interlockFeedback.jawInterlock[X] |= 0x10;
                  //  return -1;
                }
            }
            pBeamData += (sizeof(SD_RI_DATA) - offsetof(SD_RI_DATA, jawTarget[X]));
         //   LOG_I("last diff %d\r\n", diff);
           // LOG_I("%d[min:%u max:%u]\r\n",ri,rdCarrierPosFromSDRAM(ri, MIN),rdCarrierPosFromSDRAM(ri, MAX));
        }
#if 1
        pauseRI = 1;
        do{
            pauseRI = findDirection(pauseRI, rtBeamData.totalRIInBeam[beamBufIndex]);
        }
        while(pauseRI != totalRI);
        beamBufIndex++;

   // for(i = 1; i <=totalRI; i++) wrCarrierPos2SDRAM(i);
        LOG_I("carrier calculate finish!\r\n");
#endif
    }
}