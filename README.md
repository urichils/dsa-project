# The Little Animal Express — DSA Project

A cozy terminal RPG in C. Build it with `make` and start it with `./animal_gacha`.

## Build and play (macOS / Linux)

```sh
make
./animal_gacha
```

Or compile manually:

```sh
clang -std=c11 -Wall -Wextra -Wpedantic -Iinclude \
  main.c game.c character.c hash_table.c linked_list.c graph.c queue.c stack.c tree.c \
  -o animal_gacha
```

## The journey

- Meet the full roster, each with a small original ASCII portrait and role/stat card.
- Miso, Clover, and Bean join for free; Toast, Pebble, Luna, and Comet can join through the warp.
- Your collection is saved to `save.txt` when you save or leave.
- Relics can be searched and equipped for combat stat bonuses.
- Explore the character-synergy graph.
- Send your team down the forest path for automatic, speed-ordered battles. Each animal uses role-aware behavior: healers patch up hurt friends, Bean guards when in danger, and attackers focus wounded enemies. Health bars and a round-by-round battle log make the fight easy to follow.
- Gacha pity and featured guarantee status persist between runs.

## Data structures in the game

| Structure | Use |
|---|---|
| Hash table | Character catalogue lookup |
| Singly linked list | Owned collection and duplicate copies |
| Undirected graph + BFS | Character synergy network |
| Circular queue | Each round's speed-ordered battle turns |
| Stack | Recent battle story/history |
| Binary search tree | Sorted relic index |
| File I/O | Persist collection and banner pity |

All combat is automatic; choose the activity from the menu and watch the squad fight. The project uses only the standard C library.
