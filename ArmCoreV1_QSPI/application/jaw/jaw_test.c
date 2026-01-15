#include "jaw_test.h"
#include "jaw_drv.h"
#include <stdlib.h>
#include "planData.h"
#include "ulog.h"
#include "tim.h"
#include "shell.h"
#include "init_call.h"
#include "cmsis_os2.h"

/**
  * @brief This function handles TIM4 global interrupt.
  */
void TIM4_IRQHandler(void)
{
  /* USER CODE BEGIN TIM4_IRQn 0 */

  /* USER CODE END TIM4_IRQn 0 */
  HAL_TIM_IRQHandler(&htim4);
  /* USER CODE BEGIN TIM4_IRQn 1 */

  /* USER CODE END TIM4_IRQn 1 */
}

// 定义X轴和Y轴的PID对象
static struct PID_IncTypeDef hPID_X;
static struct PID_IncTypeDef hPID_Y;
static struct PID_IncTypeDef hPID_Vel_X;
static struct PID_IncTypeDef hPID_Vel_Y;

// 定义目标位置 (由外部指令修改)
static float g_target_pos_X = 0.0f;
static float g_target_pos_Y = 0.0f;

static osMessageQueueId_t jaw_info_queueHandle = NULL;
struct jaw_info
{
    uint32_t timestamp;
    float vel_tar;
    float vel_cur;
    float duty_cycle;
};


struct PID_IncTypeDef
{
  float Kp, Ki, Kd;     // PID参数
  float ek, ek_1, ek_2; // 当前、前一次、前两次的误差
  float output;         // 当前输出值
  float output_prev;    // 上一次输出值
  float out_max;        // 输出上限
  float out_min;        // 输出下限
} PID_IncTypeDef;

// 增量式PID计算函数
static float pid_increase_calculate(struct PID_IncTypeDef *pid, float setpoint, float feedback)
{
    /* 1. 计算当前误差 */
    pid->ek = setpoint - feedback;

    /* 2. 计算控制增量 Δu(k) */
    float delta_u = pid->Kp * (pid->ek - pid->ek_1)
                  + pid->Ki * pid->ek
                  + pid->Kd * (pid->ek - 2*pid->ek_1 + pid->ek_2);

    /* 3. 计算本次输出 = 上次输出 + 增量 */
    pid->output = pid->output_prev + delta_u;

    /* 4. 对输出进行限幅，防止过驱动 */
    if (pid->output > pid->out_max)
    {
    pid->output = pid->out_max;
    }
    else if (pid->output < pid->out_min)
    {
        pid->output = pid->out_min;
    }

    /* 5. 更新历史状态 */
    pid->ek_2 = pid->ek_1;
    pid->ek_1 = pid->ek;
    pid->output_prev = pid->output;

    return pid->output;
}

static struct PID_IncTypeDef jaw_pid_inst = {0};
static uint32_t position_code_tar = 0, position_time_ms = 0, position_code_prev = 0;
static float velocity_code_tar = 0.0f, velocity_code_cur = 0.0f;

static void jaw_pwm_position_ctrl_periodically(TIM_HandleTypeDef *htim)
{
    // 确保是控制回路定时器 (TIM4)
    if (htim->Instance == TIM4)
    {
        static uint8_t inited = 0;
        static float last_pos_X = 0.0f;
        static float last_pos_Y = 0.0f;

        getEncodeTotalValue(X);
        getEncodeTotalValue(Y);

        float pos_cur_X = (float)rtFeedback.jawRTPos[X];
        float pos_cur_Y = (float)rtFeedback.jawRTPos[Y];

        if (inited == 0)
        {
            last_pos_X = pos_cur_X;
            last_pos_Y = pos_cur_Y;
            inited = 1;
        }
        
        float vel_act_X = pos_cur_X - last_pos_X;
        float vel_act_Y = pos_cur_Y - last_pos_Y;
        
        float v_ref_X = 0.0f;
        float v_ref_Y = 0.0f;
        if (jawPlanMotionTime != 0)
        {
            v_ref_X = (float)jawPlanPos[X] / (float)jawPlanMotionTime;
            v_ref_Y = (float)jawPlanPos[Y] / (float)jawPlanMotionTime;
        }

        // 2. 位置环 + 前馈：Vcmd = PosPID + Vref
        float v_cmd_X = pid_increase_calculate(&hPID_X, g_target_pos_X, pos_cur_X) + v_ref_X;
        float v_cmd_Y = pid_increase_calculate(&hPID_Y, g_target_pos_Y, pos_cur_Y) + v_ref_Y;
        
        // 3. 速度环：PWM = VelPID(Vcmd - Vact)
        float output_X = pid_increase_calculate(&hPID_Vel_X, v_cmd_X, vel_act_X);
        float output_Y = pid_increase_calculate(&hPID_Vel_Y, v_cmd_Y, vel_act_Y);
        
        // 4. 执行电机驱动
        // 你的 motorCtrlByPWM 已经包含了死区处理和正反转逻辑
        motorCtrlByPWM((double)output_X, X);
        motorCtrlByPWM((double)output_Y, Y);

        last_pos_X = pos_cur_X;
        last_pos_Y = pos_cur_Y;
    }
}


// 辅助函数：复位PID内部状态（防止切模式时飞车）
static void pid_reset(struct PID_IncTypeDef *pid)
{
    pid->ek = 0; pid->ek_1 = 0; pid->ek_2 = 0;
    pid->output = 0; pid->output_prev = 0;
}

static int8_t jaw_ctrl_init(void)
{
    HAL_StatusTypeDef status = HAL_OK;

    MX_TIM4_Init();

    // ============================================
    // 1. 补充：初始化 X 轴 PID 参数 (根据实际情况调整)
    // ============================================
    hPID_X.Kp = 1.2f;       // 比例
    hPID_X.Ki = 0.05f;      // 积分 (增量式由于自带累积效应，Ki通常很小)
    hPID_X.Kd = 0.5f;       // 微分
    hPID_X.out_max = 95.0f; // 限制 PWM 占空比
    hPID_X.out_min = -95.0f;
    pid_reset(&hPID_X);

    // ============================================
    // 2. 补充：初始化 Y 轴 PID 参数
    // ============================================
    hPID_Y.Kp = 1.2f;
    hPID_Y.Ki = 0.05f;
    hPID_Y.Kd = 0.5f;
    hPID_Y.out_max = 95.0f;
    hPID_Y.out_min = -95.0f;
    pid_reset(&hPID_Y);

    // ============================================
    // 3. 速度环 PID 参数
    // ============================================
    hPID_Vel_X.Kp = 0.8f;
    hPID_Vel_X.Ki = 0.02f;
    hPID_Vel_X.Kd = 0.1f;
    hPID_Vel_X.out_max = 95.0f;
    hPID_Vel_X.out_min = -95.0f;
    pid_reset(&hPID_Vel_X);

    hPID_Vel_Y.Kp = 0.8f;
    hPID_Vel_Y.Ki = 0.02f;
    hPID_Vel_Y.Kd = 0.1f;
    hPID_Vel_Y.out_max = 95.0f;
    hPID_Vel_Y.out_min = -95.0f;
    pid_reset(&hPID_Vel_Y);

    // 4. 初始化目标位置为当前位置 (防止上电瞬间乱动)
    // 注意：需确保 getEncodeValue 在此时能正常工作
    g_target_pos_X = (float)getEncodeValue(X);
    g_target_pos_Y = (float)getEncodeValue(Y);

    status = HAL_TIM_RegisterCallback(&htim4, HAL_TIM_PERIOD_ELAPSED_CB_ID, jaw_pwm_position_ctrl_periodically);
    if (status != HAL_OK)
    {
        printf("tim4 register callback err: %d\r\n", status);
        return -1;
    }

    return 0;
}

static int8_t jaw_test_entry(void *argument)
{
    int8_t ret = 0;

    ret = jaw_ctrl_init();
    if (ret != 0)
    {
        printf("jaw ctrl init err: %d\r\n", ret);
        osThreadExit();
    }

    struct jaw_info pwm_info = {0};

    for (;;)
    {
        osMessageQueueGet(jaw_info_queueHandle, &pwm_info, NULL, osWaitForever);

        LOG_I("[%u]: vel_tar: %f, vel_cur: %f, error: %f, duty_cycle: %f\r\n", pwm_info.timestamp, pwm_info.vel_tar, pwm_info.vel_cur, pwm_info.vel_tar - pwm_info.vel_cur, pwm_info.duty_cycle);

        uint32_t pos_cur = getEncodeValue(X);

        LOG_I("pos_cur: %d\r\n", pos_cur);
    }

    return 0;
}

static int8_t jaw_test_thread_init(void)
{
    jaw_info_queueHandle = osMessageQueueNew(16, sizeof(struct jaw_info), NULL);
    if (jaw_info_queueHandle == NULL)
    {
        printf("queue jaw info create failed\r\n");
        return -1;
    }

    osThreadAttr_t thread_attributes = {
    .name = "jaw_test_thread",
    .stack_size = 1024 * 4,
    .priority = (osPriority_t) osPriorityAboveNormal,
    };

    osThreadId_t tid = osThreadNew(jaw_test_entry, NULL, &thread_attributes);
    if (tid == NULL)
    {
        printf("thread timestamp create err\r\n");
        return -2;
    }

    return 0;
}
INIT_APP_EXPORT(jaw_test_thread_init);


#ifndef JAW_PID_TEST

// =======================================================
// 命令 1: 设置 PID 参数
// 格式: jaw_set_pid <axis: x/y> <kp> <ki> <kd>
// 示例: jaw_set_pid x 1.5 0.01 0.5
// =======================================================
static int8_t jaw_set_pid(int argc, char **argv)
{
    if (argc < 5) 
    {
        printf("Usage: jaw_set_pid <x/y> <kp> <ki> <kd>\r\n");
        return -1;
    }

    struct PID_IncTypeDef *pTargetPID = NULL;
    
    // 判断是设置 X 轴还是 Y 轴
    if (*argv[1] == 'x' || *argv[1] == 'X') pTargetPID = &hPID_X;
    else if (*argv[1] == 'y' || *argv[1] == 'Y') pTargetPID = &hPID_Y;
    else {
        printf("Err: axis must be x or y\r\n");
        return -1;
    }

    pTargetPID->Kp = (float)atof(argv[2]);
    pTargetPID->Ki = (float)atof(argv[3]);
    pTargetPID->Kd = (float)atof(argv[4]);
    
    // 重置一下状态，应用新参数
    pid_reset(pTargetPID);

    printf("Set %c Axis PID: P=%.3f, I=%.3f, D=%.3f\r\n", *argv[1], pTargetPID->Kp, pTargetPID->Ki, pTargetPID->Kd);
    return 0;
}
MSH_CMD_EXPORT(jaw_set_pid, set jaw pid param: axis kp ki kd);


// =======================================================
// 命令 2: 设置位置并启动运动 (位置伺服核心)
// 格式: jaw_set_pos <axis: x/y> <target_code>
// 示例: jaw_set_pos x 10000
// =======================================================
static int8_t jaw_set_pos(int argc, char **argv)
{
    if (argc < 3) 
    {
        printf("Usage: jaw_set_pos <x/y> <target_val>\r\n");
        return -1;
    }

    uint8_t axis_enum = 0;
    float new_target = (float)atof(argv[2]);

    // 1. 更新全局目标位置变量
    if (*argv[1] == 'x' || *argv[1] == 'X') 
    {
        g_target_pos_X = new_target;
        axis_enum = X;
        printf("Target X set to: %.1f\r\n", g_target_pos_X);
    }
    else if (*argv[1] == 'y' || *argv[1] == 'Y') 
    {
        g_target_pos_Y = new_target;
        axis_enum = Y;
        printf("Target Y set to: %.1f\r\n", g_target_pos_Y);
    }
    else 
    {
        printf("Err: axis must be x or y\r\n");
        return -1;
    }

    // 2. 硬件使能
    motorEnable(axis_enum);

    // 3. 开启控制回路 (如果定时器没开，就开启)
    if (htim4.State != HAL_TIM_STATE_BUSY)
    {
        if (HAL_TIM_Base_Start_IT(&htim4) != HAL_OK)
        {
            printf("TIM4 Start Failed!\r\n");
            return -1;
        }
        printf("TIM4 Loop Started.\r\n");
    }

    return 0;
}
MSH_CMD_EXPORT(jaw_set_pos, set jaw target position: axis pos);


// =======================================================
// 命令 3: 停止伺服 (急停)
// =======================================================
static int8_t jaw_stop(int argc, char **argv)
{
    // 关闭定时器中断，停止计算
    HAL_TIM_Base_Stop_IT(&htim4);
    
    // 物理层 disable
    motorDisable(X);
    motorDisable(Y);
    
    // 输出归零
    motorCtrlByPWM(0, X);
    motorCtrlByPWM(0, Y);

    printf("Jaw Control Stopped.\r\n");
    return 0;
}
MSH_CMD_EXPORT(jaw_stop, stop all jaw movement);

// =======================================================
// 命令 4: 监控状态 (Debug用)
// =======================================================
static int8_t jaw_status(int argc, char **argv)
{
    printf("=== Jaw Status ===\r\n");
    printf("X: Cur=%d, Tar=%.1f, PID_Out=%.2f\r\n", getEncodeValue(X), g_target_pos_X, hPID_X.output);
    printf("Y: Cur=%d, Tar=%.1f, PID_Out=%.2f\r\n", getEncodeValue(Y), g_target_pos_Y, hPID_Y.output);
    return 0;
}
MSH_CMD_EXPORT(jaw_status, show jaw status);

#endif