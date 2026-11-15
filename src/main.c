#include "hash_table.h"
#include <stdio.h>

int main() {

  Hashmap *map = new_hashmap();
  insert(map, "name", "Satyam");
  insert(map, "age", "22");
  insert(map, "city", "Delhi");

  printf("After insertion:\n");
  printf("Count: %d\n", map->count);

  printf("\nSearching:\n");
  printf("name: %s\n", search(map, "name"));
  printf("age: %s\n", search(map, "age"));
  printf("city: %s\n", search(map, "city"));

  // 3. Search for a key that doesn't exist
  printf("\nSearching for missing key:\n");
  char *result = search(map, "country");
  printf("country: %s\n", result ? result : "Not found");

  // 4. Update an existing key
  insert(map, "name", "Satyam Jha");

  printf("\nAfter updating name:\n");
  printf("name: %s\n", search(map, "name"));
  printf("Count: %d\n", map->count);

  // 5. Delete an existing key
  remove_key(map, "age");

  printf("\nAfter deleting age:\n");
  printf("Count: %d\n", map->count);

  result = search(map, "age");
  printf("age: %s\n", result ? result : "Not found");

  // 6. Delete a key that doesn't exist
  remove_key(map, "country");

  printf("\nAfter attempting to delete country:\n");
  printf("Count: %d\n", map->count);

  // 7. Verify remaining entries
  printf("\nRemaining entries:\n");
  printf("name: %s\n", search(map, "name"));
  printf("city: %s\n", search(map, "city"));

  delete_hash_map(map);
  return 0;
}
