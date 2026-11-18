
#include <stdio.h>
#include <string.h>
#include "hash_table.h"

#define TEST_SIZE 10000

static int test_hashmap(CollisionHandling handling, const char* name) {
    printf("\n========== %s ==========\n", name);

    Hashmap* map = new_hashmap(handling);

    if (map == NULL) {
        printf("Failed to create hashmap\n");
        return 0;
    }

    insert(map, "name", "Satyam");
    insert(map, "city", "Delhi");
    insert(map, "language", "C");

    if (map->count != 3) {
        printf("Failed: incorrect count after insertion\n");
        goto fail;
    }

    char* value = search(map, "name");

    if (value == NULL || strcmp(value, "Satyam") != 0) {
        printf("Failed: search\n");
        goto fail;
    }

    if (search(map, "country") != NULL) {
        printf("Failed: missing key search\n");
        goto fail;
    }

    insert(map, "name", "Satyam Jha");

    value = search(map, "name");

    if (value == NULL || strcmp(value, "Satyam Jha") != 0) {
        printf("Failed: update\n");
        goto fail;
    }

    if (map->count != 3) {
        printf("Failed: count changed after update\n");
        goto fail;
    }

    remove_key(map, "city");

    if (search(map, "city") != NULL || map->count != 2) {
        printf("Failed: deletion\n");
        goto fail;
    }

    remove_key(map, "country");

    if (map->count != 2) {
        printf("Failed: count changed after deleting missing key\n");
        goto fail;
    }

    // Insert enough entries to trigger resizing.
    char key[32];
    char val[32];

    for (int i = 0; i < TEST_SIZE; i++) {
        snprintf(key, sizeof(key), "key-%d", i);
        snprintf(val, sizeof(val), "value-%d", i);
        insert(map, key, val);
    }

    for (int i = 0; i < TEST_SIZE; i++) {
        snprintf(key, sizeof(key), "key-%d", i);
        snprintf(val, sizeof(val), "value-%d", i);

        value = search(map, key);

        if (value == NULL || strcmp(value, val) != 0) {
            printf("Failed: lookup after resizing (%s)\n", key);
            goto fail;
        }
    }

    // Remove the inserted entries to exercise shrinking.
    for (int i = 0; i < TEST_SIZE; i++) {
        snprintf(key, sizeof(key), "key-%d", i);
        remove_key(map, key);
    }

    if (map->count != 2) {
        printf("Failed: incorrect count after bulk deletion\n");
        goto fail;
    }

    if (search(map, "name") == NULL ||
        search(map, "language") == NULL) {
        printf("Failed: original entries lost after resizing\n");
        goto fail;
    }

    delete_hash_map(map);
    printf("Passed\n");
    return 1;

fail:
    delete_hash_map(map);
    return 0;
}

int main(void) {
    int chaining_ok = test_hashmap(
        CHAINING,
        "Separate Chaining"
    );

    int double_hashing_ok = test_hashmap(
        DOUBLE_HASHING,
        "Double Hashing"
    );

    if (chaining_ok && double_hashing_ok) {
        printf("\nAll tests passed!\n");
        return 0;
    }

    printf("\nSome tests failed.\n");
    return 1;
}
