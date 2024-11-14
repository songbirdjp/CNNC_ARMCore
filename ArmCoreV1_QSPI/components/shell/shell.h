#ifndef __SHELL_H__
#define __SHELL_H__

#include "stdint.h"

#ifdef __plusplus
extern "C" {
#endif

struct shell_cmd_desc
{
    const char *name;
    const char *desc;
    const int8_t (*cmd_fn)(uint8_t argc, uint8_t **argv);
};


#define MSH_CMD_EXPORT_ALIAS(name, cmd, desc)               \
                const char __shell_##cmd##_name[] = #cmd;   \
                const char __shell_##cmd##_desc[] = #desc;  \
                const struct shell_cmd_desc __shell_##cmd __attribute__((section(".shell_cmd_fn"))) = \
                {__shell_##cmd##_name, __shell_##cmd##_desc, name};


int8_t shell_cmd_parse_entry(uint8_t *cmd_buf, uint16_t len);

#ifdef __plusplus
}
#endif

#endif
