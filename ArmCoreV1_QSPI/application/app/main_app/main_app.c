#include "main_app.h"
#include "fmc_sdram_port.h"
#include "fpga_rw.h"
#include "tcp_tasks.h"
#include "main.h"
#include "init_call.h"
#include "websocket.h"
#include "planData.h"
#include "ulog.h"

// #define PARAM_SETTING_TAG 1
// #define PLAN_DATA_SETTING_TAG 2

// #define	 CRC_TABLE_SIZE		256
/********************************************************************************************/
#include "ethercat.h"

#define DATA_PROCESS_FPGA_EVENT   (1<<0)
#define DATA_PROCESS_LAN_EVENT    (1<<1)
#define DATA_PROCESS_TCP_EVENT    (1<<2)
static osEventFlagsId_t data_process_eventHandle = NULL;
static uint16_t planCmd, stateCmd;

static int8_t non_realtime_fpga_data_process(uint8_t *recvBuf)
{
    uint32_t checkSum = 0;
    uint16_t i;

    for (i = 2; i < (RECV_BUF_LEN - 1); i++) 
    {
        checkSum += recvBuf[i];
    }

    if (((uint8_t)checkSum != recvBuf[RECV_BUF_LEN - 1]) || (checkSum == 0))
    {
        LOG_E("Checksum err 0x%x 0x%x 0x%x 0x%x 0x%x\r\n", recvBuf[0],recvBuf[1],recvBuf[2],recvBuf[3],recvBuf[4]);
        return -1;
    }
    else
    {
        #ifndef TEST
        operateSendMutex(1, 0, osWaitForever);// 1:Acquire 0:Release
        if (recvBuf[4] == PACKF0_CMD) 
        {
           // LOG_I("F0\r\n");
            // for (i = 0; i < (RT_FPGA_UPLOAD_PAYLOAD_LEN - 2); i += 2) 
            // {   //RT 0 - 165 ：82 leaf and carrier pos
            //     rtFeedback.rtPosUpload[i/2] = (recvBuf[i + FPGA_RT_UPLOAD_START] << 8) + recvBuf[i + FPGA_RT_UPLOAD_START + 1];
            // }
            memcpy(rtFeedback.rtPosUpload, &recvBuf[FPGA_RT_UPLOAD_START], 166);
          //  LOG_E("car %d\r\n",rtFeedback.rtPosUpload[82]);
            rtFeedback.faultInfo1 = recvBuf[FPGA_RT_UPLOAD_START+166];//RT 166
            rtFeedback.faultInfo2 = recvBuf[FPGA_RT_UPLOAD_START+167];//RT 167
            rtFeedback.MlcCurFsm = recvBuf[FPGA_RT_UPLOAD_START+168];//RT 168
          //  LOG_I("sta: %d %d %d %d\r\n", rtFeedback.faultInfo1,rtFeedback.faultInfo2, rtFeedback.MlcCurFsm,recvBuf[FPGA_RT_UPLOAD_START+169]);
          //  rtFeedback.jawTowardPos[X] = (recvBuf[170 + FPGA_RT_UPLOAD_START] << 8) + recvBuf[FPGA_RT_UPLOAD_START + 171];//RT 170 - 171
         //   rtFeedback.jawTowardPos[Y] = (recvBuf[172 + FPGA_RT_UPLOAD_START] << 8) + recvBuf[FPGA_RT_UPLOAD_START + 173];//RT 172 - 173
            memcpy(&rtFeedback.jawTowardPos[X], &recvBuf[FPGA_RT_UPLOAD_START+170], 4);
           // LOG_I("toward %d %d\r\n",rtFeedback.jawTowardPos[X],rtFeedback.jawTowardPos[Y]);
          //  LOG_I("sta 0x%x 0x%x 0x%x\r\n",rtFeedback.faultInfo1,rtFeedback.faultInfo2,rtFeedback.MlcCurFsm);
            // for (i = 0; i < 164; i += 2) 
            // {  //NRT 0 - 163 ：82 leaf second pos
            //     secondPosFeedback.leafSecondPos[i/2] = (recvBuf[i + FPGA_NRT_UPLOAD_START] << 8) + recvBuf[i + FPGA_NRT_UPLOAD_START + 1];                            
            // }
            memcpy(secondPosFeedback.leafSecondPos, &recvBuf[FPGA_NRT_UPLOAD_START], 164);
          //  LOG_I("sec %d\r\n",secondPosFeedback.leafSecondPos[80]); 
           // interlockFeedback.boardLoss = recvBuf[FPGA_NRT_UPLOAD_START + 168];//NRT 166 bit6-7
          //  interlockFeedback.FPGAStatus = (recvBuf[FPGA_NRT_UPLOAD_START + 166] << 8) + recvBuf[FPGA_NRT_UPLOAD_START + 167]; //NRT 166-167 
            memcpy(&interlockFeedback.FPGAStatus, &recvBuf[FPGA_NRT_UPLOAD_START + 166], 2);  
         //   LOG_I("FPGAStatus %x\r\n",interlockFeedback.FPGAStatus);      
        }
        else if (recvBuf[4] == PACKF1_CMD) 
        {
           // LOG_I("F1\r\n");
            // for (i = 0; i < (RT_FPGA_UPLOAD_PAYLOAD_LEN - 2); i += 2) 
            // {   //RT 0 - 165 ：82 leaf and carrier pos
            //     rtFeedback.rtPosUpload[i/2] = (recvBuf[i + FPGA_RT_UPLOAD_START] << 8) + recvBuf[i + FPGA_RT_UPLOAD_START + 1];
            // }
            memcpy(rtFeedback.rtPosUpload, &recvBuf[FPGA_RT_UPLOAD_START], 166);
            rtFeedback.faultInfo1 = recvBuf[FPGA_RT_UPLOAD_START+166];//RT 166
            rtFeedback.faultInfo2 = recvBuf[FPGA_RT_UPLOAD_START+167];//RT 167
            rtFeedback.MlcCurFsm = recvBuf[FPGA_RT_UPLOAD_START+168];//RT 168
           // rtFeedback.jawTowardPos[X] = (recvBuf[170 + FPGA_RT_UPLOAD_START] << 8) + recvBuf[FPGA_RT_UPLOAD_START + 171];//RT 170 - 171
          //  rtFeedback.jawTowardPos[Y] = (recvBuf[172 + FPGA_RT_UPLOAD_START] << 8) + recvBuf[FPGA_RT_UPLOAD_START + 173];//RT 172 - 173
            memcpy(&rtFeedback.jawTowardPos[X], &recvBuf[FPGA_RT_UPLOAD_START+170], 4);
           // LOG_I("jaw %d %d\r\n", rtFeedback.jawTowardPos[X],rtFeedback.jawTowardPos[Y]);
            // for (i = 0; i < 166; i += 2) 
            // {//NRT 0 - 165 ：82 leaf and carrier interlock
            //    interlockFeedback.leafNcarInterlock[i/2] = (recvBuf[i + FPGA_NRT_UPLOAD_START] << 8) + recvBuf[i + FPGA_NRT_UPLOAD_START + 1];
            // }
            memcpy(interlockFeedback.leafNcarInterlock, &recvBuf[FPGA_NRT_UPLOAD_START], 166);
            memcpy(&interlockFeedback.versionFPGA, &recvBuf[FPGA_NRT_UPLOAD_START + 166], 4);
        //    interlockFeedback.versionFPGA = 
        //        (recvBuf[FPGA_NRT_UPLOAD_START + 166]<<24)+(recvBuf[FPGA_NRT_UPLOAD_START + 167]<<16)+(recvBuf[FPGA_NRT_UPLOAD_START + 168]<<8)+recvBuf[FPGA_NRT_UPLOAD_START + 169];
        //    LOG_I("ver %x\r\n",interlockFeedback.versionFPGA);       
        }
        operateSendMutex(0, 0, 0);
        #endif
    }

    return 0;
}

#include "lan9252_app.h"
#include "jaw_control.h"
static int8_t realtime_ethercat_data_process(void)
{
    static uint16_t oldState, oldPlanCmd, oldRadiationIndex, oldBeamIndex, oldErrState;
    struct JawFlagType JawState;
    static uint16_t cnt;
    uint16_t errReset;

    TOBJ7010 recv_data = {0};
    TOBJ6000 send_data = {0};

    TOBJ7010 *recv = (TOBJ7010 *)ethercat_recv_data_get((uint16_t *)&recv_data, sizeof(recv_data));
    TOBJ6000 *send = (TOBJ6000 *)ethercat_send_data_get((uint16_t *)&send_data, sizeof(send_data));
    if (recv == NULL || send == NULL)
    {
        LOG_E("ethercat data get failed\r\n");
        return -1;
    }
 
#ifdef TEST
    rtFeedback.faultInfo1 = 0x50;
  //  memcpy(&send->InfoIn[0], &recv->InfoOut[0], sizeof(UINT16) * 8);         //rt upload， echo
    memcpy(send, recv, sizeof(UINT16) * 8);
#else
    cnt++;
    send->InU16_CrtFsmState = rtFeedback.MlcCurFsm;
    send->InU16_BeamIndexFB = oldBeamIndex;
    send->InU16_RidiationIndexFB = oldRadiationIndex;
    send->InU16_BankIndexFB = BANK_NO;
    memcpy(&send->InU16_LeafCrtControlMode, &recv->OutU16_LeafControlModeSetting, sizeof(uint16_t) * 4);//control mode feedback
#endif
    send->InU16_PlanCmdFB = recv->OutU16_PlanCmd;
	send->InU16_FaultInfo1 &= 0xff00; 
    send->InU16_FaultInfo1 |= rtFeedback.faultInfo1;
    if(cnt++ >= 200){
        if((send->InU16_FaultInfo1 & 0x800) == 0x800)   send->InU16_FaultInfo1 &= ~0x800;
        else    send->InU16_FaultInfo1 |= 0x800;  
        cnt = 0;
    }    
  //  LOG_I("%x\r\n", send->InU16_FaultInfo1);
  //  if(interlockFeedback.boardLoss&0x0007)  send->InU16_FaultInfo1 |= 0x0002;
  //  if(tcp_link_status_get() == false) 
  //  {
  //      send->InU16_FaultInfo1 |= 0x400;
  //  }
   // send->InU16_FaultInfo2 = rtFeedback.faultInfo2&0x00ff;
    send->InU16_FaultInfo2 = ((rtFeedback.jawInfo[Y]&0x00f0)<< 4) + ((rtFeedback.jawInfo[X]&0x00f0)<< 8) + (rtFeedback.faultInfo2&0x00ff);
    memcpy(send->InAU16_LeafCrtPos, rtFeedback.rtPosUpload, sizeof(uint16_t) * (8 * 10 + 3));
    send->InAU16_JawCrtPos[X] = rtFeedback.jawRTPos[X];
    send->InAU16_JawCrtPos[Y] = rtFeedback.jawRTPos[Y];
   // LOG_E("self %d %d\r\n",rtFeedback.jawRTPos[X],rtFeedback.jawRTPos[Y]);
    send->InU16_JawInfo = ((rtFeedback.jawInfo[Y]&0x0f)<< 4) + (rtFeedback.jawInfo[X]&0x0f);

    stateCmd = recv->OutU16_FsmStateSetting;   //save rt cmd
    rtBeamData.beamIndex = recv->OutU16_BeamIndex;
    rtBeamData.radiationIndex = recv->OutU16_RidiationIndex;
    planCmd = recv->OutU16_PlanCmd;
    errReset = recv->OutU16_ErrReset; /* Subindex12 - OutU16_ErrReset */

    if(oldState != stateCmd)
    {
        LOG_I("fsm state: %d -> %d\r\n",oldState,stateCmd);
        uint8_t newState = stateCmd;
        make_cmd_to_fpga(CMD_STA_REQ, &newState);
        oldState = stateCmd;

        uint16_t state[2];
        state[0] = state[1] = stateCmd;
        messageToJawTask(JawState, COMMAND, XY, state);
        rtFeedback.MlcCurFsm &= 0x00ff;
        rtFeedback.MlcCurFsm |= (stateCmd << 8);

        if(stateCmd == FSM_MANUAL)
        {
            uint16_t pos = 0;
            if(recv->OutU16_JawXControlModeSetting == 2){
                pos = recv->OutU16_JawXPositionSetting;
                messageToJawTask(JawState, PLAN_DATA, X, &pos);
            }
            else if(recv->OutU16_JawYControlModeSetting == 2){
                pos = recv->OutU16_JawYPositionSetting;
                messageToJawTask(JawState, PLAN_DATA, Y, &pos);
            }
        }
    }

    if(oldPlanCmd != planCmd)
    {
        LOG_I("plan cmd: %d -> %d\r\n",oldPlanCmd,planCmd);  
        switch(planCmd)
        {
        case NO_USE://Plan send finish
            if(oldPlanCmd == SEND_PLAN){
                secondPosFeedback.packIndexInOneBeam = 0;
                secondPosFeedback.errorCode = 0;
            }
        break;
        case SEND_PLAN://Plan send start
            oldBeamIndex = oldRadiationIndex = 0;
        break;
        case CLOSE_PLAN://clear plan
            if(stateCmd == FSM_IDLE){
                clearPlan();
                oldBeamIndex = 0;
                oldRadiationIndex = 0;
            }  
        break;
        default:    break;
        }
        oldPlanCmd = planCmd;
    }
    if(oldErrState != errReset)
    {
        if(errReset == 1){
            uint16_t state[2];
            state[0] = state[1] = FSM_IDLE;
            messageToJawTask(JawState, COMMAND, XY, state);
        }
        oldErrState = errReset;
    }
    if(((stateCmd == FSM_IDLE)||(stateCmd == FSM_SERVO))&&(rtBeamData.beamIndex > 0))
    {
        if((oldBeamIndex != rtBeamData.beamIndex) || (oldRadiationIndex != rtBeamData.radiationIndex))
        {
           // LOG_I("RI: %d.%d -> %d.%d\r\n",oldBeamIndex,oldRadiationIndex,rtBeamData.beamIndex,rtBeamData.radiationIndex);
            if(sendCPtoDevice(rtBeamData.beamIndex, rtBeamData.radiationIndex, JawState))
            {
                oldRadiationIndex = rtBeamData.radiationIndex;
                oldBeamIndex = rtBeamData.beamIndex;
            }
        }
    }

    return ethercat_send_data_update(send, sizeof(send_data));
}

 static int8_t non_realtime_tcp_callback(uint8_t sn)
 {
#ifdef IS_TCP_SERVER
    if(/*(stateCmd > FSM_NOSTATE )&&*/(planCmd == NO_USE))
    {
        if((isClientTypeMatch(sn, 0) > 0) && isSendPeriod(sn, 0))
        {
            operateSendMutex(1, 0, osWaitForever);//require
            updateNRTFeedback(ACTIVE);
            operateSendMutex(0, 0, 0);//release
        }  
        setTCPSendControlSignal(0, TCP_SEND_PERIOD);
    }
    else    setTCPSendControlSignal(0, STOP_SEND);//stop period feedback when PLC send or clear plan

    return tcp_send_process(sn);
#else
    // if(beam_cmd_get() == NO_USE)
    // {
    //     sendFeedback();
    // }
    return 0;
#endif
 }

static int8_t non_realtime_tcp_recv_data_callback(void)
{
    osEventFlagsSet(data_process_eventHandle, DATA_PROCESS_TCP_EVENT);
    return 0;
}

static int8_t non_realtime_fpga_recv_data_callback(void)
{
    osEventFlagsSet(data_process_eventHandle, DATA_PROCESS_FPGA_EVENT);
    return 0;
}

static int8_t data_process_init(void)
{
    int8_t ret = 0;
    ret = tcp_establish_cb_register(non_realtime_tcp_callback);
    if (ret != 0)
    {
        LOG_E("tcp callback register err:%d\r\n", ret);
        return ret;
    }

    ret = tcp_recv_data_callback_register(non_realtime_tcp_recv_data_callback);
    if (ret != 0)
    {
        LOG_E("tcp recv data callback register err:%d\r\n", ret);
        return ret;
    }

    ret = ethercat_slave_appl_cb_register(data_process_eventHandle, DATA_PROCESS_LAN_EVENT, realtime_ethercat_data_process);
    if (ret != 0)
    {
        LOG_E("ethercat callback register err:%d\r\n", ret);
        return ret;
    }

    ret = recv_from_fpga_callback_register(non_realtime_fpga_recv_data_callback);
    if (ret != 0)
    {
        LOG_E("recv fpga callback register err:%d\r\n", ret);
        return ret;
    }

    planDataInit();

    return 0;
}

static void data_process_entry(void *argument)
{
  /* USER CODE BEGIN data_process_entry */
  /* Infinite loop */
  osStatus_t stat = 0;
  uint32_t event_flag = 0;  
  uint8_t recv_from_fpga_buf[RECV_BUF_LEN];
  TCP_DATA_t tcp_info = {0};

  osDelay(1000);

  data_process_init();  /* register callback functions for tcp 、ethercat、fpga */

  /* here need add start functions for data receive */
  recv_from_fpga_data_start();

  for(;;)
  {
    event_flag = osEventFlagsWait(data_process_eventHandle, DATA_PROCESS_FPGA_EVENT | DATA_PROCESS_LAN_EVENT | DATA_PROCESS_TCP_EVENT, osFlagsWaitAny, osWaitForever);
    if (event_flag & DATA_PROCESS_LAN_EVENT)
    {
        ethercat_recv_data_update_with_block(0);
    }

    if (event_flag & DATA_PROCESS_FPGA_EVENT)
    {
        stat = recv_from_fpga_data_get(recv_from_fpga_buf);
        if (stat == osOK)
        {
            non_realtime_fpga_data_process(recv_from_fpga_buf);
          // LOG_I("buf: %x %x %x %x\r\n", recv_from_fpga_buf[0], recv_from_fpga_buf[1], recv_from_fpga_buf[2], recv_from_fpga_buf[3]);
        }
        else
        {
            LOG_E("no msg in spi2 rx queue:%d\r\n", stat);
        }
    }

    if (event_flag & DATA_PROCESS_TCP_EVENT)
    {
        stat = tcp_client_data_recv_get_with_block(&tcp_info, 0);
        if (stat == osOK)
        {
    #ifdef IS_TCP_SERVER
          tcp_recv_process(&tcp_info);
    #endif
            // LOG_I("tcp_info len:%d\r\n", tcp_info.Len);
            // LOG_I("tcp_info %x %x %x %x\r\n", tcp_info.gDATABUF[0], tcp_info.gDATABUF[1], tcp_info.gDATABUF[2], tcp_info.gDATABUF[3]);
        }
        else
        {
            LOG_E("no msg in tcp rx queue:%d\r\n", stat);
        }
    } 
  }
  /* USER CODE END data_process_entry */
}

static int8_t main_app_thread_init(void)
{
    osThreadAttr_t recv_data_process_thread_attributes = {
    .name = "recv_data_process_thread",
    .stack_size = 2048 * 4,
    .priority = (osPriority_t) osPriorityAboveNormal,
    };

    // osThreadAttr_t DataProcess_attributes = {
    // .name = "DataProcess",
    // .stack_size = 1024 * 4,
    // .priority = (osPriority_t) osPriorityNormal,
    // };

    data_process_eventHandle = osEventFlagsNew(NULL);
    if (data_process_eventHandle == NULL)
    {
        LOG_E("event data process create failed\r\n");
        return -1;
    }

    // osThreadId_t DataProcessHandle = osThreadNew(DataProccessTask, NULL, &DataProcess_attributes);
    // if (DataProcessHandle == NULL)
    // {
    //     LOG_E("thread data process create failed\r\n");
    //     return -1;
    // }

    osThreadId_t recv_data_process_threadHandle = osThreadNew(data_process_entry, NULL, &recv_data_process_thread_attributes);
    if (recv_data_process_threadHandle == NULL)
    {
        LOG_E("thread recv data process create failed\r\n");
        return -1;
    }

    return 0;
}
INIT_APP_EXPORT(main_app_thread_init);