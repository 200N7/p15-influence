#ifndef GREEDY_H
#define GREEDY_H

struct Activity {
    int start;
    int finish;
};

struct FractionalItem {
    double value;
    double weight;
};

struct HuffmanCode {
    unsigned char symbol;
    char code[256];
};

/*
 * Activity selection.
 *
 * Returns the number of selected non-overlapping activities.
 * selected_indices receives indices from the original array.
 */
int activity_selection(
    const struct Activity activities[],
    int n,
    int selected_indices[],
    long long *comparisons
);

/*
 * Interval partitioning.
 *
 * Returns the minimum number of parallel channels required.
 * channel_assignment[i] gives the channel for activity i.
 */
int interval_partition(
    const struct Activity activities[],
    int n,
    int channel_assignment[],
    long long *comparisons
);

/*
 * Fractional knapsack.
 *
 * Returns maximum achievable value.
 * fractions[i] receives the fraction taken from original item i.
 *
 * Returns -1.0 for invalid input.
 */
double fractional_knapsack(
    const struct FractionalItem items[],
    int n,
    double capacity,
    double fractions[],
    long long *comparisons
);

/*
 * Build Huffman codes from symbol frequencies.
 *
 * frequencies contains 256 entries.
 * codes must have room for 256 HuffmanCode structures.
 *
 * Returns number of symbols encoded, or -1 on error.
 */
int huffman_build_codes(
    const unsigned long frequencies[256],
    struct HuffmanCode codes[256],
    long long *operations
);

/*
 * Verify that no generated Huffman code is a prefix
 * of another generated code.
 */
int huffman_is_prefix_free(
    const struct HuffmanCode codes[],
    int count
);
/* =========================================================
   DIJKSTRA SHORTEST PATH
   ========================================================= */

struct WeightedEdge {
    int from;
    int to;
    double weight;
};

/*
 * Array-based Dijkstra.
 *
 * vertices: number of vertices
 * edges: directed weighted edge list
 * edge_count: number of edges
 * source: source vertex
 *
 * distance and previous must each contain
 * at least vertices entries.
 *
 * Returns:
 *  1 on success
 *  0 on invalid input
 * -1 if a negative edge is detected
 */
int dijkstra_array(
    int vertices,
    const struct WeightedEdge edges[],
    int edge_count,
    int source,
    double distance[],
    int previous[],
    long long *relaxations
);

/*
 * Binary-heap Dijkstra.
 *
 * Same output contract as dijkstra_array.
 */
int dijkstra_heap(
    int vertices,
    const struct WeightedEdge edges[],
    int edge_count,
    int source,
    double distance[],
    int previous[],
    long long *relaxations
);

/*
 * Reconstruct source -> target path using a previous[]
 * array produced by a shortest-path algorithm.
 *
 * Returns number of vertices in path.
 * Returns 0 if target is unreachable or input is invalid.
 */
int reconstruct_path(
    int source,
    int target,
    const int previous[],
    int vertices,
    int path[]
);
/* =========================================================
   MINIMUM SPANNING TREE / FOREST
   ========================================================= */

struct UndirectedEdge {
    int u;
    int v;
    double weight;
};

/*
 * Prim minimum spanning forest.
 *
 * The graph is undirected.
 * forest_edges must have room for at least vertices - 1 edges.
 *
 * Returns number of selected forest edges.
 * Returns -1 on invalid input.
 *
 * total_weight receives the total forest weight.
 */
int prim_msf(
    int vertices,
    const struct UndirectedEdge edges[],
    int edge_count,
    struct UndirectedEdge forest_edges[],
    double *total_weight,
    long long *operations
);

/*
 * Kruskal minimum spanning forest.
 *
 * Same return contract as prim_msf.
 */
int kruskal_msf(
    int vertices,
    const struct UndirectedEdge edges[],
    int edge_count,
    struct UndirectedEdge forest_edges[],
    double *total_weight,
    long long *operations
);

/*
 * Kruskal k-clustering / single-linkage clustering.
 *
 * Processes edges in ascending weight order and stops
 * when exactly k connected components remain.
 *
 * labels[i] receives a component label in [0, k-1].
 *
 * Returns actual number of components on success.
 * Returns -1 on invalid input or when k components
 * cannot be formed under the supplied graph model.
 */
int kruskal_k_clustering(
    int vertices,
    const struct UndirectedEdge edges[],
    int edge_count,
    int k,
    int labels[],
    long long *operations
);
#endif