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

int graph_add_edge(struct Graph *g, const char *a, const char *b, double p) {
    // 1. if p is 0 or less, or more than 1, return 1
    if (p <= 0 || p > 1) {
        return 1;
    }
    // 2. if a and b are the same name (strcmp equals 0), return 2
    if (strcmp(a, b) == 0) {
        return 2;
    }
    // 3. int u = get_node for a, and int v = get_node for b
    //    if either is -1, return 3
    int u = get_node(g, a);
    int v = get_node(g, b);
    if (u == -1 || v == -1) {
        return 3;
    }
    // 4. make a new edge: struct Edge *e = malloc(sizeof(struct Edge));
    //    if e is NULL, return 4
    struct Edge *e = malloc(sizeof(struct Edge));
    if (e == NULL) {
        return 4;
    }
    // 5. e->to = v;  e->p = p;  e->next = g->head[u];
    e->to = v;
    e->p = p;
    e->next = g->head[u];
    // 6. g->head[u] = e;
    g->head[u] = e;
    // 7. return 0
    return 0;
}
int graph_load(struct Graph *g, const char *filename) {
    // 1. open the file: FILE *f = fopen(filename, "r");
    //    if f is NULL, return 1
    FILE *f = fopen(filename, "r");
    if (f == NULL) {
        return 1;
    }

    // 2. declare: char line[256];
    char line[256];

    // 3. loop: while (fgets(line, sizeof(line), f) != NULL) {
    //      a. if line[0] is '#', continue
    //      b. declare char a[32], b[32], extra[32]; double p;
    //      c. int got = sscanf(line, "%31s %31s %lf %31s", a, b, &p, extra);
    //      d. if got is EOF (a blank line), continue
    //      e. if got is not 3, fclose(f) and return 2
    //      f. int r = graph_add_edge(g, a, b, p);
    //         if r is not 0, fclose(f) and return 3
    //    }
    while (fgets(line, sizeof(line), f) != NULL) {
        if (line[0] == '#') {
            continue;
        }
        char a[32], b[32], extra[32];
        double p;
        int got = sscanf(line, "%31s %31s %lf %31s", a, b, &p, extra);
        if (got == EOF) {
            continue;
        }
        if (got != 3) {
            fclose(f);
            return 2;
        }
        int r = graph_add_edge(g, a, b, p);
        if (r != 0) {
            fclose(f);
            return 3;
        }
    }

    // 4. fclose(f); return 0
    fclose(f);
    return 0;
}