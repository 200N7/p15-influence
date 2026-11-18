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

static int get_node(struct Graph *g, const char *name) {
    // 1. loop i from 0 up to g->n - 1
    //    if strcmp(g->names[i], name) == 0, return i
    for (int i = 0; i < g->n; i++) {
        if (strcmp(g->names[i], name) == 0) {
            return i;
        }
    }

    // 2. if g->n is 1000 or more, return -1 (no room)
    if (g->n >= 1000) {
        return -1;
    }
    // 3. copy name into g->names[g->n] using strncpy, copying at most 31 characters
    //    then set character number 31 of that slot to '\0'
    strncpy(g->names[g->n], name, 31);
    g->names[g->n][31] = '\0';
    // 4. set g->head[g->n] to NULL
    g->head[g->n] = NULL;
    // 5. return g->n, and then add 1 to g->n
    int id = g->n;
    g->n++;
    return id;
}