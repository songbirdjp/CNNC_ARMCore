#include "jaw_test.h"
#include "jaw_control.h" 
#include "jaw_drv.h"     
#include <stdlib.h>
#include <math.h>        
#include "planData.h"
#include "ulog.h"
#include "tim.h"
#include "shell.h"
#include "init_call.h"
#include "cmsis_os2.h"

// =========================================================
// 1. RI 数据模拟配置 (已修复结构体定义)
// =========================================================
#define USING_RI_DATA_TEST
#ifdef USING_RI_DATA_TEST

// 修正结构体定义，使其与初始化列表匹配
struct jaw_ri_data
{
    uint32_t jaw_pos_y;      // 对应 position
    uint32_t motion_time_ms; // 对应 time_ms
    uint32_t dir;
};

#define DATA_SIZE 20
struct jaw_ri_data data_info[DATA_SIZE] = 
{
    [0] = {.jaw_pos_y = 5000, .motion_time_ms = 1000, .dir = 0},
    [1] = {.jaw_pos_y = 10000, .motion_time_ms = 500, .dir = 0},
    [2] = {.jaw_pos_y = 15000, .motion_time_ms = 500, .dir = 0},
    [3] = {.jaw_pos_y = 20000, .motion_time_ms = 250, .dir = 0},
    [4] = {.jaw_pos_y = 25000, .motion_time_ms = 500, .dir = 0},
    [5] = {.jaw_pos_y = 30000, .motion_time_ms = 250, .dir = 0},
    [6] = {.jaw_pos_y = 35000, .motion_time_ms = 500, .dir = 0},
    [7] = {.jaw_pos_y = 40000, .motion_time_ms = 250, .dir = 0},
    [8] = {.jaw_pos_y = 45000, .motion_time_ms = 500, .dir = 0},
    [9] = {.jaw_pos_y = 50000, .motion_time_ms = 500, .dir = 0},

    [10] = {.jaw_pos_y = 45000, .motion_time_ms = 500, .dir = 1},
    [11] = {.jaw_pos_y = 40000, .motion_time_ms = 250, .dir = 1},
    [12] = {.jaw_pos_y = 35000, .motion_time_ms = 500, .dir = 1},
    [13] = {.jaw_pos_y = 30000, .motion_time_ms = 250, .dir = 1},
    [14] = {.jaw_pos_y = 25000, .motion_time_ms = 500, .dir = 1},
    [15] = {.jaw_pos_y = 20000, .motion_time_ms = 250, .dir = 1},
    [16] = {.jaw_pos_y = 15000, .motion_time_ms = 500, .dir = 1},
    [17] = {.jaw_pos_y = 10000, .motion_time_ms = 500, .dir = 1},
    [18] = {.jaw_pos_y = 5000, .motion_time_ms = 1000, .dir = 1},
    [19] = {.jaw_pos_y = 100,  .motion_time_ms = 1000, .dir = 1},
};

static uint32_t ri_data_index = 0;
static uint32_t position_code_tar = 0, position_time_ms = 0; 
// static uint32_t position_code_prev = 0; // 暂时未用警告消除
static float velocity_code_tar = 0.0f; // , velocity_code_cur = 0.0f, velocity_code_offset = 0.0f;

// 补充缺失的获取函数
static void ri_data_get(uint32_t type, uint32_t index, struct jaw_ri_data *out)
{
    if (index < DATA_SIZE) {
        *out = data_info[index];
    } else {
        *out = data_info[DATA_SIZE - 1]; // 防止越界
    }
}
#endif

// =========================================================
// 2. 本地控制数据结构
// =========================================================

struct PID_IncTypeDef {
    float Kp, Ki, Kd;
    float ek, ek_1, ek_2;
    float output;
    float out_max;
    float out_min;
    float integral; 
};

typedef struct {
    uint16_t last_enc_raw;      
    int64_t  total_position;    
    float    last_vel_filtered; 
    
    struct PID_IncTypeDef pid_pos;
    struct PID_IncTypeDef pid_vel;
} LocalAxis_t;

// =========================================================
// 3. 全局变量
// =========================================================

static LocalAxis_t AxisState[2] = 
{
    [0] = {.pid_pos.Kp = 0.8f,.pid_pos.Ki = 0.0f,.pid_pos.Kd = 0.0f, .pid_pos.out_max = 20000.0f, .pid_pos.out_min = -20000.0f, .pid_vel.Kp = 0.1f, .pid_vel.Ki = 0.05f, .pid_vel.Kd = 0.3f, .pid_vel.out_max = 99.0f, .pid_vel.out_min = -99.0f},
    [1] = {.pid_pos.Kp = 0.8f,.pid_pos.Ki = 0.0f,.pid_pos.Kd = 0.0f, .pid_pos.out_max = 20000.0f, .pid_pos.out_min = -20000.0f, .pid_vel.Kp = 0.1f, .pid_vel.Ki = 0.05f, .pid_vel.Kd = 0.3f, .pid_vel.out_max = 99.0f, .pid_vel.out_min = -99.0f},
};

volatile float g_RI_End_Pos_X[2] = {0.0f};
// volatile float g_RI_End_Pos_Y = 0.0f;
volatile float g_RI_Cur_Ref_X[2] = {0.0f};
// volatile float g_RI_Cur_Ref_Y = 0.0f;
volatile float g_RI_V_FF_X[2]    = {0.0f};
// volatile float g_RI_V_FF_Y    = 0.0f;
volatile uint8_t g_Is_Moving[2]  = {0};

static osMessageQueueId_t jaw_info_queueHandle = NULL;

struct jaw_info {
    uint32_t timestamp;
    uint8_t axis_idx;
    float pos_ref;
    float pos_act;
    float vel_ref;
    float vel_act;
    float pwm;
};

// =========================================================
// 4. 算法与辅助函数
// =========================================================

static float LowPassFilter(float new_val, float prev_val) {
    return 0.05f * new_val + 0.95f * prev_val; 
}

// 补充缺失的 PID 复位函数 (解决 undefined reference)
static void pid_reset(struct PID_IncTypeDef *pid) {
    pid->ek = 0; pid->ek_1 = 0; pid->ek_2 = 0;
    pid->integral = 0; 
    pid->output = 0;
}

static void pid_init(struct PID_IncTypeDef *pid, float kp, float ki, float kd, float limit) {
    pid->Kp = kp; pid->Ki = ki; pid->Kd = kd;
    pid->out_max = limit; pid->out_min = -limit;
    pid_reset(pid);
}

// 位置式 PID 计算
static float pid_calc(struct PID_IncTypeDef *pid, float setpoint, float feedback) {
    pid->ek = setpoint - feedback;
    
    if (pid->Ki != 0) {
        pid->integral += pid->ek;
        
        // 积分限幅
        float integral_limit = 30.0f / pid->Ki; 
        if (pid->integral > integral_limit) pid->integral = integral_limit;
        if (pid->integral < -integral_limit) pid->integral = -integral_limit;
    } else {
        pid->integral = 0;
    }

    float out = pid->Kp * pid->ek + 
                pid->Ki * pid->integral + 
                pid->Kd * (pid->ek - pid->ek_1);

    pid->ek_2 = pid->ek_1;
    pid->ek_1 = pid->ek;
    
    if (out > pid->out_max) out = pid->out_max;
    if (out < pid->out_min) out = pid->out_min;
    
    pid->output = out; 
    return pid->output;
}

// 核心编码器处理 (代替 getEncodeTotalValue)
static void UpdateEncoderLogic(uint8_t axis_idx) {
    // uint16_t curr_raw = (uint16_t)getEncodeValue(axis_idx);
    // int16_t diff = (int16_t)(curr_raw - AxisState[axis_idx].last_enc_raw);

    // AxisState[axis_idx].total_position += diff;
    // AxisState[axis_idx].last_enc_raw = curr_raw;
    getEncodeTotalValue(axis_idx);
    AxisState[axis_idx].total_position = rtFeedback.jawRTPos[axis_idx] << 2;

}

// =========================================================
// 5. 中断回调与核心控制
// =========================================================
static float prev_pos_X[2] = {0.0f};
void Update_RI_Target(struct JawFlagType *recvMsg, uint8_t axis_idx)
{
    float time_ms = 0.0f, last_ri_pos = 0.0f;
    if(recvMsg->cmdTime_ms > 0) time_ms = (float)recvMsg->cmdTime_ms;// 1000.0f;
    else time_ms = 1.0f;

    last_ri_pos = g_RI_End_Pos_X[axis_idx];
    g_RI_End_Pos_X[axis_idx] = (float)recvMsg->cmdPos[axis_idx];
    AxisState[axis_idx].last_enc_raw = (uint16_t)getEncodeValue(axis_idx);
    UpdateEncoderLogic(axis_idx);
    g_RI_Cur_Ref_X[axis_idx] = g_RI_End_Pos_X[axis_idx];
    g_RI_V_FF_X[axis_idx] = (g_RI_End_Pos_X[axis_idx] - (float)AxisState[axis_idx].total_position) / time_ms;
    prev_pos_X[axis_idx] = g_RI_Cur_Ref_X[axis_idx];
    // LOG_I("[%d]Tar=%.0f, T=%.3f, Vff=%.1f, last_ri_pos=%.0f, cur_pos=%d\r\n", axis_idx, g_RI_End_Pos_X[axis_idx], time_ms, g_RI_V_FF_X[axis_idx], last_ri_pos, AxisState[axis_idx].total_position);


    // if (axis_idx == X)
    // {
    //     last_ri_pos = g_RI_End_Pos_X;
    //     g_RI_End_Pos_X = (float)recvMsg->cmdPos[X];
    //     AxisState[X].last_enc_raw = (uint16_t)getEncodeValue(X);
    //     UpdateEncoderLogic(X);
    //     g_RI_Cur_Ref_X = g_RI_End_Pos_X;
    //     g_RI_V_FF_X = (g_RI_End_Pos_X - (float)AxisState[X].total_position) / time_ms;
    //     prev_pos_X = g_RI_Cur_Ref_X;
    //     LOG_I("[%d]Tar=%.0f, T=%.3f, Vff=%.1f, last_ri_pos=%.0f, cur_pos=%d\r\n", axis_idx, g_RI_End_Pos_X, time_ms, g_RI_V_FF_X, last_ri_pos, AxisState[X].total_position);
    // }
    // else if (axis_idx == Y)
    // {
    //     last_ri_pos = g_RI_End_Pos_Y;
    //     g_RI_End_Pos_Y = (float)recvMsg->cmdPos[Y];
    //     AxisState[Y].last_enc_raw = (uint16_t)getEncodeValue(Y);
    //     UpdateEncoderLogic(Y);
    //     g_RI_Cur_Ref_Y = g_RI_End_Pos_Y;
    //     g_RI_V_FF_Y = (g_RI_End_Pos_Y - (float)AxisState[Y].total_position) / time_ms;
    //     prev_pos_Y = g_RI_Cur_Ref_Y;
    //     LOG_I("[%d]Tar=%.0f, T=%.3f, Vff=%.1f, last_ri_pos=%.0f, cur_pos=%d\r\n", axis_idx, g_RI_End_Pos_Y, time_ms, g_RI_V_FF_Y, last_ri_pos, AxisState[Y].total_position);
    // }

    if (g_Is_Moving[axis_idx] == 0)
    {
        g_Is_Moving[axis_idx] = 1;
    }

}

volatile uint8_t g_AutoRun_Mode = 0; // 0:手动/单步, 1:自动序列
volatile int32_t g_AutoRun_Countdown = 0; // 倒计时 (ms)
static osThreadId_t g_auto_run_tid = NULL;
static volatile uint8_t g_stop_signal = 0;

static int8_t jaw_pid_ctrl(uint8_t axis_idx)
{
    UpdateEncoderLogic(axis_idx);
    float pos_act_X = (float)AxisState[axis_idx].total_position;
    // 2. 计算速度 (滤波)
    float raw_vel_X = pos_act_X - prev_pos_X[axis_idx];
    prev_pos_X[axis_idx] = pos_act_X;
    float vel_act_X = raw_vel_X;/* code/ms *///LowPassFilter(raw_vel_X, AxisState[X].last_vel_filtered);
    AxisState[axis_idx].last_vel_filtered = vel_act_X;
    // 3. 轨迹插补
    // if (g_Is_Moving[X]) 
    // {
    //     float next_rabbit = g_RI_Cur_Ref_X + g_RI_V_FF_X * 0.001f;
    //     // 终点判断
    //     if ((g_RI_V_FF_X > 0 && next_rabbit >= g_RI_End_Pos_X) ||
    //         (g_RI_V_FF_X < 0 && next_rabbit <= g_RI_End_Pos_X)) {
    //         g_RI_Cur_Ref_X = g_RI_End_Pos_X;
    //         g_RI_V_FF_X = 0; 
    //         g_Is_Moving[X] = 0; // [修改点1] 到达终点，标记插补结束
    //     } else {
    //         g_RI_Cur_Ref_X = next_rabbit;
    //     }
    // }
    // 4. 双环 PID + 死区锁定
    float pos_err = g_RI_Cur_Ref_X[axis_idx] - pos_act_X;
    float pwm_X = 0.0f;
    uint8_t is_locked = 0; // 标记是否进入死区锁定状态
    if (fabsf(pos_err) < 10.0f && fabsf(vel_act_X) < 0.1f)
    {
        pwm_X = 0.0f;
        pid_reset(&AxisState[axis_idx].pid_vel);
        is_locked = 1; // [修改点2] 标记已锁定
    }
    else
    {
        float v_comp_X = AxisState[axis_idx].pid_pos.Kp * pos_err;
        float v_cmd_X = g_RI_V_FF_X[axis_idx] + v_comp_X;
        // printf("g_RI_V_FF_X[axis_idx]: %f, v_comp_X: %f\r\n", g_RI_V_FF_X[axis_idx], v_comp_X);
        pwm_X = pid_calc(&AxisState[axis_idx].pid_vel, v_cmd_X, vel_act_X);
    }
    motorCtrlByPWM(pwm_X, axis_idx);
    // ============================================
    // [修改点3] 自动状态上报逻辑
    // ============================================
    // 条件：正在插补(g_Is_Moving) 或者 还没锁死(is_locked==0)
    // 也就是说：只要还在动，或者还在调整，就一直发数据
#if 0
    if (g_Is_Moving[axis_idx] || !is_locked)
    {
        static uint8_t report_div[2] = {0};
        // 20ms 发一次 (1kHz / 20 = 50Hz)，避免串口刷爆
        if (++report_div[axis_idx] >= 100)
        {
            report_div[axis_idx] = 0;
            struct jaw_info info;
            info.axis_idx = axis_idx;
            info.timestamp = osKernelGetTickCount(); // 需要包含 cmsis_os2.h
            info.pos_ref = g_RI_Cur_Ref_X[axis_idx];
            info.pos_act = pos_act_X;
            info.vel_ref = g_RI_V_FF_X[axis_idx]; // 或者显示 PID 计算的目标速度
            info.vel_act = vel_act_X;
            info.pwm = pwm_X;
            // 发送给打印线程 (不阻塞)
            if (jaw_info_queueHandle != NULL) {
                osMessageQueuePut(jaw_info_queueHandle, &info, 0, 0);
            }
        }
    }
#endif

    return 0;
}

void jaw_pwm_position_ctrl_periodically(TIM_HandleTypeDef *htim)
{
#if 0
    if (htim->Instance == TIM4)
    {
        UpdateEncoderLogic(X);
        float pos_act_X = (float)AxisState[X].total_position;

        // 2. 计算速度 (滤波)
        float raw_vel_X = pos_act_X - prev_pos_X;
        prev_pos_X = pos_act_X;
        float vel_act_X = raw_vel_X;/* code/ms *///LowPassFilter(raw_vel_X, AxisState[X].last_vel_filtered);
        AxisState[X].last_vel_filtered = vel_act_X;

        // 3. 轨迹插补
        // if (g_Is_Moving[X]) 
        // {
        //     float next_rabbit = g_RI_Cur_Ref_X + g_RI_V_FF_X * 0.001f;
        //     // 终点判断
        //     if ((g_RI_V_FF_X > 0 && next_rabbit >= g_RI_End_Pos_X) ||
        //         (g_RI_V_FF_X < 0 && next_rabbit <= g_RI_End_Pos_X)) {
        //         g_RI_Cur_Ref_X = g_RI_End_Pos_X;
        //         g_RI_V_FF_X = 0; 
        //         g_Is_Moving[X] = 0; // [修改点1] 到达终点，标记插补结束
        //     } else {
        //         g_RI_Cur_Ref_X = next_rabbit;
        //     }
        // }

        // 4. 双环 PID + 死区锁定
        float pos_err = g_RI_Cur_Ref_X - pos_act_X;
        float pwm_X = 0.0f;
        uint8_t is_locked = 0; // 标记是否进入死区锁定状态

        if (fabsf(pos_err) < 10.0f && fabsf(vel_act_X) < 0.1f) 
        {
            pwm_X = 0.0f;
            pid_reset(&AxisState[X].pid_vel); 
            is_locked = 1; // [修改点2] 标记已锁定
        }
        else 
        {
            float v_comp_X = AxisState[X].pid_pos.Kp * pos_err;
            float v_cmd_X = g_RI_V_FF_X + v_comp_X;
            pwm_X = pid_calc(&AxisState[X].pid_vel, v_cmd_X, vel_act_X);
        }

        motorCtrlByPWM(pwm_X, X);

        // ============================================
        // [修改点3] 自动状态上报逻辑
        // ============================================
        // 条件：正在插补(g_Is_Moving) 或者 还没锁死(is_locked==0)
        // 也就是说：只要还在动，或者还在调整，就一直发数据
        if (g_Is_Moving[X] || !is_locked) 
        {
            static uint8_t report_div = 0;
            // 20ms 发一次 (1kHz / 20 = 50Hz)，避免串口刷爆
            if (++report_div >= 100) 
            {
                report_div = 0;
                struct jaw_info info;
                info.timestamp = osKernelGetTickCount(); // 需要包含 cmsis_os2.h
                info.pos_ref = g_RI_Cur_Ref_X;
                info.pos_act = pos_act_X;
                info.vel_ref = g_RI_V_FF_X; // 或者显示 PID 计算的目标速度
                info.vel_act = vel_act_X;
                info.pwm = pwm_X;

                // 发送给打印线程 (不阻塞)
                if (jaw_info_queueHandle != NULL) {
                    osMessageQueuePut(jaw_info_queueHandle, &info, 0, 0);
                }
            }
        }

        // Y 轴
        UpdateEncoderLogic(Y);
        motorCtrlByPWM(0, Y);
    }
#endif
    if (g_Is_Moving[X])
    {
        jaw_pid_ctrl(X);
    }
    if (g_Is_Moving[Y])
    {
        jaw_pid_ctrl(Y);
    }
}

void TIM4_IRQHandler(void)
{
  HAL_TIM_IRQHandler(&htim4);
}

void TIM23_IRQHandler(void)
{
  HAL_TIM_IRQHandler(&htim23);
}

#if 0
// RI 模拟数据更新中断 (TIM23)
static uint32_t radiation_index = 0;
struct realtime_pos
{
    uint32_t ri;
    uint32_t position;
};
static struct realtime_pos rt_pos[2048] = {0};

static void jaw_ri_data_update(TIM_HandleTypeDef *htim)
{
    /* 1. 记录当前位置 */
    rt_pos[radiation_index].ri = radiation_index;

    // 修复：使用本地 UpdateLogic 代替 getEncodeTotalValue
    UpdateEncoderLogic(Y); 
    rt_pos[radiation_index].position = (uint32_t)AxisState[Y].total_position; // 假设不需要移位，直接用脉冲

    /* 2. 更新ri值 */
    if (radiation_index < 2048)
    {
        radiation_index++;
    }

    struct jaw_ri_data ri_data = {0};
    ri_data_get(0, radiation_index, &ri_data); // 修复：使用补充的函数
    
    position_code_tar = ri_data.jaw_pos_y;     // 修复：成员名匹配
    position_time_ms = ri_data.motion_time_ms;
    
    if (position_time_ms > 0 && radiation_index > 0) {
        velocity_code_tar = ((float)position_code_tar - rt_pos[radiation_index - 1].position) / position_time_ms;
    } else {
        velocity_code_tar = 0;
    }

    /* 3. 设置下一次中断 */
    if (position_time_ms > 0) {
        __HAL_TIM_SET_AUTORELOAD(&htim23, position_time_ms * 1000 - 1);    /* Unit: us */
    }
}
#endif
// =========================================================
// 6. 初始化与任务
// =========================================================

static int8_t jaw_ctrl_init(void)
{
    MX_TIM4_Init();
    MX_TIM23_Init();
    HAL_StatusTypeDef status = HAL_OK;

    // 初始化 PID 参数 (这里写入你的“黄金参数”)
    // X轴: Pos_P=1.0, Vel_P=0.1, Vel_I=0.02, Vel_D=0.5
    // pid_init(&AxisState[X].pid_pos, 1.0f, 0, 0, 20000.0f);
    // pid_init(&AxisState[X].pid_vel, 0.1f, 0.02f, 0.5f, 99.0f);

    // AxisState[X].last_enc_raw = (uint16_t)getEncodeValue(X);
    // AxisState[X].total_position = 0;

    // 注册 TIM4
    status = HAL_TIM_RegisterCallback(&htim4, HAL_TIM_PERIOD_ELAPSED_CB_ID, jaw_pwm_position_ctrl_periodically);
    if (status != HAL_OK) printf("tim4 callback err: %d\r\n", status);

    // 暂时不开启 TIM4，等 Shell 命令 jaw_ri 开启，或者这里开启也行
    // HAL_TIM_Base_Start_IT(&htim4);

    // 注册 TIM23
    // status = HAL_TIM_RegisterCallback(&htim23, HAL_TIM_PERIOD_ELAPSED_CB_ID, jaw_ri_data_update);
    // if (status != HAL_OK) printf("tim23 callback err: %d\r\n", status);

    return 0;
}

static void motor_start(uint8_t axesType)
{
    /* 启动定时器 */
    HAL_TIM_Base_Start_IT(&htim4);
}
static void motor_stop(uint8_t axesType)
{
    HAL_TIM_Base_Stop_IT(&htim4);

    /* 清空PID积分项等 */
    pid_reset(&AxisState[axesType].pid_pos);
    pid_reset(&AxisState[axesType].pid_vel);
    // pid_reset(&AxisState[Y].pid_pos);
    // pid_reset(&AxisState[Y].pid_vel);

    g_Is_Moving[axesType] = 0;
}
static void jaw_test_entry(void *argument) 
{
    jaw_ctrl_init();
    motor_callback_register(0, motor_start);
    motor_callback_register(1, motor_stop);

    jaw_info_queueHandle = osMessageQueueNew(64, sizeof(struct jaw_info), NULL);

    struct jaw_info info;
    for (;;)
    {
        // 无限等待队列消息
        if(osMessageQueueGet(jaw_info_queueHandle, &info, NULL, osWaitForever) == osOK) {
            // 格式化打印：时间戳 | Rabbit | Turtle | 误差 | PWM
            // 使用 LOG_I 或 printf 均可
            LOG_I("[%d-%u] Ref:%.0f Act:%.0f Err:%.0f PWM:%.1f, vel_ref:%.2f vel_act:%.2f\r\n", 
                    info.axis_idx,
                    info.timestamp,
                    info.pos_ref,
                    info.pos_act,
                    info.pos_ref - info.pos_act,
                    info.pwm,
                    info.vel_ref,
                    info.vel_act);
        }
    }
}

static int jaw_test_thread_init(void)
{
    osThreadAttr_t attr = { .name = "jaw_test", .stack_size = 2048, .priority = osPriorityNormal };
    osThreadNew(jaw_test_entry, NULL, &attr);
    return 0;
}
INIT_APP_EXPORT(jaw_test_thread_init);

static int8_t pid_get(uint8_t argc, char **argv)
{
    LOG_I("PID: Pos_P=%.2f, Vel_P=%.2f, Vel_I=%.2f, Vel_D=%.2f\r\n", AxisState[X].pid_pos.Kp, AxisState[X].pid_vel.Kp, AxisState[X].pid_vel.Ki, AxisState[X].pid_vel.Kd);
    LOG_I("PID: Pos_P=%.2f, Vel_P=%.2f, Vel_I=%.2f, Vel_D=%.2f\r\n", AxisState[Y].pid_pos.Kp, AxisState[Y].pid_vel.Kp, AxisState[Y].pid_vel.Ki, AxisState[Y].pid_vel.Kd);

}
MSH_CMD_EXPORT_ALIAS(pid_get, pid_get, "get pid: p(pos)/v(vel) kp ki kd");

// =========================================================
// 7. Shell 命令
// =========================================================

static int8_t jaw_ri(int argc, char **argv) {
    if(argc < 3) { printf("Usage: jaw_ri <pos> <time_ms>\r\n"); return -1; }
    
    struct JawFlagType msg = {0};
    msg.cmdPos[X] = atoi(argv[1]);
    msg.cmdPos[Y] = 0;
    msg.cmdTime_ms = atoi(argv[2]);
    
    motorEnable(X);
    HAL_TIM_Base_Start_IT(&htim4); // 确保控制环开启
    Update_RI_Target(&msg, X);
    return 0;
}
MSH_CMD_EXPORT_ALIAS(jaw_ri, jaw_ri, "sim ri move: pos time_ms");

static int8_t jaw_stat(int argc, char **argv) {
    printf("--- Dual Loop Status ---\r\n");
    printf("Ref(Rabbit): %.2f\r\n", g_RI_Cur_Ref_X);
    printf("Act(Turtle): %lld\r\n", AxisState[X].total_position);
    printf("V_FF:        %.2f\r\n", g_RI_V_FF_X);
    printf("V_Act:       %.2f\r\n", AxisState[X].last_vel_filtered);
    printf("PID_Vel_Out: %.2f\r\n", AxisState[X].pid_vel.output);
    return 0;
}
MSH_CMD_EXPORT_ALIAS(jaw_stat, jaw_stat, "show dual loop status");

static int8_t jaw_set_pid(int argc, char **argv) {
    if(argc < 5) { printf("Usage: jaw_set_pid <p/v> <kp> <ki> <kd>\r\n"); return -1; }
    
    struct PID_IncTypeDef *target = (*argv[1] == 'p') ? &AxisState[X].pid_pos : &AxisState[X].pid_vel;
    target->Kp = atof(argv[2]);
    target->Ki = atof(argv[3]);
    target->Kd = atof(argv[4]);
    
    pid_reset(target);
    printf("PID Updated.\r\n");
    return 0;
}
MSH_CMD_EXPORT_ALIAS(jaw_set_pid, jaw_set_pid, "set pid: p(pos)/v(vel) kp ki kd");

static int8_t motor_pwm_set(int argc, char **argv)
{
    if(argc < 3) return -1;
    HAL_TIM_Base_Stop_IT(&htim4);
    motorEnable((uint8_t)atoi(argv[1]));
    motorCtrlByPWM((double)atoi(argv[2]), (uint8_t)atoi(argv[1]));
    
    // 简单的延时测试
    osDelay(500);
    
    motorCtrlByPWM(0.0,(uint8_t)atoi(argv[1]));
    motorDisable((uint8_t)atoi(argv[1]));
    printf("Manual PWM done.\r\n");
    HAL_TIM_Base_Start_IT(&htim4);
    return 0;
}
MSH_CMD_EXPORT_ALIAS(motor_pwm_set, motor_pwm_set, "motor_pwm_set <axis> <pwm>");

// =========================================================
// 恢复并适配的 jaw_movement_start
// =========================================================
// 自动序列控制变量
static void jaw_auto_sequence_entry(void *argument)
{
    #ifdef USING_RI_DATA_TEST
    printf("[AutoRun] Thread Started. Total Steps: %d\r\n", DATA_SIZE);

    for (int i = 0; i < DATA_SIZE; i++)
    {
        // 1. 检查是否有停止信号
        if (g_stop_signal) {
            printf("[AutoRun] Aborted by User at step %d.\r\n", i);
            break;
        }

        struct jaw_ri_data *item = &data_info[i];
        
        // 2. 构造并发送指令
        struct JawFlagType msg = {0};
        msg.cmdPos[X] = item->jaw_pos_y;
        msg.cmdTime_ms = item->motion_time_ms;
        
        printf("[AutoRun] Step %d/%d: Go -> %d (T=%dms)\r\n", 
               i+1, DATA_SIZE, msg.cmdPos[X], msg.cmdTime_ms);
        
        Update_RI_Target(&msg, X); // 发给底层

        // 3. 延时等待动作完成
        // 延时 = 运动时间 + 缓冲时间(200ms)
        // 使用 osDelay 让出 CPU，这样你可以同时输入 jaw_stat
        osDelay(item->motion_time_ms - 50);
    }

    if (!g_stop_signal) {
        printf("[AutoRun] Sequence Completed Successfully.\r\n");
    }
    #endif

    // 任务结束，自我销毁句柄标记
    g_auto_run_tid = NULL;
    osThreadExit();
}
// Shell 命令入口
static int8_t jaw_movement_start(int argc, char **argv)
{
    struct JawFlagType msg = {0};
    
    // ------------------------------------------------
    // 模式 1: 停止命令 (jaw_move stop)
    // ------------------------------------------------
    if (argc >= 2 && strcmp(argv[1], "stop") == 0)
    {
        if (g_auto_run_tid != NULL) {
            g_stop_signal = 1; // 发送停止信号
            printf("Stopping AutoRun...\r\n");
        } else {
            printf("No AutoRun is running.\r\n");
        }
        return 0;
    }

    // ------------------------------------------------
    // 模式 2: 手动单点运动 (jaw_move 5000 1000)
    // ------------------------------------------------
    if (argc >= 3) 
    {
        // 如果自动任务在跑，先停掉它
        if (g_auto_run_tid != NULL) {
            g_stop_signal = 1;
            osDelay(100); // 等一下线程退出
        }

        msg.cmdPos[X] = atoi(argv[1]);
        msg.cmdTime_ms = atoi(argv[2]);
        
        motorEnable(X);
        HAL_TIM_Base_Start_IT(&htim4); 
        Update_RI_Target(&msg, X);
        
        printf("Manual Trigger: Target=%d, Time=%d ms\r\n", msg.cmdPos[X], msg.cmdTime_ms);
    }
    // ------------------------------------------------
    // 模式 3: 启动自动序列 (jaw_move)
    // ------------------------------------------------
    else 
    {
        // 如果已经在跑，就提示
        if (g_auto_run_tid != NULL) {
            printf("AutoRun is already running! Use 'jaw_move stop' to cancel.\r\n");
            return 0;
        }

        // 启动前的准备
        motorEnable(X);
        HAL_TIM_Base_Start_IT(&htim4);
        
        // 创建一个一次性线程来跑循环
        // 这样不会阻塞 Shell，你可以继续输命令
        g_stop_signal = 0;
        osThreadAttr_t attr = { .name = "jaw_seq", .stack_size = 1024, .priority = osPriorityNormal };
        g_auto_run_tid = osThreadNew(jaw_auto_sequence_entry, NULL, &attr);
        
        if (g_auto_run_tid == NULL) {
            printf("Error: Failed to create AutoRun thread.\r\n");
        }
    }

    return 0;
}
// 导出命令
MSH_CMD_EXPORT_ALIAS(jaw_movement_start, jaw_move, "move: [pos time] or [stop] or [auto]");