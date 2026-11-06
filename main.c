#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "include/game.h"

int main(void) {
    Game game;
    game_init(&game);
    game_load_collection(&game, "save.txt");
    srand((unsigned)time(NULL));

    int choice = 0;
    puts("========================================");
    puts("       THE LITTLE ANIMAL EXPRESS");
    puts("       a playable terminal RPG prototype");
    puts("========================================");

    do {
        puts("\n1. View character catalogue");
        puts("2. View my collection");
        puts("3. Warp x1");
        puts("4. Warp x10");
        puts("5. View character synergy graph + BFS");
        puts("6. Play battle");
        puts("7. Browse relic tree and equip");
        puts("8. Save collection");
        puts("9. Show battle history");
        puts("0. Exit");
        printf("> ");

        if (scanf("%d", &choice) != 1) {
            puts("Please enter a number.");
            int ch;
            while ((ch = getchar()) != '\n' && ch != EOF) { }
            choice = -1;
            continue;
        }

        switch (choice) {
            case 1:
                hash_table_print(&game.catalogue);
                break;
            case 2:
                collection_print(&game.collection);
                break;
            case 3:
                game_gacha(&game, 1);
                break;
            case 4:
                game_gacha(&game, 10);
                break;
            case 5:
                graph_print(&game.synergy);
                graph_bfs(&game.synergy, "Miso");
                break;
            case 6:
                game_battle_play(&game);
                break;
            case 7:
                game_show_relics(&game);
                break;
            case 8:
                game_save(&game, "save.txt");
                break;
            case 9:
                stack_print(&game.history);
                break;
            case 0:
                game_save(&game, "save.txt");
                puts("Bye! May your pulls be fluffy.");
                break;
            default:
                puts("Unknown option.");
                break;
        }
    } while (choice != 0);

    game_free(&game);
    return 0;
}
