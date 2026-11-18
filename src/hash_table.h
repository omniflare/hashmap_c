#ifndef HASH_TABLE_H
#define HASH_TABLE_H

typedef struct Entry{
    char* key;
    char* value; 
    struct Entry* next;
}Entry;

typedef enum {
    CHAINING,
    DOUBLE_HASHING,
} CollisionHandling;

typedef struct {
    int size;
    int count;
    int base_size; 
    CollisionHandling handling;
    Entry** entries; //[array of pointers to struct -- remember for later;]
}Hashmap; 

Hashmap* new_sized_hashmap(const int size, CollisionHandling handling) ;
Hashmap* new_hashmap(CollisionHandling handling);
void delete_hash_map(Hashmap* hash_map);
void insert(Hashmap* hash_map, char* key, char* value); 
char* search(Hashmap* hash_map, char* key);
void remove_key(Hashmap* hash_map, char* key);
// TODO: make it bool for error handling; 
#endif