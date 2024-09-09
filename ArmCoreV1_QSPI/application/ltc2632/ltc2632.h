#ifndef __LTC2632_H__
#define __LTC2632_H__

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

enum 
{
    LTC2632_CHANNEL_OUTA = 0,
    LTC2632_CHANNEL_OUTB = 1,
    LTC2632_CHANNEL_ALL = 0x0f,
    LTC2632_CHANNEL_MAX
};

struct ltc2632_object
{
    union
    {
        uint32_t bytes;
        struct
        {
            uint32_t reserved : 4;
            uint32_t data : 12;
            uint32_t channel : 4;
            uint32_t cmd : 4;
            uint32_t reserved_1 : 8;
        }bits;
    }value;

    uint16_t out_a_value;
    uint16_t out_b_value;
};

struct ltc2632_object *ltc2632_object_data_get(void);
int8_t ltc2632_data_write(struct ltc2632_object *buf);


#ifdef __cplusplus
}
#endif



#endif /* __LTC2632_H__ */