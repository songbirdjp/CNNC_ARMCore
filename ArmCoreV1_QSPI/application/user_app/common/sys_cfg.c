#include "sys_cfg.h"

static struct sys_info system_info __attribute__((section(".system_info"))) = 
{
    .fw_version = FW_VERSION,
    .compile_time = __DATE__"  "__TIME__
};

struct sys_info *system_info_get(void)
{
    return &system_info;
}
