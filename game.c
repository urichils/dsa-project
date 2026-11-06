#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "include/game.h"

typedef struct {
    char name[NAME_LEN];
    int hp;
    int max_hp;
    int attack;
    int defense;
    int speed;
    int is_enemy;
    int slot;
    int guard;
} BattleActor;

static Character make(const char *id, const char *name, int rarity, int hp, int atk, int def, int spd, const char *role) {
    Character c = {0};
    snprintf(c.id, sizeof(c.id), "%s", id);
    snprintf(c.name, sizeof(c.name), "%s", name);
    c.rarity = rarity;
    c.hp = hp;
    c.attack = atk;
    c.defense = def;
    c.speed = spd;
    snprintf(c.role, sizeof(c.role), "%s", role);
    return c;
}

static void battle_log(Game *g, const char *fmt, ...) {
    va_list args;
    char buffer[160];
    va_start(args, fmt);
    vsnprintf(buffer, sizeof(buffer), fmt, args);
    va_end(args);
    stack_push(&g->history, buffer);
    puts(buffer);
}

static int relic_value_for_name(const char *name) {
    if (strstr(name, "Crown") || strstr(name, "Talisman")) return 14;
    if (strstr(name, "Bell") || strstr(name, "Pendant")) return 12;
    if (strstr(name, "Scarf") || strstr(name, "Charm")) return 10;
    if (strstr(name, "Boots") || strstr(name, "Ring")) return 9;
    return 8;
}

static void ensure_party_slots(Game *g, OwnedNode *party[MAX_PARTY], int *party_count) {
    const char *starter_ids[] = {"miso", "clover", "bean"};
    for (int i = 0; i < (int)(sizeof(starter_ids) / sizeof(starter_ids[0])); i++) {
        int already = 0;
        for (int j = 0; j < *party_count; j++) {
            if (party[j] && strcmp(party[j]->character.id, starter_ids[i]) == 0) {
                already = 1;
                break;
            }
        }
        if (!already) {
            Character *starter = hash_table_find(&g->catalogue, starter_ids[i]);
            if (starter) {
                OwnedNode *node = NULL;
                for (OwnedNode *n = g->collection.head; n; n = n->next) {
                    if (strcmp(n->character.id, starter_ids[i]) == 0) {
                        node = n;
                        break;
                    }
                }
                if (!node) {
                    if (collection_add(&g->collection, starter) > 0) {
                        for (OwnedNode *n = g->collection.head; n; n = n->next) {
                            if (strcmp(n->character.id, starter_ids[i]) == 0) {
                                node = n;
                                break;
                            }
                        }
                    }
                }
                if (node && *party_count < MAX_PARTY) {
                    party[*party_count] = node;
                    (*party_count)++;
                }
            }
        }
    }
}

static void collect_party(Game *g, OwnedNode *party[MAX_PARTY], int *party_count) {
    *party_count = 0;
    for (OwnedNode *n = g->collection.head; n && *party_count < MAX_PARTY; n = n->next) {
        if (n->character.id[0] != '\0') {
            party[*party_count] = n;
            (*party_count)++;
        }
    }
    ensure_party_slots(g, party, party_count);
}

void game_init(Game *g) {
    hash_table_init(&g->catalogue);
    collection_init(&g->collection);
    graph_init(&g->synergy);
    queue_init(&g->turns);
    stack_init(&g->history);
    g->relics = relic_tree_create_demo();
    g->pity5 = 0;
    g->pity4 = 0;
    g->featured_guaranteed = 0;
    for (int i = 0; i < MAX_PARTY; i++) {
        g->relic_slots[i][0] = '\0';
        g->relic_bonus[i] = 0;
    }

    Character c[] = {
        make("miso", "Miso", 4, 900, 120, 80, 110, "Support"),
        make("clover", "Clover", 4, 1100, 90, 100, 100, "Healer"),
        make("bean", "Bean", 4, 1500, 100, 150, 75, "Tank"),
        make("toast", "Toast", 4, 850, 170, 65, 120, "Attacker"),
        make("pebble", "Pebble", 4, 1000, 110, 90, 95, "Debuffer"),
        make("luna", "Luna", 5, 1200, 220, 100, 115, "Support"),
        make("comet", "Comet", 5, 1400, 260, 130, 90, "Attacker")
    };

    for (size_t i = 0; i < sizeof(c) / sizeof(c[0]); i++) {
        hash_table_insert(&g->catalogue, &c[i]);
    }

    Character *starter = hash_table_find(&g->catalogue, "miso");
    collection_add(&g->collection, starter);
    starter = hash_table_find(&g->catalogue, "clover");
    collection_add(&g->collection, starter);
    starter = hash_table_find(&g->catalogue, "bean");
    collection_add(&g->collection, starter);

    graph_add_edge(&g->synergy, "Miso", "Clover", 15);
    graph_add_edge(&g->synergy, "Clover", "Bean", 10);
    graph_add_edge(&g->synergy, "Toast", "Comet", 20);
    graph_add_edge(&g->synergy, "Pebble", "Luna", 15);
    graph_add_edge(&g->synergy, "Miso", "Luna", 25);
}

void game_free(Game *g) {
    hash_table_free(&g->catalogue);
    collection_free(&g->collection);
    relic_tree_free(g->relics);
    g->relics = NULL;
}

void game_gacha(Game *g, int pulls) {
    const char *pool4[] = {"toast", "pebble"};
    const char *pool5[] = {"luna", "comet"};
    const char *featured_pool[] = {"luna", "comet"};

    for (int i = 0; i < pulls; i++) {
        int roll = rand() % 10000;
        const char *id = NULL;

        if (g->pity5 >= 89) {
            id = featured_pool[rand() % 2];
            g->pity5 = 0;
            g->pity4 = 0;
            g->featured_guaranteed = 0;
        } else if (roll < 60) {
            if (g->featured_guaranteed) {
                id = featured_pool[rand() % 2];
                g->featured_guaranteed = 0;
            } else {
                id = (rand() % 100 < 50) ? featured_pool[rand() % 2] : ((rand() % 2 == 0) ? "luna" : "comet");
            }
            g->pity5 = 0;
            g->pity4 = 0;
        } else if (g->pity4 >= 9 || roll < 600) {
            id = pool4[rand() % 2];
            g->pity4 = 0;
            g->pity5++;
        } else {
            g->pity4++;
            g->pity5++;
            printf("  No pull this time. Pity: 5-star %d/90 | 4-star %d/10\n", g->pity5, g->pity4);
            continue;
        }

        Character *ch = hash_table_find(&g->catalogue, id);
        if (!ch) {
            puts("  The warp fizzled. Please try again.");
            continue;
        }

        collection_add(&g->collection, ch);
        printf("  ★%d  %s joined your squad!\n", ch->rarity, ch->name);
    }

    printf("Pity status: 5-star %d/90 | 4-star %d/10\n", g->pity5, g->pity4);
    puts("Featured banner status: active guarantee available.");
}

static void battle_enemy_team(BattleActor *actors, int *count) {
    static const char *names[] = {"Moss Rat", "Leafling", "Stump Beetle"};
    for (int i = 0; i < 3; i++) {
        snprintf(actors[*count].name, sizeof(actors[*count].name), "%s", names[i]);
        actors[*count].max_hp = 120 + i * 40;
        actors[*count].hp = actors[*count].max_hp;
        actors[*count].attack = 24 + i * 8;
        actors[*count].defense = 8 + i * 3;
        actors[*count].speed = 80 + i * 12;
        actors[*count].is_enemy = 1;
        actors[*count].slot = i;
        actors[*count].guard = 0;
        (*count)++;
    }
}

void game_battle_play(Game *g) {
    OwnedNode *party[MAX_PARTY] = {0};
    int party_count = 0;
    collect_party(g, party, &party_count);

    if (party_count <= 0) {
        puts("Your squad is empty. Pull a few animal allies first.");
        return;
    }

    if (party_count < MAX_PARTY) {
        puts("You can only fight with the first three allies in your collection.");
    }

    BattleActor actors[6] = {{0}};
    int actor_count = 0;

    for (int i = 0; i < party_count && i < MAX_PARTY; i++) {
        Character *c = &party[i]->character;
        snprintf(actors[actor_count].name, sizeof(actors[actor_count].name), "%s", c->name);
        actors[actor_count].max_hp = c->hp + party[i]->level * 25 + g->relic_bonus[i];
        actors[actor_count].hp = actors[actor_count].max_hp;
        actors[actor_count].attack = c->attack + g->relic_bonus[i] / 2;
        actors[actor_count].defense = c->defense + g->relic_bonus[i] / 3;
        actors[actor_count].speed = c->speed + party[i]->level * 5;
        actors[actor_count].is_enemy = 0;
        actors[actor_count].slot = i;
        actors[actor_count].guard = 0;
        actor_count++;
    }

    battle_enemy_team(actors, &actor_count);

    int order[6];
    int active = actor_count;
    for (int i = 0; i < active; i++) order[i] = i;
    for (int i = 1; i < active; i++) {
        int j = i;
        while (j > 0 && actors[order[j - 1]].speed < actors[order[j]].speed) {
            int tmp = order[j - 1];
            order[j - 1] = order[j];
            order[j] = tmp;
            j--;
        }
    }

    queue_init(&g->turns);
    for (int i = 0; i < active; i++) queue_enqueue(&g->turns, order[i]);

    puts("\n=== BATTLE: LITTLE ANIMALS VS. WILD FOREST CREATURES ===");
    puts("Use the queue to keep turns in speed order and the stack to log actions.");

    int player_alive = 0;
    int enemy_alive = 0;
    int rounds = 0;

    while (!queue_is_empty(&g->turns) && rounds < 30) {
        rounds++;
        int current = -1;
        if (!queue_dequeue(&g->turns, &current)) break;
        if (current < 0 || current >= actor_count) continue;
        if (actors[current].hp <= 0) continue;

        player_alive = 0;
        enemy_alive = 0;
        for (int i = 0; i < actor_count; i++) {
            if (actors[i].hp > 0 && !actors[i].is_enemy) player_alive = 1;
            if (actors[i].hp > 0 && actors[i].is_enemy) enemy_alive = 1;
        }

        if (!player_alive || !enemy_alive) break;

        if (!actors[current].is_enemy) {
            int target = -1;
            for (int i = 0; i < actor_count; i++) {
                if (actors[i].is_enemy && actors[i].hp > 0) {
                    target = i;
                    break;
                }
            }

            int choice = 0;
            printf("\n%s's turn. Choose action [1=Strike, 2=Guard, 3=Heal]: ", actors[current].name);
            if (scanf("%d", &choice) != 1) {
                choice = 1;
                int ch;
                while ((ch = getchar()) != '\n' && ch != EOF) { }
            }

            if (choice == 2) {
                actors[current].guard = 1;
                battle_log(g, "%s braces and raises a guard.", actors[current].name);
            } else if (choice == 3) {
                int heal = 35 + actors[current].attack / 4;
                actors[current].hp += heal;
                if (actors[current].hp > actors[current].max_hp) actors[current].hp = actors[current].max_hp;
                battle_log(g, "%s heals %d HP.", actors[current].name, heal);
            } else {
                if (target < 0) continue;
                int damage = actors[current].attack + (rand() % 18);
                int reduction = actors[target].defense / 2;
                int final = damage - reduction;
                if (final < 4) final = 4;
                actors[target].hp -= final;
                if (actors[target].hp < 0) actors[target].hp = 0;
                battle_log(g, "%s hits %s for %d damage.", actors[current].name, actors[target].name, final);
            }
        } else {
            int target = -1;
            for (int i = 0; i < actor_count; i++) {
                if (!actors[i].is_enemy && actors[i].hp > 0) {
                    target = i;
                    break;
                }
            }
            if (target < 0) continue;

            if (actors[current].guard) {
                actors[current].guard = 0;
            }

            int damage = actors[current].attack + rand() % 12;
            int reduction = actors[target].defense / 2;
            int final = damage - reduction;
            if (final < 3) final = 3;
            actors[target].hp -= final;
            if (actors[target].hp < 0) actors[target].hp = 0;
            battle_log(g, "%s attacks %s for %d damage.", actors[current].name, actors[target].name, final);
        }

        if (actors[current].guard) {
            actors[current].hp += 8;
            if (actors[current].hp > actors[current].max_hp) actors[current].hp = actors[current].max_hp;
        }

        for (int i = 0; i < actor_count; i++) {
            if (actors[i].hp > 0) {
                queue_enqueue(&g->turns, i);
            }
        }
    }

    player_alive = 0;
    enemy_alive = 0;
    for (int i = 0; i < actor_count; i++) {
        if (actors[i].hp > 0 && !actors[i].is_enemy) player_alive = 1;
        if (actors[i].hp > 0 && actors[i].is_enemy) enemy_alive = 1;
    }

    if (player_alive && !enemy_alive) {
        puts("\nVictory! Your squad wins the skirmish.");
        battle_log(g, "Victory! The animals return to camp with treasure and stories.");
    } else if (!player_alive && enemy_alive) {
        puts("\nDefeat. The critters win this round.");
        battle_log(g, "Defeat. The party retreats to regroup.");
    } else {
        puts("\nThe battle ends in a draw after a long, exhausting chase.");
        battle_log(g, "Draw. The forest settles back into silence.");
    }
}

void game_battle_demo(Game *g) {
    game_battle_play(g);
}

void game_show_relics(Game *g) {
    puts("\n=== RELIC TREE (BST) ===");
    relic_tree_print(g->relics, 0);

    char query[48];
    printf("Search relic name (or - to skip): ");
    if (scanf("%47s", query) != 1) {
        int ch;
        while ((ch = getchar()) != '\n' && ch != EOF) { }
        return;
    }

    if (strcmp(query, "-") == 0) {
        return;
    }

    if (!relic_tree_contains(g->relics, query)) {
        puts("Not found in the relic index.");
        return;
    }

    OwnedNode *party[MAX_PARTY] = {0};
    int party_count = 0;
    collect_party(g, party, &party_count);

    if (party_count <= 0) {
        puts("No party to equip a relic to yet.");
        return;
    }

    printf("Choose a squad member to equip this relic (1-%d): ", party_count);
    int choice = 0;
    if (scanf("%d", &choice) != 1) {
        int ch;
        while ((ch = getchar()) != '\n' && ch != EOF) { }
        return;
    }

    if (choice < 1 || choice > party_count) {
        puts("Invalid selection.");
        return;
    }

    int idx = choice - 1;
    snprintf(g->relic_slots[idx], sizeof(g->relic_slots[idx]), "%s", query);
    g->relic_bonus[idx] = relic_value_for_name(query);
    printf("%s equipped to %s. Bonus: +%d\n", query, party[idx]->character.name, g->relic_bonus[idx]);
}

void game_save(const Game *g, const char *path) {
    FILE *f = fopen(path, "w");
    if (!f) {
        perror("save");
        return;
    }
    for (OwnedNode *n = g->collection.head; n; n = n->next) {
        fprintf(f, "%s %d %d\n", n->character.id, n->copies, n->level);
    }
    fprintf(f, "meta %d %d %d\n", g->pity5, g->pity4, g->featured_guaranteed);
    fclose(f);
    puts("Collection saved to save.txt.");
}

void game_load_collection(Game *g, const char *path) {
    FILE *f = fopen(path, "r");
    if (!f) {
        return;
    }

    char id[ID_LEN];
    int copies, level;
    while (fscanf(f, "%31s %d %d", id, &copies, &level) == 3) {
        if (strcmp(id, "meta") == 0) {
            int pity5, pity4, featured;
            if (fscanf(f, "%d %d %d", &pity5, &pity4, &featured) == 3) {
                g->pity5 = pity5;
                g->pity4 = pity4;
                g->featured_guaranteed = featured;
            }
            continue;
        }

        Character *c = hash_table_find(&g->catalogue, id);
        if (c) {
            collection_add(&g->collection, c);
            for (OwnedNode *n = g->collection.head; n; n = n->next) {
                if (strcmp(n->character.id, id) == 0) {
                    n->copies = copies;
                    n->level = level;
                    break;
                }
            }
        }
    }
    fclose(f);
}
