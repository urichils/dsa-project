#ifndef LINKED_LIST_H
#define LINKED_LIST_H
#include "character.h"

typedef struct OwnedNode {
    Character character;
    int copies;
    int level;
    struct OwnedNode *next;
} OwnedNode;

typedef struct {
    OwnedNode *head;
    int size;
} Collection;

void collection_init(Collection *collection);
int collection_add(Collection *collection, const Character *character);
void collection_print(const Collection *collection);
int collection_count(const Collection *collection);
void collection_free(Collection *collection);

#endif
