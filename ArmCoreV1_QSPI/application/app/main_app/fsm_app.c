/* 
 * ARM Core状态机设计基于文档《同轴共面CT引导直线加速器原型机-系统状态机&模块状态机设计说明书 V0.2 ChenLong 240307.docx》5.2小节
 * URL：https://a24nvvy0ob.feishu.cn/file/OK4zbywImodUjsxJvyTcSHilnJf
 *
 * +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 *
 * 0. 主设备：PLC   从设备：ARM Core
 * 1. 主设备同步状态至从设备，当从设备接收到主设备的控制指令后，需要根据从设备的当前状态，判断是否可以执行相应的动作
 * 2. 从设备同步状态至主设备，由主设备根据从设备的状态，判断是否下发控制指令至从设备
 * 3. 正常时：由主设备发起状态跳转，并同步至从设备
 * 4. 异常时：若发生故障的为主设备或级联的其他从设备，则主设备无需同步fault状态至无异常的从设备
 *           若发生故障的为从设备，则从设备自动跳转至fault状态，并设置故障码。考虑到一些偶发性因素，比如执行机构故障
 *           在一段时间后消失了，此时从设备依旧保持在fault状态
 * 5. 从设备退出fault状态：由主设备控制从设备退出fault状态，若此时重新检测到了故障，则从设备再次自动跳转至fault状态，并设置故障码
 *                       主设备控制退出fault状态前，应确保故障被排除，依据的是相应机构/部件的状态码，而不是故障码
 * 
 * +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * 
 * 1. ARM Core状态跳转，受控于主设备，其内部各个状态之间可直接跳转，无跳转条件限制
 * 2. ARM Core状态机，约束的是执行机构/部件的start行为，而非stop行为，也即在任何状态下均可stop机构/部件的运行
 * 
 * +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
*/

#include "fsm_app.h"
#include "init_call.h"
#include "cmsis_os2.h"

static uint8_t fsm_state_table[ACTION_MAX][FSM_STATE_MAX] = {
    /* 
     * 0: disable    -> carry out is forbidden in this state
     * 1: enable     -> carry out is permitted in this state
     * 
    function_index       post  init  idle  prepare  ready  work  move  fault */    
    [ACTION_1]        = { 1,    0,    0,    0,       0,     0,    0,     0  },
    [ACTION_2]        = { 0,    1,    0,    0,       0,     0,    0,     0  },
    [ACTION_3]        = { 0,    0,    1,    0,       0,     0,    0,     0  },
    [ACTION_4]        = { 0,    0,    0,    1,       0,     0,    0,     0  },
    [ACTION_5]        = { 0,    0,    0,    0,       1,     0,    0,     0  },
    [ACTION_6]        = { 0,    0,    0,    0,       0,     1,    0,     0  },
    [ACTION_7]        = { 0,    0,    0,    0,       0,     0,    1,     0  },
    [ACTION_8]        = { 0,    0,    0,    0,       0,     0,    0,     1  },
    [ACTION_9]        = { 0,    0,    0,    0,       0,     0,    1,     0  },
    [ACTION_10]       = { 0,    0,    0,    0,       0,     1,    0,     0  }
};


static osMutexId_t fsm_mutex = NULL;
static enum fsm_state fsm_state_current = FSM_STATE_POST;

int8_t fsm_state_set(enum fsm_state state)
{
    if (state >= FSM_STATE_MAX) 
    {
        return -1;
    }

    osMutexAcquire(fsm_mutex, osWaitForever);

    fsm_state_current = state;

    osMutexRelease(fsm_mutex);

    return 0;
}

enum fsm_state fsm_state_get(void)
{
    enum fsm_state state;

    osMutexAcquire(fsm_mutex, osWaitForever);

    state = fsm_state_current;

    osMutexRelease(fsm_mutex);

    return state;
}

int8_t action_of_function_authority_check(enum function_index function_index)
{
    if (function_index >= ACTION_MAX)
    {
        return -1;
    }

    enum fsm_state state = fsm_state_get();

    return fsm_state_table[function_index][state];
}

static int8_t fsm_init(void)
{
    osMutexAttr_t fsm_mutex_attributes = {
    .name = "fsm_mutex",
    .attr_bits = osMutexRecursive | osMutexPrioInherit
    };

    fsm_mutex = osMutexNew(&fsm_mutex_attributes);
    if (fsm_mutex == NULL)
    {
        printf("fsm mutex create failed\r\n");
        return -1;
    }

    return 0;
}
INIT_APP_EXPORT(fsm_init);


#ifndef FSM_TEST
#include "shell.h"
int8_t fsm_state_test(uint8_t argc, char *argv[])
{
    for (int i = 0; i < ACTION_MAX; i++)
    {
        for (int j = 0; j < FSM_STATE_MAX; j++)
        {
            printf("%u ", fsm_state_table[i][j]);
        }
        printf("\r\n");
    }

    enum fsm_state state = fsm_state_get();
    printf("state:%d\r\n", state);

    fsm_state_set(state+1);

    state = fsm_state_get();
    printf("state:%d\r\n", state);

    return 0;
}
MSH_CMD_EXPORT_ALIAS(fsm_state_test, fsm_state_test, test fsm state table);
#endif


