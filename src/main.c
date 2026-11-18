#include <stdio.h>
#include "graph.h"
#include "influence.h"

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

    printf("\n");
    printf("========================================\n");
    printf(" SOCIAL NETWORK INFLUENCE ANALYSIS TOOL\n");
    printf("========================================\n");

    printf("\nNetwork loaded successfully.\n");
    printf("Users: %d\n\n", g.n);

    printf("Network connections:\n");
    printf("----------------------------------------\n");

    for (int i = 0; i < g.n; i++) {
        for (struct Edge *e = g.head[i];
             e != NULL;
             e = e->next) {

            printf("%s -> %s  probability: %.2f\n",
                   g.names[i],
                   g.names[e->to],
                   e->p);
        }
    }

    printf("----------------------------------------\n");

    char source[32];
    char target[32];

    printf("\nMOST LIKELY INFLUENCE PATH\n");
    printf("----------------------------------------\n");

    printf("Enter source user: ");

    if (scanf("%31s", source) != 1) {
        printf("invalid source\n");
        return 1;
    }

    printf("Enter target user: ");

    if (scanf("%31s", target) != 1) {
        printf("invalid target\n");
        return 1;
    }

    printf("\n");

    most_likely_path(&g, source, target);

    printf("\n");

    return 0;
}