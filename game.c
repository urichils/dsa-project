#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "include/game.h"

static Character make(const char *id,const char *name,int rarity,int hp,int atk,int def,int spd,const char *role){
    Character c={0};snprintf(c.id,sizeof(c.id),"%s",id);snprintf(c.name,sizeof(c.name),"%s",name);
    c.rarity=rarity;c.hp=hp;c.attack=atk;c.defense=def;c.speed=spd;snprintf(c.role,sizeof(c.role),"%s",role);return c;
}
void game_init(Game *g){
    hash_table_init(&g->catalogue);collection_init(&g->collection);graph_init(&g->synergy);
    queue_init(&g->turns);stack_init(&g->history);g->relics=relic_tree_create_demo();
    g->pity5=0;g->pity4=0;g->featured_guaranteed=0;
    Character c[]={
      {"miso","Miso",4,900,120,80,110,"Support"},
      {"clover","Clover",4,1100,90,100,100,"Healer"},
      {"bean","Bean",4,1500,100,150,75,"Tank"},
      {"toast","Toast",4,850,170,65,120,"Attacker"},
      {"pebble","Pebble",4,1000,110,90,95,"Debuffer"},
      {"luna","Luna",5,1200,220,100,115,"Support"},
      {"comet","Comet",5,1400,260,130,90,"Attacker"}
    };
    for(size_t i=0;i<sizeof(c)/sizeof(c[0]);i++)hash_table_insert(&g->catalogue,&c[i]);
    /* Three free starters; all other catalogue entries are gacha-only. */
    Character *starter=hash_table_find(&g->catalogue,"miso");collection_add(&g->collection,starter);
    starter=hash_table_find(&g->catalogue,"clover");collection_add(&g->collection,starter);
    starter=hash_table_find(&g->catalogue,"bean");collection_add(&g->collection,starter);
    graph_add_edge(&g->synergy,"Miso","Clover",15);
    graph_add_edge(&g->synergy,"Clover","Bean",10);
    graph_add_edge(&g->synergy,"Toast","Comet",20);
    graph_add_edge(&g->synergy,"Pebble","Luna",15);
    graph_add_edge(&g->synergy,"Miso","Luna",25);
}
void game_gacha(Game *g,int pulls){
    const char *pool4[]={"toast","pebble"};
    const char *pool5[]={"luna","comet"};
    for(int i=0;i<pulls;i++){
        int roll=rand()%10000; const char *id;
        if(g->pity5>=89 || roll<60){
            id=pool5[rand()%2];g->pity5=0;g->pity4=0;g->featured_guaranteed=0;
        }else if(g->pity4>=9 || roll<600){
            id=pool4[rand()%2];g->pity4=0;g->pity5++;
        }else{g->pity4++;g->pity5++;continue;}
        Character *ch=hash_table_find(&g->catalogue,id);
        collection_add(&g->collection,ch);
        printf("  ★%d  %s joined your squad!\n",ch->rarity,ch->name);
    }
    printf("Pity status: 5-star %d/90 | 4-star %d/10\n",g->pity5,g->pity4);
}
void game_battle_demo(Game *g){
    int ids[]={0,1,2}; const char *names[]={"Miso","Clover","Bean"};
    queue_init(&g->turns);
    for(int i=0;i<3;i++)queue_enqueue(&g->turns,ids[i]);
    puts("\n=== PRACTICE BATTLE: TURN QUEUE ===");
    for(int turn=0;turn<6;turn++){
        int who;queue_dequeue(&g->turns,&who);
        char action[96];snprintf(action,sizeof(action),"%s takes turn %d",names[who],turn+1);
        puts(action);stack_push(&g->history,action);queue_enqueue(&g->turns,who);
    }
    stack_print(&g->history);
}
void game_show_relics(Game *g){
    puts("\n=== RELIC INDEX (BINARY SEARCH TREE, IN-ORDER) ===");
    relic_tree_print(g->relics,0);
    char query[48];printf("Search relic name (or - to skip): ");
    if(scanf("%47s",query)==1 && strcmp(query,"-") && !relic_tree_contains(g->relics,query))
        puts("Not found (search is exact; names with spaces need a future input improvement).");
}
void game_save(const Game *g,const char *path){
    FILE *f=fopen(path,"w");if(!f){perror("save");return;}
    for(OwnedNode *n=g->collection.head;n;n=n->next)
        fprintf(f,"%s %d %d\n",n->character.id,n->copies,n->level);
    fclose(f);puts("Collection saved to save.txt.");
}
void game_load_collection(Game *g,const char *path){
    FILE *f=fopen(path,"r");if(!f)return;
    char id[ID_LEN];int copies,level;
    while(fscanf(f,"%31s %d %d",id,&copies,&level)==3){
        Character *c=hash_table_find(&g->catalogue,id);
        if(c){collection_add(&g->collection,c);
            for(OwnedNode *n=g->collection.head;n;n=n->next)if(!strcmp(n->character.id,id)){n->copies=copies;n->level=level;break;}}
    }
    fclose(f);
}
void game_free(Game *g){
    hash_table_free(&g->catalogue);collection_free(&g->collection);relic_tree_free(g->relics);g->relics=NULL;
}
