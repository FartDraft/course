#include <stdio.h>
#include <stdlib.h>
#include "extendible_hashtable.h"

typedef char person_t[52];
typedef char phone_t[12];
GENERATE_EXTENDIBLE_HASHTABLE(person_t, phone_t)

#define FNV_OFFSET 14695981039346656037UL
#define FNV_PRIME  1099511628211UL

size_t
hash_fnv1a(person_t key) {
    size_t hash = FNV_OFFSET;
    for (const char* p = key; *p; p++) {
        hash ^= (size_t)*p;
        hash *= FNV_PRIME;
    }
    return hash;
}

int
main(int argc, char* argv[]) {
    size_t bucket_size;
    if (2 != argc || (1 != sscanf(argv[1], "%zu", &bucket_size))) {
        fprintf(stderr, "Provide bucket size argument\n");
        return EXIT_FAILURE;
    }

    const char path[] = "res/data.txt";
    FILE* file = fopen(path, "rt");
    if (NULL == file) {
        fprintf(stderr, "Could not open %s for reading\n", path);
        return EXIT_FAILURE;
    }

    extendible_hashtable_person_t_phone_t_t table;
    if (EH_FAILURE == eh_init(&table, bucket_size, hash_fnv1a, strcmp)) {
        fprintf(stderr, "Failure in eh_init_person_t_phone_t\n");
        fclose(file);
        return EXIT_FAILURE;
    }

    /*person_t person;*/
    /*phone_t phone;*/
    /*while (2 == fscanf(file, "%[^,], %11s\n", person, phone)) {*/
    /*if (EH_UPDATE == eh_insert(&table, &person, &phone)) {*/
    /*    printf("EH_UPDATE\n");*/
    /*}*/
    /*    phone_t tmp;*/
    /*    eh_insert(&table, &person, &phone);*/
    /*    eh_lookup(&table, &person, &tmp);*/
    /*    if (0 != strcmp(phone, tmp)) {*/
    /*        printf("Ooops!\n");*/
    /*    }*/
    /*}*/

    /*for (extendible_hashtable_it_person_t_phone_t_t it = eh_create_it(&table);*/
    /*     EH_SUCCESS == eh_next(&it, &person, &phone);) {*/
    /*    printf("%s, %s\n", person, phone);*/
    /*}*/

    /*printf("82659131653 should be equal:\n%s\n", (eh_lookup(&table, &(person_t){"Ouida Howles"}, &phone), phone));*/
    /*printf("%zu\n", table._dirs.num);*/

    eh_free(&table);
    fclose(file);
    return EXIT_SUCCESS;
}
