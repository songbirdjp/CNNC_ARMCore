#ifndef __DRV_SPI_H__
#define __DRV_SPI_H__

#include "stm32h7xx_hal.h"
#include "cmsis_os2.h"

#ifdef __cplusplus
extern "C" {
#endif

#define DEVICE_NAME_LENGTH      16

#define DEVICE_NAME_SPI1       "spi1"
#define DEVICE_NAME_SPI2       "spi2"
#define DEVICE_NAME_SPI3       "spi3"
#define DEVICE_NAME_SPI6       "spi6"

#define USING_SPI_OPTION_FUNCTION

#ifdef USING_SPI_OPTION_FUNCTION
typedef struct drv_opt
{
    int8_t (*before_write)(struct drv_spi *spi);
    int8_t (*after_write)(struct drv_spi *spi);
    int8_t (*complete_write)(struct drv_spi *spi);

    int8_t (*before_read)(struct drv_spi *spi);
    int8_t (*after_read)(struct drv_spi *spi);
    int8_t (*complete_read)(struct drv_spi *spi);

}DEVICE_SPI_OPT;
#endif

#define USING_SPI_SLAVE_TO_MASTER_INTERRUPT

#ifdef USING_SPI_SLAVE_TO_MASTER_INTERRUPT
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

typedef enum spi_mode
{
    SPI_SLAVE = 0,
    SPI_MASTER = 1
}SPI_MODE;

struct drv_spi
{
    SPI_HandleTypeDef hspi;
    uint8_t name[DEVICE_NAME_LENGTH];
    uint8_t open_state;
    SPI_MODE master_or_slave;    /* 1: master    0:slave */
    osMessageQueueId_t rx_queue;
    osEventFlagsId_t rx_event;  /* for master mode */
    osMutexId_t tx_mutex;
    osEventFlagsId_t tx_event;

    uint8_t *rx_buf;  /* used for dma */ 
    uint16_t rx_buf_len;

#ifdef USING_SPI_SLAVE_TO_MASTER_INTERRUPT
    DEVICE_IRQ_LIST *irq_list;
#endif

    int8_t (*open)(struct drv_spi *spi);
    int8_t (*close)(struct drv_spi *spi);
    int8_t (*write)(struct drv_spi *spi, uint8_t *buf, uint16_t size, uint32_t timeout);
    int8_t (*read)(struct drv_spi *spi, uint8_t *buf, uint16_t size, uint32_t timeout);
    int8_t (*write_and_read)(struct drv_spi *spi, uint8_t *send_buf, uint8_t *recv_buf, uint16_t size, uint32_t timeout);
    int8_t (*ioctl)(struct drv_spi *spi, uint8_t cmd, void *arg);
    int8_t (*rx_queue_cb)(void *arg); /* for slave mode */

#ifdef USING_SPI_OPTION_FUNCTION
    struct drv_opt opt;
#endif

};

typedef struct drv_spi DEVICE_SPI;


int8_t spi_init(DEVICE_SPI *spi, uint8_t *device_name, SPI_MODE mode);
int8_t spi_rx_queue_init(DEVICE_SPI *spi, osMessageQueueId_t queue, int8_t (*rx_queue_cb)(void *arg));
int8_t spi_dma_rx_buf_init(DEVICE_SPI *spi, uint8_t *buf, uint16_t len);

#ifdef USING_SPI_OPTION_FUNCTION
int8_t spi_opt_init(DEVICE_SPI *spi, DEVICE_SPI_OPT *opt_func);
#endif

#ifdef USING_SPI_SLAVE_TO_MASTER_INTERRUPT
int8_t device_irq_node_add(DEVICE_SPI *spi, IRQ_INFO_NODE *node);
DEVICE_IRQ_LIST *device_irq_node_find(DEVICE_SPI *spi, uint8_t *node_name);
int8_t device_irq_node_delete(DEVICE_SPI *spi, uint8_t *node_name);
int8_t device_irq_list_clear(DEVICE_SPI *spi);
int8_t device_irq_list_list(DEVICE_SPI *spi);
int32_t device_irq_wait_with_block(DEVICE_SPI *spi, uint8_t *node_name, char splitter, uint32_t timeout);
#endif

#ifdef __cplusplus
}
#endif

#endif