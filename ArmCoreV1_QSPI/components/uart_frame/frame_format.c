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
#include "ulog.h"

#define FRAME_HEADER_OFFSET     0
#define FRAME_HEADER_LEN        2

#define FRAME_COUNT_OFFSET      (FRAME_HEADER_OFFSET + FRAME_HEADER_LEN)//2
#define FRAME_COUNT_LEN         2


#define FRAME_PAYLOAD_LEN_OFFSET   (FRAME_COUNT_OFFSET + FRAME_COUNT_LEN)//4
#define FRAME_PAYLOAD_LEN_LEN         2

#define FRAME_ID_ACK_OFFSET   (FRAME_PAYLOAD_LEN_OFFSET + FRAME_PAYLOAD_LEN_LEN)//6
#define FRAME_ID_ACK_LEN 4

#define FRAME_TYPE_OFFSET   (FRAME_ID_ACK_OFFSET + FRAME_ID_ACK_LEN)//10
#define FRAME_TYPE_LEN 1

#define FRAME_DATA_LEN_OFFSET   (FRAME_TYPE_OFFSET + FRAME_TYPE_LEN)//11
#define FRAME_DATA_LEN_LEN 2

#define FRAME_ZERO_OFFSET   (FRAME_DATA_LEN_OFFSET + FRAME_DATA_LEN_LEN)//13
#define FRAME_ZERO_LEN  1

#define FRAME_DATA_OFFSET   (FRAME_ZERO_OFFSET + FRAME_ZERO_LEN)//13

#define FRAME_CRC_LEN           4
#define FRAME_EXTRA_LEN         (FRAME_DATA_OFFSET + FRAME_CRC_LEN)
//55 aa   13 01   0e 00   01 00 00 00  05  07 00  00 80 02 02 00 b0 00 af 15 e1 31
//55 aa   03 00   0e 00   01 00 00 00  05  07 00  00 80 02 02 00 40 03 f3 32 51 15
//55 aa   06 00   0f 00   01 00 00 00  85  09 00  00 00 00 82 04 00 40 03 4c 4a 4c f2 3a ef 
//55 aa   07 00   0f 00   01 00 00 00  85  09 00  00 82 04 00 40 03 4c 4a 20 ef 63 c2 
//55 aa   08 00   0f 00   01 00 00 00  85  09 00  00 00 82 04 00 40 03 4c 4a 6b 4e 56 d4 
//55 aa   09 00   0f 00   01 00 00 00  85  08 00  00 00 82 04 00 40 03 4c 4a  0d a5 76 ec 
//55 aa   0b 00   0f 00   01 00 00 00  85  09 00  00 00 82 04 00 40 03 49 4a  d5 ff f7 21 
//55 aa   0c 00   0f 00   01 00 00 00  85  09 00  00 00 82 04 00 40 03 49 4a  d5 ff f7 21 
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
        LOG_E("args error\r\n");
        return -1;
    }

    int8_t ret = 0;

    /* 1. check header */
    if (buf[FRAME_HEADER_OFFSET] != 0x55 || buf[FRAME_HEADER_OFFSET + 1] != 0xAA)
    {
        LOG_E("header error\r\n");
        ret = -2;
    }

    /* 2. check crc32 */
    // HAL_StatusTypeDef stat = hardware_crc_config(CRC32);
    // if (stat != HAL_OK)
    // {
    //     LOG_E("hw crc32 config error\r\n");
    //     return -3;
    // }

    uint16_t len = buf[4] | buf[4 + 1] << 8;
    LOG_I("len = %d\r\n",len);
    if (size < len + 10)
    {
        LOG_E("size error: %d, %d\r\n", size, len);
        ret = -4;
    }
    uint32_t crc_cal = hardware_crc_calculate(CRC32, &buf[2], len + 4) ^ 0xFFFFFFFF;
    uint32_t crc_recv = buf[len + 6] | buf[len + 6 +1] << 8 | buf[len + 6 + 2] << 16 | buf[len + 6 + 3] << 24;
    if (crc_cal != crc_recv)
    {
        LOG_E("crc32 check error: %.8x, %.8x\r\n", crc_cal, crc_recv);
#if 1
        for (int i = 0; i < size; i++)
        {
            LOG_I("%02x ", buf[i]);
        }
        LOG_I("\r\n");
#endif
        ret = -5;
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
        LOG_E("frame stats update cnt error: %d\r\n", ret);
    }

    ret = frame_stats_recv_update(stats, FRAME_UPDATE_CRC, ret);
    if (ret != 0)
    {
        LOG_E("frame stats update crc error: %d\r\n", ret);
    }

    ret = frame_stats_recv_update(stats, FRAME_UPDATE_LOST, buf[FRAME_COUNT_OFFSET] | buf[FRAME_COUNT_OFFSET + 1] << 8);
    if (ret != 0)
    {
        LOG_E("frame stats update lost error: %d\r\n", ret);
    }
#endif

    return ret;
}


int8_t frame_format_pack_and_send(struct frame_statistics *stats, uint8_t *buf, uint16_t len, int8_t (*cb)(uint8_t *buf, uint16_t size, uint32_t timeout), uint32_t timeout)
{
    //cjh tag

    if (buf == NULL || len == 0)
    {
        LOG_E("args error\r\n");
        return -1;
    }

    int8_t ret = 0;

    uint8_t buf_send[128] = {0};
  

   // header: 55 aa   count:02 00   payload len:0e 00   7bytes:01 00 00 00 05(85) 07 00   data :00 80 02 02 00 40 03 12 84 03 fa 
    /* 1. fill header */
    buf_send[FRAME_HEADER_OFFSET] = 0x55; //0//55
    buf_send[FRAME_HEADER_OFFSET + 1] = 0xAA;//1//aa

    /* 2. fill count */
    uint16_t *cnt = frame_stats_send_cnt_get(stats);
    buf_send[FRAME_COUNT_OFFSET] = *cnt & 0xFF; //2//02
    buf_send[FRAME_COUNT_OFFSET + 1] = (*cnt >> 8) & 0xFF; //3//00  

    /* 3. fill payload len */
    buf_send[FRAME_PAYLOAD_LEN_OFFSET] = (len+8) & 0xFF;//4//0e
    buf_send[FRAME_PAYLOAD_LEN_OFFSET + 1] = ((len+8) >> 8) & 0xFF;//5//00

    buf_send[FRAME_ID_ACK_OFFSET] = 0x01& 0xFF; //6//01
    buf_send[FRAME_ID_ACK_OFFSET + 1] = 0x00& 0xFF; //7//00
    buf_send[FRAME_ID_ACK_OFFSET + 2] = 0x00& 0xFF; //8//00
    buf_send[FRAME_ID_ACK_OFFSET + 3] = 0x00& 0xFF; //9//05

    buf_send[FRAME_TYPE_OFFSET] = 0x85& 0xFF; //10//85

    buf_send[FRAME_DATA_LEN_OFFSET] = (len + 1 )& 0xFF; //11//01
    buf_send[FRAME_DATA_LEN_OFFSET + 1] = (len + 1 >> 8) & 0xFF; //12//00
      
    buf_send[FRAME_ZERO_OFFSET] = 0x00& 0xFF; //13//00

    /* 4. fill payload */
    memcpy(&buf_send[FRAME_DATA_OFFSET], buf, len);//14

    /* 5. fill crc32 */
    // HAL_StatusTypeDef stat = hardware_crc_config(CRC32);
    // if (stat != HAL_OK)
    // {
    //     LOG_E("hw crc32 config error\r\n");
    //     return -2;
    // }
    // LOG_I("len: %d\r\n", len);
    uint32_t crc_cal = hardware_crc_calculate(CRC32, &buf_send[FRAME_COUNT_OFFSET], len + 1 + FRAME_COUNT_LEN + FRAME_PAYLOAD_LEN_LEN + 7) ^ 0xFFFFFFFF;
    buf_send[len + FRAME_DATA_OFFSET] = crc_cal & 0xFF;
    buf_send[len + FRAME_DATA_OFFSET + 1] = (crc_cal >> 8) & 0xFF;
    buf_send[len + FRAME_DATA_OFFSET + 2] = (crc_cal >> 16) & 0xFF;
    buf_send[len + FRAME_DATA_OFFSET + 3] = (crc_cal >> 24) & 0xFF;

    /* 6. send data */
    if (cb != NULL)
    {
        ret = cb(buf_send, len + FRAME_EXTRA_LEN + 2, timeout);
    }
    LOG_I("bAAAAAAAAAAAAAAAAAAuf_send data: \r\n");
    for (uint16_t i = 0; i < len + FRAME_EXTRA_LEN; i++) {
        LOG_I("%02x ", buf_send[i]);
    }
    LOG_I("\r\n");
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
        LOG_E("frame_stats_mutex create failed\r\n");
        return -1;
    }

    osTimerId_t timer_stats = osTimerNew(frame_stats_polling, osTimerPeriodic, frame_stats_get(), NULL);
    if (timer_stats == NULL)
    {
        LOG_E("timer_stats create failed\r\n");
        return -2;
    }

    osStatus_t stat = osTimerStart(timer_stats, 1);
    if (stat != osOK)
    {
        LOG_E("timer_stats start failed\r\n");
        return -3;
    }

    return 0;
}
INIT_APP_EXPORT(frame_statistics_init);
#endif

