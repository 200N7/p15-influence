#ifndef GRAPH_H
#define GRAPH_H
struct Edge {
    int to;
    double p;
    struct Edge *next;
};
struct Graph {
    int n;
    char names[1000][32];
    struct Edge *head[1000];
};
void graph_init(struct Graph *g);
int graph_add_edge(struct Graph *g, const char *a, const char *b, double p);
int graph_load(struct Graph *g, const char *filename);
#endif