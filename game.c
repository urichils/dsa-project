#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "include/game.h"

typedef struct {
    char name[NAME_LEN];
    int hp, max_hp, attack, defense, speed;
    int is_enemy;
    int party_index;
    int guarding;
} BattleActor;

static Character make(const char *id, const char *name, int rarity, int hp, int atk, int def, int spd, const char *role) {
    Character c = {0};
    snprintf(c.id, sizeof(c.id), "%s", id);
    snprintf(c.name, sizeof(c.name), "%s", name);
    c.rarity = rarity; c.hp = hp; c.attack = atk; c.defense = def; c.speed = spd;
    snprintf(c.role, sizeof(c.role), "%s", role);
    return c;
}

static void battle_log(Game *g, const char *fmt, ...) {
    char text[160];
    va_list args;
    va_start(args, fmt);
    vsnprintf(text, sizeof(text), fmt, args);
    va_end(args);
    stack_push(&g->history, text);
    printf("  %s\n", text);
}

static int relic_value_for_name(const char *name) {
    if (strstr(name, "Crown")) return 14;
    if (strstr(name, "Bell") || strstr(name, "Pendant")) return 12;
    if (strstr(name, "Scarf") || strstr(name, "Charm")) return 10;
    if (strstr(name, "Boots")) return 9;
    return 8;
}

static OwnedNode *find_owned(Game *g, const char *id) {
    for (OwnedNode *n = g->collection.head; n; n = n->next)
        if (strcmp(n->character.id, id) == 0) return n;
    return NULL;
}

static int add_starter_if_missing(Game *g, const char *id) {
    if (find_owned(g, id)) return 1;
    Character *c = hash_table_find(&g->catalogue, id);
    return c && collection_add(&g->collection, c) >= 0;
}

static void collect_party(Game *g, OwnedNode *party[MAX_PARTY], int *count) {
    static const char *preferred[] = {"miso", "clover", "bean"};
    *count = 0;
    for (int i = 0; i < MAX_PARTY; i++) {
        add_starter_if_missing(g, preferred[i]);
        OwnedNode *n = find_owned(g, preferred[i]);
        if (n) party[(*count)++] = n;
    }
    for (OwnedNode *n = g->collection.head; n && *count < MAX_PARTY; n = n->next) {
        int exists = 0;
        for (int i = 0; i < *count; i++) if (party[i] == n) exists = 1;
        if (!exists) party[(*count)++] = n;
    }
}

void game_init(Game *g) {
    hash_table_init(&g->catalogue); collection_init(&g->collection); graph_init(&g->synergy);
    queue_init(&g->turns); stack_init(&g->history); g->relics = relic_tree_create_demo();
    g->pity5 = 0; g->pity4 = 0; g->featured_guaranteed = 0;
    for (int i = 0; i < MAX_PARTY; i++) { g->relic_slots[i][0] = '\0'; g->relic_bonus[i] = 0; }
    Character c[] = {
        make("miso", "Miso", 4, 900, 120, 80, 110, "Support"),
        make("clover", "Clover", 4, 1100, 90, 100, 100, "Healer"),
        make("bean", "Bean", 4, 1500, 100, 150, 75, "Tank"),
        make("toast", "Toast", 4, 850, 170, 65, 120, "Attacker"),
        make("pebble", "Pebble", 4, 1000, 110, 90, 95, "Debuffer"),
        make("luna", "Luna", 5, 1200, 220, 100, 115, "Support"),
        make("comet", "Comet", 5, 1400, 260, 130, 90, "Attacker")
    };
    for (size_t i = 0; i < sizeof(c) / sizeof(c[0]); i++) hash_table_insert(&g->catalogue, &c[i]);
    collection_add(&g->collection, hash_table_find(&g->catalogue, "miso"));
    collection_add(&g->collection, hash_table_find(&g->catalogue, "clover"));
    collection_add(&g->collection, hash_table_find(&g->catalogue, "bean"));
    graph_add_edge(&g->synergy, "Miso", "Clover", 15);
    graph_add_edge(&g->synergy, "Clover", "Bean", 10);
    graph_add_edge(&g->synergy, "Toast", "Comet", 20);
    graph_add_edge(&g->synergy, "Pebble", "Luna", 15);
    graph_add_edge(&g->synergy, "Miso", "Luna", 25);
}

void game_free(Game *g) {
    hash_table_free(&g->catalogue); collection_free(&g->collection);
    relic_tree_free(g->relics); g->relics = NULL;
}

void game_gacha(Game *g, int pulls) {
    const char *pool4[] = {"toast", "pebble"};
    const char *pool5[] = {"luna", "comet"};
    for (int i = 0; i < pulls; i++) {
        int roll = rand() % 10000;
        const char *id = NULL;
        if (g->pity5 >= 89 || roll < 60) {
            int five_star = g->featured_guaranteed || g->pity5 >= 89 || (rand() % 2 == 0);
            id = pool5[rand() % 2];
            if (!five_star) g->featured_guaranteed = 1;
            else g->featured_guaranteed = 0;
            g->pity5 = 0; g->pity4 = 0;
        } else if (g->pity4 >= 9 || roll < 600) {
            id = pool4[rand() % 2]; g->pity4 = 0; g->pity5++;
        } else {
            g->pity4++; g->pity5++;
            printf("  No new friend. Pity: 5★ %d/90, 4★ %d/10\n", g->pity5, g->pity4);
            continue;
        }
        Character *c = hash_table_find(&g->catalogue, id);
        if (c) {
            collection_add(&g->collection, c);
            printf("  ★%d %s joined your squad!\n", c->rarity, c->name);
            character_print_art(c);
        }
    }
    printf("Pity: 5★ %d/90 | 4★ %d/10 | featured guarantee: %s\n",
           g->pity5, g->pity4, g->featured_guaranteed ? "next 5-star guaranteed" : "not active");
}

static void show_battlefield(const BattleActor *actors, int count, int round) {
    printf("\n--- ROUND %d ---\n", round);
    puts("YOUR TEAM");
    for (int i = 0; i < count; i++) if (!actors[i].is_enemy) {
        printf(" %-9s ", actors[i].name);
        int bars = actors[i].max_hp > 0 ? actors[i].hp * 16 / actors[i].max_hp : 0;
        if (bars < 0) bars = 0;
        printf("[");
        for (int b = 0; b < 16; b++) putchar(b < bars ? '#' : '-');
        printf("] %d/%d%s\n", actors[i].hp, actors[i].max_hp, actors[i].hp ? "" : " KO");
    }
    puts("WILD TEAM");
    for (int i = 0; i < count; i++) if (actors[i].is_enemy) {
        printf(" %-12s ", actors[i].name);
        int bars = actors[i].max_hp > 0 ? actors[i].hp * 16 / actors[i].max_hp : 0;
        if (bars < 0) bars = 0;
        printf("[");
        for (int b = 0; b < 16; b++) putchar(b < bars ? '#' : '-');
        printf("] %d/%d%s\n", actors[i].hp, actors[i].max_hp, actors[i].hp ? "" : " KO");
    }
}

static int lowest_hp_actor(const BattleActor *actors, int count, int enemy_side, int missing_only) {
    int best = -1;
    for (int i = 0; i < count; i++) {
        if (actors[i].hp <= 0 || actors[i].is_enemy != enemy_side) continue;
        if (missing_only && actors[i].hp == actors[i].max_hp) continue;
        if (best < 0 || actors[i].hp < actors[best].hp) best = i;
    }
    return best;
}

static void take_auto_turn(Game *g, BattleActor *actors, int count, int current) {
    BattleActor *actor = &actors[current];
    if (actor->is_enemy) {
        int target = lowest_hp_actor(actors, count, 0, 0);
        if (target < 0) return;
        int damage = actor->attack + rand() % 10 - actors[target].defense / 3;
        if (actors[target].guarding) { damage /= 2; actors[target].guarding = 0; }
        if (damage < 3) damage = 3;
        actors[target].hp -= damage;
        if (actors[target].hp < 0) actors[target].hp = 0;
        battle_log(g, "%s lunges at %s for %d damage!", actor->name, actors[target].name, damage);
        return;
    }

    int heal_target = lowest_hp_actor(actors, count, 0, 1);
    int enemy = lowest_hp_actor(actors, count, 1, 0);
    int threshold = actor->max_hp * 2 / 3;
    if ((!strcmp(actor->name, "Clover") || !strcmp(actor->name, "Luna")) && heal_target >= 0 && actors[heal_target].hp < threshold) {
        int heal = !strcmp(actor->name, "Luna") ? 100 : 75;
        actors[heal_target].hp += heal;
        if (actors[heal_target].hp > actors[heal_target].max_hp) actors[heal_target].hp = actors[heal_target].max_hp;
        battle_log(g, "%s uses a soothing skill: %s recovers HP!", actor->name, actors[heal_target].name);
    } else if (!strcmp(actor->name, "Bean") && actor->hp < actor->max_hp / 2 && !actor->guarding) {
        actor->guarding = 1;
        battle_log(g, "%s plants his feet and guards against the next hit!", actor->name);
    } else if (enemy >= 0) {
        int damage = actor->attack + rand() % 16 - actors[enemy].defense / 3;
        if (!strcmp(actor->name, "Toast") || !strcmp(actor->name, "Comet")) damage += 15;
        if (damage < 4) damage = 4;
        actors[enemy].hp -= damage;
        if (actors[enemy].hp < 0) actors[enemy].hp = 0;
        battle_log(g, "%s attacks %s for %d damage!", actor->name, actors[enemy].name, damage);
    }
}

void game_battle_play(Game *g) {
    OwnedNode *party[MAX_PARTY] = {0};
    int party_count = 0;
    collect_party(g, party, &party_count);
    if (!party_count) { puts("Your squad is empty."); return; }

    BattleActor actors[MAX_PARTY + MAX_ENEMIES] = {{0}};
    int count = 0;
    puts("\n=== FOREST PATH: AUTOMATIC BATTLE ===");
    puts("Your animals choose actions using their roles. Faster friends act first.");
    for (int i = 0; i < party_count; i++) {
        Character *c = &party[i]->character;
        BattleActor *a = &actors[count++];
        snprintf(a->name, sizeof(a->name), "%s", c->name);
        a->max_hp = c->hp + party[i]->level * 25 + g->relic_bonus[i];
        a->hp = a->max_hp;
        a->attack = c->attack + g->relic_bonus[i] / 2;
        a->defense = c->defense + g->relic_bonus[i] / 3;
        a->speed = c->speed + party[i]->level * 5;
        a->party_index = i;
        if (i == 0) character_print_art(c);
    }
    static const char *enemy_names[] = {"Moss Rat", "Leafling", "Stump Beetle"};
    for (int i = 0; i < MAX_ENEMIES; i++) {
        BattleActor *a = &actors[count++];
        snprintf(a->name, sizeof(a->name), "%s", enemy_names[i]);
        a->hp = a->max_hp = 180 + i * 55;
        a->attack = 30 + i * 7; a->defense = 12 + i * 4; a->speed = 95 + i * 8;
        a->is_enemy = 1;
    }

    stack_init(&g->history);
    int round = 0, players_alive, enemies_alive;
    do {
        round++;
        int order[MAX_PARTY + MAX_ENEMIES], norder = 0;
        for (int i = 0; i < count; i++) if (actors[i].hp > 0) order[norder++] = i;
        for (int i = 1; i < norder; i++) {
            int j = i;
            while (j > 0 && actors[order[j - 1]].speed < actors[order[j]].speed) {
                int temp = order[j - 1]; order[j - 1] = order[j]; order[j] = temp; j--;
            }
        }
        queue_init(&g->turns);
        for (int i = 0; i < norder; i++) queue_enqueue(&g->turns, order[i]);
        show_battlefield(actors, count, round);
        int current;
        while (queue_dequeue(&g->turns, &current)) {
            if (actors[current].hp <= 0) continue;
            take_auto_turn(g, actors, count, current);
            players_alive = enemies_alive = 0;
            for (int i = 0; i < count; i++) if (actors[i].hp > 0) {
                if (actors[i].is_enemy) enemies_alive++; else players_alive++;
            }
            if (!players_alive || !enemies_alive) break;
        }
        players_alive = enemies_alive = 0;
        for (int i = 0; i < count; i++) if (actors[i].hp > 0) {
            if (actors[i].is_enemy) enemies_alive++; else players_alive++;
        }
    } while (players_alive && enemies_alive && round < 40);

    show_battlefield(actors, count, round);
    if (players_alive && !enemies_alive) {
        puts("\nVICTORY! The forest path is clear. Your little guys celebrate!");
        battle_log(g, "Victory! The squad clears the forest path.");
    } else if (!players_alive) {
        puts("\nThe wild creatures win this time. Your squad will be ready for another try.");
        battle_log(g, "Defeat. The squad retreats to camp.");
    } else {
        puts("\nThe battle ended in a draw.");
        battle_log(g, "Draw. Both sides retreat.");
    }
}

void game_battle_demo(Game *g) { game_battle_play(g); }

void game_show_relics(Game *g) {
    puts("\n=== RELIC TREE — FIND ONE TO EQUIP ===");
    relic_tree_print(g->relics, 0);
    puts("Enter the exact relic name with underscores instead of spaces, or - to cancel.");
    char query[48];
    printf("> ");
    if (scanf("%47s", query) != 1) return;
    for (int i = 0; query[i]; i++) if (query[i] == '_') query[i] = ' ';
    if (!strcmp(query, "-")) return;
    if (!relic_tree_contains(g->relics, query)) { puts("That relic is not in the index."); return; }
    OwnedNode *party[MAX_PARTY] = {0};
    int count = 0; collect_party(g, party, &count);
    puts("Choose an animal:");
    for (int i = 0; i < count; i++) printf(" %d) %s\n", i + 1, party[i]->character.name);
    int choice;
    if (scanf("%d", &choice) != 1 || choice < 1 || choice > count) { puts("Cancelled: invalid selection."); return; }
    int slot = choice - 1;
    snprintf(g->relic_slots[slot], sizeof(g->relic_slots[slot]), "%s", query);
    g->relic_bonus[slot] = relic_value_for_name(query);
    printf("%s equipped to %s (+%d to battle stats).\n", query, party[slot]->character.name, g->relic_bonus[slot]);
}

void game_save(const Game *g, const char *path) {
    FILE *f = fopen(path, "w");
    if (!f) { perror("save"); return; }
    for (OwnedNode *n = g->collection.head; n; n = n->next)
        fprintf(f, "%s %d %d\n", n->character.id, n->copies, n->level);
    fprintf(f, "meta %d %d %d\n", g->pity5, g->pity4, g->featured_guaranteed);
    fclose(f);
    printf("Game saved to %s.\n", path);
}

void game_load_collection(Game *g, const char *path) {
    FILE *f = fopen(path, "r");
    if (!f) return;
    char id[ID_LEN]; int copies, level;
    while (fscanf(f, "%31s %d %d", id, &copies, &level) == 3) {
        if (!strcmp(id, "meta")) {
            int p5, p4, guarantee;
            if (fscanf(f, "%d %d %d", &p5, &p4, &guarantee) == 3) {
                if (p5 >= 0 && p5 < 90) g->pity5 = p5;
                if (p4 >= 0 && p4 < 10) g->pity4 = p4;
                g->featured_guaranteed = guarantee != 0;
            }
            continue;
        }
        Character *c = hash_table_find(&g->catalogue, id);
        if (!c || copies < 1 || level < 1 || level > 6) continue;
        if (collection_add(&g->collection, c) < 0) continue;
        OwnedNode *n = find_owned(g, id);
        if (n) { n->copies = copies; n->level = level; }
    }
    fclose(f);
}
