#ifndef CHARACTER_H
#define CHARACTER_H

#define ID_LEN 32
#define NAME_LEN 48
#define ROLE_LEN 24

typedef struct {
    char id[ID_LEN];
    char name[NAME_LEN];
    int rarity;
    int hp;
    int attack;
    int defense;
    int speed;
    char role[ROLE_LEN];
} Character;

void character_print_art(const Character *character);

#endif
