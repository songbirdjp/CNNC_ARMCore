#ifndef __RTM_APP_H__
#define __RTM_APP_H__


#ifdef __cplusplus
extern "C" {
#endif

#define RS422_BUS_MODULE_ID_BROADCAST   (0 << 0)
#define RS422_BUS_MODULE_ID_RTM_ON_PLC  (1 << 0)
#define RS422_BUS_MODULE_ID_RTM_ON_ARM  (1 << 1)
#define RS422_BUS_MODULE_ID_ICM         (1 << 2)
#define RS422_BUS_MODULE_ID_BGM         (1 << 3)
#define RS422_BUS_MODULE_ID_QAM         (1 << 4)
#define RS422_BUS_MODULE_ID_BSM         (1 << 5)
#define RS422_BUS_MODULE_ID_RTM_OFF_ARM (1 << 6)
#define RS422_BUS_MODULE_ID_GMM         (1 << 7)
#define RS422_BUS_MODULE_ID_PSM         (1 << 8)
#define RS422_BUS_MODULE_ID_FKP         (1 << 9)
#define RS422_BUS_MODULE_ID_CPG         (1 << 10)

#define RS422_BUS_MODULE_ID_LOCAL       (0)


enum uart_subcmd_type
{
    UART_CMD_SEND_RADIATION_INDEX = 0x01,
    UART_DATA_CMD_SEND_FSM_STATE = 0x31,
    UART_DATA_CMD_SEND_BGM_INFO,
    UART_DATA_CMD_SEND_DOSE_INFO,
    UART_DATA_CMD_SEND_QAM,

    UART_DATA_CMD_RECV_BEAM_ID = 0,
    UART_DATA_CMD_RECV_RADIATION_INDEX,
    UART_DATA_CMD_RECV_FSM_STATE = 0x13,
    UART_DATA_CMD_RECV_GMM = 0x82,
};


#ifdef __cplusplus
}
#endif
#endif /* __RTM_APP_H__ */