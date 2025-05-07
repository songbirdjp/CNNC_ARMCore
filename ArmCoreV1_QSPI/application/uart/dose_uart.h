#ifndef __DOSE_UART_H__
#define __DOSE_UART_H__

#include <stdint.h>
#include "cmsis_os2.h"

#ifdef __cplusplus
extern "C" {
#endif

#define DOSE_UART_ID    0
#define DOSE_UART_FRAME_SIZE_MAX    128

struct dose_object
{
    union
    {
        uint8_t byte;
        struct
        {
            uint8_t cmd_id : 7;
            uint8_t cmd_ack : 1;
        }bits;
    }id;    /* device rs422 id */

    uint8_t type;   /* cmd type */
    uint16_t *len;   /* data length */
    uint8_t *data;  /* data pointer */
};

enum dose_board
{
    DOSE_BOARD_TRIGGER_OUT = 1,
    DOSE_BOARD_NO_TRIGGER_OUT,
    DOSE_BOARD_MAX
};

struct calibration_para
{
    union
    {
        struct
        {
            uint8_t lock : 1;   /* 0: unlock  1: lock */
            uint8_t valid : 1;  /* 0: invalid 1: valid */
            uint8_t reserved : 6;
        }bits;
        
        uint8_t byte;
    }status;

    uint32_t adc_factor[5];     /* Kadc <=> 1MU */
    uint32_t dac_factor;        /* dummy dose dac factor, Kdac <=> 1MU */
    uint32_t trig_interval_min; /* trigger interval minimal value */
};

struct treatment_para
{
    union
    {
        struct
        {
            uint8_t lock : 1;   /* 0: unlock  1: lock */
            uint8_t check : 1;  /* 0: fail  1: pass */
            uint8_t reserved : 6;
        }bits;
        
        uint8_t byte;
    }status;

    uint8_t dose_mode;  /* 0: dummy 1: normal */
    uint8_t pulse_mode; /* 0: PRF   1: fixed dose rate */
    uint8_t prf_hz;     /* PRF fixed */
    uint8_t ri_src;     /* radiation index source, 0: bgm  1: internal */
};

struct dose_rate_threshold
{
    uint8_t low;    /* percentage */
    uint8_t high;   /* percentage */
};

struct dose_cp_threshold
{
    uint8_t low;    /* percentage */
    uint8_t high;   /* percentage */
};

struct dose_one_pulse
{
    uint8_t threshold_low;  /* percentage */
    uint8_t threshold_high; /* percentage */

    uint16_t count_low;     /* count of low threshold pulse */
    uint16_t count_high;    /* count of high threshold pulse */
    uint32_t count_abnormal;/* count of abnormal pulse */

};

struct interlock_para
{
    struct dose_rate_threshold threshold_dose_rate;
    struct dose_cp_threshold threshold_dose_cp;
    struct dose_one_pulse one_pulse;
    uint8_t threshold_symmetry;     /* percentage */
    uint16_t communication_timeout; /* Unit: ms */
};

struct radiation_point_para
{
    uint8_t cp;                         /* radiation index at CP */
    uint16_t index;                     /* current radiation index */
    uint16_t index_max_in_cp;           /* max radiation index at current CP */
    uint32_t dose_rate_interpolated;    /* dose rate interpolated at current radiation index */
    uint64_t dose_interpolated;         /* dose interpolated at current radiation index */

};

struct control_para
{
    enum dose_board board_id;    
    struct calibration_para calibration;
    struct treatment_para treatment;
    struct interlock_para interlock;
    struct radiation_point_para radiation;

    osMutexId_t mutex;
};

int8_t radiation_index_update_callback(int8_t (*cb)(void));
int8_t control_data_pointer_get(void **ptr);
int8_t dose_uart_cmd_write(struct dose_object *cmd);

#ifdef __cplusplus
}
#endif

#endif /* __DOSE_UART_H__ */