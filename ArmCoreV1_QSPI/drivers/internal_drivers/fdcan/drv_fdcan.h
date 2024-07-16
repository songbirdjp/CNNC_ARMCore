#ifndef __DRV_FDCAN_H__
#define __DRV_FDCAN_H__

#include <stdint.h>
#include "stm32h7xx_hal.h"
#include "cmsis_os2.h"

#ifdef __cplusplus
extern "C" {
#endif

#define FDCAN_MAX_DATA_LEN      64

#define DEVICE_NAME_LENGTH      16
#define DEVICE_NAME_FDCAN1      "fdcan1"
#define DEVICE_NAME_FDCAN2      "fdcan2"
#define DEVICE_NAME_FDCAN3      "fdcan3"


struct fdcan_filter
{
    FDCAN_FilterTypeDef filter;
    struct global_filter
    {
        uint32_t NonMatchingStd;
        uint32_t NonMatchingExt;
        uint32_t RejectRemoteStd;
        uint32_t RejectRemoteExt
    }global_filter;
};

struct fdcan_tx_msg
{
    FDCAN_TxHeaderTypeDef header;
    uint8_t *buf;
};
struct fdcan_rx_msg
{
    FDCAN_RxHeaderTypeDef header;
    uint8_t buf[FDCAN_MAX_DATA_LEN];
};


struct device_fdcan
{
    FDCAN_HandleTypeDef fdcan;

    uint8_t name[DEVICE_NAME_LENGTH];
    uint8_t open_state;
    osMutexId_t tx_mutex;
    osMessageQueueId_t rx_queue;

    int8_t (*open)(struct device_fdcan *fdcan);
    int8_t (*close)(struct device_fdcan *fdcan);
    int8_t (*write)(struct device_fdcan *fdcan, struct fdcan_tx_msg *msg, uint32_t timeout);
    int8_t (*read)(struct device_fdcan *fdcan, struct fdcan_rx_msg *msg, uint32_t timeout);
    int8_t (*ioctl)(struct device_fdcan *fdcan, uint32_t cmd, void *arg);
    int8_t (*rx_cb)(void *arg);

};

enum fdcan_cmd
{
    FDCAN_CMD_SET_RX_GLOBAL_FILTER,     /* global filter */
    FDCAN_CMD_SET_RX_FILTER,            /* std or ext filter */
    FDCAN_CMD_SET_RX_QUEUE,
    FDCAN_CMD_SET_RX_CALLBACK,
    FDCAN_CMD_SET_IRQ_LINE,             /* set irq to line0 or line1 */
    FDCAN_CMD_SET_IRQ_ENABLE,           /* set irq enable or disable */
    FDCAN_CMD_SET_START,                /* start or stop */
    FDCAN_CMD_GET_ERR_CNT,
    FDCAN_CMD_GET_PROTO_STAT
};

int8_t fdcan_init(struct device_fdcan *fdcan, uint8_t *device_name);

#ifdef __cplusplus
}
#endif

#endif /* __DRV_FDCAN_H__ */