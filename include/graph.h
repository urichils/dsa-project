#ifndef GRAPH_H
#define GRAPH_H
#define GRAPH_MAX 32

typedef struct {
    int to;
    int weight;
} Edge;

typedef struct {
    char names[GRAPH_MAX][48];
    Edge edges[GRAPH_MAX][GRAPH_MAX];
    int degree[GRAPH_MAX];
    int count;
} SynergyGraph;

void graph_init(SynergyGraph *graph);
int graph_add_vertex(SynergyGraph *graph, const char *name);
void graph_add_edge(SynergyGraph *graph, const char *a, const char *b, int weight);
void graph_print(const SynergyGraph *graph);
void graph_bfs(const SynergyGraph *graph, const char *start);

#endif
