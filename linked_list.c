#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "include/linked_list.h"

void collection_init(Collection *c) { c->head = NULL; c->size = 0; }

int collection_add(Collection *c, const Character *ch) {
    if (!c || !ch) return -1;
    for (OwnedNode *n = c->head; n; n = n->next) {
        if (n->character.id[0] && ch->id[0] && strcmp(n->character.id, ch->id) == 0) {
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
    puts("\n=== YOUR LITTLE GUYS ===");
    if (!c->head) { puts("Your collection is empty. Go pull some friends!"); return; }
    for (OwnedNode *n = c->head; n; n = n->next) {
        printf("\n★%d  %s — %s | copies %d | bond %d | HP %d ATK %d DEF %d SPD %d\n",
               n->character.rarity, n->character.name, n->character.role,
               n->copies, n->level, n->character.hp, n->character.attack,
               n->character.defense, n->character.speed);
        character_print_art(&n->character);
    }
    printf("\nUnique friends: %d\n", c->size);
}

int collection_count(const Collection *c) { return c->size; }

void collection_free(Collection *c) {
    OwnedNode *n = c->head;
    while (n) { OwnedNode *next = n->next; free(n); n = next; }
    c->head = NULL; c->size = 0;
}
