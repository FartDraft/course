#ifndef EXTENDIBLE_HASHTABLE_
#define EXTENDIBLE_HASHTABLE_

#include <stdlib.h>
#include <string.h>

enum eh_status_t {
    EH_VALUE_ERROR,
    EH_BAD_ALLOC,
    EH_SUCCESS,
    EH_INSERT,
    EH_UPDATE,
    EH_OVERFLOW,
    EH_EXPANSION,
    EH_NOT_FOUND
};

#define GENERATE_EXTENDIBLE_HASHTABLE(key_t, value_t)                                                                  \
    typedef struct {                                                                                                   \
        key_t key;                                                                                                     \
        value_t value;                                                                                                 \
    } eh_item_##key_t##_##value_t##_t_;                                                                                \
                                                                                                                       \
    typedef struct eh_bucket_##key_t##_##value_t##_ {                                                                  \
        size_t local_depth;                                                                                            \
        eh_item_##key_t##_##value_t##_t_* items;                                                                       \
        struct eh_bucket_##key_t##_##value_t##_* next;                                                                 \
    } eh_bucket_##key_t##_##value_t##_t_;                                                                              \
                                                                                                                       \
    typedef size_t (*eh_hash_##key_t##_##value_t##_fn)(const key_t*);                                                  \
    typedef int (*eh_cmp_##key_t##_##value_t##_fn)(const key_t*, const key_t*);                                        \
                                                                                                                       \
    typedef struct {                                                                                                   \
        eh_hash_##key_t##_##value_t##_fn _hash;                                                                        \
        eh_cmp_##key_t##_##value_t##_fn _cmp;                                                                          \
        size_t _bucket_capacity;                                                                                       \
                                                                                                                       \
        struct {                                                                                                       \
            size_t global_depth;                                                                                       \
            size_t num;                                                                                                \
            eh_bucket_##key_t##_##value_t##_t_** links;                                                                \
        } _dirs;                                                                                                       \
                                                                                                                       \
        eh_item_##key_t##_##value_t##_t_ _null_item;                                                                   \
        eh_item_##key_t##_##value_t##_t_ _tombstone_item;                                                              \
    } extendible_hashtable_##key_t##_##value_t##_t;                                                                    \
                                                                                                                       \
    enum eh_status_t eh_init_##key_t##_##value_t(extendible_hashtable_##key_t##_##value_t##_t* table,                  \
                                                 size_t bucket_capacity, eh_hash_##key_t##_##value_t##_fn_ hash,       \
                                                 eh_cmp_##key_t##_##value_t##_fn_ cmp) {                               \
        if (NULL == table || 0 == bucket_capacity || NULL == hash || NULL == cmp) {                                    \
            return EH_FAILURE;                                                                                         \
        }                                                                                                              \
                                                                                                                       \
        if (NULL == (table->_dirs.links = malloc(2 * sizeof(table->_dirs.links)))) {                                   \
            return EH_FAILURE;                                                                                         \
        }                                                                                                              \
        table->_hash = hash;                                                                                           \
        table->_cmp = cmp;                                                                                             \
        table->_bucket_capacity = bucket_capacity;                                                                     \
        table->_dirs.global_depth = 1;                                                                                 \
        table->_dirs.num = 2;                                                                                          \
        memset(&table->_null_item, 0, sizeof(table->_null_item));                                                      \
        memset(&table->_tombstone_item, 1, sizeof(table->_null_item));                                                 \
                                                                                                                       \
        if (NULL == (table->_dirs.links[0] = malloc(sizeof(**table->_dirs.links)))) {                                  \
            goto free1;                                                                                                \
        }                                                                                                              \
        if (NULL == (table->_dirs.links[1] = malloc(sizeof(**table->_dirs.links)))) {                                  \
            goto free2;                                                                                                \
        }                                                                                                              \
                                                                                                                       \
        table->_dirs.links[0]->local_depth = 1,                                                                        \
        table->_dirs.links[0]->items = calloc(table->_bucket_capacity, sizeof(table->_null_item));                     \
        table->_dirs.links[0]->next = table->_dirs.links[1];                                                           \
        if (NULL == table->_dirs.links[0]->items) {                                                                    \
            goto free3;                                                                                                \
        }                                                                                                              \
                                                                                                                       \
        table->_dirs.links[1]->local_depth = 1;                                                                        \
        table->_dirs.links[1]->items = calloc(table->_bucket_capacity, sizeof(table->_null_item));                     \
        table->_dirs.links[1]->next = NULL;                                                                            \
        if (NULL == table->_dirs.links[1]->items) {                                                                    \
            free(table->_dirs.links[0]->items);                                                                        \
        free3:                                                                                                         \
            free(table->_dirs.links[1]);                                                                               \
        free2:                                                                                                         \
            free(table->_dirs.links[0]);                                                                               \
        free1:                                                                                                         \
            free(table->_dirs.links);                                                                                  \
            return EH_FAILURE;                                                                                         \
        }                                                                                                              \
                                                                                                                       \
        return EH_SUCCESS;                                                                                             \
    }                                                                                                                  \
                                                                                                                       \
    void eh_free_##key_t##_##value_t(extendible_hashtable_##key_t##_##value_t##_t* table) {                            \
        for (eh_bucket_##key_t##_##value_t##_t_ * tmp, *head = *table->_dirs.links; head != NULL;) {                   \
            tmp = head;                                                                                                \
            head = head->next;                                                                                         \
            free(tmp->items);                                                                                          \
            free(tmp);                                                                                                 \
        }                                                                                                              \
        free(table->_dirs.links);                                                                                      \
        table = NULL;                                                                                                  \
    }                                                                                                                  \
                                                                                                                       \
    enum eh_status_t eh_insert_##key_t##_##value_t(extendible_hashtable_##key_t##_##value_t##_t* table,                \
                                                   const key_t* key, const value_t* value) {                           \
        if (NULL == table || NULL == key || NULL == value) {                                                           \
            return EH_FAILURE;                                                                                         \
        }                                                                                                              \
                                                                                                                       \
        eh_bucket_##key_t##_##value_t##_t_* bucket =                                                                   \
            table->_dirs.links[table->_hash(key) & ((1 << table->_dirs.global_depth) - 1)];                            \
                                                                                                                       \
        size_t i = 0, tombstone_i = table->_bucket_capacity;                                                           \
        for (; memcmp(bucket->items + i, &table->_null_item, sizeof(table->_null_item)) != 0; ++i) {                   \
            if (0 == table->_cmp(&bucket->items[i].key, key)) {                                                        \
                memcpy(&bucket->items[i].value, value, sizeof(value_t));                                               \
                return EH_UPDATE;                                                                                      \
            }                                                                                                          \
            if (table->_bucket_capacity == tombstone_i                                                                 \
                && 0 == memcmp(bucket->items + i, &table->_tombstone_item, sizeof(table->_null_item))) {               \
                tombstone_i = i;                                                                                       \
            }                                                                                                          \
        }                                                                                                              \
                                                                                                                       \
        memcpy(&bucket->items[table->_bucket_capacity == tombstone_i ? i : tombstone_i].key, key, sizeof(key_t));      \
        memcpy(&bucket->items[table->_bucket_capacity == tombstone_i ? i : tombstone_i].value, value,                  \
               sizeof(value_t));                                                                                       \
        if (++i < table->_bucket_capacity) {                                                                           \
            return EH_INSERT;                                                                                          \
        }                                                                                                              \
                                                                                                                       \
        enum eh_status_t status = EH_OVERFLOW;                                                                         \
        if (table->_dirs.global_depth == bucket->local_depth) {                                                        \
            table->_dirs.links = realloc(table->_dirs.links, 2 * table->_dirs.num * sizeof(table->_dirs.links));       \
            if (NULL == table->_dirs.links) {                                                                          \
                return EH_FAILURE;                                                                                     \
            }                                                                                                          \
            for (size_t i = 0; i < table->_dirs.num; ++i) {                                                            \
                table->_dirs.links[table->_dirs.num + i] = table->_dirs.links[i];                                      \
            }                                                                                                          \
            table->_dirs.num *= 2;                                                                                     \
            ++table->_dirs.global_depth;                                                                               \
            status = EH_EXPANSION;                                                                                     \
        }                                                                                                              \
                                                                                                                       \
        eh_bucket_##key_t##_##value_t##_t_* new_bucket = malloc(sizeof(*bucket));                                      \
        if (NULL == new_bucket) {                                                                                      \
            return EH_FAILURE;                                                                                         \
        }                                                                                                              \
        *new_bucket = (eh_bucket_##key_t##_##value_t##_t_){                                                            \
            .local_depth = bucket->local_depth + 1,                                                                    \
            .items = calloc(table->_bucket_capacity, sizeof(table->_null_item)),                                       \
            .next = bucket->next,                                                                                      \
        };                                                                                                             \
        if (NULL == new_bucket->items) {                                                                               \
            free(new_bucket);                                                                                          \
            return EH_FAILURE;                                                                                         \
        }                                                                                                              \
                                                                                                                       \
        size_t high_bit = 1 << bucket->local_depth;                                                                    \
        for (size_t i = 0, j = 0; i < table->_bucket_capacity; ++i) {                                                  \
            if (table->_hash(&bucket->items[i].key) & high_bit) {                                                      \
                new_bucket->items[j++] = bucket->items[i];                                                             \
                bucket->items[i] = table->_null_item;                                                                  \
            }                                                                                                          \
        }                                                                                                              \
                                                                                                                       \
        ++bucket->local_depth;                                                                                         \
        for (size_t i = 0, j = table->_bucket_capacity - 1; i < j; ++i) {                                              \
            if (0 == memcmp(bucket->items + i, &table->_null_item, sizeof(table->_null_item))) {                       \
                for (; i < j && 0 == memcmp(bucket->items + j, &table->_null_item, sizeof(table->_null_item)); --j) {} \
                bucket->items[i] = bucket->items[j];                                                                   \
                bucket->items[j] = table->_null_item;                                                                  \
            }                                                                                                          \
        }                                                                                                              \
        bucket->next = new_bucket;                                                                                     \
                                                                                                                       \
        for (size_t i = table->_hash(key) & (high_bit - 1); i < table->_dirs.num; i += high_bit) {                     \
            table->_dirs.links[i] = i & high_bit ? new_bucket : bucket;                                                \
        }                                                                                                              \
                                                                                                                       \
        return status;                                                                                                 \
    }                                                                                                                  \
                                                                                                                       \
    enum eh_status_t eh_lookup_##key_t##_##value_t(const extendible_hashtable_##key_t##_##value_t##_t* table,          \
                                                   const key_t* key, value_t* value) {                                 \
        if (NULL == table || NULL == key || NULL == value) {                                                           \
            return EH_FAILURE;                                                                                         \
        }                                                                                                              \
                                                                                                                       \
        eh_bucket_##key_t##_##value_t##_t_* bucket =                                                                   \
            table->_dirs.links[table->_hash(key) & ((1 << table->_dirs.global_depth) - 1)];                            \
        for (size_t i = 0; 0 != memcmp(&table->_null_item, bucket->items + i, sizeof(table->_null_item)); ++i) {       \
            if (0 == table->_cmp(key, &bucket->items[i].key)) {                                                        \
                memcpy(value, &bucket->items[i].value, sizeof(value_t));                                               \
                return EH_SUCCESS;                                                                                     \
            }                                                                                                          \
        }                                                                                                              \
                                                                                                                       \
        return EH_NOT_FOUND;                                                                                           \
    }                                                                                                                  \
                                                                                                                       \
    typedef struct {                                                                                                   \
        const extendible_hashtable_##key_t##_##value_t##_t* _table;                                                    \
        eh_bucket_##key_t##_##value_t##_t_* _bucket;                                                                   \
        size_t _index;                                                                                                 \
    } extendible_hashtable_it_##key_t##_##value_t##_t;                                                                 \
                                                                                                                       \
    [[nodiscard]] extendible_hashtable_it_##key_t##_##value_t##_t eh_create_it_##key_t##_##value_t(                    \
        const extendible_hashtable_##key_t##_##value_t##_t* table) {                                                   \
        return (extendible_hashtable_it_##key_t##_##value_t##_t){                                                      \
            ._table = table, ._bucket = *table->_dirs.links, ._index = 0};                                             \
    }                                                                                                                  \
                                                                                                                       \
    enum eh_status_t eh_next_##key_t##_##value_t(extendible_hashtable_it_##key_t##_##value_t##_t* it, key_t* key,      \
                                                 value_t* value) {                                                     \
        if (NULL == it || NULL == key || NULL == value) {                                                              \
            return EH_FAILURE;                                                                                         \
        }                                                                                                              \
                                                                                                                       \
        for (; NULL != it->_bucket; it->_bucket = it->_bucket->next) {                                                 \
            for (; it->_index < it->_table->_bucket_capacity                                                           \
                   && 0                                                                                                \
                          != memcmp(it->_bucket->items + it->_index, &it->_table->_null_item,                          \
                                    sizeof(it->_table->_null_item));                                                   \
                 ++it->_index) {                                                                                       \
                if (0                                                                                                  \
                    == memcmp(it->_bucket->items + it->_index, &it->_table->_tombstone_item,                           \
                              sizeof(it->_table->_null_item))) {                                                       \
                    continue;                                                                                          \
                }                                                                                                      \
                memcpy(key, &it->_bucket->items[it->_index].key, sizeof(key_t));                                       \
                memcpy(value, &it->_bucket->items[it->_index].value, sizeof(value_t));                                 \
                ++it->_index;                                                                                          \
                return EH_SUCCESS;                                                                                     \
            }                                                                                                          \
            it->_index = 0;                                                                                            \
        }                                                                                                              \
                                                                                                                       \
        return EH_NOT_FOUND;                                                                                           \
    }
#endif
