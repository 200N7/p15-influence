#ifndef DP_H
#define DP_H

struct KnapsackItem {
    int weight;
    long long value;
};

/*
 * Exact 0/1 knapsack using dynamic programming.
 *
 * selected[i] is set to 1 when item i belongs to
 * one reconstructed optimal solution, otherwise 0.
 *
 * Returns the optimal value.
 * Returns -1 for invalid input or allocation failure.
 */
long long knapsack_dp(
    const struct KnapsackItem items[],
    int n,
    int capacity,
    int selected[],
    long long *operations
);

/*
 * Matrix-chain multiplication.
 *
 * dimensions contains matrix_count + 1 entries.
 * Matrix i has dimensions:
 * dimensions[i] x dimensions[i + 1].
 *
 * split must have room for matrix_count * matrix_count ints.
 *
 * Returns minimum scalar multiplication count.
 * Returns -1 for invalid input/allocation/overflow.
 */
long long matrix_chain_order(
    const int dimensions[],
    int matrix_count,
    int split[],
    long long *operations
);

/*
 * Longest common subsequence.
 *
 * output receives one actual LCS and must have room
 * for at least min(strlen(a), strlen(b)) + 1 characters.
 *
 * Returns LCS length.
 * Returns -1 for invalid input or allocation failure.
 */
int lcs_sequence(
    const char *a,
    const char *b,
    char output[],
    long long *operations
);
/* =========================================================
   OPTIMAL BINARY SEARCH TREE
   ========================================================= */

/*
 * frequencies[i] is the successful-search frequency
 * of sorted key i.
 *
 * root_table must have room for n*n integers.
 *
 * Returns the minimum weighted search cost.
 * Returns -1 for invalid input/allocation/overflow.
 */
long long optimal_bst(
    const long long frequencies[],
    int n,
    int root_table[],
    long long *operations
);

/* =========================================================
   BELLMAN-FORD
   ========================================================= */

struct DPEdge {
    int from;
    int to;
    double weight;
};

/*
 * Bellman-Ford single-source shortest paths.
 *
 * distance and previous must each have room for V entries.
 *
 * Returns:
 *   1  success, no reachable negative cycle
 *  -1  reachable negative cycle detected
 *   0  invalid input/allocation failure
 */
int bellman_ford(
    int vertices,
    const struct DPEdge edges[],
    int edge_count,
    int source,
    double distance[],
    int previous[],
    long long *relaxations
);
/* =========================================================
   FLOYD-WARSHALL
   ========================================================= */

/*
 * All-pairs shortest paths.
 *
 * Input:
 *   vertices  - number of vertices
 *   edges     - directed weighted edge list
 *
 * Output:
 *   distance  - flat V x V distance matrix
 *   next      - flat V x V path reconstruction matrix
 *
 * Returns:
 *   1  success, no negative cycle
 *  -1  negative cycle detected
 *   0  invalid input
 */
int floyd_warshall(
    int vertices,
    const struct DPEdge edges[],
    int edge_count,
    double distance[],
    int next[],
    long long *operations
);

/*
 * Reconstruct path produced by Floyd-Warshall.
 *
 * path must have room for at least vertices + 1 entries.
 *
 * Returns number of vertices in path.
 * Returns 0 when unreachable/invalid.
 */
int floyd_reconstruct_path(
    int source,
    int target,
    const int next[],
    int vertices,
    int path[]
);

/* =========================================================
   HELD-KARP TSP
   ========================================================= */

/*
 * Exact travelling-salesperson solution using Held-Karp DP.
 *
 * distance is a flat n x n cost matrix.
 * start is the required starting/ending vertex.
 *
 * tour must have room for n + 1 integers.
 * On success it receives:
 *
 * start -> ... -> start
 *
 * Returns minimum tour cost.
 * Returns -1.0 for invalid input, impossible tour,
 * allocation failure, or instances larger than 20.
 */
double held_karp_tsp(
    const double distance[],
    int n,
    int start,
    int tour[],
    long long *operations
);

#endif