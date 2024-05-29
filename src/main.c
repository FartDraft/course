#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "inc/extendible_hashtable.h"

#define return_defer(value)                                                                                            \
    do {                                                                                                               \
        result = (value);                                                                                              \
        goto defer;                                                                                                    \
    } while (0)

typedef char person_t[52];
typedef char phone_t[12];
GENERATE_EXTENDIBLE_HASHTABLE(person_t, phone_t)

int
cmp_str(const person_t* a, const person_t* b) {
    return strcmp(*a, *b);
}

#define FNV_OFFSET 14695981039346656037UL
#define FNV_PRIME  1099511628211UL

size_t
hash_fnv1a(const person_t* key) {
    size_t hash = FNV_OFFSET;
    for (const char* p = *key; *p; p++) {
        hash ^= (size_t)*p;
        hash *= FNV_PRIME;
    }
    return hash;
}

int
main([[maybe_unused]] int argc, [[maybe_unused]] char* argv[]) {
    int result = EXIT_SUCCESS;

    size_t bucket_size;
    if (2 != argc || (1 != sscanf(argv[1], "%zu", &bucket_size))) {
        fprintf(stderr, "Provide bucket size argument\n");
        return EXIT_FAILURE;
    }

    const char path[] = "res/data.txt";
    FILE* file = fopen(path, "rt");
    if (!file) {
        fprintf(stderr, "Could not open %s for reading\n", path);
        return_defer(EXIT_FAILURE);
    }

    extendible_hashtable_person_t_phone_t_t* table = eh_create_person_t_phone_t(bucket_size, hash_fnv1a, cmp_str);
    if (!table) {
        fprintf(stderr, "Could not allocate hashtable\n");
        return_defer(EXIT_FAILURE);
    }

    person_t person = "Egor Afanasin";
    phone_t phone = "89206012801";
    printf("%d\n", eh_insert_person_t_phone_t(&person, &phone, table));
    person_t person1 = "Pavel Afanasin";
    phone_t phone1 = "89206002801";
    printf("%d\n", eh_insert_person_t_phone_t(&person1, &phone1, table));
    person_t person2 = "Ekaterina Afanasina";
    phone_t phone2 = "89206002802";
    printf("%d\n", eh_insert_person_t_phone_t(&person2, &phone2, table));
    printf("%s\n", *eh_lookup_person_t_phone_t(&person, table));

defer:
    eh_destroy_person_t_phone_t(table);
    fclose(file);
    return result;
}
