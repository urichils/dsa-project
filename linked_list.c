#include <stdio.h>
#include <stdlib.h>
#include "include/linked_list.h"

int collection_add(Collection *c, const Character *ch) {
    for (OwnedNode *n = c->head; n; n = n->next) {
        if (n->character.id[0] && ch->id[0] && strcmp(n->character.id, ch->id) == 0) {
            n->copies++;
            if (n->copies % 2 == 0 && n->level < 6) {
                n->level++;
            }
            return 0;
        }
    }

    OwnedNode *n = malloc(sizeof(*n));
    if (!n) return -1;

    n->character = *ch;
    n->copies = 1;
    n->level = 1;
    n->next = c->head;
    c->head = n;
    c->size++;
    return 1;
}
