#include "bgm_def.h"
#include "sys_cfg.h"
#include <stddef.h>
#include <stdlib.h>
#include "plan_data.h"
#include "ulog.h"

#define BGM_ARM_HW_VERSION  0x19

/* 1. bgm with dose board communication interface */
int8_t dose_handshake(enum uart_id id)
{
    int8_t ret = 0;
    uint8_t *fw_ver = system_info_get()->fw_version;
    uint8_t versionToHandshake[5] = {0};

    versionToHandshake[0] = BGM_ARM_HW_VERSION;
    versionToHandshake[1] = strtoul(&fw_ver[0], NULL, 10);;//sw version XX
    versionToHandshake[2] = strtoul(&fw_ver[3], NULL, 10);;//sw version YY
    versionToHandshake[3] = strtoul(&fw_ver[6], NULL, 10);;//sw version ZZ
    versionToHandshake[4] = (uint8_t)id;

    ret = dose_data_info_set(id, DOSE_INFO_VERSION, versionToHandshake, sizeof(versionToHandshake));
    if(ret != 0)
    {
        LOG_E("dose info set err: %d\r\n", ret);
    }

    return ret;
}

int8_t dose_adc_value_set(enum uart_id id, uint32_t *value)
{
    return dose_data_info_set(id, DOSE_INFO_ADC_CALI, (void *)value, 0);
}

int8_t dose_dac_value_set(enum uart_id id, uint32_t *value)
{
    return dose_data_info_set(id, DOSE_INFO_DAC_CALI, (void *)value, 0);
}

int8_t dose_meter_value_set(enum uart_id id, float dose_meter)
{
    int8_t ret = 0;

    uint16_t value = (uint16_t)(dose_meter * 10.0f);

    ret = dose_data_info_set(id, DOSE_INFO_METER_SET, &value, 0);
    if(ret != 0)
    {
        LOG_E("dose info set err: %d\r\n", ret);
    }

    return ret;
}

float dose_meter_value_get(enum uart_id id)
{
    return dose_data_info_get(id, DOSE_INFO_METER_GET, NULL);
}

int8_t dose_rate_value_set(enum uart_id id, float *dose_rate)
{
    int8_t ret = 0;

    ret = dose_data_info_get(id, DOSE_INFO_DOSE_RATE_SET, dose_rate);
    if(ret != 0)
    {
        LOG_E("dose info get err: %d\r\n", ret);
    }

    return ret;  
}

float dose_rate_value_get(enum uart_id id)
{
    return dose_data_info_get(id, DOSE_INFO_DOSE_RATE_GET, NULL);
}

int8_t dose_prf_value_set(enum uart_id id, uint8_t *prf)
{
    return dose_data_info_set(id, DOSE_INFO_PRF_SET, (void *)prf, 0);
}

int8_t dose_generate_mode_set(enum uart_id id, uint8_t *mode)
{
    return dose_data_info_set(id, DOSE_INFO_GENERATE_MODE_SET, (void *)mode, 0);
}

int8_t dose_pulse_mode_set(enum uart_id id, uint8_t *pulse_mode)
{
    return dose_data_info_set(id, DOSE_INFO_PULSE_MODE_SET, (void *)pulse_mode, 0);
}

int8_t dose_fsm_state_set(enum uart_id id, enum dose_fsm_state state)
{
    int8_t ret = 0;

    uint8_t stat = (uint8_t)state;

    ret = dose_data_info_set(id, DOSE_INFO_FSM_STATE_SET, &stat, sizeof(stat));
    if(ret != 0)
    {
        LOG_E("dose info set err: %d\r\n", ret);
    }

    return ret;
}

enum dose_fsm_state dose_fsm_state_get(enum uart_id id)
{
    return dose_data_info_get(id, DOSE_INFO_FSM_STATE_GET, NULL);
}

uint16_t dose_interlock_get(enum uart_id id)
{
    return dose_data_info_get(id, DOSE_INFO_INTERLOCK_GET, NULL);
}

int8_t dose_state_polling(enum uart_id id)
{
    int8_t ret = 0;

    ret = dose_data_info_set(id, DOSE_INFO_FSM_STATE_GET, NULL, 0);
    ret |= dose_data_info_set(id, DOSE_INFO_INTERLOCK_GET, NULL, 0);
    if(ret != 0)
    {
        LOG_E("dose info set err: %d\r\n", ret);
    }

    return ret;
}

int8_t dose_beam_cumulated_clear(enum uart_id id)
{
    int8_t ret = 0;

    ret = dose_data_info_set(id, DOSE_INFO_CUMULATED_CLEAR, NULL, 0);
    if(ret != 0)
    {
        LOG_E("dose info set err: %d\r\n", ret);
    }

    return ret;
}

int8_t beam_deliver_type_get(uint16_t beam_id, uint8_t *deliver_type)
{
    int8_t ret = 0;
    struct one_beam_order beam_obj = {0};

    ret = getPlanBeamData(beam_id, &beam_obj);
    if (ret != 0)
    {
        LOG_E("get beam data err: %d\r\n", ret);
        return -1;
    }

    *deliver_type = beam_obj.info->deliveryType;

    return 0;
}

uint16_t dose_radiation_index_get(enum uart_id id)
{
    return dose_data_info_get(id, DOSE_INFO_RADIATION_INDEX_GET, NULL);
}

int8_t dose_beam_info_set(enum uart_id id, uint8_t *data)
{
    return dose_data_info_set(id, DOSE_INFO_BEAM_TYPE_SET, data, 0);
}

int8_t dose_beam_parameter_set(enum uart_id id, uint16_t beam_id)
{
    int8_t ret = 0;
    struct one_beam_order beam_obj = {0};

    ret = getPlanBeamData(beam_id, &beam_obj);
    if (ret != 0)
    {
        LOG_E("get beam data err: %d\r\n", ret);
        return -1;
    }

    ret = dose_data_info_set(id, DOSE_INFO_BEAM_SET, &beam_obj, 0);

    /* 1. beam unlock */

    /* 2. beam meter */

    /* 3. beam cp & ri num */

    /* 4. beam cp & ri map */

    /* 5. beam ri value */

    /* 6. beam lock and validate */

    return ret;
}

int8_t dose_radiation_data_get(enum uart_id id)
{
    int8_t ret = 0;

    ret = dose_data_info_set(id, DOSE_INFO_RADIATION_GET, NULL, 0);
    if(ret != 0)
    {
        LOG_E("dose info set err: %d\r\n", ret);
    }

    return ret;
}

int8_t dose_radiation_index_set(enum uart_id id, uint16_t index, uint8_t emergency)
{
    int8_t ret = 0;
    uint8_t data[3] = {0};

    data[0] = index;
    data[1] = index >> 8;
    data[2] = emergency;

    ret = dose_data_info_set(id, DOSE_INFO_RADIATION_SET, data, sizeof(data) / sizeof(uint8_t));
    if(ret != 0)
    {
        LOG_E("dose info set err: %d\r\n", ret);
    }

    return ret;
}

#ifndef DOSE_CMD_TEST
#include "shell.h"
static int8_t dose_cmd_test(int8_t argc, uint8_t **argv)
{
    int8_t ret = 0;

    if(argc != 4)
    {
        printf("dose_cmd_test: invalid arguments\r\n");
        return -1;
    }

    enum uart_id id = atoi(argv[2]);
    uint32_t value = atoi(argv[3]);

    switch (atoi(argv[1]))
    {
    case 0:
        dose_handshake(id);
        break;
    case 1:
        dose_adc_value_set(id, &value);
        break;
    case 2:
        dose_dac_value_set(id, &value);
        break;
    case 3:
        dose_fsm_state_set(id, DOSE_FSM_STATE_IDLE);
        break;
    case 4:
        printf("state = %d\r\n", dose_fsm_state_get(id));
        break;
    case 5:
        dose_state_polling(id);
        break;
    case 6:
        dose_beam_cumulated_clear(id);
        break;
    case 7:
        dose_beam_parameter_set(id, value);
        break;
    case 8:
        dose_radiation_data_get(id);
        break;
    case 9:
        dose_radiation_index_set(id, value, 0);
        break;
    case 10:
        dose_meter_value_set(id, value);
        break;
    case 11:
        dose_prf_value_set(id, &value);
        break;
    case 12:
        dose_generate_mode_set(id, &value);
        break;
    case 13:
        dose_pulse_mode_set(id, &value);
        break;
    default:
        break;
    }

    return ret;
}
MSH_CMD_EXPORT_ALIAS(dose_cmd_test, dose_cmd_test, test dose cmd);
#endif



/* 2. bgm with afc board communication interface */
void BGM_SendCmd(enum uart_id uartID, uint8_t cmdType, uint8_t *cmdData, uint8_t len)
{
    uint16_t length = len;
    struct cmd_object BGMCmdToSend;
    BGMCmdToSend.id.bits.cmd_id = 0;
    BGMCmdToSend.id.bits.cmd_ack = 1;
    BGMCmdToSend.type = cmdType;
    BGMCmdToSend.len = &length;
    BGMCmdToSend.data = cmdData;
    uart_cmd_write(uartID,&BGMCmdToSend);
}
