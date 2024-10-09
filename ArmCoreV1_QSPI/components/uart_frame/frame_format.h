#ifndef __FRAME_FORMAT_H__
#define __FRAME_FORMAT_H__

#include <stdint.h>
#include "frame_statistics.h"

#ifdef __cplusplus
extern "C" {
#endif

int8_t frame_format_parse(struct frame_statistics *stats, uint8_t *buf, uint16_t size, uint16_t *offset, uint16_t *length);
int8_t frame_format_pack_and_send(struct frame_statistics *stats, uint8_t *buf, uint16_t len, int8_t (*cb)(uint8_t *buf, uint16_t size, uint32_t timeout), uint32_t timeout);

#ifdef __cplusplus
}
#endif

#endif /* __FRAME_FORMAT_H__ */