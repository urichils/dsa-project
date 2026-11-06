#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "include/game.h"

int main(void) {
    Game game;
    game_init(&game);
    game_load_collection(&game, "save.txt");
    srand((unsigned)time(NULL));

    puts("========================================");
    puts("       THE LITTLE ANIMAL EXPRESS");
    puts("       a cozy terminal RPG");
    puts("========================================");
    int choice = -1;
    while (choice != 0) {
        puts("\nWhat would you like to do?");
        puts("1. Meet the animal roster");
        puts("2. Visit my friends");
        puts("3. Warp x1");
        puts("4. Warp x10");
        puts("5. Explore character synergies");
        puts("6. Send my team into an automatic battle");
        puts("7. Browse relics and equip one");
        puts("8. View the latest battle story");
        puts("9. Save game");
        puts("0. Leave the train");
        printf("> ");
        if (scanf("%d", &choice) != 1) {
            if (feof(stdin)) { choice = 0; break; }
            puts("Please enter a menu number.");
            int ch; while ((ch = getchar()) != '\n' && ch != EOF) { }
            continue;
        }
        switch (choice) {
            case 1: hash_table_print(&game.catalogue); break;
            case 2: collection_print(&game.collection); break;
            case 3: game_gacha(&game, 1); break;
            case 4: game_gacha(&game, 10); break;
            case 5: graph_print(&game.synergy); graph_bfs(&game.synergy, "Miso"); break;
            case 6: game_battle_play(&game); break;
            case 7: game_show_relics(&game); break;
            case 8: stack_print(&game.history); break;
            case 9: game_save(&game, "save.txt"); break;
            case 0: puts("Saving your journey..."); game_save(&game, "save.txt"); break;
            default: puts("That option is not on the route map."); break;
        }
    }
    puts("Thanks for riding. See you next time!");
    game_free(&game);
    return 0;
}
