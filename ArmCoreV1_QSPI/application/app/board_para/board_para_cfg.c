#include "board_para_cfg.h"
#include "fram_port.h"
#include "cJSON.h"
#include "json_struct.h"
#include "FreeRTOS.h"
#include "shell.h"
#include "init_call.h"
#include "utilities.h"

static int8_t board_cfg_read(uint8_t *buf, uint16_t len)
{
    int8_t ret = 0;
    uint16_t size = 0, offset = 0;

    while (len > 0)
    {
        size = len > BYTE_LEN_PER_LINE ? BYTE_LEN_PER_LINE : len;

        ret = device_fram_read(offset, buf, size, 1000);
        if (ret != 0)
        {
            printf("fram read err: %d\r\n", ret);
            return ret;
        }

        len -= size;
        offset += size;
        buf += size;
    }

    return 0;
}

static int8_t board_cfg_write(uint8_t *buf, uint16_t len)
{
    int8_t ret = 0;
    uint16_t size = 0, offset = 0, length = len;


    while (length > 0)
    {
        size = length > BYTE_LEN_PER_LINE ? BYTE_LEN_PER_LINE : length;

        ret = device_fram_write(offset, buf, size, 1000);
        if (ret != 0)
        {
            printf("fram write err: %d\r\n", ret);
            return ret;
        }

        length -= size;
        offset += size;
        buf += size;
    }

#if 1
    offset = 0;
    length = len;
    uint8_t buffer[BYTE_LEN_PER_LINE] = {0};
    while (length > 0)
    {
        size = length > BYTE_LEN_PER_LINE ? BYTE_LEN_PER_LINE : length;

        ret = device_fram_read(offset, buffer, size, 1000);
        if (ret != 0)
        {
            printf("fram read err: %d\n", ret);
            return ret;
        }

        offset += size;
        length -= size;

        for (uint16_t i = 0; i < size; i++)
        {
            printf("%c", buffer[i]);
        }
    }
    printf("\r\n");
#endif

    return 0;
}

static int8_t board_cfg_update(uint8_t argc, char **argv)
{
    uint16_t len = strlen(argv[1]);
    printf("len: %u\r\n", len);

    board_cfg_write(argv[1], strlen(argv[1]));

    return 0;
}
MSH_CMD_EXPORT_ALIAS(board_cfg_update, board_cfg_update, update board config);



static int8_t json_hooks_init(void)
{
    cJSON_Hooks hooks = {
       .malloc_fn = pvPortMalloc,
       .free_fn = vPortFree
    };

    cJSON_InitHooks(&hooks);

    return 0;
}
INIT_APP_EXPORT(json_hooks_init);


static struct fram_cfg board_cfg = {0};
static struct fram_cfg *board_cfg_get(void)
{
    return &board_cfg;
}

static int8_t json_element_printf(cJSON *root)
{
    if (root == NULL)
    {
        printf("root is NULL\r\n");
        return -1;
    }

    printf("------------------------begin---------------------\r\n");
    printf("address: %p\r\n", root);

    printf("type: %#.8x\r\n", root->type);
    printf("string: %s\r\n", root->string);
    printf("valuestring: %s\r\n", root->valuestring);
    printf("valueint: %d\r\n", root->valueint);
    printf("valuedouble: %f\r\n", root->valuedouble);

    printf("next: %p\r\n", root->next);
    printf("prev: %p\r\n", root->prev);
    printf("child: %p\r\n", root->child);

    printf("#######################end######################\r\n");

    return 0;
}

static int16_t json_to_struct(cJSON *root, void *cfg)
{
    if (root == NULL || cfg == NULL)
    {
        printf("root or cfg is NULL\r\n");
        return -1;
    }

    if (!cJSON_IsObject(root) && !cJSON_IsArray(root))
    {
        printf("root is not an object\r\n");
        return -2;
    }

    uint8_t *copy_ptr = NULL;
    uint8_t *struct_ptr = (uint8_t *)cfg;
    cJSON *json_ptr = root->child;
    // json_element_printf(json_ptr);

    uint8_t type_is_str = 0, num_is_double = 0;
    uint32_t len = 0;
    int16_t offset = 0;

    while (json_ptr)
    {
        offset = 0;
        len = 0;
        copy_ptr = NULL;
        type_is_str = 0;
        num_is_double = 0;

        switch (json_ptr->type)
        {
        case cJSON_Invalid:
            printf("invalid\r\n");
            break;

        case cJSON_False:
        case cJSON_True:
        case cJSON_NULL:
            len = sizeof(json_ptr->valueint);
            copy_ptr = (uint8_t *)&json_ptr->valueint;
            // printf("len: %u, value: %d\r\n", len, json_ptr->valueint);
            break;

        case cJSON_Number:
            if (json_ptr->valuedouble == (double)json_ptr->valueint)
            {
                len = sizeof(json_ptr->valueint);
                copy_ptr = (uint8_t *)&json_ptr->valueint;
                // printf("len: %u, value: %d\r\n", len, json_ptr->valueint);
            }
            else
            {
                len = sizeof(json_ptr->valuedouble);
                copy_ptr = (uint8_t *)&json_ptr->valuedouble;
                // printf("len: %u, value: %f\r\n", len, json_ptr->valuedouble);
                num_is_double = 1;
            }
            break;

        case cJSON_String:
            len = strlen(json_ptr->valuestring);
            // printf("len: %u, string: %s\r\n", len, json_ptr->valuestring);

            copy_ptr = (uint8_t *)pvPortMalloc(len + 1);
            if (copy_ptr == NULL)
            {
                printf("malloc err\r\n");
                return -3;
            }
            memset(copy_ptr, 0, len + 1);
            memcpy(copy_ptr, json_ptr->valuestring, len);
            type_is_str = 1;
            break;

        case cJSON_Array:
            // printf("array: %s\r\n", json_ptr->string);
            offset = json_to_struct(json_ptr, struct_ptr);
            break;

        case cJSON_Object:
            // printf("object: %s\r\n", json_ptr->string);
            offset = json_to_struct(json_ptr, struct_ptr);
            break;

        case cJSON_Raw:
            break;

        default:
            break;
        }

        if (offset < 0)
        {
            printf("offset err: %d\r\n", offset);
            return -4;
        }

        struct_ptr += offset;

        switch (json_ptr->type)
        {
        case cJSON_String:
            len = 4;
            break;

        case cJSON_Array:
        case cJSON_Object:
            len = 0;
            break;

        default:
            break;
        }

        if (len != 0)
        {
            // printf("struct_ptr: %p\r\n", struct_ptr);
            struct_ptr = num_is_double == 0 ? struct_ptr : ALIGN((uint32_t)struct_ptr, 8);
            type_is_str == 0 ? memcpy(struct_ptr, copy_ptr, len) : memcpy(struct_ptr, &copy_ptr, len);
            struct_ptr += ALIGN(len, 4);
        }

        json_ptr = json_ptr->next;
    }

    return (struct_ptr - (uint8_t *)cfg);
}


static int8_t board_cfg_parse(void)
{
    uint8_t buffer[1024] = {0};
    int16_t ret = 0;

    ret = board_cfg_read(buffer, sizeof(buffer));
    if (ret != 0)
    {
        printf("board_cfg_read err: %d\r\n", ret);
        return ret;
    }

    cJSON *root = cJSON_Parse((const char *)buffer);
    if (root == NULL)
    {
        printf("cJSON_Parse err: %s\r\n", cJSON_GetErrorPtr());
        return -1;
    }

    ret = json_to_struct(root, board_cfg_get());
    if (ret < 0)
    {
        printf("json_to_struct err: %d\r\n", ret);
    }

#if 0
    char *str = cJSON_PrintUnformatted(root);
    printf("board_cfg: %s\r\n", str);

#if 0
    printf("name addr: %p\r\n", &board_cfg_get()->board.name);
    printf("id addr: %p\r\n", &board_cfg_get()->board.id);
    printf("version addr: %p\r\n", &board_cfg_get()->board.version);
    printf("console addr: %p\r\n", &board_cfg_get()->log.console);
    printf("base_addr addr: %p\r\n", &board_cfg_get()->log.flash.base_addr);
    printf("size addr: %p\r\n", &board_cfg_get()->log.flash.size);
    printf("sector_size addr: %p\r\n", &board_cfg_get()->log.flash.sector_size);
    printf("offset_current addr: %p\r\n", &board_cfg_get()->log.flash.offset_current);
    printf("test addr: %p\r\n", &board_cfg_get()->test);
    printf("haha addr: %p\r\n", &board_cfg_get()->haha);
    printf("heihei addr: %p\r\n", &board_cfg_get()->heihei);
    printf("a0 addr: %p\r\n", &board_cfg_get()->a0);
    printf("a1 addr: %p\r\n", &board_cfg_get()->a1);
    printf("a2 addr: %p\r\n", &board_cfg_get()->a2);
    printf("a3 addr: %p\r\n", &board_cfg_get()->a3);
    printf("a4 addr: %p\r\n", &board_cfg_get()->a4);
#endif

    printf("name: %s\r\n", board_cfg_get()->board.name);
    printf("id: %s\r\n", board_cfg_get()->board.id);
    printf("version: %s\r\n", board_cfg_get()->board.version);

    printf("console: %d\r\n", board_cfg_get()->log.console);

    printf("base_addr: %s\r\n", board_cfg_get()->log.flash.base_addr);
    printf("size: %d\r\n", board_cfg_get()->log.flash.size);
    printf("sector_size: %d\r\n", board_cfg_get()->log.flash.sector_size);
    printf("offset_current: %s\r\n", board_cfg_get()->log.flash.offset_current);

    printf("test: %f\r\n", board_cfg_get()->test);
    printf("haha: %f\r\n", board_cfg_get()->haha);
    printf("heihei: %f\r\n", board_cfg_get()->heihei);

    printf("a0: ");
    for (uint8_t i = 0; i < 5; i++)
    {
        printf("%d ", board_cfg_get()->a0[i]);
    }
    printf("\r\n");

    printf("a1: ");
    for (uint8_t i = 0; i < 6; i++)
    {
        printf("%s  ", board_cfg_get()->a1[i]);
    }
    printf("\r\n");

    printf("a2: ");
    for (uint8_t i = 0; i < 4; i++)
    {
        printf("%s ", board_cfg_get()->a2[i]);
    }
    printf("\r\n");

    printf("a3: ");
    for (uint8_t i = 0; i < 3; i++)
    {
        printf("%f ", board_cfg_get()->a3[i]);
    }
    printf("\r\n");

    printf("a4: ");
    for (uint8_t i = 0; i < 5; i++)
    {
        printf("%d ", board_cfg_get()->a4[i]);
    }
    printf("\r\n");

    cJSON_free(str);
#endif

    cJSON_Delete(root);

    return 0;
}
MSH_CMD_EXPORT_ALIAS(board_cfg_parse, board_cfg_parse, parse board config);


#define TYPE_OF(x) _Generic((x), \
    uint8_t *: "string", \
    uint8_t **: "string array", \
    int32_t: "int32_t", \
    int32_t *: "int32_t array", \
    double: "double", \
    double *: "double array", \
    default: "other")

static int8_t board_cfg_type_check(void)
{
    struct fram_cfg *cfg = board_cfg_get();

    printf("Type of name: %s\n", TYPE_OF(cfg->board.name));
    printf("Type of id: %s\n", TYPE_OF(cfg->board.id));
    printf("Type of version: %s\n", TYPE_OF(cfg->board.version));

    printf("Type of console: %s\n", TYPE_OF(cfg->log.console));

    printf("Type of base_addr: %s\n", TYPE_OF(cfg->log.flash.base_addr));
    printf("Type of size: %s\n", TYPE_OF(cfg->log.flash.size));
    printf("Type of sector_size: %s\n", TYPE_OF(cfg->log.flash.sector_size));
    printf("Type of offset_current: %s\n", TYPE_OF(cfg->log.flash.offset_current));

    printf("Type of test: %s\n", TYPE_OF(cfg->test));
    printf("Type of haha: %s\n", TYPE_OF(cfg->haha));
    printf("Type of heihei: %s\n", TYPE_OF(cfg->heihei));

    printf("Type of a0: %s\n", TYPE_OF(cfg->a0));
    printf("Type of a1: %s\n", TYPE_OF(cfg->a1));
    printf("Type of a2: %s\n", TYPE_OF(cfg->a2));
    printf("Type of a3: %s\n", TYPE_OF(cfg->a3));
    printf("Type of a4: %s\n", TYPE_OF(cfg->a4));

    return 0;
}
MSH_CMD_EXPORT_ALIAS(board_cfg_type_check, board_cfg_type_check, check board config type);


static int8_t board_cfg_update_local(uint16_t offset, uint8_t *buf, uint16_t len)
{
    if (buf == NULL)
    {
        printf("buf is NULL\r\n");
        return -1;
    }

    int8_t ret = 0;
    uint16_t size = 0;

    while (len > 0)
    {
        size = len > BYTE_LEN_PER_LINE? BYTE_LEN_PER_LINE : len;

        ret = device_fram_write(0 + offset, buf, size, 1000);
        if (ret != 0)
        {
            printf("fram write err: %d\r\n", ret);
            return ret;
        }

        len -= size;
        offset += size;
        buf += size;
    }

    return 0;
}

static int8_t json_string_replace(char *old_str, char *str_begin, char *new_str)
{
    if (old_str == NULL || str_begin == NULL || new_str == NULL)
    {
        printf("pointer is NULL\r\n");
        return -1;
    }

    char *p = strstr(old_str, str_begin);
    if (p != NULL)
    {
        uint32_t old_len = strlen(old_str);
        uint32_t new_len = strlen(new_str);
        uint32_t offset = p - old_str;

        if (offset + new_len > old_len)
        {
            printf("len err\r\n");
            return -2;
        }

        memcpy(p, new_str, new_len);
    }
    else
    {
        printf("str_begin not found\r\n");
        return -3;
    }

    return 0;
}


static int8_t board_cfg_update_test(void)
{
    uint8_t buffer[1024] = {0};
    int16_t ret = 0;

    ret = board_cfg_read(buffer, sizeof(buffer));
    if (ret != 0)
    {
        printf("board_cfg_read err: %d\r\n", ret);
        return ret;
    }

    cJSON *root = cJSON_Parse((const char *)buffer);
    if (root == NULL)
    {
        printf("cJSON_Parse err: %s\r\n", cJSON_GetErrorPtr());
        return -1;
    }

    char *board_str = cJSON_PrintUnformatted(root);
    printf("board_str: %s\r\n", board_str);

    // cJSON *test_item = cJSON_GetObjectItem(root, "test");
    // if (cJSON_IsNumber(test_item))
    // {
    //     cJSON_SetNumberValue(test_item, 3.5);
    // }

    // cJSON *log_item = cJSON_GetObjectItem(root, "log");
    // if (cJSON_IsObject(log_item))
    // {
    //     cJSON *console_item = cJSON_GetObjectItem(log_item, "console");
    //     if (cJSON_IsBool(console_item))
    //     {
    //         cJSON_SetBoolValue(console_item, cJSON_True);
    //     }
    // }

    // cJSON *board_item = cJSON_GetObjectItem(root, "board");
    // if (cJSON_IsObject(board_item))
    // {
    //     cJSON *name_item = cJSON_GetObjectItem(board_item, "name");
    //     if (cJSON_IsString(name_item))
    //     {
    //         cJSON_SetValuestring(name_item, "new_name");
    //     }
    // }

    json_string_replace(board_str, "\"test\":", "\"test\":3.3");
    printf("new board_str: %s\r\n", board_str);

    char *p = strstr(board_str, "\"test\":");
    if (p != NULL)
    {
        board_cfg_update_local(p - board_str + strlen("\"test\":"), "1.9", strlen("3.3"));
    }

    return 0;
}
MSH_CMD_EXPORT_ALIAS(board_cfg_update_test, board_cfg_update_test, update board config test);