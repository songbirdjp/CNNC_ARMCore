#ifndef W5500H7_FPGA_RW_H
#define W5500H7_FPGA_RW_H

#include <stdint.h>
#include "stdbool.h"
#include "cmsis_os2.h"

#define TOTAL_FPGA_CMD_NUM    26
#define HEAD_LEN    5
#define CHECKSUM_LEN    1
#define PROTO_LEN   (HEAD_LEN+CHECKSUM_LEN)
#define FPGA_RT_UPLOAD_START   HEAD_LEN
#define RT_FPGA_UPLOAD_PAYLOAD_LEN  168 //it is the same as nrt payload from fpga
#define NRT_FPGA_UPLOAD_PAYLOAD_LEN  168
#define FPGA_NRT_UPLOAD_START   (HEAD_LEN+RT_FPGA_UPLOAD_PAYLOAD_LEN)
#define RT_ARM_UPLOAD_POS_LEN  170  //arm upload to plc all pos(leaf, carrier, jaw)
#define RT_DOWNLOAD_PAYLOAD_LEN  174 //plan data from plc
#define RT_SAVE_PAYLOAD_LEN  (RT_DOWNLOAD_PAYLOAD_LEN+2)    //+ RI
#define RT_SDRAM_PAYLOAD_LEN  (RT_SAVE_PAYLOAD_LEN+4)       //+ 2 leaf pos, because get 80 leafs pos from PLC, but fpga need 82 leafs pos
#define RECV_BUF_LEN  (RT_FPGA_UPLOAD_PAYLOAD_LEN*2+PROTO_LEN) //rt half（168） + nrt half（168） + head&tail(6)6
#define MAX_CMD_DATA_SIZE 180//180B in total, 174B payload

#define	CMD_BANK_SET	0x00
#define	CMD_TAR_MOD		0x01
#define	CMD_PWM_MOD		0x02
#define	CMD_PACK_MOD	0x03
#define	CMD_SINGLE_BANK	0x04
#define	CMD_CHECK_PASS	0x05

#define	CMD_CTRL_VALID	0x11
#define	CMD_SCREEN_2ND	0x12

#define	CMD_POS_MNI		0x14

#define	CMD_POS_ADJ		0x15

#define	CMD_OOT_TH		0x16

#define	CMD_WAIT_POS	0x18
#define	CMD_REF_OFFSET	0x19
#define	CMD_SEEK_STEP	0x1a

#define	CMD_PID_LEAF	0x20
#define	CMD_PID_BOX		0x21

#define	CMD_PWM_LIMIT	0x22

#define	CMD_2ND_K		0x24
#define	CMD_2ND_B		0x25
#define	CMD_2ND_TH		0x26

#define	CMD_ADC_MNI		0x27

#define	CMD_DEBUG_ENB	0x30 // debug mode, driver enable
#define	CMD_DEBUG_REF	0x31 // debug mode, ref latch enable
#define	CMD_DEBUG_PWM	0x32 // debug mode, pwm set

#define	CMD_TAR_SET		0x40 // target set

#define	CMD_STA_REQ		0x50 // state request

#define	CMD_FB_SHOW		0xff
#define	CMD_FB_RUN		0xf0

#define	CMD_FB_PID		0xC0
#define	CMD_FB_ADC		0xC8

#define PACKF0_CMD   0xF0
#define PACKF1_CMD   0xF1

// sync Word / Byte
#define	RX_SYNC_BYTE_H	0xaa
#define	RX_SYNC_BYTE_L	0x55

#define	TX_SYNC_BYTE_H	0x55
#define	TX_SYNC_BYTE_L	0xaa

typedef struct {
    uint8_t cmd;
    uint8_t TxLen;
   // uint16_t offsetInTCPAry;
    bool useAsParam;
}SEND_PARAM;

typedef struct {
    uint8_t cmdSendBuf[MAX_CMD_DATA_SIZE];
    uint8_t singleSize[TOTAL_FPGA_CMD_NUM];
    uint8_t writeIndex[TOTAL_FPGA_CMD_NUM];
  //  uint8_t paramSendBuf[CURRENT_USE_PARAM_LEN];
    uint8_t* paramSendBuf;
    uint8_t* pCrt;
    uint16_t totalSize;
}SEND_CONTROL;

typedef struct {
    uint8_t head;
    uint8_t tail;
    uint8_t size;
    uint8_t *array;
}RINGBUFFER;

//extern uint16_t DMACnt;
extern bool DMATransmitting;
extern SEND_CONTROL sndCtrl;
extern SEND_PARAM sendCmd[TOTAL_FPGA_CMD_NUM];

void FPGA_WriteByteArray(uint8_t *pTxData, uint16_t size);
void FPGA_ReadByteArray(uint8_t *pRxData, uint16_t size);
void makeSingleSendAry(uint8_t index, uint8_t * pData, uint8_t size, bool isFirstSegment, bool isCmd);
void makeParamSendAry(uint8_t * pData);





/*************************************************************/

#define DATA_PROCESS_FPGA_EVENT   (1<<0)

struct send_to_fpga_msg
{
    uint8_t buf[MAX_CMD_DATA_SIZE];
    uint8_t len;
};

int8_t recv_from_fpga_init(osMessageQueueId_t queue);
int8_t send_to_fpga_init(void);
int8_t send_to_fpga_write(uint8_t *buf, uint16_t size, uint32_t timeout);

#endif //W5500H7_FPGA_RW_H
