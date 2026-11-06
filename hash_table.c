#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "include/hash_table.h"

static unsigned long hash_id(const char *s) {
    unsigned long h = 5381;
    int c;
    while ((c = (unsigned char)*s++)) h = ((h << 5) + h) + (unsigned long)c;
    return h % HASH_BUCKETS;
}

void hash_table_init(HashTable *t) {
    memset(t->buckets, 0, sizeof(t->buckets));
    t->size = 0;
}

int hash_table_insert(HashTable *t, const Character *v) {
    if (!t || !v || hash_table_find(t, v->id)) return 0;
    unsigned long i = hash_id(v->id);
    HashNode *n = malloc(sizeof(*n));
    if (!n) return -1;
    n->value = *v; n->next = t->buckets[i]; t->buckets[i] = n; t->size++;
    return 1;
}

Character *hash_table_find(HashTable *t, const char *id) {
    if (!t || !id) return NULL;
    for (HashNode *n = t->buckets[hash_id(id)]; n; n = n->next)
        if (strcmp(n->value.id, id) == 0) return &n->value;
    return NULL;
}

void hash_table_print(const HashTable *t) {
    puts("\n=== CHARACTER CATALOGUE (HASH BUCKETS) ===");
    for (int i = 0; i < HASH_BUCKETS; i++) {
        printf("[%02d]", i);
        for (HashNode *n = t->buckets[i]; n; n = n->next)
            printf(" -> %s(%s)", n->value.name, n->value.id);
        puts(" -> NULL");
    }
}

void hash_table_free(HashTable *t) {
    for (int i = 0; i < HASH_BUCKETS; i++) {
        HashNode *n = t->buckets[i];
        while (n) { HashNode *next = n->next; free(n); n = next; }
        t->buckets[i] = NULL;
    }
    t->size = 0;
}
