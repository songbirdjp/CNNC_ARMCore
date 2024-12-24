/* frame format :   | header | count | data len | data | crc32 |
 *
 *   字节序: LSB first
 *   比特序: lsb first
 *  
 *   header: 2 bytes，固定为0x55 0xAA
 *   count:  2 bytes，表示当前帧序号，从0开始，每发送一帧递增1，溢出时归零继续计数
 *   data len: 2 bytes，表示数据长度
 *   data: 表示数据内容，见下述说明
 *   crc32: 4 bytes，表示数据校验值，采用CRC32算法， polynomial=0x04C11DB7, init=0xFFFFFFFF, xor=0xFFFFFFFF
 *                   计算时，包含count、data len、data三个字段
 *  
 *   data segment:  | ack | id | protocol data |
 *   
 *   id 与 ack 共占用 1 byte -> id：7 bits, ack：1 bit
 *   id: 标识访问的总线上设备的id标号，范围0~127，0：广播    1~127：具体设备
 *   ack: 表示是否需要应答，0：不需要，1：需要
 *   protocol data: 协议数据，占用字节数由具体协议确定
 *
 * hardware configuration:
 *   - UART: 500kbps, 8-N-1, no flow control;  DMA mode, no fifo, idle interrupt
 */

#include "frame_format.h"
#include "hw_crc.h"

#define FRAME_HEADER_OFFSET     0
#define FRAME_HEADER_LEN        2

#define FRAME_COUNT_OFFSET      (FRAME_HEADER_OFFSET + FRAME_HEADER_LEN)
#define FRAME_COUNT_LEN         2

#define FRAME_DATA_LEN_OFFSET   (FRAME_COUNT_OFFSET + FRAME_COUNT_LEN)
#define FRAME_DATA_LEN          2

#define FRAME_DATA_OFFSET       (FRAME_DATA_LEN_OFFSET + FRAME_DATA_LEN)

#define FRAME_CRC_LEN           4
#define FRAME_EXTRA_LEN         (FRAME_DATA_OFFSET + FRAME_CRC_LEN)

#define FRAME_TIMEOUT_COM1 1000

struct frame_statistics frame_stats_com1 = {

#if DEVICE_IS_MASTER
    .timeout = FRAME_TIMEOUT_COM1,
#endif

#if DEVICE_IS_MASTER && FRAME_IS_ACK_MODE
    .min_time_cnt = UINT16_MAX,
#endif

#if DEVICE_IS_SLAVE_PERIODIC
    .ostick_begin = FRAME_TIMEOUT_COM1,
#endif

};

static uint16_t *frame_stats_send_cnt_get(struct frame_statistics *stats)
{
#if DEVICE_IS_MASTER
    return (uint16_t *)&stats->send_cnt;
#else
    return &stats->ack_cnt;
#endif
}

int8_t frame_format_parse(struct frame_statistics *stats, uint8_t *buf, uint16_t size, uint16_t *offset, uint16_t *length)
{
    if (stats == NULL || buf == NULL || size < FRAME_EXTRA_LEN)
    {
        printf("args error\r\n");
        return -1;
    }

    int8_t ret = 0;

    /* 1. check header */
    if (buf[FRAME_HEADER_OFFSET] != 0x55 || buf[FRAME_HEADER_OFFSET + 1] != 0xAA)
    {
        printf("header error\r\n");
        ret = -2;
    }

    /* 2. check crc32 */
    uint16_t len = buf[FRAME_DATA_LEN_OFFSET] | buf[FRAME_DATA_LEN_OFFSET + 1] << 8;
    if (size < len + FRAME_EXTRA_LEN)
    {
        printf("size error: %d, %d\r\n", size, len);
        ret = -3;
    }

    uint32_t crc_cal = hardware_crc_calculate(CRC32, &buf[FRAME_COUNT_OFFSET], len + FRAME_COUNT_LEN + FRAME_DATA_LEN) ^ 0xFFFFFFFF;
    uint32_t crc_recv = buf[len + FRAME_DATA_OFFSET] | buf[len + FRAME_DATA_OFFSET + 1] << 8 | buf[len + FRAME_DATA_OFFSET + 2] << 16 | buf[len + FRAME_DATA_OFFSET + 3] << 24;
    if (crc_cal != crc_recv)
    {
        printf("crc32 check error\r\n");
#if 1
        for (int i = 0; i < size; i++)
        {
            printf("%02x ", buf[i]);
        }
        printf("\r\n");
#endif
        ret = -4;
    }

    /* 3. update data offset and len */
    *offset = FRAME_DATA_OFFSET;
    *length = len;

#if !DEVICE_IS_MASTER
    /* 4. update frame count */
    *frame_stats_send_cnt_get(stats) = buf[FRAME_COUNT_OFFSET] | buf[FRAME_COUNT_OFFSET + 1] << 8;
#endif

#ifdef USING_FRAME_STATISTICS
    /* 5. update frame statistics */
    ret = frame_stats_recv_update(stats, FRAME_UPDATE_CNT, 1);
    if (ret != 0)
    {
        printf("frame stats update cnt error: %d\r\n", ret);
    }

    ret = frame_stats_recv_update(stats, FRAME_UPDATE_CRC, ret);
    if (ret != 0)
    {
        printf("frame stats update crc error: %d\r\n", ret);
    }

    ret = frame_stats_recv_update(stats, FRAME_UPDATE_LOST, buf[FRAME_COUNT_OFFSET] | buf[FRAME_COUNT_OFFSET + 1] << 8);
    if (ret != 0)
    {
        printf("frame stats update lost error: %d\r\n", ret);
    }
#endif

    return ret;
}


int8_t frame_format_pack_and_send(struct frame_statistics *stats, uint8_t *buf, uint16_t len, int8_t (*cb)(uint8_t *buf, uint16_t size, uint32_t timeout), uint32_t timeout)
{
    if (buf == NULL || len == 0)
    {
        printf("args error\r\n");
        return -1;
    }

    int8_t ret = 0;

    uint8_t buf_send[128] = {0};

    /* 1. fill header */
    buf_send[FRAME_HEADER_OFFSET] = 0x55;
    buf_send[FRAME_HEADER_OFFSET + 1] = 0xAA;

    /* 2. fill count */
    uint16_t *cnt = frame_stats_send_cnt_get(stats);
    buf_send[FRAME_COUNT_OFFSET] = *cnt & 0xFF;
    buf_send[FRAME_COUNT_OFFSET + 1] = (*cnt >> 8) & 0xFF;

    /* 3. fill data len */
    buf_send[FRAME_DATA_LEN_OFFSET] = len & 0xFF;
    buf_send[FRAME_DATA_LEN_OFFSET + 1] = (len >> 8) & 0xFF;

    /* 4. fill data */
    memcpy(&buf_send[FRAME_DATA_OFFSET], buf, len);

    /* 5. fill crc32 */
    uint32_t crc_cal = hardware_crc_calculate(CRC32, &buf_send[FRAME_COUNT_OFFSET], len + FRAME_COUNT_LEN + FRAME_DATA_LEN) ^ 0xFFFFFFFF;
    buf_send[len + FRAME_DATA_OFFSET] = crc_cal & 0xFF;
    buf_send[len + FRAME_DATA_OFFSET + 1] = (crc_cal >> 8) & 0xFF;
    buf_send[len + FRAME_DATA_OFFSET + 2] = (crc_cal >> 16) & 0xFF;
    buf_send[len + FRAME_DATA_OFFSET + 3] = (crc_cal >> 24) & 0xFF;

    /* 6. send data */
    if (cb != NULL)
    {
        ret = cb(buf_send, len + FRAME_EXTRA_LEN, timeout);
    }

#ifdef USING_FRAME_STATISTICS
    /* 7. update frame statistics */
    ret |= frame_stats_send_update(stats);

#endif

    return ret;
}

#ifdef FRAME_TIMEOUT_THREAD_DETECT
static struct frame_statistics *frame_stats_get(void)
{
    return &frame_stats_com1;
}

static int8_t frame_statistics_init(void)
{
    osMutexAttr_t mutex_attributes = {
    .name = "frame_stats_mutex",
    .attr_bits = osMutexRecursive | osMutexPrioInherit
    };

    frame_stats_get()->mutex = osMutexNew(&mutex_attributes);
    if (frame_stats_get()->mutex == NULL)
    {
        printf("frame_stats_mutex create failed\r\n");
        return -1;
    }

    osTimerId_t timer_stats = osTimerNew(frame_stats_polling, osTimerPeriodic, frame_stats_get(), NULL);
    if (timer_stats == NULL)
    {
        printf("timer_stats create failed\r\n");
        return -2;
    }

    osStatus_t stat = osTimerStart(timer_stats, 1);
    if (stat != osOK)
    {
        printf("timer_stats start failed\r\n");
        return -3;
    }

    return 0;
}
INIT_APP_EXPORT(frame_statistics_init);
#endif

