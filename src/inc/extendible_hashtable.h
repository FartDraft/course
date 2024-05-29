#ifndef EXTENDIBLE_HASHTABLE_
#define EXTENDIBLE_HASHTABLE_

#include <stdlib.h>
#include <string.h>

enum eh_status_t { EH_FAILURE, EH_UPDATE, EH_INSERT, EH_OVERFLOW, EH_EXPANSION };

#define GENERATE_EXTENDIBLE_HASHTABLE(key_t, value_t)                                                                  \
    typedef struct {                                                                                                   \
        key_t _key;                                                                                                    \
        value_t value;                                                                                                 \
    } eh_item_##key_t##_##value_t##_t_;                                                                                \
                                                                                                                       \
    typedef struct eh_bucket_##key_t##_##value_t##_ {                                                                  \
        size_t _local_depth;                                                                                           \
        eh_item_##key_t##_##value_t##_t_* _items;                                                                      \
        struct eh_bucket_##key_t##_##value_t##_* _prev;                                                                \
        struct eh_bucket_##key_t##_##value_t##_* _next;                                                                \
    } eh_bucket_##key_t##_##value_t##_t_;                                                                              \
                                                                                                                       \
    typedef size_t (*eh_hash_##key_t##_##value_t##_fn_)(const key_t*);                                                 \
    typedef int (*eh_cmp_##key_t##_##value_t##_fn_)(const key_t*, const key_t*);                                       \
                                                                                                                       \
    typedef struct {                                                                                                   \
        eh_hash_##key_t##_##value_t##_fn_ hash;                                                                        \
        eh_cmp_##key_t##_##value_t##_fn_ cmp;                                                                          \
        size_t _bucket_capacity;                                                                                       \
                                                                                                                       \
        struct {                                                                                                       \
            size_t _global_depth;                                                                                      \
            size_t _num;                                                                                               \
            eh_bucket_##key_t##_##value_t##_t_** _links;                                                               \
        } _dirs;                                                                                                       \
                                                                                                                       \
        eh_item_##key_t##_##value_t##_t_ _null_item;                                                                   \
        eh_item_##key_t##_##value_t##_t_ _tombstone_item;                                                              \
    } extendible_hashtable_##key_t##_##value_t##_t;                                                                    \
                                                                                                                       \
    [[nodiscard]] extendible_hashtable_##key_t##_##value_t##_t* eh_create_##key_t##_##value_t(                         \
        size_t bucket_capacity, eh_hash_##key_t##_##value_t##_fn_ hash, eh_cmp_##key_t##_##value_t##_fn_ cmp) {        \
        if (bucket_capacity == 0 || hash == NULL || cmp == NULL) {                                                     \
            goto defer;                                                                                                \
        }                                                                                                              \
                                                                                                                       \
        extendible_hashtable_##key_t##_##value_t##_t* table = malloc(sizeof(*table));                                  \
        if (table == NULL) {                                                                                           \
            goto defer;                                                                                                \
        }                                                                                                              \
                                                                                                                       \
        *table = (extendible_hashtable_##key_t##_##value_t##_t){                                                       \
            .hash = hash,                                                                                              \
            .cmp = cmp,                                                                                                \
            ._bucket_capacity = bucket_capacity,                                                                       \
            ._dirs =                                                                                                   \
                {                                                                                                      \
                    ._global_depth = 1,                                                                                \
                    ._num = 2,                                                                                         \
                    ._links = malloc(2 * sizeof(table->_dirs._links)),                                                 \
                },                                                                                                     \
        };                                                                                                             \
        if (table->_dirs._links == NULL) {                                                                             \
            goto defer;                                                                                                \
        }                                                                                                              \
        memset(&table->_tombstone_item, 1, sizeof(table->_null_item));                                                 \
                                                                                                                       \
        if ((table->_dirs._links[0] = malloc(sizeof(**table->_dirs._links))) == NULL                                   \
            || (table->_dirs._links[1] = malloc(sizeof(**table->_dirs._links))) == NULL) {                             \
            goto defer;                                                                                                \
        }                                                                                                              \
                                                                                                                       \
        *table->_dirs._links[0] = (eh_bucket_##key_t##_##value_t##_t_){                                                \
            ._local_depth = 1,                                                                                         \
            ._items = calloc(table->_bucket_capacity, sizeof(table->_null_item)),                                      \
            ._prev = NULL,                                                                                             \
            ._next = table->_dirs._links[1],                                                                           \
        };                                                                                                             \
        if (table->_dirs._links[0]->_items == NULL) {                                                                  \
            goto defer;                                                                                                \
        }                                                                                                              \
                                                                                                                       \
        *table->_dirs._links[1] = (eh_bucket_##key_t##_##value_t##_t_){                                                \
            ._local_depth = 1,                                                                                         \
            ._items = calloc(table->_bucket_capacity, sizeof(table->_null_item)),                                      \
            ._prev = table->_dirs._links[0],                                                                           \
            ._next = NULL,                                                                                             \
        };                                                                                                             \
                                                                                                                       \
        if (table->_dirs._links[1]->_items == NULL) {                                                                  \
        defer:                                                                                                         \
            free(table->_dirs._links[1]);                                                                              \
            free(table->_dirs._links[0]->_items);                                                                      \
            free(table->_dirs._links[0]);                                                                              \
            free(table->_dirs._links);                                                                                 \
            free(table);                                                                                               \
        }                                                                                                              \
                                                                                                                       \
        return table;                                                                                                  \
    }                                                                                                                  \
                                                                                                                       \
    void eh_destroy_##key_t##_##value_t(extendible_hashtable_##key_t##_##value_t##_t* table) {                         \
        eh_bucket_##key_t##_##value_t##_t_ *tmp, *head = *table->_dirs._links;                                         \
        while (head != NULL) {                                                                                         \
            tmp = head;                                                                                                \
            head = head->_next;                                                                                        \
            free(tmp->_items);                                                                                         \
            free(tmp);                                                                                                 \
        }                                                                                                              \
        free(table->_dirs._links);                                                                                     \
        free(table);                                                                                                   \
    }                                                                                                                  \
                                                                                                                       \
    enum eh_status_t eh_bucket_insert_##key_t##_##value_t##_(const key_t* key, const value_t* value,                   \
                                                             eh_bucket_##key_t##_##value_t##_t_* bucket,               \
                                                             extendible_hashtable_##key_t##_##value_t##_t* table) {    \
        size_t i = 0;                                                                                                  \
        while (memcmp(bucket->_items + i, &table->_null_item, sizeof(table->_null_item)) != 0                          \
               && memcmp(bucket->_items + i, &table->_tombstone_item, sizeof(table->_null_item)) != 0) {               \
            if (table->cmp(&bucket->_items[i]._key, key) == 0) {                                                       \
                memcpy(&bucket->_items[i].value, value, sizeof(value_t));                                              \
                return EH_UPDATE;                                                                                      \
            }                                                                                                          \
            ++i;                                                                                                       \
        }                                                                                                              \
                                                                                                                       \
        memcpy(&bucket->_items[i]._key, key, sizeof(key_t));                                                           \
        memcpy(&bucket->_items[i].value, value, sizeof(value_t));                                                      \
        return i + 1 == table->_bucket_capacity ? EH_OVERFLOW : EH_INSERT;                                             \
    }                                                                                                                  \
                                                                                                                       \
    enum eh_status_t eh_insert_##key_t##_##value_t(const key_t* key, const value_t* value,                             \
                                                   extendible_hashtable_##key_t##_##value_t##_t* table) {              \
        if (key == NULL || value == NULL || table == NULL) {                                                           \
            return EH_FAILURE;                                                                                         \
        }                                                                                                              \
                                                                                                                       \
        eh_bucket_##key_t##_##value_t##_t_* bucket =                                                                   \
            table->_dirs._links[table->hash(key) & ((1 << table->_dirs._global_depth) - 1)];                           \
        enum eh_status_t status = eh_bucket_insert_##key_t##_##value_t##_(key, value, bucket, table);                  \
        if (status != EH_OVERFLOW) {                                                                                   \
            return status;                                                                                             \
        }                                                                                                              \
                                                                                                                       \
        if (bucket->_local_depth == table->_dirs._global_depth) {                                                      \
            ++table->_dirs._global_depth;                                                                              \
            if ((table->_dirs._links = realloc(table->_dirs._links,                                                    \
                                               2 * table->_dirs._num * sizeof(table->_dirs._links)))                   \
                == NULL) {                                                                                             \
                return EH_FAILURE;                                                                                     \
            }                                                                                                          \
            for (size_t i = 0; i < table->_dirs._num; ++i) {                                                           \
                table->_dirs._links[table->_dirs._num + i] = table->_dirs._links[i];                                   \
            }                                                                                                          \
            table->_dirs._num *= 2;                                                                                    \
            status = EH_EXPANSION;                                                                                     \
        }                                                                                                              \
                                                                                                                       \
        eh_bucket_##key_t##_##value_t##_t_ *old_bucket, *new_bucket;                                                   \
        if ((old_bucket = malloc(sizeof(*bucket))) == NULL || (new_bucket = malloc(sizeof(*bucket))) == NULL) {        \
            goto defer;                                                                                                \
        }                                                                                                              \
                                                                                                                       \
        *old_bucket = (eh_bucket_##key_t##_##value_t##_t_){                                                            \
            ._local_depth = bucket->_local_depth + 1,                                                                  \
            ._items = calloc(table->_bucket_capacity, sizeof(table->_null_item)),                                      \
            ._prev = bucket->_prev,                                                                                    \
            ._next = new_bucket,                                                                                       \
        };                                                                                                             \
        if (old_bucket->_items == NULL) {                                                                              \
            goto defer;                                                                                                \
        }                                                                                                              \
                                                                                                                       \
        *new_bucket = (eh_bucket_##key_t##_##value_t##_t_){                                                            \
            ._local_depth = bucket->_local_depth + 1,                                                                  \
            ._items = calloc(table->_bucket_capacity, sizeof(table->_null_item)),                                      \
            ._prev = old_bucket,                                                                                       \
            ._next = bucket->_next,                                                                                    \
        };                                                                                                             \
        if (new_bucket->_items == NULL) {                                                                              \
        defer:                                                                                                         \
            free(new_bucket);                                                                                          \
            free(old_bucket->_items);                                                                                  \
            free(old_bucket);                                                                                          \
            return EH_FAILURE;                                                                                         \
        }                                                                                                              \
        bucket->_prev->_next = old_bucket;                                                                             \
                                                                                                                       \
        size_t high_bit = 1 << bucket->_local_depth;                                                                   \
        for (size_t i = 0; i < table->_bucket_capacity; ++i) {                                                         \
            eh_bucket_insert_##key_t##_##value_t##_(                                                                   \
                &bucket->_items[i]._key, &bucket->_items[i].value,                                                     \
                table->hash(&bucket->_items[i]._key) & high_bit ? new_bucket : old_bucket, table);                     \
        }                                                                                                              \
                                                                                                                       \
        free(bucket->_items);                                                                                          \
        free(bucket);                                                                                                  \
        for (size_t i = table->hash(key) & (high_bit - 1); i < table->_dirs._num; i += high_bit) {                     \
            table->_dirs._links[i] = i & high_bit ? new_bucket : old_bucket;                                           \
        }                                                                                                              \
                                                                                                                       \
        return status;                                                                                                 \
    }                                                                                                                  \
                                                                                                                       \
    [[nodiscard]] value_t* eh_lookup_##key_t##_##value_t(const key_t* key,                                             \
                                                         const extendible_hashtable_##key_t##_##value_t##_t* table) {  \
        if (key == NULL || table == NULL) {                                                                            \
            return NULL;                                                                                               \
        }                                                                                                              \
                                                                                                                       \
        eh_bucket_##key_t##_##value_t##_t_* bucket =                                                                   \
            table->_dirs._links[table->hash(key) & ((1 << table->_dirs._global_depth) - 1)];                           \
        size_t i = 0;                                                                                                  \
        while (memcmp(bucket->_items + i, &table->_null_item, sizeof(table->_null_item)) != 0                          \
               && memcmp(bucket->_items + i, &table->_tombstone_item, sizeof(table->_null_item)) != 0) {               \
            if (table->cmp(&bucket->_items[i]._key, key) == 0) {                                                       \
                return &bucket->_items[i].value;                                                                       \
            }                                                                                                          \
            ++i;                                                                                                       \
        }                                                                                                              \
        return NULL;                                                                                                   \
    }

#endif
