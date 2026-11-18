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
        printf("4. Detect communities\n");
        printf("5. Plan high-value dissemination\n");
        printf("6. Audience capacity and bottleneck\n");
        printf("7. Exit\n");
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

            if (scanf("%31s", source) != 1) {
                printf("Invalid source user.\n");
                return 1;
            }

            printf("Enter target user: ");

            if (scanf("%31s", target) != 1) {
                printf("Invalid target user.\n");
                return 1;
            }

            printf("\n");

            most_likely_path(&g, source, target);
        }

        else if (choice == 3) {
            rank_influencers(&g);
        }

        else if (choice == 4) {
    int k;

    printf("\nCOMMUNITY ANALYSIS\n");
    printf("----------------------------------------\n");
    printf("Enter desired number of communities: ");

    if (scanf("%d", &k) != 1) {
        printf("Invalid input.\n");
        return 1;
    }

    detect_communities(&g, k);
}

       else if (choice == 5) {
    int budget;
    int costs[1000];

    printf("\nBUDGET-CONSTRAINED DISSEMINATION PLANNER\n");
    printf("----------------------------------------\n");

    printf("Enter total campaign budget: ");

    if (scanf("%d", &budget) != 1) {
        printf("Invalid budget input.\n");
        return 1;
    }

    if (budget < 0) {
        printf("Budget cannot be negative.\n");
        continue;
    }

    printf("\nEnter campaign cost for each user:\n");

    int valid_costs = 1;

    for (int i = 0; i < g.n; i++) {
        printf("%s: ", g.names[i]);

        if (scanf("%d", &costs[i]) != 1) {
            printf("Invalid cost input.\n");
            return 1;
        }

        if (costs[i] < 0) {
            printf("Cost cannot be negative.\n");
            valid_costs = 0;
            break;
        }
    }

    if (!valid_costs) {
        continue;
    }

    plan_dissemination(
        &g,
        budget,
        costs
    );
}

    else if (choice == 6) {
    char source[64];
    char target[64];

    printf("\nAUDIENCE CAPACITY AND BOTTLENECK\n");
    printf("----------------------------------------\n");

    printf("Enter source user: ");
    scanf("%63s", source);

    printf("Enter target user: ");
    scanf("%63s", target);

    analyze_audience_capacity(
        &g,
        source,
        target
    );
}
else if (choice == 7) {
    printf("\nExiting program.\n");
    break;
}

        else {
            printf("\nInvalid choice. Please select 1-7.\n");
        }

    } while (choice != 6);

    return 0;
}