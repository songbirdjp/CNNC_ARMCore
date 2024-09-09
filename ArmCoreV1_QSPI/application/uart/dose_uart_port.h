#ifndef __DOSE_UART_PORT_H__
#define __DOSE_UART_PORT_H__

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define DEVICE_DOSE_UART_NAME_DEFAULT    "uart5"

#define DOSE_UART_MAX_LENGTH      128
struct dose_uart
{
    uint8_t buf[DOSE_UART_MAX_LENGTH];
    uint16_t len;
};

int8_t device_dose_uart_init(uint8_t *device_name);
int8_t device_dose_uart_open(void);
int8_t device_dose_uart_data_read(struct dose_uart *buf, uint32_t timeout);
int8_t device_dose_uart_data_write(struct dose_uart *buf, uint16_t size, uint32_t timeout);

#ifdef __cplusplus
}
#endif


#endif /* __DOSE_UART_PORT_H__ */