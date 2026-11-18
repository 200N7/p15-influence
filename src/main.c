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

    // 4 and 5 come next

    return 0;
}