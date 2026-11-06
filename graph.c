#include <stdio.h>
#include <string.h>
#include "include/graph.h"

void graph_init(SynergyGraph *g) { memset(g, 0, sizeof(*g)); }
static int vertex(SynergyGraph *g, const char *name) {
    for (int i=0;i<g->count;i++) if (!strcmp(g->names[i],name)) return i;
    return -1;
}
int graph_add_vertex(SynergyGraph *g, const char *name) {
    int found=vertex(g,name); if(found>=0) return found;
    if(g->count>=GRAPH_MAX) return -1;
    snprintf(g->names[g->count],sizeof(g->names[g->count]),"%s",name);
    return g->count++;
}
void graph_add_edge(SynergyGraph *g,const char *a,const char *b,int w) {
    int x=graph_add_vertex(g,a), y=graph_add_vertex(g,b);
    if(x<0||y<0||x==y) return;
    if(g->degree[x]<GRAPH_MAX) g->edges[x][g->degree[x]++]=(Edge){y,w};
    if(g->degree[y]<GRAPH_MAX) g->edges[y][g->degree[y]++]=(Edge){x,w};
}
void graph_print(const SynergyGraph *g) {
    puts("\n=== SYNERGY NETWORK (ADJACENCY LIST) ===");
    for(int i=0;i<g->count;i++) {
        printf("%s:",g->names[i]);
        for(int j=0;j<g->degree[i];j++) printf(" -> %s (+%d)",g->names[g->edges[i][j].to],g->edges[i][j].weight);
        puts("");
    }
}
void graph_bfs(const SynergyGraph *g,const char *start) {
    int s=-1;
    for(int i=0;i<g->count;i++) if(!strcmp(g->names[i],start)) s=i;
    if(s<0){puts("No such character in the synergy graph.");return;}
    int seen[GRAPH_MAX]={0},q[GRAPH_MAX],f=0,r=0;
    q[r++]=s;seen[s]=1;
    printf("BFS from %s: ",g->names[s]);
    while(f<r){int v=q[f++];printf("%s ",g->names[v]);
        for(int j=0;j<g->degree[v];j++){int u=g->edges[v][j].to;if(!seen[u]){seen[u]=1;q[r++]=u;}}
    } puts("");
}
