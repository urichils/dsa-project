#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "include/tree.h"

static RelicNode *node(const char *name){
    RelicNode *n=calloc(1,sizeof(*n));if(n)snprintf(n->name,sizeof(n->name),"%s",name);return n;
}
/* A small binary search tree: relic names are ordered lexicographically. */
static RelicNode *insert(RelicNode *r,const char *name){
    if(!r)return node(name);
    int c=strcmp(name,r->name);
    if(c<0)r->left=insert(r->left,name);else if(c>0)r->right=insert(r->right,name);
    return r;
}
RelicNode *relic_tree_create_demo(void){
    const char *names[]={"Mossy Crown","Moon Bell","Cloud Scarf","Star Pendant","Acorn Charm","Comet Boots"};
    RelicNode *r=NULL;for(int i=0;i<6;i++)r=insert(r,names[i]);return r;
}
void relic_tree_print(const RelicNode *r,int d){
    if(!r)return;
    relic_tree_print(r->left,d+1);
    for(int i=0;i<d;i++)printf("  ");
    printf("- %s\n",r->name);
    relic_tree_print(r->right,d+1);
}
int relic_tree_contains(const RelicNode *r,const char *name){
    while(r){int c=strcmp(name,r->name);if(!c)return 1;r=c<0?r->left:r->right;}return 0;
}
void relic_tree_free(RelicNode *r){if(r){relic_tree_free(r->left);relic_tree_free(r->right);free(r);}}
