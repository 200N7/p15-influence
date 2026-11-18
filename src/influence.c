#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "influence.h"
#include "algorithms/greedy.h"
#include "algorithms/dp.h"
#include "algorithms/flow_matching.h"
#include "algorithms/string_matching.h"

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
 * Plan high-value information dissemination under
 * a limited campaign budget using exact 0/1 knapsack.
 *
 * Each user's value is derived from total direct outgoing
 * influence. Because the DP implementation uses integer
 * values, influence scores are scaled by 1000.
 *
 * costs[i] is the campaign cost of selecting user i.
 */
void plan_dissemination(
    const struct Graph *g,
    int budget,
    const int costs[]
) {
    if (g == NULL ||
        costs == NULL ||
        g->n <= 0 ||
        budget < 0) {

        printf("invalid dissemination request\n");
        return;
    }

    struct KnapsackItem *items = malloc(
        (size_t)g->n * sizeof(*items)
    );

    int *selected = calloc(
        (size_t)g->n,
        sizeof(int)
    );

    if (items == NULL || selected == NULL) {
        free(items);
        free(selected);
        printf("memory allocation failed\n");
        return;
    }

    double influence_score[1000];

    for (int i = 0; i < g->n; i++) {
        if (costs[i] < 0) {
            free(items);
            free(selected);
            printf("seed costs cannot be negative\n");
            return;
        }

        influence_score[i] = 0.0;

        for (struct Edge *edge = g->head[i];
             edge != NULL;
             edge = edge->next) {

            influence_score[i] += edge->p;
        }

        items[i].weight = costs[i];

        /*
         * Scale probability-based influence to an integer
         * value while retaining three decimal places.
         */
        items[i].value =
            (long long)(
                influence_score[i] * 1000.0 + 0.5
            );
    }

    long long operations = 0;

    long long optimal_value = knapsack_dp(
        items,
        g->n,
        budget,
        selected,
        &operations
    );

    if (optimal_value < 0) {
        free(items);
        free(selected);
        printf("dissemination optimization failed\n");
        return;
    }

    printf("\nBUDGET-CONSTRAINED DISSEMINATION PLAN\n");
    printf("----------------------------------------\n");
    printf("Budget: %d\n", budget);
    printf("Selected seed users:\n");

    int total_cost = 0;
    int seed_count = 0;

    for (int i = 0; i < g->n; i++) {
        if (selected[i]) {
            seed_count++;
            total_cost += costs[i];

            printf(
                "%d. %-20s cost: %-5d influence: %.4f\n",
                seed_count,
                g->names[i],
                costs[i],
                influence_score[i]
            );
        }
    }

    if (seed_count == 0) {
        printf("None within the available budget.\n");
    }

    printf("----------------------------------------\n");
    printf("Total cost: %d / %d\n",
           total_cost,
           budget);

    printf(
        "Total dissemination value: %.3f\n",
        (double)optimal_value / 1000.0
    );

    printf(
        "Knapsack DP operations: %lld\n",
        operations
    );

    printf(
        "Model: maximize additive direct influence "
        "under campaign budget\n"
    );

    free(items);
    free(selected);
}
/*
 * Analyze maximum dissemination capacity between two users.
 *
 * The current network stores propagation probability rather
 * than physical forwarding capacity. For this normalized
 * analysis, each probability is converted to an integer
 * capacity on a 0-100 scale:
 *
 * capacity = round(probability * 100)
 *
 * Ford-Fulkerson computes maximum flow and the final residual
 * graph is used to identify a corresponding minimum cut.
 */
void analyze_audience_capacity(
    const struct Graph *g,
    const char *source,
    const char *target
) {
    if (g == NULL ||
        source == NULL ||
        target == NULL ||
        g->n <= 0) {

        printf("invalid capacity-analysis request\n");
        return;
    }


    int src = find_user(g, source);
    int dst = find_user(g, target);

    if (src == -1 || dst == -1) {
        printf("user not found\n");
        return;
    }

    if (src == dst) {
        printf(
            "source and target must be different "
            "for capacity analysis\n"
        );
        return;
    }

    int edge_count = 0;

    for (int u = 0; u < g->n; u++) {
        for (struct Edge *edge = g->head[u];
             edge != NULL;
             edge = edge->next) {

            if (edge->p <= 0.0 ||
                edge->p > 1.0) {

                printf(
                    "invalid probability on edge %s -> %s\n",
                    g->names[u],
                    g->names[edge->to]
                );
                return;
            }

            edge_count++;
        }
    }

    struct FlowEdge *edges = NULL;

    if (edge_count > 0) {
        edges = malloc(
            (size_t)edge_count *
            sizeof(*edges)
        );

        if (edges == NULL) {
            printf("memory allocation failed\n");
            return;
        }
    }

    int position = 0;

    for (int u = 0; u < g->n; u++) {
        for (struct Edge *edge = g->head[u];
             edge != NULL;
             edge = edge->next) {

            edges[position].from = u;
            edges[position].to = edge->to;

            edges[position].capacity =
                (int)(edge->p * 100.0 + 0.5);

            position++;
        }
    }

    size_t cells =
        (size_t)g->n * (size_t)g->n;

    long long *flow =
        calloc(cells, sizeof(long long));

    long long *residual =
        calloc(cells, sizeof(long long));

    int *source_side =
        calloc((size_t)g->n, sizeof(int));

    if (flow == NULL ||
        residual == NULL ||
        source_side == NULL) {

        free(edges);
        free(flow);
        free(residual);
        free(source_side);

        printf("memory allocation failed\n");
        return;
    }

    long long augmentations = 0;

    long long max_flow = ford_fulkerson(
        g->n,
        edges,
        edge_count,
        src,
        dst,
        flow,
        residual,
        &augmentations
    );

    if (max_flow < 0) {
        free(edges);
        free(flow);
        free(residual);
        free(source_side);

        printf("maximum-flow analysis failed\n");
        return;
    }

    if (!min_cut_source_side(
            g->n,
            src,
            residual,
            source_side)) {

        free(edges);
        free(flow);
        free(residual);
        free(source_side);

        printf("minimum-cut analysis failed\n");
        return;
    }

    long long min_cut = cut_capacity(
        g->n,
        edges,
        edge_count,
        source_side
    );

    printf("\nAUDIENCE CAPACITY AND BOTTLENECK ANALYSIS\n");
    printf("----------------------------------------\n");

    printf(
        "Source: %s\nTarget: %s\n",
        source,
        target
    );

    printf(
        "Maximum dissemination capacity: %lld\n",
        max_flow
    );

    printf(
        "Minimum cut capacity: %lld\n",
        min_cut
    );

    printf("Bottleneck relationships:\n");

    int cut_edges = 0;

    for (int i = 0; i < edge_count; i++) {
        if (source_side[edges[i].from] &&
            !source_side[edges[i].to]) {

            cut_edges++;

            printf(
                "%d. %s -> %s  capacity: %d\n",
                cut_edges,
                g->names[edges[i].from],
                g->names[edges[i].to],
                edges[i].capacity
            );
        }
    }

    if (cut_edges == 0) {
        printf("None; target is unreachable from source.\n");
    }

    printf("----------------------------------------\n");

    printf(
        "Ford-Fulkerson augmentations: %lld\n",
        augmentations
    );

    printf(
        "Verification: max-flow %s min-cut\n",
        max_flow == min_cut ? "=" : "!="
    );

    printf(
        "Model: capacity = round(probability * 100)\n"
    );

    free(edges);
    free(flow);
    free(residual);
    free(source_side);
}

/*
 * Assign network users to campaigns using exact maximum
 * bipartite matching.
 *
 * Compatibility is entered explicitly because the follower
 * graph does not contain campaign eligibility information.
 *
 * The exact augmenting-path solution is compared with a
 * greedy edge-order baseline to expose solution quality.
 */
void plan_campaign_assignments(
    const struct Graph *g,
    int campaign_count
) {
    if (g == NULL ||
        g->n <= 0 ||
        campaign_count <= 0 ||
        campaign_count > 100) {

        printf("invalid campaign-assignment request\n");
        return;
    }

    int max_edges = g->n * campaign_count;

    struct BipartiteEdge *edges = malloc(
        (size_t)max_edges * sizeof(*edges)
    );

    int *exact_left = malloc(
        (size_t)g->n * sizeof(int)
    );

    int *exact_right = malloc(
        (size_t)campaign_count * sizeof(int)
    );

    int *greedy_left = malloc(
        (size_t)g->n * sizeof(int)
    );

    int *greedy_right = malloc(
        (size_t)campaign_count * sizeof(int)
    );

    if (edges == NULL ||
        exact_left == NULL ||
        exact_right == NULL ||
        greedy_left == NULL ||
        greedy_right == NULL) {

        free(edges);
        free(exact_left);
        free(exact_right);
        free(greedy_left);
        free(greedy_right);

        printf("memory allocation failed\n");
        return;
    }

    int edge_count = 0;

    printf("\nCAMPAIGN COMPATIBILITY INPUT\n");
    printf("----------------------------------------\n");
    printf(
        "Enter 1 if a user is eligible for a campaign, "
        "otherwise 0.\n"
    );

    for (int user = 0; user < g->n; user++) {
        for (int campaign = 0;
             campaign < campaign_count;
             campaign++) {

            int compatible;

            printf(
                "%s -> Campaign %d: ",
                g->names[user],
                campaign + 1
            );

            if (scanf("%d", &compatible) != 1) {
                free(edges);
                free(exact_left);
                free(exact_right);
                free(greedy_left);
                free(greedy_right);

                printf("invalid compatibility input\n");
                return;
            }

            if (compatible != 0 &&
                compatible != 1) {

                free(edges);
                free(exact_left);
                free(exact_right);
                free(greedy_left);
                free(greedy_right);

                printf("compatibility must be 0 or 1\n");
                return;
            }

            if (compatible) {
                edges[edge_count].left = user;
                edges[edge_count].right = campaign;
                edge_count++;
            }
        }
    }

    long long exact_operations = 0;
    long long greedy_operations = 0;

    int exact_size =
        bipartite_maximum_matching(
            g->n,
            campaign_count,
            edges,
            edge_count,
            exact_left,
            exact_right,
            &exact_operations
        );

    int greedy_size =
        bipartite_greedy_matching(
            g->n,
            campaign_count,
            edges,
            edge_count,
            greedy_left,
            greedy_right,
            &greedy_operations
        );

    if (exact_size < 0 || greedy_size < 0) {
        free(edges);
        free(exact_left);
        free(exact_right);
        free(greedy_left);
        free(greedy_right);

        printf("campaign matching failed\n");
        return;
    }

    printf("\nCAMPAIGN ASSIGNMENT PLAN\n");
    printf("----------------------------------------\n");
    printf("Optimal assignments:\n");

    if (exact_size == 0) {
        printf("None.\n");
    } else {
        int number = 0;

        for (int user = 0; user < g->n; user++) {
            if (exact_left[user] != -1) {
                number++;

                printf(
                    "%d. %-20s -> Campaign %d\n",
                    number,
                    g->names[user],
                    exact_left[user] + 1
                );
            }
        }
    }

    printf("----------------------------------------\n");

    printf(
        "Maximum matching size: %d\n",
        exact_size
    );

    printf(
        "Greedy matching size: %d\n",
        greedy_size
    );

    double quality =
        exact_size == 0
            ? 1.0
            : (double)greedy_size /
              (double)exact_size;

    printf(
        "Greedy solution quality: %.2f%% of optimum\n",
        quality * 100.0
    );

    printf(
        "Exact matching operations: %lld\n",
        exact_operations
    );

    printf(
        "Greedy matching operations: %lld\n",
        greedy_operations
    );

    printf(
        "Model: users and campaigns form "
        "a bipartite eligibility graph\n"
    );

    free(edges);
    free(exact_left);
    free(exact_right);
    free(greedy_left);
    free(greedy_right);
}

/*
 * Search a post/message for a phrase or hashtag using
 * four exact string-matching algorithms.
 *
 * The algorithms must agree on both the number of matches
 * and every starting position. This gives an integrated
 * correctness cross-check while also exposing operation
 * counts for algorithm comparison.
 */
void analyze_post_pattern(
    const char *text,
    const char *pattern
) {
    if (text == NULL ||
        pattern == NULL ||
        text[0] == '\0' ||
        pattern[0] == '\0') {

        printf("text and pattern must be non-empty\n");
        return;
    }

    size_t text_length = strlen(text);

    int *naive_positions = malloc(
        (text_length + 1U) * sizeof(int)
    );

    int *rk_positions = malloc(
        (text_length + 1U) * sizeof(int)
    );

    int *fa_positions = malloc(
        (text_length + 1U) * sizeof(int)
    );

    int *kmp_positions = malloc(
        (text_length + 1U) * sizeof(int)
    );

    if (naive_positions == NULL ||
        rk_positions == NULL ||
        fa_positions == NULL ||
        kmp_positions == NULL) {

        free(naive_positions);
        free(rk_positions);
        free(fa_positions);
        free(kmp_positions);

        printf("memory allocation failed\n");
        return;
    }

    long long naive_operations = 0;
    long long rk_operations = 0;
    long long fa_operations = 0;
    long long kmp_operations = 0;

    int naive_count = naive_string_match(
        text,
        pattern,
        naive_positions,
        &naive_operations
    );

    int rk_count = rabin_karp_match(
        text,
        pattern,
        rk_positions,
        &rk_operations
    );

    int fa_count = finite_automaton_match(
        text,
        pattern,
        fa_positions,
        &fa_operations
    );

    int kmp_count = kmp_string_match(
        text,
        pattern,
        kmp_positions,
        &kmp_operations
    );

    if (naive_count < 0 ||
        rk_count < 0 ||
        fa_count < 0 ||
        kmp_count < 0) {

        free(naive_positions);
        free(rk_positions);
        free(fa_positions);
        free(kmp_positions);

        printf("string-matching analysis failed\n");
        return;
    }

    int agree =
        naive_count == rk_count &&
        naive_count == fa_count &&
        naive_count == kmp_count;

    if (agree) {
        for (int i = 0; i < naive_count; i++) {
            if (naive_positions[i] != rk_positions[i] ||
                naive_positions[i] != fa_positions[i] ||
                naive_positions[i] != kmp_positions[i]) {

                agree = 0;
                break;
            }
        }
    }

    printf("\nPOST / HASHTAG PATTERN ANALYSIS\n");
    printf("----------------------------------------\n");

    printf("Pattern: \"%s\"\n", pattern);
    printf("Matches found: %d\n", naive_count);

    printf("Match positions: ");

    if (naive_count == 0) {
        printf("none");
    } else {
        for (int i = 0; i < naive_count; i++) {
            if (i > 0) {
                printf(", ");
            }

            printf("%d", naive_positions[i]);
        }
    }

    printf("\n");
    printf("----------------------------------------\n");

    printf(
        "Naive operations:            %lld\n",
        naive_operations
    );

    printf(
        "Rabin-Karp operations:       %lld\n",
        rk_operations
    );

    printf(
        "Finite-automaton operations: %lld\n",
        fa_operations
    );

    printf(
        "KMP operations:              %lld\n",
        kmp_operations
    );

    printf(
        "Cross-check: all four algorithms %s\n",
        agree
            ? "agree"
            : "DO NOT AGREE"
    );

    printf(
        "Rabin-Karp note: every hash hit is "
        "verified before reporting a match\n"
    );

    free(naive_positions);
    free(rk_positions);
    free(fa_positions);
    free(kmp_positions);
}
/*
 * Analyze Huffman compression for a post/message.
 *
 * Character frequencies are measured directly from the
 * supplied text. Huffman codes are then generated and
 * compared with the original fixed 8-bit representation.
 */
void analyze_text_compression(
    const char *text
) {
    if (text == NULL || text[0] == '\0') {
        printf("text must be non-empty\n");
        return;
    }

    unsigned long frequencies[256] = {0};

    for (const unsigned char *p =
             (const unsigned char *)text;
         *p != '\0';
         p++) {

        frequencies[*p]++;
    }

    struct HuffmanCode codes[256];
    long long operations = 0;

    int code_count = huffman_build_codes(
        frequencies,
        codes,
        &operations
    );

    if (code_count < 0) {
        printf("Huffman compression analysis failed\n");
        return;
    }

    if (!huffman_is_prefix_free(
            codes,
            code_count)) {

        printf("generated Huffman codes are invalid\n");
        return;
    }

    unsigned long long original_bits = 0;
    unsigned long long compressed_bits = 0;

    size_t text_length = strlen(text);

    original_bits =
        (unsigned long long)text_length * 8ULL;

    for (int i = 0; i < code_count; i++) {
        unsigned char symbol = codes[i].symbol;

        compressed_bits +=
            (unsigned long long)frequencies[symbol] *
            (unsigned long long)strlen(codes[i].code);
    }

    printf("\nHUFFMAN TEXT COMPRESSION ANALYSIS\n");
    printf("----------------------------------------\n");
    printf("Input characters: %zu\n", text_length);
    printf("Distinct symbols: %d\n", code_count);
    printf("Original size: %llu bits\n", original_bits);
    printf(
        "Huffman encoded size: %llu bits\n",
        compressed_bits
    );

    if (original_bits > 0) {
        double reduction =
            100.0 *
            (1.0 -
             (double)compressed_bits /
             (double)original_bits);

        printf(
            "Bit reduction: %.2f%%\n",
            reduction
        );
    }

    printf("----------------------------------------\n");
    printf("Generated codes:\n");

    for (int i = 0; i < code_count; i++) {
        unsigned char symbol = codes[i].symbol;

        if (symbol == ' ') {
            printf(
                "[space]  freq=%lu  code=%s\n",
                frequencies[symbol],
                codes[i].code
            );
        } else if (symbol == '\n') {
            printf(
                "[newline] freq=%lu  code=%s\n",
                frequencies[symbol],
                codes[i].code
            );
        } else if (symbol >= 32 &&
                   symbol <= 126) {

            printf(
                "'%c'      freq=%lu  code=%s\n",
                symbol,
                frequencies[symbol],
                codes[i].code
            );
        } else {
            printf(
                "0x%02X     freq=%lu  code=%s\n",
                symbol,
                frequencies[symbol],
                codes[i].code
            );
        }
    }

    printf("----------------------------------------\n");
    printf(
        "Prefix-free verification: PASS\n"
    );
    printf(
        "Huffman operations: %lld\n",
        operations
    );
    printf(
        "Model: variable-length prefix coding "
        "from observed character frequencies\n"
    );
}
