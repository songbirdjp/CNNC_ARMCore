/**
 * @file dev_hash.h
 * @author SI (siyunlong@cnncpm.com)
 * @brief 
 * @version 0.1
 * @date 2024-08-01
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#ifndef _DEV_HASH_H_
#define _DEV_HASH_H_

#include "stdint.h"

#ifdef __cplusplus
extern "C" {
#endif

#define HASH_TABLE_SIZE 23

typedef enum hash_map_state
{
    HASH_TABLE_STATE_INACTIVE = 0,
    HASH_TABLE_STATE_ACTIVE
}hash_map_state_t;

typedef struct hash_node
{
    char const* key;
    void *value;
    struct hash_node *next;
} hash_node_t;

typedef struct hash_map
{
    hash_map_state_t hash_map_state; 
    uint32_t table_size;
    hash_node_t *table[HASH_TABLE_SIZE];
} hash_map_t;

device_err_t hash_map_create(hash_map_t *hash_map);

device_err_t hash_map_insert(hash_map_t *hash_map, char const* key, void *value);
device_err_t hash_map_search(hash_map_t *hash_map, char const* key, void **value);
device_err_t hash_map_show(hash_map_t *hash_map);
hash_map_state_t hash_map_get_state(hash_map_t *hash_map);

void hash_test(void);
#ifdef __cplusplus
}
#endif

#endif
