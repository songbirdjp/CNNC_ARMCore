#ifndef __BGM_DEF_H__
#define __BGM_DEF_H__

#include "bgm_uart.h"
#include "dose_app.h"
#include "afc_app.h"

int8_t dose_handshake(enum uart_id id);
int8_t dose_adc_value_set(enum uart_id id, uint32_t *value);
int8_t dose_dac_value_set(enum uart_id id, uint32_t *value);
int8_t dose_meter_value_set(enum uart_id id, float *dose_meter);
float dose_meter_value_get(enum uart_id id);
int8_t dose_rate_value_set(enum uart_id id, float *dose_rate);
float dose_rate_value_get(enum uart_id id);
int8_t dose_prf_value_set(enum uart_id id, uint8_t *prf);
int8_t dose_generate_mode_set(enum uart_id id, uint8_t *mode);
int8_t dose_pulse_mode_set(enum uart_id id, uint8_t *pulse_mode);
int8_t dose_fsm_state_set(enum uart_id id, enum dose_fsm_state state);
enum dose_fsm_state dose_fsm_state_get(enum uart_id id);
uint16_t dose_interlock_get(enum uart_id id);
int8_t dose_state_polling(enum uart_id id);
int8_t dose_beam_cumulated_clear(enum uart_id id);
int8_t beam_deliver_type_get(uint16_t beam_id, uint8_t *deliver_type);
uint16_t dose_radiation_index_get(enum uart_id id);
int8_t dose_beam_info_set(enum uart_id id, uint8_t *data);
int8_t dose_beam_parameter_set(enum uart_id id, uint16_t beam_id);
int8_t dose_radiation_data_get(enum uart_id id);
int8_t dose_radiation_index_set(enum uart_id id, uint16_t index, uint8_t emergency);

void BGM_SendCmd(enum uart_id uartID, uint8_t cmdType, uint8_t *cmdData, uint8_t len);

int8_t afc_encoder_value_get(void);

#endif /* __BGM_DEF_H__ */