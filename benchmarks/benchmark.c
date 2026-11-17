
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "hash_table.h"

#define NUMBER_OF_DATASETS 4

static const int DATASETS[NUMBER_OF_DATASETS] = {
    100000,
    500000,
    1000000
};

static double elapsed(clock_t start, clock_t end) {
    return (double)(end - start) / CLOCKS_PER_SEC;
}

static void benchmark(int total_keys) {
    Hashmap* map = new_hashmap();

    if (map == NULL) {
        fprintf(stderr, "Failed to create hashmap\n");
        return;
    }

    char key[64];
    clock_t start, end;

    volatile unsigned long found = 0;

    printf("\n---------------------------------------------\n");
    printf("Dataset size: %d entries\n", total_keys);
    printf("---------------------------------------------\n");

    // 1. Insertion
    start = clock();

    for (int i = 0; i < total_keys; i++) {
        snprintf(key, sizeof(key), "key-%d", i);
        insert(map, key, "benchmark-value");
    }

    end = clock();
    double insertion_time = elapsed(start, end);

    // 2. Successful search
    start = clock();

    for (int i = 0; i < total_keys; i++) {
        snprintf(key, sizeof(key), "key-%d", i);

        if (search(map, key) != NULL) {
            found++;
        }
    }

    end = clock();
    double successful_search = elapsed(start, end);

    // 3. Unsuccessful search
    start = clock();

    for (int i = 0; i < total_keys; i++) {
        snprintf(key, sizeof(key), "missing-%d", i);
        search(map, key);
    }

    end = clock();
    double unsuccessful_search = elapsed(start, end);

    // 4. Deletion
    start = clock();

    for (int i = 0; i < total_keys; i++) {
        snprintf(key, sizeof(key), "key-%d", i);
        remove_key(map, key);
    }

    end = clock();
    double deletion_time = elapsed(start, end);

    printf("Successfully searched : %lu / %d\n", found, total_keys);
    printf("Insertion             : %.6f seconds\n", insertion_time);
    printf("Successful search     : %.6f seconds\n", successful_search);
    printf("Unsuccessful search   : %.6f seconds\n", unsuccessful_search);
    printf("Deletion              : %.6f seconds\n", deletion_time);

    delete_hash_map(map);
}

int main(void) {
    printf("=============================================\n");
    printf("          HASHMAP BENCHMARK\n");
    printf("=============================================\n");

    for (int i = 0; i < NUMBER_OF_DATASETS; i++) {
        benchmark(DATASETS[i]);
    }

    printf("\nBenchmark completed!\n");

    return EXIT_SUCCESS;
}
