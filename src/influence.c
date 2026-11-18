#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "influence.h"
#include "algorithms/greedy.h"

/*
 * Find the internal index of a user by name.
 * Returns -1 if the user does not exist.
 */
static int find_user(const struct Graph *g, const char *name) {
    for (int i = 0; i < g->n; i++) {
        if (strcmp(g->names[i], name) == 0) {
            return i;
        }
    }

    return -1;
}

/*
 * Find the path with the maximum product of
 * influence probabilities.
 *
 * This uses a Dijkstra-style greedy approach.
 */
int most_likely_path(
    const struct Graph *g,
    const char *source,
    const char *target
) {
    if (g == NULL ||
        source == NULL ||
        target == NULL) {

        printf("invalid path request\n");
        return 1;
    }

    int src = find_user(g, source);
    int dst = find_user(g, target);

    if (src == -1 || dst == -1) {
        printf("user not found\n");
        return 1;
    }
    if (src == dst) {
    printf("Most likely influence path: %s\n",
           g->names[src]);
    printf("Influence probability: 1.0000\n");
    printf("Dijkstra edge relaxations: 0\n");
    printf("Model: source and target are identical\n");
    return 0;
}

    /*
     * Count directed influence edges.
     */
    int edge_count = 0;

    for (int u = 0; u < g->n; u++) {
        for (struct Edge *edge = g->head[u];
             edge != NULL;
             edge = edge->next) {

            edge_count++;
        }
    }

    struct WeightedEdge *edges = NULL;

    if (edge_count > 0) {
        edges = malloc(
            (size_t)edge_count *
            sizeof(*edges)
        );

        if (edges == NULL) {
            printf("memory allocation failed\n");
            return 1;
        }
    }

    /*
     * Transform propagation probabilities into
     * non-negative shortest-path weights:
     *
     * weight = -log(p)
     *
     * Minimizing the sum of these weights is
     * equivalent to maximizing the product of p.
     */
    int index = 0;

    for (int u = 0; u < g->n; u++) {
        for (struct Edge *edge = g->head[u];
             edge != NULL;
             edge = edge->next) {

            if (edge->p <= 0.0 ||
                edge->p > 1.0) {

                free(edges);
                printf(
                    "invalid probability on edge %s -> %s\n",
                    g->names[u],
                    g->names[edge->to]
                );

                return 1;
            }

            edges[index].from = u;
            edges[index].to = edge->to;
            edges[index].weight =
                -log(edge->p);

            index++;
        }
    }

    double distance[1000];
    int previous[1000];
    long long relaxations = 0;

    int result = dijkstra_heap(
        g->n,
        edges,
        edge_count,
        src,
        distance,
        previous,
        &relaxations
    );

    free(edges);

    if (result != 1) {
        printf("influence path analysis failed\n");
        return 1;
    }

    int path[1000];

    int path_length = reconstruct_path(
        src,
        dst,
        previous,
        g->n,
        path
    );

    if (path_length == 0) {
        printf(
            "no influence path from %s to %s\n",
            source,
            target
        );

        return 2;
    }

    /*
     * Convert shortest transformed distance back
     * to propagation probability.
     *
     * exp(-sum(-log p)) = product(p)
     */
    double probability =
        exp(-distance[dst]);

    printf("Most likely influence path: ");

    for (int i = 0; i < path_length; i++) {
        printf("%s", g->names[path[i]]);

        if (i + 1 < path_length) {
            printf(" -> ");
        }
    }

    printf("\n");
    printf(
        "Influence probability: %.4f\n",
        probability
    );

    printf(
        "Dijkstra edge relaxations: %lld\n",
        relaxations
    );

    printf(
        "Model: edge cost = -log(probability)\n"
    );

    return 0;
}
/*
 * Rank users according to their total
 * direct outgoing influence.
 */
void rank_influencers(const struct Graph *g) {
    double score[1000];
    int order[1000];

    /*
     * Calculate direct influence score.
     */
    for (int i = 0; i < g->n; i++) {
        score[i] = 0.0;
        order[i] = i;

        for (struct Edge *edge = g->head[i];
             edge != NULL;
             edge = edge->next) {

            score[i] += edge->p;
        }
    }

    /*
     * Selection sort in descending order
     * of influence score.
     */
    for (int i = 0; i < g->n - 1; i++) {
        int best = i;

        for (int j = i + 1; j < g->n; j++) {
            if (score[order[j]] >
                score[order[best]]) {

                best = j;
            }
        }

        if (best != i) {
            int temp = order[i];
            order[i] = order[best];
            order[best] = temp;
        }
    }

    printf("\nINFLUENCE RANKING\n");
    printf("----------------------------------------\n");
    printf("Rank  User                 Score\n");
    printf("----------------------------------------\n");

    for (int i = 0; i < g->n; i++) {
        printf("%-5d %-20s %.4f\n",
               i + 1,
               g->names[order[i]],
               score[order[i]]);
    }

    printf("----------------------------------------\n");
}

/*
 * Detect k structural communities using
 * Kruskal single-linkage clustering.
 *
 * The directed influence graph is converted into an
 * undirected relationship graph. For each unordered
 * pair of users having at least one directed relation,
 * the strongest available influence probability is used.
 *
 * distance = 1 - strongest_probability
 *
 * Therefore stronger relationships have smaller
 * distances and are joined earlier by Kruskal.
 */
void detect_communities(
    const struct Graph *g,
    int k
) {
    if (g == NULL || g->n <= 0) {
        printf("community analysis requires a non-empty graph\n");
        return;
    }

    if (k <= 0 || k > g->n) {
        printf(
            "invalid number of communities; choose 1-%d\n",
            g->n
        );
        return;
    }

    /*
     * strongest[u][v] stores the strongest relationship
     * observed in either direction for unordered pair u,v.
     */
    double *strongest = calloc(
        (size_t)g->n * (size_t)g->n,
        sizeof(double)
    );

    if (strongest == NULL) {
        printf("memory allocation failed\n");
        return;
    }

    for (int u = 0; u < g->n; u++) {
        for (struct Edge *edge = g->head[u];
             edge != NULL;
             edge = edge->next) {

            int v = edge->to;

            if (u == v) {
                continue;
            }

            int a = u < v ? u : v;
            int b = u < v ? v : u;

            size_t index =
                (size_t)a * (size_t)g->n +
                (size_t)b;

            if (edge->p > strongest[index]) {
                strongest[index] = edge->p;
            }
        }
    }

    int edge_count = 0;

    for (int u = 0; u < g->n; u++) {
        for (int v = u + 1; v < g->n; v++) {
            if (strongest[
                    (size_t)u * (size_t)g->n +
                    (size_t)v
                ] > 0.0) {

                edge_count++;
            }
        }
    }

    struct UndirectedEdge *edges = NULL;

    if (edge_count > 0) {
        edges = malloc(
            (size_t)edge_count *
            sizeof(*edges)
        );

        if (edges == NULL) {
            free(strongest);
            printf("memory allocation failed\n");
            return;
        }
    }

    int position = 0;

    for (int u = 0; u < g->n; u++) {
        for (int v = u + 1; v < g->n; v++) {
            double probability =
                strongest[
                    (size_t)u * (size_t)g->n +
                    (size_t)v
                ];

            if (probability > 0.0) {
                edges[position].u = u;
                edges[position].v = v;

                /*
                 * Strong probability -> small distance.
                 */
                edges[position].weight =
                    1.0 - probability;

                position++;
            }
        }
    }

    free(strongest);

    int labels[1000];
    long long operations = 0;

    int community_count =
        kruskal_k_clustering(
            g->n,
            edges,
            edge_count,
            k,
            labels,
            &operations
        );

    free(edges);

    if (community_count < 0) {
        printf(
            "cannot form exactly %d communities "
            "from the available relationships\n",
            k
        );
        return;
    }

    printf("\nKRUSKAL COMMUNITY ANALYSIS\n");
    printf("----------------------------------------\n");

    for (int community = 0;
         community < community_count;
         community++) {

        printf("Community %d: ", community + 1);

        int first = 1;

        for (int user = 0; user < g->n; user++) {
            if (labels[user] == community) {
                if (!first) {
                    printf(", ");
                }

                printf("%s", g->names[user]);
                first = 0;
            }
        }

        printf("\n");
    }

    printf("----------------------------------------\n");
    printf(
        "Total communities: %d\n",
        community_count
    );
    printf(
        "Kruskal operations: %lld\n",
        operations
    );
    printf(
        "Model: undirected distance = "
        "1 - strongest tie probability\n"
    );
}

/*
 * Select high-value seed users for
 * information dissemination.
 *
 * The greedy strategy selects users with
 * the highest total outgoing influence.
 */
void select_seeds(const struct Graph *g, int k) {
    if (k <= 0 || k > g->n) {
        printf("invalid number of seeds\n");
        return;
    }

    double score[1000];
    int selected[1000] = {0};

    /*
     * Calculate dissemination score.
     */
    for (int i = 0; i < g->n; i++) {
        score[i] = 0.0;

        for (struct Edge *edge = g->head[i];
             edge != NULL;
             edge = edge->next) {

            score[i] += edge->p;
        }
    }

    printf("\nHIGH-VALUE DISSEMINATION PLAN\n");
    printf("----------------------------------------\n");
    printf("Recommended seed users:\n");

    /*
     * Greedily choose the highest-scoring
     * unselected user k times.
     */
    for (int position = 0;
         position < k;
         position++) {

        int best_user = -1;

        for (int i = 0; i < g->n; i++) {
            if (!selected[i] &&
                (best_user == -1 ||
                 score[i] > score[best_user])) {

                best_user = i;
            }
        }

        selected[best_user] = 1;

        printf(
            "%d. %s  score: %.4f\n",
            position + 1,
            g->names[best_user],
            score[best_user]
        );
    }

    printf("----------------------------------------\n");
}