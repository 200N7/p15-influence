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
 * Recursive DFS used by community detection.
 *
 * For this analysis, edge direction is ignored.
 * Therefore both outgoing and incoming
 * relationships are considered.
 */
static void community_dfs(
    const struct Graph *g,
    int user,
    int visited[]
) {
    visited[user] = 1;

    printf("%s ", g->names[user]);

    /*
     * Follow outgoing relationships.
     */
    for (struct Edge *edge = g->head[user];
         edge != NULL;
         edge = edge->next) {

        if (!visited[edge->to]) {
            community_dfs(
                g,
                edge->to,
                visited
            );
        }
    }

    /*
     * Follow incoming relationships.
     */
    for (int i = 0; i < g->n; i++) {
        for (struct Edge *edge = g->head[i];
             edge != NULL;
             edge = edge->next) {

            if (edge->to == user &&
                !visited[i]) {

                community_dfs(
                    g,
                    i,
                    visited
                );
            }
        }
    }
}

/*
 * Detect structural communities as
 * connected components.
 */
void detect_communities(const struct Graph *g) {
    int visited[1000] = {0};
    int community_count = 0;

    printf("\nCOMMUNITY ANALYSIS\n");
    printf("----------------------------------------\n");

    for (int i = 0; i < g->n; i++) {
        if (!visited[i]) {
            community_count++;

            printf(
                "Community %d: ",
                community_count
            );

            community_dfs(
                g,
                i,
                visited
            );

            printf("\n");
        }
    }

    printf("----------------------------------------\n");
    printf(
        "Total communities: %d\n",
        community_count
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