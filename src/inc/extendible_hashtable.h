#ifndef EXTENDIBLE_HASHTABLE_
#define EXTENDIBLE_HASHTABLE_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define EH_PRINT_DIE_(...)                                                                                             \
    do {                                                                                                               \
        fprintf(stderr, __VA_ARGS__);                                                                                  \
        abort();                                                                                                       \
    } while (0)

extern inline void*
eh_malloc_or_die_(size_t memory_size) {
    void* memory = malloc(memory_size);
    if (memory == NULL) {
        EH_PRINT_DIE_("Could not allocate memory!");
    }
    return memory;
}

extern inline void*
eh_calloc_or_die_(size_t num, size_t size) {
    void* memory = calloc(num, size);
    if (memory == NULL) {
        EH_PRINT_DIE_("Could not allocate memory!");
    }
    return memory;
}

extern inline void*
eh_realloc_or_die_(void* ptr, size_t new_size) {
    ptr = realloc(ptr, new_size);
    if (ptr == NULL) {
        EH_PRINT_DIE_("Could not allocate memory!");
    }
    return ptr;
}

enum eh_status_t { EH_INSERT, EH_UPDATE, EH_OVERFLOW, EH_EXPANSION, EH_FOUND, EH_NOT_FOUND };

static const char* const eh_status_name[] = {"EH_INSERT",    "EH_UPDATE", "EH_OVERFLOW",
                                             "EH_EXPANSION", "EH_FOUND",  "EH_NOT_FOUND"};

#define GENERATE_EXTENDIBLE_HASHTABLE(key_t, value_t)                                                                  \
    typedef struct {                                                                                                   \
        key_t _key;                                                                                                    \
        value_t value;                                                                                                 \
    } eh_item_##key_t##_##value_t##_t_;                                                                                \
                                                                                                                       \
    typedef struct eh_bucket_##key_t##_##value_t##_ {                                                                  \
        size_t _local_depth;                                                                                           \
        eh_item_##key_t##_##value_t##_t_* _items;                                                                      \
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
    [[nodiscard]] extendible_hashtable_##key_t##_##value_t##_t eh_make_##key_t##_##value_t(                            \
        size_t bucket_capacity, eh_hash_##key_t##_##value_t##_fn_ hash, eh_cmp_##key_t##_##value_t##_fn_ cmp) {        \
        if (bucket_capacity == 0 || hash == NULL || cmp == NULL) {                                                     \
            EH_PRINT_DIE_("Invalid parameters in %s", __func__);                                                       \
        }                                                                                                              \
                                                                                                                       \
        extendible_hashtable_##key_t##_##value_t##_t table = (extendible_hashtable_##key_t##_##value_t##_t){           \
            .hash = hash,                                                                                              \
            .cmp = cmp,                                                                                                \
            ._bucket_capacity = bucket_capacity,                                                                       \
            ._dirs =                                                                                                   \
                {                                                                                                      \
                    ._global_depth = 1,                                                                                \
                    ._num = 2,                                                                                         \
                    ._links = eh_malloc_or_die_(2 * sizeof(table._dirs._links)),                                       \
                },                                                                                                     \
        };                                                                                                             \
        memset(&table._tombstone_item, 1, sizeof(table._null_item));                                                   \
                                                                                                                       \
        table._dirs._links[0] = eh_malloc_or_die_(sizeof(**table._dirs._links));                                       \
        table._dirs._links[1] = eh_malloc_or_die_(sizeof(**table._dirs._links));                                       \
        *table._dirs._links[0] = (eh_bucket_##key_t##_##value_t##_t_){                                                 \
            ._local_depth = 1,                                                                                         \
            ._items = eh_calloc_or_die_(table._bucket_capacity, sizeof(table._null_item)),                             \
            ._next = table._dirs._links[1],                                                                            \
        };                                                                                                             \
        *table._dirs._links[1] = (eh_bucket_##key_t##_##value_t##_t_){                                                 \
            ._local_depth = 1,                                                                                         \
            ._items = eh_calloc_or_die_(table._bucket_capacity, sizeof(table._null_item)),                             \
            ._next = NULL,                                                                                             \
        };                                                                                                             \
                                                                                                                       \
        return table;                                                                                                  \
    }                                                                                                                  \
                                                                                                                       \
    void eh_destroy_##key_t##_##value_t(extendible_hashtable_##key_t##_##value_t##_t* table) {                         \
                                                                                                                       \
        for (eh_bucket_##key_t##_##value_t##_t_ * tmp, *head = *table->_dirs._links; head != NULL;) {                  \
            tmp = head;                                                                                                \
            head = head->_next;                                                                                        \
            free(tmp->_items);                                                                                         \
            free(tmp);                                                                                                 \
        }                                                                                                              \
        free(table->_dirs._links);                                                                                     \
    }                                                                                                                  \
                                                                                                                       \
    enum eh_status_t eh_insert_##key_t##_##value_t(const key_t* key, const value_t* value,                             \
                                                   extendible_hashtable_##key_t##_##value_t##_t* table) {              \
        if (key == NULL || value == NULL || table == NULL) {                                                           \
            EH_PRINT_DIE_("Invalid parameters in %s", __func__);                                                       \
        }                                                                                                              \
                                                                                                                       \
        eh_bucket_##key_t##_##value_t##_t_* bucket =                                                                   \
            table->_dirs._links[table->hash(key) & ((1 << table->_dirs._global_depth) - 1)];                           \
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
        if (++i < table->_bucket_capacity) {                                                                           \
            return EH_INSERT;                                                                                          \
        }                                                                                                              \
        enum eh_status_t status = EH_OVERFLOW;                                                                         \
                                                                                                                       \
        if (bucket->_local_depth == table->_dirs._global_depth) {                                                      \
            ++table->_dirs._global_depth;                                                                              \
            table->_dirs._links = eh_realloc_or_die_(table->_dirs._links,                                              \
                                                     2 * table->_dirs._num * sizeof(table->_dirs._links));             \
            for (size_t i = 0; i < table->_dirs._num; ++i) {                                                           \
                table->_dirs._links[table->_dirs._num + i] = table->_dirs._links[i];                                   \
            }                                                                                                          \
            table->_dirs._num *= 2;                                                                                    \
            status = EH_EXPANSION;                                                                                     \
        }                                                                                                              \
                                                                                                                       \
        eh_bucket_##key_t##_##value_t##_t_* new_bucket = eh_malloc_or_die_(sizeof(*bucket));                           \
        *new_bucket = (eh_bucket_##key_t##_##value_t##_t_){                                                            \
            ._local_depth = bucket->_local_depth + 1,                                                                  \
            ._items = eh_calloc_or_die_(table->_bucket_capacity, sizeof(table->_null_item)),                           \
            ._next = bucket->_next,                                                                                    \
        };                                                                                                             \
        size_t high_bit = 1 << bucket->_local_depth;                                                                   \
        for (size_t i = 0, j = 0; i < table->_bucket_capacity; ++i) {                                                  \
            if (table->hash(&bucket->_items[i]._key) & high_bit) {                                                     \
                new_bucket->_items[j++] = bucket->_items[i];                                                           \
                bucket->_items[i] = table->_null_item;                                                                 \
            }                                                                                                          \
        }                                                                                                              \
                                                                                                                       \
        ++bucket->_local_depth;                                                                                        \
        for (size_t i = 0, j = table->_bucket_capacity - 1; i < j; ++i) {                                              \
            if (memcmp(bucket->_items + i, &table->_null_item, sizeof(table->_null_item)) == 0) {                      \
                for (; i < j && memcmp(bucket->_items + j, &table->_null_item, sizeof(table->_null_item)) == 0; --j) { \
                }                                                                                                      \
                bucket->_items[i] = bucket->_items[j];                                                                 \
                bucket->_items[j] = table->_null_item;                                                                 \
            }                                                                                                          \
        }                                                                                                              \
        bucket->_next = new_bucket;                                                                                    \
                                                                                                                       \
        for (size_t i = table->hash(key) & (high_bit - 1); i < table->_dirs._num; i += high_bit) {                     \
            table->_dirs._links[i] = i & high_bit ? new_bucket : bucket;                                               \
        }                                                                                                              \
                                                                                                                       \
        return status;                                                                                                 \
    }                                                                                                                  \
                                                                                                                       \
    [[nodiscard]] value_t* eh_lookup_##key_t##_##value_t(const key_t* key,                                             \
                                                         const extendible_hashtable_##key_t##_##value_t##_t* table) {  \
        if (key == NULL || table == NULL) {                                                                            \
            EH_PRINT_DIE_("Invalid parameters in %s", __func__);                                                       \
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
    }                                                                                                                  \
                                                                                                                       \
    typedef struct {                                                                                                   \
        const extendible_hashtable_##key_t##_##value_t##_t* _table;                                                    \
        eh_bucket_##key_t##_##value_t##_t_* _bucket;                                                                   \
        size_t _index;                                                                                                 \
    } extendible_hashtable_it_##key_t##_##value_t##_t;                                                                 \
                                                                                                                       \
    [[nodiscard]] extendible_hashtable_it_##key_t##_##value_t##_t eh_make_it_##key_t##_##value_t(                      \
        const extendible_hashtable_##key_t##_##value_t##_t* table) {                                                   \
        return (extendible_hashtable_it_##key_t##_##value_t##_t){                                                      \
            ._table = table, ._bucket = *table->_dirs._links, ._index = 0};                                            \
    }                                                                                                                  \
                                                                                                                       \
/*enum eh_status_t eh_next_##key_t##_##value_t(const extendible_hashtable_it_##key_t##_##value_t##_t* it_table,      \*/
/*                                             key_t* key, value_t* value) {                                         \*/
/*    for (eh_bucket_##key_t##_##value_t##_t_* head = it_table->_bucket; head != NULL; head = head->_next) {         \*/
/*        for (size_t i = it_table->_index;                                                                          \*/
/*             i < it_table->_table->_bucket_capacity                                                                \*/
/*             && memcmp(it_table->_bucket->_items + i, &it_table->_table->_null_item,                               \*/
/*                       sizeof(it_table->_table->_null_item))                                                       \*/
/*                    != 0                                                                                           \*/
/*             && memcmp(it_table->_bucket->_items + i, &it_table->_table->_tombstone_item,                          \*/
/*                       sizeof(it_table->_table->_null_item))                                                       \*/
/*                    != 0;                                                                                          \*/
/*             ++i) {                                                                                                \*/
/*            memcpy(value, &it_table->_bucket->_items[i].value, sizeof(value_t));                                   \*/
/*            memcpy(key, &it_table->_bucket->_items[i]._key, sizeof(key_t));                                        \*/
/*            return EH_FOUND;                                                                                       \*/
/*        }                                                                                                          \*/
/*        it_table->_index = 0;                                                                                      \*/
/*    }                                                                                                              \*/
/*    return EH_NOT_FOUND;                                                                                           \*/
/*}*/

// Skip tombstone, stop null.
#endif
