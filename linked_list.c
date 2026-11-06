#include <stdio.h>
#include <stdlib.h>
#include "include/linked_list.h"

void collection_init(Collection *c) { c->head = NULL; c->size = 0; }

int collection_add(Collection *c, const Character *ch) {
    for (OwnedNode *n = c->head; n; n = n->next) {
        if (n->character.id[0] && ch->id[0] &&
            __builtin_strcmp(n->character.id, ch->id) == 0) {
            n->copies++;
            if (n->copies % 2 == 0 && n->level < 6) n->level++;
            return 0;
        }
    }
    OwnedNode *n = malloc(sizeof(*n));
    if (!n) return -1;
    n->character = *ch; n->copies = 1; n->level = 1;
    n->next = c->head; c->head = n; c->size++;
    return 1;
}

void collection_print(const Collection *c) {
    puts("\n=== YOUR ANIMAL SQUAD (LINKED LIST) ===");
    if (!c->head) { puts("Your collection is empty. Go pull some friends!"); return; }
    for (OwnedNode *n = c->head; n; n = n->next)
        printf("%-10s %d-star | copies: %d | bond: %d | ATK: %d\n",
               n->character.name, n->character.rarity, n->copies, n->level, n->character.attack);
    printf("Unique characters: %d\n", c->size);
}
int collection_count(const Collection *c) { return c->size; }
void collection_free(Collection *c) {
    OwnedNode *n = c->head;
    while (n) { OwnedNode *next = n->next; free(n); n = next; }
    c->head = NULL; c->size = 0;
}
