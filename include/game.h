#ifndef GAME_H
#define GAME_H
#include "hash_table.h"
#include "linked_list.h"
#include "graph.h"
#include "queue.h"
#include "stack.h"
#include "tree.h"

#define MAX_PARTY 3
#define MAX_ENEMIES 3

typedef struct {
    HashTable catalogue;
    Collection collection;
    SynergyGraph synergy;
    TurnQueue turns;
    ActionStack history;
    RelicNode *relics;
    int pity5;
    int pity4;
    int featured_guaranteed;
    char relic_slots[MAX_PARTY][48];
    int relic_bonus[MAX_PARTY];
} Game;

void game_init(Game *game);
void game_free(Game *game);
void game_gacha(Game *game, int pulls);
void game_battle_demo(Game *game);
void game_battle_play(Game *game);
void game_show_relics(Game *game);
void game_save(const Game *game, const char *path);
void game_load_collection(Game *game, const char *path);

#endif
