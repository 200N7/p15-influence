#ifndef FLOW_MATCHING_H
#define FLOW_MATCHING_H

struct FlowEdge {
    int from;
    int to;
    int capacity;
};

/*
 * Ford-Fulkerson using DFS augmenting paths.
 *
 * Capacities must be non-negative integers.
 *
 * flow_matrix and residual_matrix are flat V x V arrays
 * with room for vertices * vertices integers.
 *
 * Returns maximum flow.
 * Returns -1 for invalid input/allocation/overflow.
 */
long long ford_fulkerson(
    int vertices,
    const struct FlowEdge edges[],
    int edge_count,
    int source,
    int sink,
    long long flow_matrix[],
    long long residual_matrix[],
    long long *augmentations
);

/*
 * After Ford-Fulkerson, identify the source side of
 * the minimum cut using the final residual graph.
 *
 * source_side[i] = 1 if vertex i is reachable from
 * source through positive residual capacity.
 *
 * Returns 1 on success, 0 on invalid input.
 */
int min_cut_source_side(
    int vertices,
    int source,
    const long long residual_matrix[],
    int source_side[]
);

/*
 * Calculate the capacity of a cut.
 *
 * source_side identifies the source partition.
 *
 * Returns cut capacity, or -1 for invalid input.
 */
long long cut_capacity(
    int vertices,
    const struct FlowEdge edges[],
    int edge_count,
    const int source_side[]
);
struct BipartiteEdge {
    int left;
    int right;
};

/*
 * Exact maximum bipartite matching using augmenting paths.
 *
 * left_count  = number of influencer-side vertices
 * right_count = number of campaign-side vertices
 *
 * left_match[i] gives matched right vertex or -1.
 * right_match[j] gives matched left vertex or -1.
 *
 * Returns maximum matching size.
 * Returns -1 for invalid input/allocation failure.
 */
int bipartite_maximum_matching(
    int left_count,
    int right_count,
    const struct BipartiteEdge edges[],
    int edge_count,
    int left_match[],
    int right_match[],
    long long *operations
);

/*
 * Greedy baseline.
 *
 * Processes edges in input order and accepts an edge
 * whenever both endpoints are currently unmatched.
 *
 * Returns matching size or -1 on invalid input.
 */
int bipartite_greedy_matching(
    int left_count,
    int right_count,
    const struct BipartiteEdge edges[],
    int edge_count,
    int left_match[],
    int right_match[],
    long long *operations
);

#endif