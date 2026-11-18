#include <stdio.h>
#include "graph.h"

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("usage: p15 <graph file>\n");
        return 1;
    }

    struct Graph g;
    graph_init(&g);

    int r = graph_load(&g, argv[1]);
    if (r != 0) {
        printf("load failed, code %d\n", r);
        return 1;
    }

    printf("users: %d\n", g.n);
    for (int i = 0; i < g.n; i++) {
        for (struct Edge *e = g.head[i]; e != NULL; e = e->next) {
            printf("%s -> %s  %.2f\n", g.names[i], g.names[e->to], e->p);
        }
    }

    return 0;
}