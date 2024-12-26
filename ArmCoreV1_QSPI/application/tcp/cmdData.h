#ifndef __CMD_DATA_H
#define __CMD_DATA_H
#include "main.h"
#include "websocket.h"

typedef struct
{
    uint8_t id;
    uint8_t type;
    uint16_t len;
    uint8_t *data;
}CMD2UART_DATA;

void nrtRecvCommandParse(APP_DATA_RECV* info);
void AFC_ADCSampleDataFeedback(uint8_t socket);
void AFC_MagMotorFeedback(APP_DATA_RECV* info);
#endif  