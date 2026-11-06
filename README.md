# The Little Animal Express — DSA Project

A terminal-only C gacha / collection / battle prototype. Standard C library only.

## Build (macOS / Linux)

```sh
make
./animal_gacha
```

Or compile manually:

```sh
clang -std=c11 -Wall -Wextra -Wpedantic -Iinclude \
  main.c game.c hash_table.c linked_list.c graph.c queue.c stack.c tree.c \
  -o animal_gacha
```

## Data structures demonstrated

| Structure / algorithm | Where it is used |
|---|---|
| Hash table with separate chaining | Character catalogue lookup by ID |
| Singly linked list | Player-owned character collection; duplicate copies update existing nodes |
| Undirected graph, adjacency lists | Character synergy network |
| Breadth-first search (BFS) | Explore characters connected to Miso |
| Circular queue | Rotate battle turns |
| Stack (LIFO) | Recent battle action history |
| Binary search tree | Relic name index; in-order traversal prints sorted names |
| Arrays | Character pools, graph adjacency storage, queue backing storage |
| File I/O | Save/load owned characters in `save.txt` |

## Current gameplay

- Miso, Clover, and Bean are free starters.
- Toast and Pebble are 4-star gacha characters.
- Luna and Comet are 5-star gacha characters.
- Pulls update the linked-list collection; duplicates increase copies and bond level.
- Collection is saved automatically on exit and can be saved from the menu.
- Practice battle demonstrates a circular turn queue and action-history stack.

## Important prototype assumptions

- Gacha rates are project-defined: 0.6% 5-star base chance, 6% 4-star chance, hard pity at 90 for 5-star and 10 for 4-star. A 5-star takes priority over a 4-star.
- The two 5-star characters are selected evenly from the small featured pool. This prototype does not yet model a separate off-banner/50-50 system; `featured_guaranteed` is reserved for a future banner implementation.
- The relic BST is a small demo index, not yet attached to character equipment.
- Battle is a data-structure demonstration, not a full combat simulator.
- Catalogue and player collection are intentionally separate: all seven characters are searchable in the catalogue, but only the three starters are initially owned.

## Suggested next upgrades

1. Add an actual featured-banner guarantee / banner selection.
2. Equip relics to owned characters and calculate stat bonuses.
3. Add a real battle loop with speed-based turn order.
4. Add collection sorting and filtering.
5. Add stronger save-file validation and persistence of gacha pity.
