#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "hash_table.h"
#include "prime.h"

void* xmalloc(size_t size);
void* xcalloc(size_t size, long count);
static void size_up (Hashmap* hashmap);
static void size_down (Hashmap* hashmap);

const int PRIME1 = 131;
const int PRIME2 = 57;
const int INITIAL_BASE_SIZE = 53;

static Entry DELETED = {NULL, NULL};

static Entry* new_entry(const char* key, const char* val) {
    Entry* item = malloc(sizeof(Entry)); 
    item ->  key = strdup(key);
    item -> value = strdup(val);
    return item; 
}

Hashmap* new_sized_hashmap(const int base_size) {
    Hashmap* hash_map = xmalloc(sizeof(Hashmap));
    hash_map -> count = 0; 
    
    hash_map -> base_size = base_size; 
    hash_map -> size = next_prime(hash_map -> base_size);
    
    hash_map -> entries = xcalloc((size_t)hash_map ->size, sizeof(Entry**));
    return hash_map;
}

Hashmap* new_hashmap() {
    return new_sized_hashmap(INITIAL_BASE_SIZE);
}

static void delete_entry(Entry* item){
    free(item -> key);
    free(item -> value);
    free(item);
}

void delete_hash_map(Hashmap* hash_map ) {
    for(int i=0; i<hash_map->size; i++){
        Entry* item = hash_map ->entries[i];
        if (item != NULL && item != &DELETED) {
            delete_entry(item);
        }
    }
    free(hash_map ->entries);
    free(hash_map);
}

int hash(const char* key, int a, int num_buckets) {
    long hash = 0; 
    const int len = strlen(key);
    for (int i=0; i< len; i++) {
        hash += (long)pow(a, len - (i+1)) * key[i];
        hash = hash % num_buckets;
    }
    return (int)hash;
}

static int get_hash(const char* key, const int total_buck, const int attempt) {
    const int hash_a = hash(key, PRIME1, total_buck);
    const int hash_b = hash(key, PRIME2, total_buck);

    return (hash_a + (attempt * (hash_b+1))) % total_buck;
}

void insert(Hashmap *hash_map, char *key, char *value) {
    const int load = hash_map -> count * 100/ hash_map -> size; 
    if (load > 70) {
        size_up(hash_map);
    }
    Entry* entry = new_entry(key, value);
    int pos = get_hash(entry -> key, hash_map->size, 0);
    Entry* current_item = hash_map->entries[pos];
    int i = 1; 
    int first_deleted = -1; 
    
    while(current_item != NULL ) {
        if (current_item == &DELETED){
            if (first_deleted == -1) {
                first_deleted = pos;
            }
        }else {
            if (strcmp(key, current_item -> key) == 0) {
                delete_entry(current_item);
                hash_map -> entries[pos] = entry;
                return;
            }
        }
        
        pos = get_hash(entry -> key, hash_map->size, i);
        current_item = hash_map->entries[pos];
        if(i > hash_map -> size) {
            break;
        }
        i ++;
    }
    if (first_deleted != -1){
        pos = first_deleted;
    }else if (current_item != NULL) {
        delete_entry(entry);
        return;
    }
    hash_map->entries[pos] = entry;
    hash_map->count ++;
}

char* search(Hashmap* hash_map, char* key){
    int pos = get_hash(key, hash_map -> size, 0);
    Entry* entry = hash_map->entries[pos];
    int i = 1; 
    while (entry != NULL) {
        if (entry != &DELETED){    
            if (strcmp(entry -> key, key) == 0){
                return entry -> value; 
            }
        }
        pos = get_hash(key, hash_map -> size, i);
        entry = hash_map->entries[pos];
        i++;
    }
    return NULL;
}

void remove_key(Hashmap *hash_map, char *key){
    if (hash_map == NULL || key == NULL) {
           return;
    }
    const int load = hash_map -> count * 100/ hash_map -> size; 
    if (load < 10) {
        size_down(hash_map);
    }
    int pos = get_hash(key, hash_map -> size, 0);
    Entry* entry = hash_map->entries[pos];
    int i = 0; 
    while(entry != NULL) {
        if (entry != &DELETED && strcmp(key, entry -> key) == 0) {
            delete_entry(entry);
            hash_map -> entries[pos] = &DELETED;
            hash_map -> count --; 
            return;
        }
        pos = get_hash(key, hash_map -> size, i);
        entry = hash_map->entries[pos];
        i++;
    }
}

static void resize(Hashmap* hashmap, const int base_size) {
    if (base_size < INITIAL_BASE_SIZE) {
        return;
    }
    Hashmap* mp = new_sized_hashmap(base_size);
    for(int i =0; i< hashmap -> size; i++) {
        Entry* entry = hashmap -> entries[i];
        if (entry != NULL && entry != &DELETED){
            insert(mp, entry-> key, entry -> value);
        }
    }

    hashmap->base_size = mp->base_size;
    hashmap->count = mp->count;

    const int temp = hashmap-> size; 
    hashmap -> size = mp -> size;
    mp -> size = temp; 

    Entry** items = hashmap -> entries; 
    hashmap -> entries = mp -> entries; 
    mp -> entries = items; 

    delete_hash_map(mp);
}

static void size_up (Hashmap* hashmap){
    const int new_size = hashmap -> base_size  * 2; 
    resize(hashmap, new_size);
}

static void size_down (Hashmap* hashmap){
    const int new_size = hashmap -> base_size  / 2; 
    resize(hashmap, new_size);
}

void* xmalloc(size_t size) {
    void* ptr = malloc(size);

    if (ptr == NULL) {
        fprintf(stderr, "Memory allocation failed\n");
        exit(EXIT_FAILURE);
    }

    return ptr;
}
void* xcalloc(size_t size, long count) {
    void* ptr = calloc(size, count);
    if (ptr == NULL) {
        fprintf(stderr, "Memory allocation failed\n");
        exit(EXIT_FAILURE);
    }
    return ptr;
}
