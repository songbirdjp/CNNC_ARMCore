#ifndef __JAW_TEST_H__
#define __JAW_TEST_H__


#include "jaw_control.h"

#ifdef __cplusplus
extern "C" {
#endif

// 暴露给外部调用的函数
void Update_RI_Target(struct JawFlagType *recvMsg, uint8_t axis_idx);




#ifdef __cplusplus
}
#endif

#endif /* __JAW_TEST_H__ */