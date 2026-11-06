#ifndef TREE_H
#define TREE_H
typedef struct RelicNode {
    char name[48];
    struct RelicNode *left;
    struct RelicNode *right;
} RelicNode;

RelicNode *relic_tree_create_demo(void);
void relic_tree_print(const RelicNode *root, int depth);
int relic_tree_contains(const RelicNode *root, const char *name);
void relic_tree_free(RelicNode *root);

#endif
