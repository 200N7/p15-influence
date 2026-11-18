#include <stdio.h>
#include <string.h>
#include "influence.h"

/* Find the internal index of a user by name. */
static int find_user(const struct Graph *g, const char *name) {
    for (int i = 0; i < g->n; i++) {
        if (strcmp(g->names[i], name) == 0) {
            return i;
        }
    }

    return -1;
}

/*
 * Finds the path having the maximum product of edge probabilities.
 * This uses a Dijkstra-style greedy approach.
 */
int most_likely_path(
    const struct Graph *g,
    const char *source,
    const char *target
) {
    int src = find_user(g, source);
    int dst = find_user(g, target);

    if (src == -1 || dst == -1) {
        printf("user not found\n");
        return 1;
    }

    double probability[1000];
    int previous[1000];
    int visited[1000];

    /* Initialize all users. */
    for (int i = 0; i < g->n; i++) {
        probability[i] = 0.0;
        previous[i] = -1;
        visited[i] = 0;
    }

    /*
     * Probability of reaching the source from itself is 1,
     * representing 100 percent.
     */
    probability[src] = 1.0;

    /*
     * Repeatedly select the unvisited user having the
     * highest probability discovered so far.
     */
    for (int step = 0; step < g->n; step++) {
        int u = -1;
        double best = -1.0;

        for (int i = 0; i < g->n; i++) {
            if (!visited[i] && probability[i] > best) {
                best = probability[i];
                u = i;
            }
        }

        /*
         * No reachable unvisited user remains.
         */
        if (u == -1 || probability[u] == 0.0) {
            break;
        }

        visited[u] = 1;

        /*
         * Once the destination is selected, its best
         * probability has been determined.
         */
        if (u == dst) {
            break;
        }

        /*
         * Relax all outgoing influence edges of user u.
         */
        struct Edge *edge = g->head[u];

        while (edge != NULL) {
            double new_probability =
                probability[u] * edge->p;

            if (new_probability > probability[edge->to]) {
                probability[edge->to] = new_probability;
                previous[edge->to] = u;
            }

            edge = edge->next;
        }
    }

    /*
     * If destination probability is still zero,
     * no influence path exists.
     */
    if (probability[dst] == 0.0) {
        printf("no influence path from %s to %s\n",
               source, target);
        return 2;
    }

    /*
     * Reconstruct the path by following the previous[]
     * array backwards from destination to source.
     */
    int path[1000];
    int path_length = 0;
    int current = dst;

    while (current != -1) {
        path[path_length++] = current;

        if (current == src) {
            break;
        }

        current = previous[current];
    }

    /*
     * Safety check: reconstructed path must reach source.
     */
    if (path[path_length - 1] != src) {
        printf("could not reconstruct influence path\n");
        return 2;
    }

    printf("Most likely influence path: ");

    /*
     * path[] currently contains destination -> source,
     * so print it in reverse.
     */
    for (int i = path_length - 1; i >= 0; i--) {
        printf("%s", g->names[path[i]]);

        if (i > 0) {
            printf(" -> ");
        }
    }

    printf("\n");
    printf("Influence probability: %.4f\n",
           probability[dst]);

    return 0;
}
/*
 * Rank users by total outgoing influence.
 *
 * Influence score of a user is the sum of the probabilities
 * of all outgoing edges from that user.
 *
 * Example:
 * A -> B = 0.90
 * A -> C = 0.50
 *
 * Score(A) = 0.90 + 0.50 = 1.40
 */
void rank_influencers(const struct Graph *g) {
    double score[1000];
    int order[1000];

    /*
     * Calculate the influence score of every user.
     */
    for (int i = 0; i < g->n; i++) {
        score[i] = 0.0;
        order[i] = i;

        struct Edge *edge = g->head[i];

        while (edge != NULL) {
            score[i] += edge->p;
            edge = edge->next;
        }
    }

    /*
     * Selection sort users in descending order
     * according to influence score.
     *
     * We implement the sorting ourselves instead
     * of using a library sorting function.
     */
    for (int i = 0; i < g->n - 1; i++) {
        int best = i;

        for (int j = i + 1; j < g->n; j++) {
            if (score[order[j]] > score[order[best]]) {
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