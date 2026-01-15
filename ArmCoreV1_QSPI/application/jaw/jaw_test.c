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
volatile float g_RI_End_Pos_X = 0.0f;    // 当前 RI 段的终点
volatile float g_RI_End_Pos_Y = 0.0f;
volatile float g_RI_Cur_Ref_X = 0.0f;    // 插补器当前的理论位置 ("兔子")
volatile float g_RI_Cur_Ref_Y = 0.0f;
volatile float g_RI_V_FF_X    = 0.0f;    // 计算出的前馈速度
volatile float g_RI_V_FF_Y    = 0.0f;
volatile uint8_t g_Is_Moving  = 0;       // 插补运行标志位
// 速度滤波用的静态变量建议放在函数内部或此处
static float g_last_vel_X = 0.0f;
static float g_last_vel_Y = 0.0f;
static float LowPassFilter(float new_val, float prev_val) 
{
    // 简单的 一阶滞后滤波
    // 系数 0.3 表示新值占 30%，旧值占 70%。可根据噪声情况调整 (0.1~0.5)
    return 0.3f * new_val + 0.7f * prev_val; 
}


// C. 核心逻辑：接收 RI 数据并启动插补 (供 jaw_control.c 调用)
// ==========================================
void Update_RI_Target(struct JawFlagType *recvMsg)
{
    float time_s;
    
    // 1. 更新终点坐标
    g_RI_End_Pos_X = (float)recvMsg->cmdPos[X];
    g_RI_End_Pos_Y = (float)recvMsg->cmdPos[Y];
    
    // 2. 初始化插补起点
    // 如果之前是静止状态，说明是新的一段运动，以当前实际位置作为起点
    // 如果已经在运动，则保持 g_RI_Cur_Ref 连续，不要突变
    if (g_Is_Moving == 0) {
        g_RI_Cur_Ref_X = (float)rtFeedback.jawRTPos[X];
        g_RI_Cur_Ref_Y = (float)rtFeedback.jawRTPos[Y];
        g_Is_Moving = 1; // 标记开始运动
    }

    // 3. 计算时间 (ms -> s)
    if(recvMsg->cmdTime > 0) {
        time_s = (float)recvMsg->cmdTime / 1000.0f;
    } else {
        time_s = 0.001f; // 防止除以0
    }
    
    // 4. 计算前馈速度 V_FF = (终点 - 当前理论起点) / 时间
    // 注意：这里用 g_RI_Cur_Ref 而不是实际位置，保证速度规划的平滑性
    g_RI_V_FF_X = (g_RI_End_Pos_X - g_RI_Cur_Ref_X) / time_s;
    g_RI_V_FF_Y = (g_RI_End_Pos_Y - g_RI_Cur_Ref_Y) / time_s;
}


// D. 定时器中断：1ms 双环控制 + 插补
// ==========================================
void jaw_pwm_position_ctrl_periodically(TIM_HandleTypeDef *htim)
{
    if (htim->Instance == TIM4)
    {
        // --- 1. 获取反馈与速度计算 ---
        getEncodeTotalValue(X);
        getEncodeTotalValue(Y);
        
        float pos_act_X = (float)rtFeedback.jawRTPos[X];
        float pos_act_Y = (float)rtFeedback.jawRTPos[Y];
        
        // 计算 X 轴速度 (Count/s) 并滤波
        float raw_vel_X = (float)jawControlByAxes[X].encoderDelta32 * 1000.0f; // encoderDelta32 是1ms内的增量
        float vel_act_X = LowPassFilter(raw_vel_X, g_last_vel_X);
        g_last_vel_X = vel_act_X;

        // 计算 Y 轴速度 (Count/s) 并滤波
        float raw_vel_Y = (float)jawControlByAxes[Y].encoderDelta32 * 1000.0f;
        float vel_act_Y = LowPassFilter(raw_vel_Y, g_last_vel_Y);
        g_last_vel_Y = vel_act_Y;

        // --- 2. 轨迹插补 (Trajectory Interpolation) ---
        // 只有当需要运动时才更新“兔子”的位置
        if (g_Is_Moving) 
        {
            // X 轴插补
            g_RI_Cur_Ref_X += g_RI_V_FF_X * 0.001f; // 理论位置 + V_ff * 1ms
            
            // Y 轴插补
            g_RI_Cur_Ref_Y += g_RI_V_FF_Y * 0.001f;
            
            // (可选) 增加一个判断：如果插补位置超过了终点，就钳位到终点并停止插补
            // 为了简单起见，这里先让它一直跑，直到下一条 RI 进来刷新
        }

        // --- 3. X 轴双环控制 (带前馈) ---
        
        // 位置环 (P控制): 追“插补点”，而不是追“终点”
        float pos_err_X = g_RI_Cur_Ref_X - pos_act_X;
        float v_comp_X  = hPID_X.Kp * pos_err_X; 

        // 速度指令合成: 前馈 + 补偿
        float v_cmd_X   = g_RI_V_FF_X + v_comp_X;

        // 速度环 (PI控制): 执行最终速度
        float pwm_X     = pid_increase_calculate(&hPID_Vel_X, v_cmd_X, vel_act_X);


        // --- 4. Y 轴双环控制 (带前馈) ---
        
        float pos_err_Y = g_RI_Cur_Ref_Y - pos_act_Y;
        float v_comp_Y  = hPID_Y.Kp * pos_err_Y; 
        float v_cmd_Y   = g_RI_V_FF_Y + v_comp_Y;
        float pwm_Y     = pid_increase_calculate(&hPID_Vel_Y, v_cmd_Y, vel_act_Y);

        // --- 5. 硬件输出 ---
        motorCtrlByPWM(pwm_X, X);
        motorCtrlByPWM(pwm_Y, Y);
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