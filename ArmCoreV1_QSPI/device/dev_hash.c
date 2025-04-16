/**
 * @file dev_hash.c
 * @author SI (siyunlong@cnncpm.com)
 * @brief
 * @version 0.1
 * @date 2024-08-01
 *
 * @copyright Copyright (c) 2024
 *
 */
#include "dev_base.h"
#include "dev_hash.h"
#include "stdlib.h"

TAG("dev_hash.c");

static uint32_t hash(hash_map_t *hash_map, char const* key)
{
    char *ch = (char *)key;
    uint32_t h = 0;

    while (*ch != '\0')
    {
        h = (h << 4) + *ch++;
        unsigned long g = h & 0xF0000000L;
        if (g)
        {
            h ^= g >> 24;
        }
        h &= ~g;
    }
    return h % hash_map->table_size;
}

device_err_t hash_map_create(hash_map_t *hash_map)
{
    dev_assert(hash_map != NULL);

    if (HASH_TABLE_STATE_ACTIVE == hash_map->hash_map_state)
    {
        return DEV_EREPTD;
    }

    hash_map->table_size = HASH_TABLE_SIZE;

    for (uint8_t i = 0; i < hash_map->table_size; i++)
    {
        hash_map->table[i] = NULL;
    }
    hash_map->hash_map_state = HASH_TABLE_STATE_ACTIVE;
    return DEV_EOK;
}

device_err_t hash_map_insert(hash_map_t *hash_map, char const* key, void *value)
{
    dev_assert(hash_map != NULL);
    dev_assert(key != NULL);
    dev_assert(value != NULL);

    uint32_t index = 0;

    if (HASH_TABLE_STATE_INACTIVE == hash_map->hash_map_state)
    {
        return DEV_ENOTFOUND;
    }
    index = hash(hash_map, key);

    hash_node_t *newNode = (hash_node_t *)malloc(sizeof(hash_node_t));
    if (newNode == NULL)
    {
        return DEV_ENOMEM;
    }

    newNode->key = key;
    newNode->value = value;
    newNode->next = NULL;

    if (hash_map->table[index] != NULL)
    {
        hash_node_t *hash_node = hash_map->table[index];
        hash_map->table[index] = newNode;
        hash_map->table[index]->next = hash_node;
    }
    else
    {
        hash_map->table[index] = newNode;
    }
    return DEV_EOK;
}
device_err_t hash_map_search(hash_map_t *hash_map, char const* key, void **value)
{
    dev_assert(hash_map != NULL);
    dev_assert(key != NULL);

    uint32_t index = 0;

    if (HASH_TABLE_STATE_INACTIVE == hash_map->hash_map_state)
    {
        return DEV_ENOTFOUND;
    }

    index = hash(hash_map, key);

    hash_node_t *hash_node = hash_map->table[index];
    while (hash_node != NULL)
    {
        if(0 == strcmp(hash_node->key, key))
        {
            *value = hash_node->value;
            return DEV_EOK;
        }
        hash_node = hash_node->next;
    }

    return DEV_ENOTFOUND;
}
device_err_t hash_map_show(hash_map_t *hash_map)
{
    dev_assert(hash_map != NULL);

    if (HASH_TABLE_STATE_INACTIVE == hash_map->hash_map_state)
    {
        return DEV_ENOTFOUND;
    }

    printf("hash map:\r\n");
    for (uint8_t i = 0; i < hash_map->table_size; i++)
    {
        hash_node_t *hash_node = hash_map->table[i];
        printf("\t[%d]:", i);
        while (hash_node != NULL)
        {
            printf("{%s: 0x%x}, ", (char *)(hash_node->key), (uint32_t)(hash_node->value));
            hash_node = hash_node->next;
        }
        printf("\r\n");
    }
    return DEV_EOK;
}
hash_map_state_t hash_map_get_state(hash_map_t *hash_map)
{
    return hash_map->hash_map_state;
}
void hash_test(void)
{
    // hash_map_t hashMap;

    // uint8_t a1 = 1, a2 = 2, a3 = 3;

    // uint32_t *b = NULL;
    // // uint32_t timeBase = 0;

    // hash_map_create(&hashMap);
    // // timeBase = HAL_GetTick();
    // hash_map_insert(&hashMap, "i2c1", &a1);
    // hash_map_insert(&hashMap, "i2c2", &a2);
    // hash_map_insert(&hashMap, "spi1", &a3);
    // hash_map_insert(&hashMap, "spi2", &a3);
    // hash_map_insert(&hashMap, "can1", &a1);
    // hash_map_insert(&hashMap, "modbus", &a2);

    // hash_map_search(&hashMap, "modbus", (void *)&b);
    // // printf("use time:%d\r\n", HAL_GetTick() - timeBase);
    // printf("b:0x%x\r\n", (int)b);
    // hash_map_show(&hashMap);
}
