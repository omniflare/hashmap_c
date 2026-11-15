typedef struct{
    char* key;
    char* value; 
}Entry;

typedef struct {
    int size;
    int count;
    int base_size; 
    Entry** entries; //[array of pointers to struct -- remember for later;]
}Hashmap; 

Hashmap* new_sized_hashmap(int size) ;
Hashmap* new_hashmap();
void delete_hash_map(Hashmap* hash_map);
void insert(Hashmap* hash_map, char* key, char* value); 
char* search(Hashmap* hash_map, char* key);
void remove_key(Hashmap* hash_map, char* key);
// TODO: make it bool for error handling; 