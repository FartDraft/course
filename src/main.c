#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "inc/extendible_hashtable.h"

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
    size_t bucket_size;
    if (2 != argc || (1 != sscanf(argv[1], "%zu", &bucket_size))) {
        fprintf(stderr, "Provide bucket size argument");
        return EXIT_FAILURE;
    }

    const char path[] = "res/data.txt";
    FILE* file = fopen(path, "rt");
    if (file == NULL) {
        fprintf(stderr, "Could not open %s for reading", path);
        return EXIT_FAILURE;
    }

    extendible_hashtable_person_t_phone_t_t table = eh_make_person_t_phone_t(bucket_size, hash_fnv1a, cmp_str);
    /*if (table == NULL) {*/
    /*    fprintf(stderr, "Could not allocate hashtable");*/
    /*    return EXIT_FAILURE;*/
    /*}*/

    // Insert.
    person_t person = "Egor Afanasin";
    phone_t phone = "89208307083";
    /*printf("%s\n", eh_status_name[eh_insert_person_t_phone_t(&person, &phone, table)]);*/
    /*printf("%s\n", *eh_lookup_person_t_phone_t(&person, table));*/

    //Update.
    /*phone_t new_phone = "89206010201";*/
    /*printf("%s\n", eh_status_name[eh_insert_person_t_phone_t(&person, &new_phone, table)]);*/
    /*printf("%s\n", *eh_lookup_person_t_phone_t(&person, table));*/

    /*person_t person1 = "Pavel Afanasin";*/
    /*phone_t phone1 = "89206002801";*/
    /*printf("%s\n", eh_status_name[eh_insert_person_t_phone_t(&person1, &phone1, table)]);*/
    /*printf("%s\n", *eh_lookup_person_t_phone_t(&person1, table));*/

    // Expansion.
    /*person_t person2 = "Ekaterina Afanasina";*/
    /*phone_t phone2 = "89206002802";*/
    /*printf("%s\n", eh_status_name[eh_insert_person_t_phone_t(&person2, &phone2, table)]);*/
    /*printf("%s\n", *eh_lookup_person_t_phone_t(&person2, table));*/
    while (fscanf(file, "%[^,], %11s\n", person, phone) == 2) {
        eh_insert_person_t_phone_t(&person, &phone, &table);
    }
    printf("%s\n", *eh_lookup_person_t_phone_t(&(person_t){"Ouida Howles"}, &table)); // 82659131653
    printf("%zu", table._dirs._num);

    /*for (extendible_hashtable_it_person_t_phone_t_t it_table = eh_make_it_person_t_phone_t(&table);*/
    /*     eh_next_person_t_phone_t(&it_table, &person, &phone) != EH_NOT_FOUND;) {*/
    /*    printf("Person: %s, phone: %s\n", person, phone);*/
    /*}*/

    eh_destroy_person_t_phone_t(&table);
    fclose(file);
    return EXIT_SUCCESS;
}
