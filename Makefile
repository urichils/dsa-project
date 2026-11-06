CC = gcc
CFLAGS = -std=c11 -Wall -Wextra -Wpedantic -O2
SOURCES = main.c game.c hash_table.c linked_list.c graph.c queue.c stack.c tree.c
TARGET = animal_gacha

all: $(TARGET)

$(TARGET): $(SOURCES)
	$(CC) $(CFLAGS) -Iinclude $(SOURCES) -o $(TARGET)

clean:
	rm -f $(TARGET) save.txt

.PHONY: all clean
