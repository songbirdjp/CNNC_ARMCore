#ifndef __DRV_OCTOSPI_H__
#define __DRV_OCTOSPI_H__

#include "stm32h7xx_hal.h"
#include "cmsis_os2.h"

#ifdef __cplusplus
extern "C" {
#endif

// #define USING_OSPI_DMA_MODE
#define DEVICE_NAME_LENGTH      16

#define DEVICE_NAME_OSPI1       "octospi1"
#define DEVICE_NAME_OSPI2       "octospi2"

#define USING_OSPI_OPTION_FUNCTION

#ifdef USING_OSPI_OPTION_FUNCTION
typedef struct drv_opt
{
    int8_t (*before_write)(struct drv_ospi *ospi);
    int8_t (*after_write)(struct drv_ospi *ospi);
    int8_t (*complete_write)(struct drv_ospi *ospi);

    int8_t (*before_read)(struct drv_ospi *ospi);
    int8_t (*after_read)(struct drv_ospi *ospi);
    int8_t (*complete_read)(struct drv_ospi *ospi);

}DEVICE_OSPI_OPT;
#endif

#define USING_OSPI_SLAVE_TO_MASTER_INTERRUPT

#ifdef USING_OSPI_SLAVE_TO_MASTER_INTERRUPT
#define NODE_NAME_LENGTH    16
typedef struct irq_info
{
    uint8_t node_name[NODE_NAME_LENGTH];
    uint16_t irq_pin;
    osEventFlagsId_t irq_event;
    uint32_t irq_event_flag;
}IRQ_INFO_NODE;

typedef struct drv_irq_list
{
    struct irq_info *node_data;
    struct drv_irq_list *next;
}DEVICE_IRQ_LIST;
#endif

struct drv_ospi
{
    OSPI_HandleTypeDef hospi;
    uint8_t name[DEVICE_NAME_LENGTH];
    uint8_t open_state;
    osMessageQueueId_t rx_queue;
    osEventFlagsId_t rx_event;
    osMutexId_t tx_mutex;
    osEventFlagsId_t tx_event;

    uint8_t *rx_buf;  /* used for dma */ 
    uint16_t rx_buf_len;

#ifdef USING_OSPI_SLAVE_TO_MASTER_INTERRUPT
    DEVICE_IRQ_LIST *irq_list;
#endif

    int8_t (*open)(struct drv_ospi *ospi);
    int8_t (*close)(struct drv_ospi *ospi);
    int8_t (*write)(struct drv_ospi *ospi, OSPI_RegularCmdTypeDef *cmd_buf, uint8_t *data_buf, uint32_t timeout);
    int8_t (*read)(struct drv_ospi *ospi, OSPI_RegularCmdTypeDef *cmd_buf, uint8_t *data_buf, uint32_t timeout);
    int8_t (*ioctl)(struct drv_ospi *ospi, uint8_t cmd, void *arg);
    int8_t (*rx_cb)(void *arg); /* for slave mode */

#ifdef USING_OSPI_OPTION_FUNCTION
    struct drv_opt opt;
#endif

};

typedef struct drv_ospi DEVICE_OSPI;


int8_t ospi_init(DEVICE_OSPI *ospi, uint8_t *device_name);
int8_t ospi_rx_queue_init(DEVICE_OSPI *ospi, osMessageQueueId_t queue);
int8_t ospi_rx_callback_register(DEVICE_OSPI *ospi, int8_t (*cb)(void *arg));
int8_t ospi_dma_rx_buf_init(DEVICE_OSPI *ospi, uint8_t *buf, uint16_t len);

#ifdef USING_OSPI_OPTION_FUNCTION
int8_t ospi_opt_init(DEVICE_OSPI *ospi, DEVICE_OSPI_OPT *opt_func);
#endif

#ifdef USING_OSPI_SLAVE_TO_MASTER_INTERRUPT
int8_t device_ospi_irq_node_add(DEVICE_OSPI *ospi, IRQ_INFO_NODE *node);
DEVICE_IRQ_LIST *device_ospi_irq_node_find(DEVICE_OSPI *ospi, uint8_t *node_name);
int8_t device_ospi_irq_node_delete(DEVICE_OSPI *ospi, uint8_t *node_name);
int8_t device_ospi_irq_list_clear(DEVICE_OSPI *ospi);
int8_t device_ospi_irq_list_list(DEVICE_OSPI *ospi);
int8_t device_ospi_irq_wait_with_block(DEVICE_OSPI *ospi, uint8_t *node_name, char splitter, uint32_t timeout);
#endif







#ifdef __cplusplus
}
#endif

#endif