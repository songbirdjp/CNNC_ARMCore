#ifndef DOSE_CMD_H
#define DOSE_CMD_H
// Commands Type
// typedef enum
// {
//     AFCCmdType_HandshakeDown = 0x01,
//     AFCCmdType_CommandDown,
//     AFCCmdType_RtDataDown
// } AFCCmdType_t;

typedef enum
{
    UARTCmdType_HandshakeDown = 0x01,
    UARTCmdType_CommandDown,
    UARTCmdType_RtDataDown
} UARTCmdType_t;

#define CMD_MagMotorCtrl                0x40
#define SubCmd_MagMotorEn               0x00
#define SubCmd_MagMotorFindZero         0x01
#define SubCmd_MagMotorPosCtrl          0x02
#define SubCmd_MagMotorFreeMove         0x03
#define SubCmd_MagMotorMoveByStep       0x04

#define CMD_AFTMotorCtrl                0x42
#define SubCmd_AFTBrakeCtrl             0x00
#define SubCmd_AFTMotorEn               0x01
#define SubCmd_AFTMotorFindZero         0x02
#define SubCmd_AFTMotorPosCtrl          0x03
#define SubCmd_AFTMotorFreeMove         0x04
#define SubCmd_AFTMotorMoveByStep       0x05

#endif