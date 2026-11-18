#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <stdint.h>
#include "algorithms/flow_matching.h"

static int valid_flow_input(
    int vertices,
    const struct FlowEdge edges[],
    int edge_count,
    int source,
    int sink
) {
    if (vertices <= 0 ||
        edge_count < 0 ||
        source < 0 ||
        source >= vertices ||
        sink < 0 ||
        sink >= vertices ||
        source == sink) {

        return 0;
    }

    if (edge_count > 0 && edges == NULL) {
        return 0;
    }

    for (int i = 0; i < edge_count; i++) {
        if (edges[i].from < 0 ||
            edges[i].from >= vertices ||
            edges[i].to < 0 ||
            edges[i].to >= vertices ||
            edges[i].capacity < 0) {

            return 0;
        }
    }

    return 1;
}

static long long dfs_augment(
    int vertices,
    int u,
    int sink,
    long long residual[],
    int visited[],
    long long pushed
) {
    if (u == sink) {
        return pushed;
    }

    visited[u] = 1;

    for (int v = 0; v < vertices; v++) {
        long long capacity =
            residual[
                (size_t)u * (size_t)vertices +
                (size_t)v
            ];

        if (!visited[v] && capacity > 0) {
            long long next_push =
                pushed < capacity
                    ? pushed
                    : capacity;

            long long result = dfs_augment(
                vertices,
                v,
                sink,
                residual,
                visited,
                next_push
            );

            if (result > 0) {
                residual[
                    (size_t)u * (size_t)vertices +
                    (size_t)v
                ] -= result;

                residual[
                    (size_t)v * (size_t)vertices +
                    (size_t)u
                ] += result;

                return result;
            }
        }
    }

    return 0;
}

long long ford_fulkerson(
    int vertices,
    const struct FlowEdge edges[],
    int edge_count,
    int source,
    int sink,
    long long flow_matrix[],
    long long residual_matrix[],
    long long *augmentations
) {
    if (augmentations != NULL) {
        *augmentations = 0;
    }

    if (!valid_flow_input(
            vertices,
            edges,
            edge_count,
            source,
            sink) ||
        flow_matrix == NULL ||
        residual_matrix == NULL) {

        return -1;
    }

    size_t cells =
        (size_t)vertices *
        (size_t)vertices;

    if (vertices != 0 &&
        cells / (size_t)vertices !=
        (size_t)vertices) {

        return -1;
    }

    if (cells > SIZE_MAX / sizeof(long long)) {
        return -1;
    }

    for (size_t i = 0; i < cells; i++) {
        flow_matrix[i] = 0;
        residual_matrix[i] = 0;
    }

    /*
     * Parallel directed edges are combined.
     */
    for (int i = 0; i < edge_count; i++) {
        size_t index =
            (size_t)edges[i].from *
            (size_t)vertices +
            (size_t)edges[i].to;

        if ((long long)edges[i].capacity >
            LLONG_MAX - residual_matrix[index]) {

            return -1;
        }

        residual_matrix[index] +=
            edges[i].capacity;
    }

    int *visited =
        malloc(
            (size_t)vertices *
            sizeof(int)
        );

    if (visited == NULL) {
        return -1;
    }

    long long max_flow = 0;

    while (1) {
        memset(
            visited,
            0,
            (size_t)vertices *
            sizeof(int)
        );

        long long pushed = dfs_augment(
            vertices,
            source,
            sink,
            residual_matrix,
            visited,
            LLONG_MAX
        );

        if (pushed <= 0) {
            break;
        }

        if (pushed > LLONG_MAX - max_flow) {
            free(visited);
            return -1;
        }

        max_flow += pushed;

        if (augmentations != NULL) {
            (*augmentations)++;
        }
    }

    /*
     * Recover net directed flow from original capacity
     * and final residual capacity.
     *
     * For each original direction:
     * flow = original capacity - residual forward capacity,
     * clipped at zero because reverse residual edges may
     * exceed original capacity.
     */
    long long *original =
        calloc(cells, sizeof(long long));

    if (original == NULL) {
        free(visited);
        return -1;
    }

    for (int i = 0; i < edge_count; i++) {
        size_t index =
            (size_t)edges[i].from *
            (size_t)vertices +
            (size_t)edges[i].to;

        original[index] +=
            edges[i].capacity;
    }

    for (int u = 0; u < vertices; u++) {
        for (int v = 0; v < vertices; v++) {
            size_t index =
                (size_t)u *
                (size_t)vertices +
                (size_t)v;

            if (original[index] > 0) {
                long long used =
                    original[index] -
                    residual_matrix[index];

                flow_matrix[index] =
                    used > 0 ? used : 0;
            }
        }
    }

    free(original);
    free(visited);

    return max_flow;
}

int min_cut_source_side(
    int vertices,
    int source,
    const long long residual_matrix[],
    int source_side[]
) {
    if (vertices <= 0 ||
        source < 0 ||
        source >= vertices ||
        residual_matrix == NULL ||
        source_side == NULL) {

        return 0;
    }

    for (int i = 0; i < vertices; i++) {
        source_side[i] = 0;
    }

    int *stack =
        malloc(
            (size_t)vertices *
            sizeof(int)
        );

    if (stack == NULL) {
        return 0;
    }

    int top = 0;

    stack[top++] = source;
    source_side[source] = 1;

    while (top > 0) {
        int u = stack[--top];

        for (int v = 0; v < vertices; v++) {
            long long residual =
                residual_matrix[
                    (size_t)u *
                    (size_t)vertices +
                    (size_t)v
                ];

            if (residual > 0 &&
                !source_side[v]) {

                source_side[v] = 1;
                stack[top++] = v;
            }
        }
    }

    free(stack);
    return 1;
}

long long cut_capacity(
    int vertices,
    const struct FlowEdge edges[],
    int edge_count,
    const int source_side[]
) {
    if (vertices <= 0 ||
        edge_count < 0 ||
        source_side == NULL ||
        (edge_count > 0 && edges == NULL)) {

        return -1;
    }

    long long capacity = 0;

    for (int i = 0; i < edge_count; i++) {
        if (edges[i].from < 0 ||
            edges[i].from >= vertices ||
            edges[i].to < 0 ||
            edges[i].to >= vertices ||
            edges[i].capacity < 0) {

            return -1;
        }

        if (source_side[edges[i].from] &&
            !source_side[edges[i].to]) {

            if ((long long)edges[i].capacity >
                LLONG_MAX - capacity) {

                return -1;
            }

            capacity += edges[i].capacity;
        }
    }

    return capacity;
}
/* =========================================================
   BIPARTITE MATCHING
   ========================================================= */

static int validate_bipartite_input(
    int left_count,
    int right_count,
    const struct BipartiteEdge edges[],
    int edge_count,
    int left_match[],
    int right_match[]
) {
    if (left_count < 0 ||
        right_count < 0 ||
        edge_count < 0) {

        return 0;
    }

    if ((left_count > 0 && left_match == NULL) ||
        (right_count > 0 && right_match == NULL) ||
        (edge_count > 0 && edges == NULL)) {

        return 0;
    }

    for (int i = 0; i < edge_count; i++) {
        if (edges[i].left < 0 ||
            edges[i].left >= left_count ||
            edges[i].right < 0 ||
            edges[i].right >= right_count) {

            return 0;
        }
    }

    return 1;
}

static int matching_dfs(
    int left,
    int right_count,
    const unsigned char adjacency[],
    int seen[],
    int right_match[],
    long long *operations
) {
    for (int right = 0;
         right < right_count;
         right++) {

        if (operations != NULL) {
            (*operations)++;
        }

        if (!adjacency[
                (size_t)left *
                (size_t)right_count +
                (size_t)right] ||
            seen[right]) {

            continue;
        }

        seen[right] = 1;

        if (right_match[right] == -1 ||
            matching_dfs(
                right_match[right],
                right_count,
                adjacency,
                seen,
                right_match,
                operations)) {

            right_match[right] = left;
            return 1;
        }
    }

    return 0;
}

int bipartite_maximum_matching(
    int left_count,
    int right_count,
    const struct BipartiteEdge edges[],
    int edge_count,
    int left_match[],
    int right_match[],
    long long *operations
) {
    if (operations != NULL) {
        *operations = 0;
    }

    if (!validate_bipartite_input(
            left_count,
            right_count,
            edges,
            edge_count,
            left_match,
            right_match)) {

        return -1;
    }

    for (int i = 0; i < left_count; i++) {
        left_match[i] = -1;
    }

    for (int i = 0; i < right_count; i++) {
        right_match[i] = -1;
    }

    if (left_count == 0 ||
        right_count == 0) {

        return 0;
    }

    size_t cells =
        (size_t)left_count *
        (size_t)right_count;

    if (left_count != 0 &&
        cells / (size_t)left_count !=
        (size_t)right_count) {

        return -1;
    }

    unsigned char *adjacency =
        calloc(cells, sizeof(unsigned char));

    int *seen =
        malloc(
            (size_t)right_count *
            sizeof(int)
        );

    if (adjacency == NULL || seen == NULL) {
        free(adjacency);
        free(seen);
        return -1;
    }

    for (int i = 0; i < edge_count; i++) {
        adjacency[
            (size_t)edges[i].left *
            (size_t)right_count +
            (size_t)edges[i].right
        ] = 1;
    }

    int matching_size = 0;

    for (int left = 0;
         left < left_count;
         left++) {

        memset(
            seen,
            0,
            (size_t)right_count *
            sizeof(int)
        );

        if (matching_dfs(
                left,
                right_count,
                adjacency,
                seen,
                right_match,
                operations)) {

            matching_size++;
        }
    }

    /*
     * Convert right-side representation into
     * the corresponding left-side mapping.
     */
    for (int right = 0;
         right < right_count;
         right++) {

        if (right_match[right] != -1) {
            left_match[
                right_match[right]
            ] = right;
        }
    }

    free(adjacency);
    free(seen);

    return matching_size;
}

int bipartite_greedy_matching(
    int left_count,
    int right_count,
    const struct BipartiteEdge edges[],
    int edge_count,
    int left_match[],
    int right_match[],
    long long *operations
) {
    if (operations != NULL) {
        *operations = 0;
    }

    if (!validate_bipartite_input(
            left_count,
            right_count,
            edges,
            edge_count,
            left_match,
            right_match)) {

        return -1;
    }

    for (int i = 0; i < left_count; i++) {
        left_match[i] = -1;
    }

    for (int i = 0; i < right_count; i++) {
        right_match[i] = -1;
    }

    int matching_size = 0;

    for (int i = 0; i < edge_count; i++) {
        if (operations != NULL) {
            (*operations)++;
        }

        int left = edges[i].left;
        int right = edges[i].right;

        if (left_match[left] == -1 &&
            right_match[right] == -1) {

            left_match[left] = right;
            right_match[right] = left;
            matching_size++;
        }
    }

    return matching_size;
}