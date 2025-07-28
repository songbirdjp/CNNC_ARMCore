#include "dose_error.h"
#include "bgm_app.h"
#include "plan_data.h"
#include "timestamp.h"
#include "ulog.h"

struct dose_err_info
{
    union
    {
        uint32_t bytes;
        struct
        {
            uint32_t adc_para : 1;
            uint32_t dac_para : 1;
            uint32_t trigger_interval_min : 1;
            uint32_t dose_mode : 1;
            uint32_t pulse_mode : 1;
            uint32_t prf_para : 1;
            uint32_t dose_meter : 1;
            uint32_t dose_meter_dummy : 1;
            uint32_t cp_num : 1;
            uint32_t ri_num : 1;
            uint32_t cp_tolerate : 1;
            uint32_t cp_ri_map : 1;
            uint32_t ri_info : 1;
            uint32_t beam_info : 1;
            uint32_t interlock_override : 1;
            uint32_t unready_override : 1;
            uint32_t timestamp : 1;
            uint32_t reserved : 15;
        }bits;
    }error_code;

    uint16_t cp_idx_pre;
    uint16_t ri_idx_pre;
};

static struct dose_err_info dose_err_info_obj[BGM_UART_DOSE2] = {0};
static struct dose_err_info *dose_err_info_object_get(enum uart_id id)
{
    if (id < BGM_UART_DOSE1 || id > BGM_UART_DOSE2)
    {
        LOG_E("invalid dose uart id: %d\r\n", id);
        return NULL;
    }

    return &dose_err_info_obj[id - BGM_UART_DOSE1];
}

int8_t dose_err_info_clear(void)
{
    struct dose_err_info *obj = NULL;

    for (uint8_t i = BGM_UART_DOSE1; i < BGM_UART_DOSE2; i++)
    {
        obj = dose_err_info_object_get(i);
        obj->error_code.bytes = 0;
        obj->cp_idx_pre = 0;
        obj->ri_idx_pre = 0;
    }

    return 0;
}
uint32_t dose_err_info_get(enum uart_id id)
{
    return dose_err_info_object_get(id)->error_code.bytes;
}
int8_t dose_para_check(enum dose_para_type para_type, enum uart_id id, uint16_t idx, void *value)
{
    if (id != BGM_UART_DOSE1 && id != BGM_UART_DOSE2)
    {
        LOG_E("invalid dose uart id: %d\r\n", id);
        return -1;
    }

    if (value == NULL)
    {
        LOG_E("dose parameter value is NULL\r\n");
        return -2;
    }

    int8_t ret = 0;
    struct dose_err_info *dose_obj = dose_err_info_object_get(id);

    struct bgm_data_info info = {0};
    struct bgm_data_info *obj = bgm_data_info_get();
    osMutexAcquire(obj->mutex, osWaitForever);
    memcpy(&info, obj, sizeof(struct bgm_data_info) - sizeof(osMutexId_t));
    osMutexRelease(obj->mutex);

    switch (para_type)
    {
    case DOSE_PARA_ADC:
        {
            uint32_t adc_value = id == BGM_UART_DOSE1 ? info.cali_dose1_adc : info.cali_dose2_adc;
            if (*(uint32_t *)value != adc_value)
            {
                LOG_E("dose adc value mismatch: %d, expected: %d\r\n", *(uint32_t *)value, adc_value);
                dose_obj->error_code.bits.adc_para = 1;
            }
        }
        break;
    case DOSE_PARA_DAC:
        {
            uint16_t dac_value = id == BGM_UART_DOSE1 ? info.cali_dose1_dac : info.cali_dose2_dac;
            if (*(uint16_t *)value != dac_value)
            {
                LOG_E("dose dac value mismatch: %d, expected: %d\r\n", *(uint16_t *)value, dac_value);
                dose_obj->error_code.bits.dac_para = 1;
            }
        }
        break;
    case DOSE_PARA_TRIGGER_INTERVAL_MIN:
        if (*(uint16_t *)value != info.trigger_interval_min)
        {
            LOG_E("dose trigger interval mismatch: %d, expected: %d\r\n", *(uint16_t *)value, info.trigger_interval_min);
            dose_obj->error_code.bits.trigger_interval_min = 1;
        }
        break;
    case DOSE_PARA_DOSE_MODE:
        if (*(uint8_t *)value != info.dose_mode)
        {
            LOG_E("dose mode mismatch: %d, expected: %d\r\n", *(uint8_t *)value, info.dose_mode);
            dose_obj->error_code.bits.dose_mode = 1;
        }
        break;
    case DOSE_PARA_PULSE_MODE:
        if (*(uint8_t *)value != info.pulse_mode)
        {
            LOG_E("dose pulse mode mismatch: %d, expected: %d\r\n", *(uint8_t *)value, info.pulse_mode);
            dose_obj->error_code.bits.pulse_mode = 1;
        }
        break;
    case DOSE_PARA_PRF:
        if (*(uint8_t *)value != info.cali_prf)
        {
            LOG_E("dose PRF mismatch: %d, expected: %d\r\n", *(uint8_t *)value, info.cali_prf);
            dose_obj->error_code.bits.prf_para = 1;
        }
        break;
    case DOSE_PARA_DOSE_METER:
        if (fabs(*(float *)value - info.dose_meter) > 1e-6)
        {
            LOG_E("dose meter value mismatch: %f, expected: %f\r\n", *(float *)value, info.dose_meter);
            dose_obj->error_code.bits.dose_meter = 1;
        }
        break;
    case DOSE_PARA_CP_NUM:
        {
            struct one_beam_order beam_obj = {0};

            ret = getPlanBeamData(info.beam_id, &beam_obj);
            if (ret != 0)
            {
                return -3;
            }

            if (*(uint16_t *)value != beam_obj.info->CPQuantityInBeam)
            {
                LOG_E("dose cp num mismatch: %d, expected: %d\r\n", *(uint16_t *)value, beam_obj.info->CPQuantityInBeam);
                dose_obj->error_code.bits.cp_num = 1;
            }
        }
        break;
    case DOSE_PARA_RI_NUM:
        {
            struct one_beam_order beam_obj = {0};

            ret = getPlanBeamData(info.beam_id, &beam_obj);
            if (ret != 0)
            {
                return -3;
            }

            if (*(uint16_t *)value != beam_obj.info->RIQuantityInBeam)
            {
                LOG_E("dose ri num mismatch: %d, expected: %d\r\n", *(uint16_t *)value, beam_obj.info->RIQuantityInBeam);
                dose_obj->error_code.bits.ri_num = 1;
            }
        }
        break;
    case DOSE_PARA_CP_TOLERATE:
        if (*(uint8_t *)value != info.cp_tolerate)
        {
            LOG_E("dose cp tolerate mismatch: %d, expected: %d\r\n", *(uint8_t *)value, info.cp_tolerate);
            dose_obj->error_code.bits.cp_tolerate = 1;
        }
        break;
    case DOSE_PARA_CP_RI_MAP:
        {
            struct one_beam_order beam_obj = {0};

            ret = getPlanBeamData(info.beam_id, &beam_obj);
            if (ret != 0)
            {
                return -3;
            }

            if (idx > beam_obj.info->CPQuantityInBeam || idx == 0)
            {
                LOG_E("invalid cp index: %d\r\n", idx);
                return -4;
            }

            if (idx != 1 && (dose_obj->cp_idx_pre + 1 != idx))
            {
                LOG_E("dose cp index not continuous: %d, pre: %d\r\n", idx, dose_obj->cp_idx_pre);
                dose_obj->error_code.bits.cp_ri_map = 1;
                dose_obj->cp_idx_pre = idx;
            }
            else if (*(uint16_t *)value != beam_obj.cp_ri_map[idx - 1])
            {
                LOG_E("dose cp ri map not match: %d, expected: %d\r\n", *(uint16_t *)value, beam_obj.cp_ri_map[idx - 1]);
                dose_obj->error_code.bits.cp_ri_map = 1;
                dose_obj->cp_idx_pre = idx;
            }
        }
        break;
    case DOSE_PARA_RI_INFO:
        {
            struct one_beam_order beam_obj = {0};

            ret = getPlanBeamData(info.beam_id, &beam_obj);
            if (ret != 0)
            {
                return -3;
            }

            if (idx > beam_obj.info->RIQuantityInBeam || idx == 0)
            {
                LOG_E("invalid ri index: %d\r\n", idx);
                return -4;
            }

            if (idx != 1 && (dose_obj->ri_idx_pre + 1 != idx))
            {
                LOG_E("dose ri index not continuous: %d, pre: %d\r\n", idx, dose_obj->ri_idx_pre);
                dose_obj->error_code.bits.ri_info = 1;
                dose_obj->ri_idx_pre = idx;
            }
            else
            {
                float *tmp = (float *)value;
                if ((fabs(*tmp - beam_obj.ri_data[idx - 1].fCumulativeDose) > 1e-6)
                    || (fabs(*(tmp + 1) - beam_obj.ri_data[idx - 1].fDoseRate) > 1e-6)
                    || (fabs(*(tmp + 2) - beam_obj.ri_data[idx - 1].DeliveryTime) > 1e-6))
                {
                    LOG_E("dose ri info not match: %f, %f, %f, expect: %f, %f, %f\r\n", *tmp, *(tmp + 1), *(tmp + 2), 
                          beam_obj.ri_data[idx - 1].fCumulativeDose, beam_obj.ri_data[idx - 1].fDoseRate, beam_obj.ri_data[idx - 1].DeliveryTime);
                    dose_obj->error_code.bits.ri_info = 1;
                    dose_obj->ri_idx_pre = idx;
                }
            }
        }
        break;
    case DOSE_PARA_BEAM_INFO:
        {
            struct one_beam_order beam_obj = {0};

            ret = getPlanBeamData(info.beam_id, &beam_obj);
            if (ret != 0)
            {
                return -3;
            }

            uint8_t *tmp = (uint8_t *)value;

            if (*tmp != 0 || *(tmp + 1) != beam_obj.info->radiationType || *(tmp + 2) != beam_obj.info->deliveryType)
            {
                LOG_E("dose beam info not match: %d, %d, %d, expected: %d, %d, %d\r\n", *tmp, *(tmp + 1), *(tmp + 2), 
                      0, beam_obj.info->radiationType, beam_obj.info->deliveryType);
                dose_obj->error_code.bits.beam_info = 1;
            }
        }
        break;
    case DOSE_PARA_INTERLOCK_OVERRIDE:
        if (*(uint32_t *)value != info.interlock_override)
        {
            LOG_E("dose interlock override mismatch: %d, expected: %d\r\n", *(uint32_t *)value, info.interlock_override);
            dose_obj->error_code.bits.interlock_override = 1;
        }
        break;
    case DOSE_PARA_UNREADY_OVERRIDE:
        if (*(uint32_t *)value != info.unready_override)
        {
            LOG_E("dose unready override mismatch: %d, expected: %d\r\n", *(uint32_t *)value, info.unready_override);
            dose_obj->error_code.bits.unready_override = 1;
        }
        break;
    case DOSE_PARA_TIMESTAMP:
        if (abs(*(uint64_t *)value - timestamp_ns_get() > 10000))   /* a tolerance of 10ms */
        {
            LOG_E("dose timestamp mismatch: %llu, expected: %llu\r\n", *(uint64_t *)value, timestamp_ns_get());
            dose_obj->error_code.bits.timestamp = 1;
        }
        break;
    default:
        LOG_E("invalid dose parameter type: %d\r\n", para_type);
        ret = -3;
        break;
    }

    return ret;
}


#ifndef DOSE_ERROR_TEST
#include "shell.h"
static int8_t dose_error_test(uint8_t argc, char **argv)
{
    switch (atoi(argv[1]))
    {
    case 0:
        struct dose_err_info *obj = dose_err_info_object_get(atoi(argv[2]));
        LOG_I("[%d] dose error info: %#.8x\r\n", atoi(argv[2]), obj->error_code.bytes);
        break;
    case 1:
        if (argc < 4)
        {
            LOG_E("Usage: dose_error_test <para_type> <value>\r\n");
            return -1;
        }

        enum dose_para_type para_type = atoi(argv[2]);
        uint32_t value = atoi(argv[3]);

        int8_t ret = dose_para_check(para_type, BGM_UART_DOSE1, 1, &value);
        if (ret != 0)
        {
            LOG_E("dose parameter check failed: %d\r\n", ret);
        }
        break;
    default:
        break;
    }

    return 0;
}
MSH_CMD_EXPORT_ALIAS(dose_error_test, dose_error_test, test dose error);
#endif