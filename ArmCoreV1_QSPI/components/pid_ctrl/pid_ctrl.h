#ifndef __PID_CTRL_H__
#define __PID_CTRL_H__

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

struct pid_instance
{
    float kp;
    float ki;
    float kd;
    float delta_t;
    float max_output;
    float min_output;

    void *pid_ins; /* user no need realization */
};

int8_t pid_para_init(struct pid_instance *pid);
int8_t pid_cal(struct pid_instance *pid, float error, float *result);
int8_t pid_state_reset(struct pid_instance *pid);


#ifdef __cplusplus
}
#endif

#endif /* __PID_CTRL_H__ */