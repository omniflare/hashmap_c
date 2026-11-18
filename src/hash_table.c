#include <_string.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "hash_table.h"
#include "prime.h"

#define GROWTH_THRESHOLD 70
#define SHRINK_THRESHOLD 10

// funcitons prototype; TODO : check if better to put in the 
void* xmalloc(size_t size);
void* xcalloc(size_t size, long count);
static void size_up (Hashmap* hashmap);
static void size_down (Hashmap* hashmap);
static void delete_entry(Entry* item);

const int PRIME1 = 131;
const int PRIME2 = 57;
const int INITIAL_BASE_SIZE = 53;

static Entry DELETED = {NULL, NULL, NULL};

static Entry* new_entry(const char* key, const char* val) {
    Entry* item = malloc(sizeof(Entry)); 

    if (item == NULL) {
        fprintf(stderr, "Memory allocation failed\n");
        return NULL;
    }
    
    item ->  key = strdup(key);
    item -> value = strdup(val);
    if (item -> key == NULL || item -> value == NULL) {
        delete_entry(item);
        return NULL;
    }
    return item; 
}

Hashmap* new_sized_hashmap(const int base_size, CollisionHandling handling) {
    if ( handling != CHAINING && handling != DOUBLE_HASHING) {
        return NULL;
    }
    Hashmap* hash_map = xmalloc(sizeof(Hashmap));
    hash_map -> count = 0; 
    
    hash_map -> base_size = base_size; 
    hash_map -> size = next_prime(hash_map -> base_size);
    hash_map -> handling = handling;
    
    hash_map -> entries = xcalloc((size_t)hash_map ->size, sizeof(Entry**));
    return hash_map;
}

Hashmap* new_hashmap(CollisionHandling handling) {
    return new_sized_hashmap(INITIAL_BASE_SIZE, handling);
}

static void delete_entry(Entry* item){
    free(item -> key);
    free(item -> value);
    free(item);
}

void delete_hash_map(Hashmap* hash_map ) {
    if (hash_map == NULL){
        return;
    }
    for(int i=0; i<hash_map->size; i++){
        Entry* item = hash_map ->entries[i];
        if (item == NULL || item == &DELETED) {
            continue;
        }
        if(hash_map-> handling == DOUBLE_HASHING){
            delete_entry(item);
        }else{
            while (item != NULL) {
                Entry* next = item -> next;
                delete_entry(item);
                item = next;
            }
        }
    }
    free(hash_map ->entries);
    free(hash_map);
}

static int hash(const char* key, int a, int num_buckets) {
    unsigned long long hash = 0; 
    for (const unsigned char* p = (const unsigned char*)key; *p; p++) {
        hash = (hash*a + *p) %num_buckets;
    }
    return (int)hash;
}

static int get_hash(const char* key, const int total_buck, const int attempt) {
    int hash_a = hash(key, PRIME1, total_buck);
    int hash_b = hash(key, PRIME2, total_buck);
    int step = 1 + hash_b % (total_buck -1);
    return (int)(((long long)hash_a + (long long)attempt * step) % total_buck);
}

void insert_double_hash(Hashmap *hash_map, char *key, char *value) {
    const int load = hash_map -> count * 100/ hash_map -> size; 
    if (load > 70) {
        size_up(hash_map);
    }
    Entry* entry = new_entry(key, value);
    if (entry == NULL) {
        return;
    }
    int pos = get_hash(entry -> key, hash_map->size, 0);
    Entry* current_item = hash_map->entries[pos];
    int i = 1; 
    int first_deleted = -1; 
    
    while(current_item != NULL && i < hash_map->size) {
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

void insert_chain(Hashmap* hash_map, char* key, char* value) {
    int pos = hash(key, PRIME1, hash_map -> size);
    Entry* current_entry = hash_map -> entries[pos]; 
    while (current_entry != NULL) {
        if (strcmp(key, current_entry -> key) == 0){
            char* new_value= strdup(value);

            if (new_value == NULL) {
                return;
            }
            free(current_entry -> value);
            current_entry -> value = new_value;
            return;
        }
        current_entry = current_entry -> next; 
    }
    Entry* entry = new_entry(key, value);
    if (entry == NULL){
        return;
    }
    entry -> next = hash_map -> entries[pos];
    hash_map -> entries[pos] = entry;

    hash_map -> count ++; 
}

void insert(Hashmap *hash_map, char *key, char *value){
    if(hash_map ==NULL || key == NULL  || value == NULL){
        return;
    }
    size_up(hash_map);
    if ( hash_map->handling == CHAINING){
        insert_chain(hash_map, key, value);
    }else{
        insert_double_hash(hash_map, key, value);
    }
}

char* search(Hashmap* hash_map, char* key){
    if (hash_map == NULL || key == NULL){
        return NULL;
    }
    if (hash_map -> handling == CHAINING){
        int pos = hash(key, PRIME1, hash_map -> size);
        Entry* entry = hash_map->entries[pos];
        while(entry != NULL){
            if (strcmp(key, entry->key) == 0){
                return entry->value;
            }
            entry = entry->next;
        }
        return NULL;
    }
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
    if (hash_map->handling == CHAINING){
        int pos = hash(key, PRIME1, hash_map->size);
        Entry* current = hash_map -> entries[pos];
        Entry* prev = NULL;

        while(current != NULL) {
            if(strcmp(key, current -> key) == 0) {
                if (prev == NULL) {
                    hash_map->entries[pos]=current->next;
                }else{
                    prev -> next = current -> next;
                }
                delete_entry(current);
                hash_map->count--;

                size_down(hash_map);
                return;
            }

            prev = current;
            current = current -> next;
        }
        return;
    }
    
    int pos = get_hash(key, hash_map -> size, 0);
    Entry* entry = hash_map->entries[pos];
    int i = 0; 
    while(entry != NULL) {
        if (entry != &DELETED && strcmp(key, entry -> key) == 0) {
            delete_entry(entry);
            hash_map -> entries[pos] = &DELETED;
            hash_map -> count --; 
            
            size_down(hash_map);
            return;
        }
        pos = get_hash(key, hash_map -> size, i);
        entry = hash_map->entries[pos];
        i++;
    }
}

static void resize(Hashmap* hashmap, int new_size) {
    if (new_size < INITIAL_BASE_SIZE) {
        return;
    }

    new_size = next_prime(new_size);
    Entry** old_entries =  hashmap -> entries;
    int old_size = hashmap -> size;

    hashmap -> entries = xcalloc((size_t)new_size, sizeof(*hashmap -> entries));
    hashmap -> size = new_size; 

    if(hashmap -> handling == CHAINING) {
        for (int i=0; i< old_size; i++){
            Entry* current = old_entries[i];
            while(current != NULL){
                Entry* next = current -> next;
                int pos = hash(current -> key, PRIME1, hashmap -> size);

                current -> next = hashmap ->entries[pos];
                hashmap->entries[pos] = current;

                current = next;
            }
        }
    }else {
        for(int i = 0; i<old_size; i++){
            Entry* entry = old_entries[i];
            if(entry == NULL || entry == &DELETED){
                continue;
            }

            for (int attempt =0; attempt < hashmap -> size; attempt++){
                int pos = get_hash(entry ->key, hashmap -> size, attempt);
                if (hashmap -> entries[pos] ==NULL){
                    hashmap->entries[pos] = entry;
                    break;
                }
            }
        }
    }
    free(old_entries);
}

static void size_up (Hashmap* hashmap){
    if ( hashmap -> count *100/hashmap->size >= GROWTH_THRESHOLD) {
        resize(hashmap, hashmap -> size*2);
    }
}

static void size_down (Hashmap* hashmap){
    if (hashmap -> size> INITIAL_BASE_SIZE && hashmap -> count *100/hashmap->size < SHRINK_THRESHOLD){
        resize(hashmap, hashmap->size/2);
    } 
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
