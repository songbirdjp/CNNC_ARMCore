#include "interlock_app.h"
#include "init_call.h"
#include "io_port.h"
#include "bgm_app.h"
#include "bgm_error.h"
#include "bgm_def.h"
#include "dose_error.h"
#include "ulog.h"


struct locked_info
{
    uint32_t io_locked_flag;
    uint32_t io_stat_locked;

    uint32_t bgm_err_locked_flag;
    uint32_t bgm_err_stat_locked;

    uint32_t dose_err_locked_flag;
    uint32_t dose_err_stat_locked[2];

    uint32_t afc_err_locked_flag;
    uint32_t afc_err_stat_locked;

    osMutexId_t mutex;
};

static struct locked_info locked_info = {0};
static struct locked_info *locked_info_obj_get(void)
{
    return &locked_info;
}

static int8_t io_state_process(struct io_status *state)
{
    int8_t ret = 0;
    struct bgm_data_info *bgm_obj = bgm_data_info_get();
    osMutexAcquire(bgm_obj->mutex, osWaitForever);
    enum bgm_fsm_state fsm_state_cur = bgm_obj->fsm_state;
    osMutexRelease(bgm_obj->mutex);

    /* 1. lock io status */
    if (fsm_state_cur == BGM_STATE_WORK && (state->extend_status.current.bytes | state->detect_status.bytes << 16) != 0xFFFFFFFF)
    {
        // LOG_E("lock io status: %#.8x\r\n", state->extend_status.current.bytes | state->detect_status.bytes << 16);
        ret = interlock_locked_set(LOCKED_TYPE_IO, state->extend_status.current.bytes | state->detect_status.bytes << 16, 0);
    }

    return 0;
}


static int8_t locked_info_init(void)
{
    osMutexAttr_t attr = {
    .name = "interlock_mutex",
    .attr_bits = osMutexRecursive | osMutexPrioInherit
    };

    locked_info_obj_get()->mutex = osMutexNew(&attr);
    if (locked_info_obj_get()->mutex == NULL)
    {
        LOG_E("interlock mutex create failed\r\n");
        return -1;
    }

    int8_t ret = io_register_callback(io_state_process);
    if (ret != 0)
    {
        LOG_E("io register callback err: %d\r\n", ret);
        return -2;
    }

    return 0;
}
INIT_ENV_EXPORT(locked_info_init);

static struct interlock interlock_object = {0};
static struct interlock *interlock_obj_get(void)
{
    return &interlock_object;
}

int8_t interlock_locked_set(enum locked_type type, uint32_t value, uint32_t value_1)
{
    int8_t ret = 0;
    struct locked_info *obj = locked_info_obj_get();
    osMutexAcquire(obj->mutex, osWaitForever);

    switch (type)
    {
    case LOCKED_TYPE_IO:
        obj->io_locked_flag = 1;
        obj->io_stat_locked = value;
        break;
    case LOCKED_TYPE_BGM_ERR:
        obj->bgm_err_locked_flag = 1;
        obj->bgm_err_stat_locked = value;
        break;
    case LOCKED_TYPE_DOSE_ERR:
        obj->dose_err_locked_flag = 1;
        obj->dose_err_stat_locked[0] = value;
        obj->dose_err_stat_locked[1] = value_1;
        break;
    case LOCKED_TYPE_AFC_ERR:
        obj->afc_err_locked_flag = 1;
        obj->afc_err_stat_locked = value;
        break;
    default:
        LOG_E("invalid locked type: %d\r\n", type);
        ret = -1;
        break;
    }

    osMutexRelease(obj->mutex);

    return ret;
}

int8_t interlock_fault_clear(void)
{
    struct bgm_data_info *bgm_obj = bgm_data_info_get();
    osMutexAcquire(bgm_obj->mutex, osWaitForever);
    enum bgm_fsm_state fsm_state_cur = bgm_obj->fsm_state;
    osMutexRelease(bgm_obj->mutex);

    struct locked_info *obj = locked_info_obj_get();

    switch (fsm_state_cur)
    {
    case BGM_STATE_INIT:
    case BGM_STATE_IDLE:
    case BGM_STATE_PARK:
    case BGM_STATE_MANUAL:
    case BGM_STATE_COMPLETE:
    case BGM_STATE_SHUTDOWN:
    case BGM_STATE_POWERSAVER:
    case BGM_STATE_TERMINATE:
        osMutexAcquire(obj->mutex, osWaitForever);
        memset(obj, 0, sizeof(struct locked_info) - sizeof(osMutexId_t));
        osMutexRelease(obj->mutex);
        break;
    case BGM_STATE_PRELIMINARY:
    case BGM_STATE_PREPARE:
    case BGM_STATE_READY:
    case BGM_STATE_WORK:
    case BGM_STATE_INTERRUPT:
        osMutexAcquire(obj->mutex, osWaitForever);
        obj->io_locked_flag = 0;
        osMutexRelease(obj->mutex);
        break;
    default:
        break;
    }

    return 0;
}

struct interlock *interlock_status_get(void)
{
    struct locked_info *obj = locked_info_obj_get(), info = {0};
    osMutexAcquire(obj->mutex, osWaitForever);
    memcpy(&info, obj, sizeof(struct locked_info) - sizeof(osMutexId_t));
    osMutexRelease(obj->mutex);

    struct interlock interlock = {0};

    /* 1. io status */
    struct io_status io_obj = io_status_get();
    interlock.io = info.io_locked_flag == 0 ? io_obj.extend_status.current.bytes | io_obj.detect_status.bytes << 16 : info.io_stat_locked;

    /* 2. bgm error status */
    interlock.bgm_err = info.bgm_err_locked_flag == 0 ? bgm_error_info_get(BGM_ERROR_ALL) : info.bgm_err_stat_locked;

    /* 3. dose error status */
    interlock.dose_err[0] = info.dose_err_locked_flag == 0 ? dose_err_info_get(BGM_UART_DOSE1) : info.dose_err_stat_locked[0];
    interlock.dose_err[1] = info.dose_err_locked_flag == 0 ? dose_err_info_get(BGM_UART_DOSE2) : info.dose_err_stat_locked[1];

    /* 4. dose interlock status */
    interlock.dose_interlock[0] = dose_interlock_get(BGM_UART_DOSE1);
    interlock.dose_interlock[1] = dose_interlock_get(BGM_UART_DOSE2);

    /* 5. afc error status */
    interlock.afc_err = info.afc_err_locked_flag == 0 ? 0/* afc_err_info_get(BGM_UART_AFC) */ : info.afc_err_stat_locked;

    /* 6. afc interlock status */
    interlock.afc_interlock = 0;//afc_interlock_get(BGM_UART_AFC);

    /* 7. update local interlock object */
    struct interlock *interlock_obj = interlock_obj_get();
    memcpy(interlock_obj, &interlock, sizeof(struct interlock));

    return interlock_obj;
}


#ifndef INTERLOCK_TEST
#include "shell.h"
static int8_t interlock_status_output(uint8_t argc, uint8_t **argv)
{
    struct interlock *obj = interlock_obj_get();

    LOG_I("io: %#.8x\r\n", obj->io);
    LOG_I("bgm_err: %#.8x\r\n", obj->bgm_err);
    LOG_I("dose_err[0]: %#.8x\r\n", obj->dose_err[0]);
    LOG_I("dose_err[1]: %#.8x\r\n", obj->dose_err[1]);
    LOG_I("dose_interlock[0]: %#.8x\r\n", obj->dose_interlock[0]);
    LOG_I("dose_interlock[1]: %#.8x\r\n", obj->dose_interlock[1]);
    LOG_I("afc_err: %#.8x\r\n", obj->afc_err);
    LOG_I("afc_interlock: %#.8x\r\n", obj->afc_interlock);

    struct locked_info *locked_obj = locked_info_obj_get();

    LOG_I("io_locked_flag: %d\r\n", locked_obj->io_locked_flag);
    LOG_I("io_stat_locked: %#.8x\r\n", locked_obj->io_stat_locked);
    LOG_I("bgm_err_locked_flag: %d\r\n", locked_obj->bgm_err_locked_flag);
    LOG_I("bgm_err_stat_locked: %#.8x\r\n", locked_obj->bgm_err_stat_locked);
    LOG_I("dose_err_locked_flag: %d\r\n", locked_obj->dose_err_locked_flag);
    LOG_I("dose_err_stat_locked[0]: %#.8x\r\n", locked_obj->dose_err_stat_locked[0]);
    LOG_I("dose_err_stat_locked[1]: %#.8x\r\n", locked_obj->dose_err_stat_locked[1]);
    LOG_I("afc_err_locked_flag: %d\r\n", locked_obj->afc_err_locked_flag);
    LOG_I("afc_err_stat_locked: %#.8x\r\n", locked_obj->afc_err_stat_locked);

    return 0;
}
MSH_CMD_EXPORT_ALIAS(interlock_status_output, interlock_status_output, output interlock status);
#endif