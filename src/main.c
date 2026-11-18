#include <stdio.h>
#include "graph.h"
#include "influence.h"

static void display_network(const struct Graph *g) {
    printf("\nNETWORK CONNECTIONS\n");
    printf("----------------------------------------\n");

    for (int i = 0; i < g->n; i++) {
        for (struct Edge *e = g->head[i];
             e != NULL;
             e = e->next) {

            printf("%s -> %s  probability: %.2f\n",
                   g->names[i],
                   g->names[e->to],
                   e->p);
        }
    }

    printf("----------------------------------------\n");
}

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
    printf("Network loaded successfully.\n");
    printf("Users: %d\n", g.n);

    int choice;

    do {
        printf("\nMAIN MENU\n");
        printf("----------------------------------------\n");
        printf("1. Display network\n");
        printf("2. Most likely influence path\n");
        printf("3. Rank influential users\n");
        printf("4. Exit\n");
        printf("----------------------------------------\n");
        printf("Enter choice: ");

        if (scanf("%d", &choice) != 1) {
            printf("Invalid input.\n");
            return 1;
        }

        if (choice == 1) {
            display_network(&g);
        }

        else if (choice == 2) {
            char source[32];
            char target[32];

            printf("\nMOST LIKELY INFLUENCE PATH\n");
            printf("----------------------------------------\n");

            printf("Enter source user: ");
            scanf("%31s", source);

            printf("Enter target user: ");
            scanf("%31s", target);

            printf("\n");
            most_likely_path(&g, source, target);
        }

        else if (choice == 3) {
            rank_influencers(&g);
        }

        else if (choice == 4) {
            printf("\nExiting program.\n");
        }

        else {
            printf("\nInvalid choice. Please select 1-4.\n");
        }

    } while (choice != 4);

    return 0;
}