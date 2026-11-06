#ifndef HASH_TABLE_H
#define HASH_TABLE_H
#include "character.h"

#define HASH_BUCKETS 11

typedef struct HashNode {
    Character value;
    struct HashNode *next;
} HashNode;

typedef struct {
    HashNode *buckets[HASH_BUCKETS];
    int size;
} HashTable;

void hash_table_init(HashTable *table);
int hash_table_insert(HashTable *table, const Character *value);
Character *hash_table_find(HashTable *table, const char *id);
void hash_table_print(const HashTable *table);
void hash_table_free(HashTable *table);

#endif
