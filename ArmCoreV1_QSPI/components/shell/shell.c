#include "shell.h"
#include "stddef.h"
#include "utilities.h"
#include "ulog.h"

typedef int (*fun_ptr)(int8_t argc, uint8_t **argv);

static fun_ptr shell_cmd_fun_find(uint8_t *cmd, uint16_t length)
{
    const struct shell_cmd_desc *desc;

    extern uint32_t  __shell_cmd_start;
    extern uint32_t  __shell_cmd_end;

    for (desc = &__shell_cmd_start; desc < &__shell_cmd_end; desc++)
    {
        if (strncmp(desc->name, cmd, length) == 0 && strlen(desc->name) == length)
        {
            return desc->cmd_fn;
        }
    }

    return NULL;
}

static int8_t shell_cmd_exec(uint8_t *cmd, uint16_t length)
{
    uint8_t cmd_len = 0;
    int (*fun)(int8_t argc, uint8_t **argv);

    while ((cmd[cmd_len] != ' ' && cmd[cmd_len] != '\t') && cmd_len < length)
    {
        cmd_len++;
    }

    if (cmd_len == 0)
    {
        return -1;
    }
    
    fun = shell_cmd_fun_find(cmd, cmd_len);
    if (fun == NULL)
    {
        cmd[cmd_len] = '\0';
        printf("[%s] command not found\r\n", cmd);
        return -2;
    }

    uint8_t *argv[8] = {NULL};

    int8_t argc = split_string(cmd, ' ', argv);
    if (argc == 0)
    {
        return -3;
    }

    return fun(argc, argv);
}

static int8_t shell_exec(uint8_t *cmd, uint16_t length)
{
    while ((length > 0) && (*cmd  == ' ' || *cmd == '\t'))
    {
        cmd++;
        length--;
    }

    if (length == 0)
    {
        return -1;
    }

    return shell_cmd_exec(cmd, length);
}

int8_t shell_cmd_parse_entry(uint8_t *cmd_buf, uint16_t len)
{
    LOG_I("shell >\r\n");
    shell_exec(cmd_buf, len);
}