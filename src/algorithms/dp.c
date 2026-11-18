#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <stdint.h>
#include "algorithms/dp.h"

/* =========================================================
   0/1 KNAPSACK DYNAMIC PROGRAMMING
   ========================================================= */

long long knapsack_dp(
    const struct KnapsackItem items[],
    int n,
    int capacity,
    int selected[],
    long long *operations
) {
    if (operations != NULL) {
        *operations = 0;
    }

    if (n < 0 ||
        capacity < 0 ||
        (n > 0 && (items == NULL || selected == NULL))) {

        return -1;
    }

    if (n == 0) {
        return 0;
    }

    for (int i = 0; i < n; i++) {
        if (items[i].weight < 0 ||
            items[i].value < 0) {

            return -1;
        }

        selected[i] = 0;
    }

    size_t rows = (size_t)n + 1U;
    size_t cols = (size_t)capacity + 1U;

    if (rows > SIZE_MAX / cols ||
        rows * cols > SIZE_MAX / sizeof(long long)) {

        return -1;
    }

    long long *dp =
        calloc(rows * cols, sizeof(long long));

    if (dp == NULL) {
        return -1;
    }

#define DP(i, w) dp[(size_t)(i) * cols + (size_t)(w)]

    for (int i = 1; i <= n; i++) {
        int weight = items[i - 1].weight;
        long long value = items[i - 1].value;

        for (int w = 0; w <= capacity; w++) {
            DP(i, w) = DP(i - 1, w);

            if (operations != NULL) {
                (*operations)++;
            }

            if (weight <= w) {
                long long previous =
                    DP(i - 1, w - weight);

                if (value > LLONG_MAX - previous) {
                    free(dp);
                    return -1;
                }

                long long candidate =
                    previous + value;

                if (candidate > DP(i, w)) {
                    DP(i, w) = candidate;
                }
            }
        }
    }

    long long answer = DP(n, capacity);

    /*
     * Reconstruct one optimal set.
     */
    int remaining = capacity;

    for (int i = n; i >= 1; i--) {
        if (DP(i, remaining) !=
            DP(i - 1, remaining)) {

            selected[i - 1] = 1;

            remaining -=
                items[i - 1].weight;
        }
    }

#undef DP

    free(dp);
    return answer;
}

/* =========================================================
   MATRIX-CHAIN MULTIPLICATION
   ========================================================= */

static int safe_chain_product(
    long long a,
    long long b,
    long long c,
    long long *result
) {
    if (a < 0 || b < 0 || c < 0 || result == NULL) {
        return 0;
    }

    if (a != 0 && b > LLONG_MAX / a) {
        return 0;
    }

    long long ab = a * b;

    if (ab != 0 && c > LLONG_MAX / ab) {
        return 0;
    }

    *result = ab * c;
    return 1;
}

long long matrix_chain_order(
    const int dimensions[],
    int matrix_count,
    int split[],
    long long *operations
) {
    if (operations != NULL) {
        *operations = 0;
    }

    if (dimensions == NULL ||
        split == NULL ||
        matrix_count <= 0) {

        return -1;
    }

    for (int i = 0; i <= matrix_count; i++) {
        if (dimensions[i] <= 0) {
            return -1;
        }
    }

    size_t cells =
        (size_t)matrix_count *
        (size_t)matrix_count;

    if (matrix_count != 0 &&
        cells / (size_t)matrix_count !=
        (size_t)matrix_count) {

        return -1;
    }

    if (cells > SIZE_MAX / sizeof(long long)) {
        return -1;
    }

    long long *cost =
        malloc(cells * sizeof(long long));

    if (cost == NULL) {
        return -1;
    }

#define COST(i, j) cost[(size_t)(i) * matrix_count + (size_t)(j)]
#define SPLIT(i, j) split[(size_t)(i) * matrix_count + (size_t)(j)]

    for (int i = 0; i < matrix_count; i++) {
        for (int j = 0; j < matrix_count; j++) {
            COST(i, j) =
                i == j ? 0 : LLONG_MAX;

            SPLIT(i, j) = -1;
        }
    }

    for (int length = 2;
         length <= matrix_count;
         length++) {

        for (int i = 0;
             i <= matrix_count - length;
             i++) {

            int j = i + length - 1;

            for (int k = i; k < j; k++) {
                if (operations != NULL) {
                    (*operations)++;
                }

                long long multiplication_cost;

                if (!safe_chain_product(
                        dimensions[i],
                        dimensions[k + 1],
                        dimensions[j + 1],
                        &multiplication_cost)) {

                    free(cost);
                    return -1;
                }

                if (COST(i, k) == LLONG_MAX ||
                    COST(k + 1, j) == LLONG_MAX) {

                    continue;
                }

                if (COST(i, k) >
                    LLONG_MAX - COST(k + 1, j)) {

                    free(cost);
                    return -1;
                }

                long long partial =
                    COST(i, k) +
                    COST(k + 1, j);

                if (partial >
                    LLONG_MAX - multiplication_cost) {

                    free(cost);
                    return -1;
                }

                long long candidate =
                    partial + multiplication_cost;

                if (candidate < COST(i, j)) {
                    COST(i, j) = candidate;
                    SPLIT(i, j) = k;
                }
            }
        }
    }

    long long answer =
        COST(0, matrix_count - 1);

#undef COST
#undef SPLIT

    free(cost);
    return answer;
}

/* =========================================================
   LONGEST COMMON SUBSEQUENCE
   ========================================================= */

int lcs_sequence(
    const char *a,
    const char *b,
    char output[],
    long long *operations
) {
    if (operations != NULL) {
        *operations = 0;
    }

    if (a == NULL || b == NULL || output == NULL) {
        return -1;
    }

    size_t m = strlen(a);
    size_t n = strlen(b);

    if (m > (size_t)INT_MAX ||
        n > (size_t)INT_MAX) {

        return -1;
    }

    size_t rows = m + 1U;
    size_t cols = n + 1U;

    if (rows > SIZE_MAX / cols ||
        rows * cols > SIZE_MAX / sizeof(int)) {

        return -1;
    }

    int *dp =
        calloc(rows * cols, sizeof(int));

    if (dp == NULL) {
        return -1;
    }

#define LCS(i, j) dp[(i) * cols + (j)]

    for (size_t i = 1; i <= m; i++) {
        for (size_t j = 1; j <= n; j++) {
            if (operations != NULL) {
                (*operations)++;
            }

            if (a[i - 1] == b[j - 1]) {
                LCS(i, j) =
                    LCS(i - 1, j - 1) + 1;
            } else {
                LCS(i, j) =
                    LCS(i - 1, j) >=
                    LCS(i, j - 1)
                        ? LCS(i - 1, j)
                        : LCS(i, j - 1);
            }
        }
    }

    int length = LCS(m, n);

    size_t i = m;
    size_t j = n;
    int position = length;

    output[position] = '\0';

    while (i > 0 && j > 0) {
        if (a[i - 1] == b[j - 1]) {
            output[--position] = a[i - 1];
            i--;
            j--;
        } else if (
            LCS(i - 1, j) >=
            LCS(i, j - 1)) {

            i--;
        } else {
            j--;
        }
    }

#undef LCS

    free(dp);
    return length;
}
/* =========================================================
   OPTIMAL BINARY SEARCH TREE
   ========================================================= */

long long optimal_bst(
    const long long frequencies[],
    int n,
    int root_table[],
    long long *operations
) {
    if (operations != NULL) {
        *operations = 0;
    }

    if (n < 0 ||
        (n > 0 &&
         (frequencies == NULL ||
          root_table == NULL))) {

        return -1;
    }

    if (n == 0) {
        return 0;
    }

    for (int i = 0; i < n; i++) {
        if (frequencies[i] < 0) {
            return -1;
        }
    }

    size_t cells = (size_t)n * (size_t)n;

    if (cells > SIZE_MAX / sizeof(long long)) {
        return -1;
    }

    long long *cost =
        malloc(cells * sizeof(long long));

    long long *prefix =
        malloc(((size_t)n + 1U) *
               sizeof(long long));

    if (cost == NULL || prefix == NULL) {
        free(cost);
        free(prefix);
        return -1;
    }

#define OBST_COST(i, j) \
    cost[(size_t)(i) * (size_t)n + (size_t)(j)]

#define OBST_ROOT(i, j) \
    root_table[(size_t)(i) * (size_t)n + (size_t)(j)]

    prefix[0] = 0;

    for (int i = 0; i < n; i++) {
        if (frequencies[i] >
            LLONG_MAX - prefix[i]) {

            free(cost);
            free(prefix);
            return -1;
        }

        prefix[i + 1] =
            prefix[i] + frequencies[i];

        for (int j = 0; j < n; j++) {
            OBST_COST(i, j) = LLONG_MAX;
            OBST_ROOT(i, j) = -1;
        }

        OBST_COST(i, i) = frequencies[i];
        OBST_ROOT(i, i) = i;
    }

    for (int length = 2;
         length <= n;
         length++) {

        for (int i = 0;
             i <= n - length;
             i++) {

            int j = i + length - 1;

            long long frequency_sum =
                prefix[j + 1] - prefix[i];

            for (int root = i;
                 root <= j;
                 root++) {

                if (operations != NULL) {
                    (*operations)++;
                }

                long long left_cost =
                    root > i
                        ? OBST_COST(i, root - 1)
                        : 0;

                long long right_cost =
                    root < j
                        ? OBST_COST(root + 1, j)
                        : 0;

                if (left_cost >
                    LLONG_MAX - right_cost) {

                    free(cost);
                    free(prefix);
                    return -1;
                }

                long long children =
                    left_cost + right_cost;

                if (children >
                    LLONG_MAX - frequency_sum) {

                    free(cost);
                    free(prefix);
                    return -1;
                }

                long long candidate =
                    children + frequency_sum;

                if (candidate < OBST_COST(i, j)) {
                    OBST_COST(i, j) = candidate;
                    OBST_ROOT(i, j) = root;
                }
            }
        }
    }

    long long answer =
        OBST_COST(0, n - 1);

#undef OBST_COST
#undef OBST_ROOT

    free(cost);
    free(prefix);

    return answer;
}

/* =========================================================
   BELLMAN-FORD
   ========================================================= */

#include <float.h>

static int validate_dp_graph(
    int vertices,
    const struct DPEdge edges[],
    int edge_count,
    int source
) {
    if (vertices <= 0 ||
        edge_count < 0 ||
        source < 0 ||
        source >= vertices) {

        return 0;
    }

    if (edge_count > 0 && edges == NULL) {
        return 0;
    }

    for (int i = 0; i < edge_count; i++) {
        if (edges[i].from < 0 ||
            edges[i].from >= vertices ||
            edges[i].to < 0 ||
            edges[i].to >= vertices) {

            return 0;
        }
    }

    return 1;
}

int bellman_ford(
    int vertices,
    const struct DPEdge edges[],
    int edge_count,
    int source,
    double distance[],
    int previous[],
    long long *relaxations
) {
    if (relaxations != NULL) {
        *relaxations = 0;
    }

    if (distance == NULL || previous == NULL) {
        return 0;
    }

    if (!validate_dp_graph(
            vertices,
            edges,
            edge_count,
            source)) {

        return 0;
    }

    for (int i = 0; i < vertices; i++) {
        distance[i] = DBL_MAX;
        previous[i] = -1;
    }

    distance[source] = 0.0;

    /*
     * At most V-1 passes are required.
     * Early exit gives O(E) best-case behaviour.
     */
    for (int pass = 1;
         pass < vertices;
         pass++) {

        int changed = 0;

        for (int i = 0;
             i < edge_count;
             i++) {

            int u = edges[i].from;
            int v = edges[i].to;

            if (relaxations != NULL) {
                (*relaxations)++;
            }

            if (distance[u] == DBL_MAX) {
                continue;
            }

            double candidate =
                distance[u] + edges[i].weight;

            if (candidate < distance[v]) {
                distance[v] = candidate;
                previous[v] = u;
                changed = 1;
            }
        }

        if (!changed) {
            break;
        }
    }

    /*
     * One additional pass detects a negative cycle
     * reachable from the chosen source.
     */
    for (int i = 0; i < edge_count; i++) {
        int u = edges[i].from;
        int v = edges[i].to;

        if (distance[u] == DBL_MAX) {
            continue;
        }

        double candidate =
            distance[u] + edges[i].weight;

        if (candidate < distance[v]) {
            return -1;
        }
    }

    return 1;
}
/* =========================================================
   FLOYD-WARSHALL
   ========================================================= */

int floyd_warshall(
    int vertices,
    const struct DPEdge edges[],
    int edge_count,
    double distance[],
    int next[],
    long long *operations
) {
    if (operations != NULL) {
        *operations = 0;
    }

    if (vertices <= 0 ||
        edge_count < 0 ||
        distance == NULL ||
        next == NULL ||
        (edge_count > 0 && edges == NULL)) {

        return 0;
    }

    for (int i = 0; i < edge_count; i++) {
        if (edges[i].from < 0 ||
            edges[i].from >= vertices ||
            edges[i].to < 0 ||
            edges[i].to >= vertices) {

            return 0;
        }
    }

#define FW(i, j) \
    distance[(size_t)(i) * (size_t)vertices + (size_t)(j)]

#define NEXT(i, j) \
    next[(size_t)(i) * (size_t)vertices + (size_t)(j)]

    for (int i = 0; i < vertices; i++) {
        for (int j = 0; j < vertices; j++) {
            if (i == j) {
                FW(i, j) = 0.0;
                NEXT(i, j) = j;
            } else {
                FW(i, j) = DBL_MAX;
                NEXT(i, j) = -1;
            }
        }
    }

    /*
     * Multiple parallel edges are allowed.
     * Keep the lightest one.
     */
    for (int i = 0; i < edge_count; i++) {
        int u = edges[i].from;
        int v = edges[i].to;

        if (edges[i].weight < FW(u, v)) {
            FW(u, v) = edges[i].weight;
            NEXT(u, v) = v;
        }
    }

    for (int k = 0; k < vertices; k++) {
        for (int i = 0; i < vertices; i++) {
            if (FW(i, k) == DBL_MAX) {
                continue;
            }

            for (int j = 0; j < vertices; j++) {
                if (FW(k, j) == DBL_MAX) {
                    continue;
                }

                if (operations != NULL) {
                    (*operations)++;
                }

                double candidate =
                    FW(i, k) + FW(k, j);

                if (candidate < FW(i, j)) {
                    FW(i, j) = candidate;
                    NEXT(i, j) = NEXT(i, k);
                }
            }
        }
    }

    /*
     * Negative diagonal means a negative cycle.
     */
   for (int i = 0; i < vertices; i++) {
    if (FW(i, i) < 0.0) {
        return -1;
    }
}

#undef FW
#undef NEXT

    return 1;
}

int floyd_reconstruct_path(
    int source,
    int target,
    const int next[],
    int vertices,
    int path[]
) {
    if (next == NULL ||
        path == NULL ||
        vertices <= 0 ||
        source < 0 ||
        source >= vertices ||
        target < 0 ||
        target >= vertices) {

        return 0;
    }

#define FNEXT(i, j) \
    next[(size_t)(i) * (size_t)vertices + (size_t)(j)]

   if (FNEXT(source, target) == -1) {

        return 0;
    }

    int count = 0;
    int current = source;

    path[count++] = current;

    while (current != target) {
        current = FNEXT(current, target);

        if (current < 0 ||
            current >= vertices ||
            count > vertices) {

            return 0;
        }

        path[count++] = current;
    }

#undef FNEXT

    return count;
}

/* =========================================================
   HELD-KARP TSP
   ========================================================= */

double held_karp_tsp(
    const double distance[],
    int n,
    int start,
    int tour[],
    long long *operations
) {
    if (operations != NULL) {
        *operations = 0;
    }

    if (distance == NULL ||
        tour == NULL ||
        n <= 0 ||
        n > 20 ||
        start < 0 ||
        start >= n) {

        return -1.0;
    }

    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            double value =
                distance[(size_t)i * (size_t)n +
                         (size_t)j];

            if (value < 0.0) {
                return -1.0;
            }
        }
    }

    if (n == 1) {
        tour[0] = start;
        tour[1] = start;
        return 0.0;
    }

    size_t states =
        (size_t)1U << (unsigned int)n;

    if (states > SIZE_MAX / (size_t)n) {
        return -1.0;
    }

    size_t cells = states * (size_t)n;

    if (cells > SIZE_MAX / sizeof(double) ||
        cells > SIZE_MAX / sizeof(int)) {

        return -1.0;
    }

    double *dp =
        malloc(cells * sizeof(double));

    int *parent =
        malloc(cells * sizeof(int));

    if (dp == NULL || parent == NULL) {
        free(dp);
        free(parent);
        return -1.0;
    }

#define TSP(mask, v) \
    dp[(size_t)(mask) * (size_t)n + (size_t)(v)]

#define TSP_PARENT(mask, v) \
    parent[(size_t)(mask) * (size_t)n + (size_t)(v)]

    for (size_t i = 0; i < cells; i++) {
        dp[i] = DBL_MAX;
        parent[i] = -1;
    }

    size_t start_mask =
        (size_t)1U << (unsigned int)start;

    TSP(start_mask, start) = 0.0;

    for (size_t mask = 0; mask < states; mask++) {
        if ((mask & start_mask) == 0) {
            continue;
        }

        for (int u = 0; u < n; u++) {
            if ((mask &
                 ((size_t)1U << (unsigned int)u)) == 0) {

                continue;
            }

            if (TSP(mask, u) == DBL_MAX) {
                continue;
            }

            for (int v = 0; v < n; v++) {
                size_t bit =
                    (size_t)1U << (unsigned int)v;

                if ((mask & bit) != 0) {
                    continue;
                }

                double edge =
                    distance[
                        (size_t)u * (size_t)n +
                        (size_t)v
                    ];

                if (edge == DBL_MAX) {
                    continue;
                }

                if (operations != NULL) {
                    (*operations)++;
                }

                size_t next_mask =
                    mask | bit;

                double candidate =
                    TSP(mask, u) + edge;

                if (candidate <
                    TSP(next_mask, v)) {

                    TSP(next_mask, v) =
                        candidate;

                    TSP_PARENT(next_mask, v) =
                        u;
                }
            }
        }
    }

    size_t full_mask = states - 1U;

    double best = DBL_MAX;
    int last = -1;

    for (int u = 0; u < n; u++) {
        if (u == start ||
            TSP(full_mask, u) == DBL_MAX) {

            continue;
        }

        double return_edge =
            distance[
                (size_t)u * (size_t)n +
                (size_t)start
            ];

        if (return_edge == DBL_MAX) {
            continue;
        }

        double candidate =
            TSP(full_mask, u) +
            return_edge;

        if (candidate < best) {
            best = candidate;
            last = u;
        }
    }

    if (last == -1) {

        free(dp);
        free(parent);
        return -1.0;
    }

    /*
     * Reconstruct backwards.
     */
    tour[n] = start;

    size_t mask = full_mask;
    int current = last;

    for (int position = n - 1;
         position >= 1;
         position--) {

        tour[position] = current;

        int previous =
            TSP_PARENT(mask, current);

        mask &=
            ~((size_t)1U <<
              (unsigned int)current);

        current = previous;
    }

    tour[0] = start;

#undef TSP
#undef TSP_PARENT

    free(dp);
    free(parent);

    return best;
}