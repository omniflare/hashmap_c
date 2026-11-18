
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "hash_table.h"

#define NUMBER_OF_DATASETS 4

static const int DATASETS[] = {
    10000,
    100000,
    500000,
    1000000
};

static double elapsed(clock_t start, clock_t end) {
    return (double)(end - start) / CLOCKS_PER_SEC;
}

static void benchmark(
    FILE* report,
    CollisionHandling handling,
    const char* name,
    int total_keys
) {
    printf("\n--- %s | %d entries ---\n", name, total_keys);

    Hashmap* map = new_hashmap(handling);

    if (map == NULL) {
        printf("Could not create hashmap\n");
        fprintf(report, "| %s | %d | Failed to create hashmap | - | - | - |\n",
                name, total_keys);
        return;
    }

    char** keys = malloc((size_t)total_keys * sizeof *keys);

    if (keys == NULL) {
        printf("Could not allocate keys\n");
        delete_hash_map(map);
        return;
    }

    for (int i = 0; i < total_keys; i++) {
        keys[i] = malloc(32);

        if (keys[i] == NULL) {
            for (int j = 0; j < i; j++) {
                free(keys[j]);
            }

            free(keys);
            delete_hash_map(map);
            printf("Could not allocate key strings\n");
            return;
        }

        snprintf(keys[i], 32, "key-%d", i);
    }

    clock_t start = clock();

    for (int i = 0; i < total_keys; i++) {
        insert(map, keys[i], "benchmark-value");
    }

    clock_t end = clock();
    double insertion_time = elapsed(start, end);

    printf("Insertion:           %.4f seconds\n", insertion_time);

    volatile unsigned long found = 0;

    start = clock();

    for (int i = 0; i < total_keys; i++) {
        if (search(map, keys[i]) != NULL) {
            found++;
        }
    }

    end = clock();
    double successful_search_time = elapsed(start, end);

    printf("Successful search:   %.4f seconds\n",
           successful_search_time);

    char missing_key[32];

    start = clock();

    for (int i = 0; i < total_keys; i++) {
        snprintf(missing_key, sizeof missing_key, "missing-%d", i);

        if (search(map, missing_key) != NULL) {
            found++;
        }
    }

    end = clock();
    double unsuccessful_search_time = elapsed(start, end);

    printf("Unsuccessful search: %.4f seconds\n",
           unsuccessful_search_time);

    start = clock();

    for (int i = 0; i < total_keys; i++) {
        remove_key(map, keys[i]);
    }

    end = clock();
    double deletion_time = elapsed(start, end);

    printf("Deletion:            %.4f seconds\n", deletion_time);
    printf("Remaining entries:   %d\n", map->count);

    fprintf(report,
        "| %s | %d | %.4f | %.4f | %.4f | %.4f |\n",
        name,
        total_keys,
        insertion_time,
        successful_search_time,
        unsuccessful_search_time,
        deletion_time
    );

    for (int i = 0; i < total_keys; i++) {
        free(keys[i]);
    }

    free(keys);
    delete_hash_map(map);

    (void)found;
}

int main(void) {
    FILE* report = fopen("benchmark.md", "w");

    if (report == NULL) {
        perror("Could not create benchmark.md");
        return 1;
    }

    fprintf(report,
        "# Hashmap Benchmark Results\n\n"
        "Performance comparison between Separate Chaining and "
        "Double Hashing.\n\n"
        "Run using `make benchmark`.\n\n"
        "All timings are in seconds. Results depend on the machine "
        "and compiler configuration.\n\n"
        "| Strategy | Entries | Insertion | Successful Search "
        "| Unsuccessful Search | Deletion |\n"
        "|---|---:|---:|---:|---:|---:|\n"
    );

    for (int i = 0; i < NUMBER_OF_DATASETS; i++) {
        int total_keys = DATASETS[i];

        printf("\n====================================\n");
        printf("       DATASET: %d ENTRIES\n", total_keys);
        printf("====================================\n");

        benchmark(report, CHAINING,
                  "Separate Chaining", total_keys);

        benchmark(report, DOUBLE_HASHING,
                  "Double Hashing", total_keys);
    }

    fclose(report);

    printf("\nBenchmark results written to benchmark.md\n");

    return 0;
}
