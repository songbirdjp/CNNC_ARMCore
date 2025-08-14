#include "pid_ctrl.h"
#include "arm_math.h"
#include "FreeRTOS.h"

int8_t pid_para_init(struct pid_instance *pid)
{
    if (pid == NULL)
    {
        return -1;
    }

    arm_pid_instance_f32 *pid_ctrl = (arm_pid_instance_f32 *)pvPortMalloc(sizeof(arm_pid_instance_f32));
    if (pid_ctrl == NULL)
    {
        return -2;
    }

    pid->pid_ins = (void *)pid_ctrl;

    pid_ctrl->Kp = pid->kp;
    pid_ctrl->Ki = pid->ki * pid->delta_t;
    pid_ctrl->Kd = pid->kd / pid->delta_t;

    arm_pid_init_f32(pid_ctrl, 1);

    return 0;
}

int8_t pid_cal(struct pid_instance *pid, float error, float *result)
{
    if (pid == NULL)
    {
        return -1;
    }

    arm_pid_instance_f32 *pid_ctrl = (arm_pid_instance_f32 *)pid->pid_ins;

    float32_t val_cal = arm_pid_f32(pid_ctrl, error);

    /* 进行输出限幅 */
    if (val_cal > pid->max_output) 
    {
        val_cal = pid->max_output;
    } 
    else if (val_cal < pid->min_output) 
    {
        val_cal = pid->min_output;
    }

    pid_ctrl->state[2] = val_cal;

    *result = val_cal;

    return 0;
}

int8_t pid_state_reset(struct pid_instance *pid)
{
    if (pid == NULL)
    {
        return -1;
    }

    arm_pid_reset_f32((arm_pid_instance_f32 *)pid->pid_ins);

    return 0;
}

#ifndef PID_TEST
#include "shell.h"
#include <stdlib.h>

#define OUTPUT_MAX 100
#define OUTPUT_MIN 0

static int8_t pid_control(uint8_t argc, char *argv[]) 
{
    if (argc != 5)
    {
        printf("Usage: pid_ctrl Kp Ki Kd delta_t\n");
        return -1;
    }

    float32_t kp = (float32_t)atof(argv[1]);
    float32_t ki = (float32_t)atof(argv[2]);
    float32_t kd = (float32_t)atof(argv[3]);
    float32_t delta_t = (float32_t)atof(argv[4]);

    struct pid_instance pid_ctrl = 
    {
        .kp = kp,
        .ki = ki,
        .kd = kd,
        .delta_t = delta_t,
        .max_output = OUTPUT_MAX,
        .min_output = OUTPUT_MIN
    };


    pid_para_init(&pid_ctrl);

    float32_t output = 0;
    float32_t r = 50;//get_reference();    // 读取当前系统的输入量
    float32_t y = 0;//get_status();       // 读取当前的被控量
    float32_t error = r - y;          // 计算偏差
    float32_t diff = 0;

    while (1) 
    {
        printf("error:%f, output:%f\n", error, output);

        pid_cal(&pid_ctrl, error, &output);

        if (error >= 0)
        {
            diff = -(float32_t)(rand() % 100) / 100;          // 计算偏差
        }
        else
        {
            diff = (float32_t)(rand() % 100) / 100;
        }

        error += diff;

        osDelay(100);
    }
}
MSH_CMD_EXPORT_ALIAS(pid_control, pid_ctrl, pid ctrl test);
#endif