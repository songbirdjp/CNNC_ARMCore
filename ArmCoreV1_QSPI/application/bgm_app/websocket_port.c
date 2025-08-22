#include "websocket_port.h"
#include "plan_data.h"
#include "ulog.h"
#include "afc_cmd.h"
#include "bgm_app.h"
#include "bgm_def.h"

typedef struct
{
    uint8_t id;
    uint8_t type;
    uint16_t len;
    uint8_t *data;
}CMD2UART_DATA;


/* -------------------------------------------------------------------------- */
/*                            websocket cmd for afc                           */
/* -------------------------------------------------------------------------- */
void nrtCommand_AFCParameterParse(CMD2UART_DATA* nrtCommand)
{
    switch(nrtCommand->data[1])
    {
        case 0x00:
            AFC_SetAFCControlMode(nrtCommand->data[2]);
            break;
        case 0x01:
           // AFC_SetADCSampleMode(nrtCommand->data[2]);
            break;
        case 0x02:
            AFC_SetADCSampleDelay((nrtCommand->data[3]<<8)|nrtCommand->data[2]);
            break;
        case 0x03:
            AFC_DeleteADCData();
            break;
        case 0x04:
            //AFC_GetADCValueByFrame();
            break;
        default:
            break;
    }
}
void nrtCommand_MagMotorParse(CMD2UART_DATA* nrtCommand)
{
    switch(nrtCommand->data[1])
    {
        case 0x00:
            AFC_IS_MagMotorFindZeroOK();
            break;
        case 0x01:
            AFC_MagMotorSetPos((nrtCommand->data[3]<<8)|nrtCommand->data[2]);
            break;
        case 0x02:
            AFC_MagMotorRunByStep(nrtCommand->data[2],(nrtCommand->data[4]<<8)|nrtCommand->data[3]);
            break;
        case 0x03:
            AFC_MagMotorGetEncValue();
            break;
        case 0x04:
            AFC_MagMotorSetPresetPos((nrtCommand->data[3]<<8)|nrtCommand->data[2]);
            break;
        case 0x05:
            // AFC_MagMotorGetPresetPos();
            break;
        default:
            break;
    }
}
void nrtCommand_AFTMotorParse(CMD2UART_DATA* nrtCommand)
{
    switch(nrtCommand->data[1])
    {
        case 0x00:
            AFC_IS_AFTMotorFindZeroOK();
            break;
        case 0x01:
            AFC_AFTMotorSetPos((nrtCommand->data[3]<<8)|nrtCommand->data[2]);
            break;
        case 0x02:
            AFC_AFTMotorRunByStep(nrtCommand->data[2],(nrtCommand->data[4]<<8)|nrtCommand->data[3]);
            break;
        case 0x03:
            AFC_AFTMotorGetEncValue();
            break;
        case 0x04:
            AFC_AFTMotorSetPresetPos((nrtCommand->data[3]<<8)|nrtCommand->data[2]);
            break;
        case 0x05:
            AFC_SetAFTBrakeStatus(nrtCommand->data[2]);
            break;
        case 0x06:
            AFC_SetAFTMotorStatus(nrtCommand->data[2]);
            break;
        case 0x07:
            //AFC_AFTMotorSetDeadZone((nrtCommand->data[3]<<8)|nrtCommand->data[2]);
            break;
        default:
            break;
    }
}
void nrtCommand_PowerStatusParse(CMD2UART_DATA* nrtCommand)
{
    switch(nrtCommand->data[1])
    {
        case 0x00:
            break;
        case 0x01:
            break;
        case 0x02:
            break;
        default:
            break;
    }
}

static int8_t afc_cmd_parse(APP_DATA_RECV *info)
{
    static CMD2UART_DATA nrtCommand = {0};
    int8_t ret = 0;

    nrtCommand.id = info->tcpData[6];
    nrtCommand.type = info->tcpData[7];
    nrtCommand.len = (info->tcpData[9] << 8) + info->tcpData[8];
    memcpy(nrtCommand.data, &info->tcpData[10], nrtCommand.len);
    switch(nrtCommand.data[0])
    {
        case 0x01:
            nrtCommand_AFCParameterParse(&nrtCommand);
            break;
        case 0x40:
            nrtCommand_MagMotorParse(&nrtCommand);
            break;
        case 0x41:
            nrtCommand_AFTMotorParse(&nrtCommand);
            break;
        case 0x60:
            break;
        case 0xEE:
            AFC_SetAFCREBOOT();
            break;
        default:
            ret = -1;
            break;
    }

    return ret;
}



/* -------------------------------------------------------------------------- */
/*                           websocket cmd for dose                           */
/* -------------------------------------------------------------------------- */
static int8_t dose_cmd_parse(APP_DATA_RECV *info)
{
    int8_t ret = 0;

    return ret;
}


/* -------------------------------------------------------------------------- */
/*                         websocket cmd for arm core                         */
/* -------------------------------------------------------------------------- */
static int8_t arm_core_cmd_parse(APP_DATA_RECV *info)
{
    int8_t ret = 0;
    uint32_t value = 0;
    uint8_t *cmd = &info->tcpData[10];

    struct bgm_data_info *obj = bgm_data_info_get();

    osMutexAcquire(obj->mutex, osWaitForever);

    switch (cmd[0])
    {
    case 0x01:
        switch (cmd[1])
        {
        case 0x00:
            // obj->dose_mode = cmd[2];
            LOG_I("set dose mode: %d\r\n", cmd[2]);
            break;
        case 0x01:
            obj->pulse_mode = cmd[2];
            LOG_I("set pulse mode: %d\r\n", obj->pulse_mode);
            break;
        case 0x02:
            obj->cali_prf = cmd[2];
            LOG_I("set cali prf: %d\r\n", obj->cali_prf);
            break;
        case 0x03:
            obj->cali_mode = cmd[2];
            LOG_I("set cali mode: %d\r\n", obj->cali_mode);
            break;
        case 0x04:
            value = cmd[2] | cmd[3] << 8 | cmd[4] << 16 | cmd[5] << 24;
            obj->dose_meter = *(float *)&value;
            LOG_I("set dose meter: %f\r\n", obj->dose_meter);
            break;
        case 0x05:
            value = cmd[2] | cmd[3] << 8 | cmd[4] << 16 | cmd[5] << 24;
            obj->dose_meter_dummy = *(float *)&value;
            LOG_I("set dose meter dummy: %f\r\n", obj->dose_meter_dummy);
            break;
        default:
            LOG_E("invalid arm core sub cmd: %x\r\n", cmd[1]);
            ret = -1;
            break;
        }
        break;
    default:
        LOG_E("invalid arm core cmd: %x\r\n", cmd[0]);
        ret = -1;
        break;
    }

    osMutexRelease(obj->mutex);

    if (ret != 0)
    {
        LOG_E("arm core cmd parse err: %d\r\n", ret);
    }

    return ret;
}



/* -------------------------------------------------------------------------- */
/*                       websocket data for calibration                       */
/* -------------------------------------------------------------------------- */
static int8_t cali_data_set(APP_DATA_RECV *info)
{
    int8_t ret = 0;

    if ((info->tcpData[5] << 8 | info->tcpData[4]) != 30)
    {
        LOG_E("cali data len err: %d\r\n", (info->tcpData[4] | info->tcpData[5] << 8));
        return -1;
    }

    struct calibration_para
    {
        uint16_t dose1_dac_ch1;
        uint16_t dose1_dac_ch2;
        uint32_t dose1_adc_ch1;
        uint32_t dose1_adc_ch2;

        uint16_t dose2_dac_ch1;
        uint16_t dose2_dac_ch2;
        uint32_t dose2_adc_ch1;
        uint32_t dose2_adc_ch2;
    };

    struct calibration_para para = {0};
    memcpy(&para, &info->tcpData[6], sizeof(struct calibration_para));

    struct bgm_data_info *obj = bgm_data_info_get();
    osMutexAcquire(obj->mutex, osWaitForever);

    obj->cali_dose1_dac[0] = para.dose1_dac_ch1;
    obj->cali_dose1_dac[1] = para.dose1_dac_ch2;
    obj->cali_dose1_adc[0] = para.dose1_adc_ch1;
    obj->cali_dose1_adc[1] = para.dose1_adc_ch2;
    obj->cali_dose2_dac[0] = para.dose2_dac_ch1;
    obj->cali_dose2_dac[1] = para.dose2_dac_ch2;
    obj->cali_dose2_adc[0] = para.dose2_adc_ch1;
    obj->cali_dose2_adc[1] = para.dose2_adc_ch2;

    osMutexRelease(obj->mutex);

    return ret;
}



/* --------------------------- external interface --------------------------- */
int8_t websocket_cmd_parse(APP_DATA_RECV *info)
{
    int8_t ret = 0;

    switch (info->tcpData[1] << 8 | info->tcpData[0])
    {
    case TAG_PLAN_DATA_SETTING:
        osDelay(1);
        ret = nrtRecvPlan(info);
        if (ret != 0)
        {
            LOG_E("nrtRecvPlan err: %d\r\n", ret);
        }
        ret = planFeedback(info->sn, TAG_PLAN_DATA_SETTING);
        if (ret < 0)
        {
            LOG_E("planFeedback err: %d\r\n", ret);
        }
        break;
    case TAG_PLAN_DATA_CLEAR:
        ret = clearPlan();
        ret |= dose_data_info_set(BGM_UART_DOSE1, DOSE_INFO_PLAN_DATA_CLEAR, NULL, 0);
        ret |= dose_data_info_set(BGM_UART_DOSE2, DOSE_INFO_PLAN_DATA_CLEAR, NULL, 0);
        break;
    case TAG_CALI_DATA_SETTING:
        ret = cali_data_set(info);
        break;
    case TAG_BGM_ARM_CMD:
        ret = arm_core_cmd_parse(info);
        break;
    case TAG_BGM_AFC_CMD:
        ret = afc_cmd_parse(info);
        break;
    case TAG_BGM_DOSE1_CMD:
        ret = dose_cmd_parse(info);
        break;
    case TAG_BGM_DOSE2_CMD:
        ret = dose_cmd_parse(info);
        break;
    default:
        LOG_E("unknown tag: %x\r\n", info->tcpData[1] << 8 |info->tcpData[0]);
        ret = -1;
        break;
    }

    return ret;
}