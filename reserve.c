/*typedef struct {                                                                                                   \*/
/*    const extendible_hashtable_##key_t##_##value_t##_t* _table;                                                    \*/
/*    eh_bucket_##key_t##_##value_t##_t_* _bucket;                                                                   \*/
/*    size_t _index;                                                                                                 \*/
/*} extendible_hashtable_it_##key_t##_##value_t##_t;                                                                 \*/
/*                                                                                                                   \*/
/*[[nodiscard]] extendible_hashtable_it_##key_t##_##value_t##_t eh_make_it_##key_t##_##value_t(                      \*/
/*    const extendible_hashtable_##key_t##_##value_t##_t* table) {                                                   \*/
/*    return (extendible_hashtable_it_##key_t##_##value_t##_t){                                                      \*/
/*        ._table = table, ._bucket = *table->_dirs._links, ._index = 0};                                            \*/
/*}*/
/**/
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
