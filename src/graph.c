#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "graph.h"

void graph_init(struct Graph *g) {
    g->n = 0;
    for (int i = 0; i < 1000; i++) {
        g->head[i] = NULL;
    }
}