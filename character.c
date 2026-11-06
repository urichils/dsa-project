#include <stdio.h>
#include <string.h>
#include "include/character.h"

void character_print_art(const Character *c) {
    if (!c) return;
    if (!strcmp(c->id, "miso")) {
        puts(" /\\_/\\\n( o.o )\n > ^ <");
    } else if (!strcmp(c->id, "clover")) {
        puts("  _   _\n ( \\_/ )\n  (o.o)\n  /|_|\\");
    } else if (!strcmp(c->id, "bean")) {
        puts("  .---.\n /     \\\n| (o o) |\n|  ___  |\n '-----'");
    } else if (!strcmp(c->id, "toast")) {
        puts(" .------.\n |      |\n | (oo) |\n |  __  |\n '------'");
    } else if (!strcmp(c->id, "pebble")) {
        puts("   __\n _(  )_\n(  o.o )\n '----'");
    } else if (!strcmp(c->id, "luna")) {
        puts("   /)  (\\\n  (  o.o )\n   / >*< \\");
    } else if (!strcmp(c->id, "comet")) {
        puts("  /\\_/\\   *\n ( ^.^ )  .\n  >-*-<  *");
    } else {
        puts("  /\\_/\\\n ( o.o )\n  > ^ <");
    }
}
