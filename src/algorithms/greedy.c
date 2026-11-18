#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include "algorithms/greedy.h"

/* =========================================================
   ACTIVITY SELECTION
   ========================================================= */

struct ActivityEntry {
    struct Activity activity;
    int original_index;
};

static int activity_entry_compare(
    const void *left,
    const void *right
) {
    const struct ActivityEntry *a = left;
    const struct ActivityEntry *b = right;

    if (a->activity.finish < b->activity.finish) {
        return -1;
    }

    if (a->activity.finish > b->activity.finish) {
        return 1;
    }

    if (a->activity.start < b->activity.start) {
        return -1;
    }

    if (a->activity.start > b->activity.start) {
        return 1;
    }

    return a->original_index - b->original_index;
}

int activity_selection(
    const struct Activity activities[],
    int n,
    int selected_indices[],
    long long *comparisons
) {
    if (comparisons != NULL) {
        *comparisons = 0;
    }

    if (n < 0 ||
        (n > 0 &&
         (activities == NULL ||
          selected_indices == NULL))) {

        return -1;
    }

    if (n == 0) {
        return 0;
    }

    struct ActivityEntry *sorted =
        malloc((size_t)n * sizeof(*sorted));

    if (sorted == NULL) {
        return -1;
    }

    for (int i = 0; i < n; i++) {
        if (activities[i].finish < activities[i].start) {
            free(sorted);
            return -1;
        }

        sorted[i].activity = activities[i];
        sorted[i].original_index = i;
    }

    qsort(
        sorted,
        (size_t)n,
        sizeof(*sorted),
        activity_entry_compare
    );

    int count = 0;
    int last_finish = 0;
    int have_selected = 0;

    for (int i = 0; i < n; i++) {
        if (comparisons != NULL) {
            (*comparisons)++;
        }

        if (!have_selected ||
            sorted[i].activity.start >= last_finish) {

            selected_indices[count++] =
                sorted[i].original_index;

            last_finish = sorted[i].activity.finish;
            have_selected = 1;
        }
    }

    free(sorted);
    return count;
}

/* =========================================================
   INTERVAL PARTITIONING
   ========================================================= */

static int activity_start_compare(
    const void *left,
    const void *right
) {
    const struct ActivityEntry *a = left;
    const struct ActivityEntry *b = right;

    if (a->activity.start < b->activity.start) {
        return -1;
    }

    if (a->activity.start > b->activity.start) {
        return 1;
    }

    if (a->activity.finish < b->activity.finish) {
        return -1;
    }

    if (a->activity.finish > b->activity.finish) {
        return 1;
    }

    return a->original_index - b->original_index;
}

int interval_partition(
    const struct Activity activities[],
    int n,
    int channel_assignment[],
    long long *comparisons
) {
    if (comparisons != NULL) {
        *comparisons = 0;
    }

    if (n < 0 ||
        (n > 0 &&
         (activities == NULL ||
          channel_assignment == NULL))) {

        return -1;
    }

    if (n == 0) {
        return 0;
    }

    struct ActivityEntry *sorted =
        malloc((size_t)n * sizeof(*sorted));

    int *channel_finish =
        malloc((size_t)n * sizeof(*channel_finish));

    if (sorted == NULL || channel_finish == NULL) {
        free(sorted);
        free(channel_finish);
        return -1;
    }

    for (int i = 0; i < n; i++) {
        if (activities[i].finish < activities[i].start) {
            free(sorted);
            free(channel_finish);
            return -1;
        }

        sorted[i].activity = activities[i];
        sorted[i].original_index = i;
        channel_assignment[i] = -1;
    }

    qsort(
        sorted,
        (size_t)n,
        sizeof(*sorted),
        activity_start_compare
    );

    int channel_count = 0;

    for (int i = 0; i < n; i++) {
        int chosen = -1;

        /*
         * Find a currently free channel.
         */
        for (int c = 0; c < channel_count; c++) {
            if (comparisons != NULL) {
                (*comparisons)++;
            }

            if (channel_finish[c] <=
                sorted[i].activity.start) {

                chosen = c;
                break;
            }
        }

        if (chosen == -1) {
            chosen = channel_count;
            channel_count++;
        }

        channel_finish[chosen] =
            sorted[i].activity.finish;

        channel_assignment[
            sorted[i].original_index
        ] = chosen;
    }

    free(sorted);
    free(channel_finish);

    return channel_count;
}

/* =========================================================
   FRACTIONAL KNAPSACK
   ========================================================= */

struct FractionalEntry {
    double value;
    double weight;
    double ratio;
    int original_index;
};

static int fractional_compare(
    const void *left,
    const void *right
) {
    const struct FractionalEntry *a = left;
    const struct FractionalEntry *b = right;

    if (a->ratio > b->ratio) {
        return -1;
    }

    if (a->ratio < b->ratio) {
        return 1;
    }

    return a->original_index - b->original_index;
}

double fractional_knapsack(
    const struct FractionalItem items[],
    int n,
    double capacity,
    double fractions[],
    long long *comparisons
) {
    if (comparisons != NULL) {
        *comparisons = 0;
    }

    if (n < 0 ||
        capacity < 0.0 ||
        (n > 0 &&
         (items == NULL ||
          fractions == NULL))) {

        return -1.0;
    }

    if (n == 0 || capacity == 0.0) {
        if (fractions != NULL) {
            for (int i = 0; i < n; i++) {
                fractions[i] = 0.0;
            }
        }

        return 0.0;
    }

    struct FractionalEntry *sorted =
        malloc((size_t)n * sizeof(*sorted));

    if (sorted == NULL) {
        return -1.0;
    }

    for (int i = 0; i < n; i++) {
        if (items[i].weight <= 0.0 ||
            items[i].value < 0.0) {

            free(sorted);
            return -1.0;
        }

        fractions[i] = 0.0;

        sorted[i].value = items[i].value;
        sorted[i].weight = items[i].weight;
        sorted[i].ratio =
            items[i].value / items[i].weight;
        sorted[i].original_index = i;
    }

    qsort(
        sorted,
        (size_t)n,
        sizeof(*sorted),
        fractional_compare
    );

    double remaining = capacity;
    double total_value = 0.0;

    for (int i = 0;
         i < n && remaining > 0.0;
         i++) {

        if (comparisons != NULL) {
            (*comparisons)++;
        }

        double fraction;

        if (sorted[i].weight <= remaining) {
            fraction = 1.0;
        } else {
            fraction =
                remaining / sorted[i].weight;
        }

        fractions[
            sorted[i].original_index
        ] = fraction;

        total_value +=
            fraction * sorted[i].value;

        remaining -=
            fraction * sorted[i].weight;
    }

    free(sorted);
    return total_value;
}

/* =========================================================
   HUFFMAN CODING
   ========================================================= */

struct HuffmanNode {
    unsigned long frequency;
    int symbol;
    int smallest_symbol;
    struct HuffmanNode *left;
    struct HuffmanNode *right;
};

static struct HuffmanNode *new_huffman_node(
    unsigned long frequency,
    int symbol
) {
    struct HuffmanNode *node =
        malloc(sizeof(*node));

    if (node == NULL) {
        return NULL;
    }

    node->frequency = frequency;
    node->symbol = symbol;
    node->smallest_symbol =
        symbol >= 0 ? symbol : 256;

    node->left = NULL;
    node->right = NULL;

    return node;
}

static void free_huffman_tree(
    struct HuffmanNode *node
) {
    if (node == NULL) {
        return;
    }

    free_huffman_tree(node->left);
    free_huffman_tree(node->right);
    free(node);
}

static int huffman_node_less(
    const struct HuffmanNode *a,
    const struct HuffmanNode *b
) {
    if (a->frequency != b->frequency) {
        return a->frequency < b->frequency;
    }

    return a->smallest_symbol <
           b->smallest_symbol;
}

static void heap_swap(
    struct HuffmanNode **a,
    struct HuffmanNode **b
) {
    struct HuffmanNode *temp = *a;
    *a = *b;
    *b = temp;
}

static void heap_push(
    struct HuffmanNode *heap[],
    int *size,
    struct HuffmanNode *node,
    long long *operations
) {
    int i = (*size)++;
    heap[i] = node;

    while (i > 0) {
        int parent = (i - 1) / 2;

        if (operations != NULL) {
            (*operations)++;
        }

        if (!huffman_node_less(
                heap[i],
                heap[parent])) {

            break;
        }

        heap_swap(
            &heap[i],
            &heap[parent]
        );

        i = parent;
    }
}

static struct HuffmanNode *heap_pop(
    struct HuffmanNode *heap[],
    int *size,
    long long *operations
) {
    if (*size <= 0) {
        return NULL;
    }

    struct HuffmanNode *result = heap[0];

    (*size)--;

    if (*size == 0) {
        return result;
    }

    heap[0] = heap[*size];

    int i = 0;

    while (1) {
        int left = 2 * i + 1;
        int right = 2 * i + 2;
        int smallest = i;

        if (left < *size) {
            if (operations != NULL) {
                (*operations)++;
            }

            if (huffman_node_less(
                    heap[left],
                    heap[smallest])) {

                smallest = left;
            }
        }

        if (right < *size) {
            if (operations != NULL) {
                (*operations)++;
            }

            if (huffman_node_less(
                    heap[right],
                    heap[smallest])) {

                smallest = right;
            }
        }

        if (smallest == i) {
            break;
        }

        heap_swap(
            &heap[i],
            &heap[smallest]
        );

        i = smallest;
    }

    return result;
}

static int build_codes_recursive(
    const struct HuffmanNode *node,
    char path[],
    int depth,
    struct HuffmanCode codes[],
    int *count
) {
    if (node == NULL) {
        return 1;
    }

    if (node->left == NULL &&
        node->right == NULL) {

        codes[*count].symbol =
            (unsigned char)node->symbol;

        if (depth == 0) {
            codes[*count].code[0] = '0';
            codes[*count].code[1] = '\0';
        } else {
            memcpy(
                codes[*count].code,
                path,
                (size_t)depth
            );

            codes[*count].code[depth] = '\0';
        }

        (*count)++;
        return 1;
    }

    if (depth >= 255) {
        return 0;
    }

    path[depth] = '0';

    if (!build_codes_recursive(
            node->left,
            path,
            depth + 1,
            codes,
            count)) {

        return 0;
    }

    path[depth] = '1';

    return build_codes_recursive(
        node->right,
        path,
        depth + 1,
        codes,
        count
    );
}

int huffman_build_codes(
    const unsigned long frequencies[256],
    struct HuffmanCode codes[256],
    long long *operations
) {
    if (operations != NULL) {
        *operations = 0;
    }

    if (frequencies == NULL ||
        codes == NULL) {

        return -1;
    }

    struct HuffmanNode *heap[512];
    int heap_size = 0;
    int symbol_count = 0;

    for (int i = 0; i < 256; i++) {
        if (frequencies[i] == 0) {
            continue;
        }

        struct HuffmanNode *node =
            new_huffman_node(
                frequencies[i],
                i
            );

        if (node == NULL) {
            while (heap_size > 0) {
                free_huffman_tree(
                    heap_pop(
                        heap,
                        &heap_size,
                        NULL
                    )
                );
            }

            return -1;
        }

        heap_push(
            heap,
            &heap_size,
            node,
            operations
        );

        symbol_count++;
    }

    if (symbol_count == 0) {
        return 0;
    }

    while (heap_size > 1) {
        struct HuffmanNode *left =
            heap_pop(
                heap,
                &heap_size,
                operations
            );

        struct HuffmanNode *right =
            heap_pop(
                heap,
                &heap_size,
                operations
            );

        if (left == NULL || right == NULL) {
            free_huffman_tree(left);
            free_huffman_tree(right);
            return -1;
        }

        if (ULONG_MAX - left->frequency <
            right->frequency) {

            free_huffman_tree(left);
            free_huffman_tree(right);

            while (heap_size > 0) {
                free_huffman_tree(
                    heap_pop(
                        heap,
                        &heap_size,
                        NULL
                    )
                );
            }

            return -1;
        }

        struct HuffmanNode *parent =
            new_huffman_node(
                left->frequency +
                right->frequency,
                -1
            );

        if (parent == NULL) {
            free_huffman_tree(left);
            free_huffman_tree(right);

            while (heap_size > 0) {
                free_huffman_tree(
                    heap_pop(
                        heap,
                        &heap_size,
                        NULL
                    )
                );
            }

            return -1;
        }

        parent->left = left;
        parent->right = right;

        parent->smallest_symbol =
            left->smallest_symbol <
            right->smallest_symbol
                ? left->smallest_symbol
                : right->smallest_symbol;

        heap_push(
            heap,
            &heap_size,
            parent,
            operations
        );
    }

    struct HuffmanNode *root =
        heap_pop(
            heap,
            &heap_size,
            operations
        );

    char path[256];
    int count = 0;

    int ok = build_codes_recursive(
        root,
        path,
        0,
        codes,
        &count
    );

    free_huffman_tree(root);

    if (!ok) {
        return -1;
    }

    return count;
}

int huffman_is_prefix_free(
    const struct HuffmanCode codes[],
    int count
) {
    if (count < 0 ||
        (count > 0 && codes == NULL)) {

        return 0;
    }

    for (int i = 0; i < count; i++) {
        size_t len_i = strlen(codes[i].code);

        for (int j = 0; j < count; j++) {
            if (i == j) {
                continue;
            }

            size_t len_j =
                strlen(codes[j].code);

            if (len_i <= len_j &&
                strncmp(
                    codes[i].code,
                    codes[j].code,
                    len_i
                ) == 0) {

                return 0;
            }
        }
    }

    return 1;

}
/* =========================================================
   DIJKSTRA SHORTEST PATH
   ========================================================= */

#include <float.h>

/*
 * Validate a weighted directed graph.
 */
static int validate_weighted_graph(
    int vertices,
    const struct WeightedEdge edges[],
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

        if (edges[i].weight < 0.0) {
            return -1;
        }
    }

    return 1;
}

/* =========================================================
   ARRAY-BASED DIJKSTRA
   ========================================================= */

int dijkstra_array(
    int vertices,
    const struct WeightedEdge edges[],
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

    int valid = validate_weighted_graph(
        vertices,
        edges,
        edge_count,
        source
    );

    if (valid != 1) {
        return valid;
    }

    int *visited =
        calloc((size_t)vertices, sizeof(int));

    if (visited == NULL) {
        return 0;
    }

    for (int i = 0; i < vertices; i++) {
        distance[i] = DBL_MAX;
        previous[i] = -1;
    }

    distance[source] = 0.0;

    for (int step = 0; step < vertices; step++) {
        int u = -1;
        double best = DBL_MAX;

        /*
         * Select unvisited vertex with minimum
         * tentative distance.
         */
        for (int i = 0; i < vertices; i++) {
            if (!visited[i] &&
                distance[i] < best) {

                best = distance[i];
                u = i;
            }
        }

        /*
         * Remaining vertices are unreachable.
         */
        if (u == -1) {
            break;
        }

        visited[u] = 1;

        /*
         * Relax outgoing edges of u.
         */
        for (int i = 0; i < edge_count; i++) {
            if (edges[i].from != u) {
                continue;
            }

            if (relaxations != NULL) {
                (*relaxations)++;
            }

            int v = edges[i].to;

            if (distance[u] != DBL_MAX) {
                double candidate =
                    distance[u] + edges[i].weight;

                if (candidate < distance[v]) {
                    distance[v] = candidate;
                    previous[v] = u;
                }
            }
        }
    }

    free(visited);
    return 1;
}

/* =========================================================
   BINARY MIN-HEAP DIJKSTRA
   ========================================================= */

struct DijkstraHeapNode {
    int vertex;
    double distance;
};

struct DijkstraHeap {
    struct DijkstraHeapNode *data;
    int size;
    int capacity;
};

static int dijkstra_heap_less(
    struct DijkstraHeapNode a,
    struct DijkstraHeapNode b
) {
    if (a.distance < b.distance) {
        return 1;
    }

    if (a.distance > b.distance) {
        return 0;
    }

    return a.vertex < b.vertex;
}

static void dijkstra_heap_swap(
    struct DijkstraHeapNode *a,
    struct DijkstraHeapNode *b
) {
    struct DijkstraHeapNode temp = *a;
    *a = *b;
    *b = temp;
}

static int dijkstra_heap_push(
    struct DijkstraHeap *heap,
    int vertex,
    double distance
) {
    if (heap->size >= heap->capacity) {
        int new_capacity =
            heap->capacity == 0
                ? 16
                : heap->capacity * 2;

        struct DijkstraHeapNode *new_data =
            realloc(
                heap->data,
                (size_t)new_capacity *
                sizeof(*new_data)
            );

        if (new_data == NULL) {
            return 0;
        }

        heap->data = new_data;
        heap->capacity = new_capacity;
    }

    int i = heap->size++;

    heap->data[i].vertex = vertex;
    heap->data[i].distance = distance;

    while (i > 0) {
        int parent = (i - 1) / 2;

        if (!dijkstra_heap_less(
                heap->data[i],
                heap->data[parent])) {

            break;
        }

        dijkstra_heap_swap(
            &heap->data[i],
            &heap->data[parent]
        );

        i = parent;
    }

    return 1;
}

static int dijkstra_heap_pop(
    struct DijkstraHeap *heap,
    struct DijkstraHeapNode *result
) {
    if (heap->size <= 0 || result == NULL) {
        return 0;
    }

    *result = heap->data[0];

    heap->size--;

    if (heap->size == 0) {
        return 1;
    }

    heap->data[0] =
        heap->data[heap->size];

    int i = 0;

    while (1) {
        int left = 2 * i + 1;
        int right = 2 * i + 2;
        int smallest = i;

        if (left < heap->size &&
            dijkstra_heap_less(
                heap->data[left],
                heap->data[smallest])) {

            smallest = left;
        }

        if (right < heap->size &&
            dijkstra_heap_less(
                heap->data[right],
                heap->data[smallest])) {

            smallest = right;
        }

        if (smallest == i) {
            break;
        }

        dijkstra_heap_swap(
            &heap->data[i],
            &heap->data[smallest]
        );

        i = smallest;
    }

    return 1;
}

int dijkstra_heap(
    int vertices,
    const struct WeightedEdge edges[],
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

    int valid = validate_weighted_graph(
        vertices,
        edges,
        edge_count,
        source
    );

    if (valid != 1) {
        return valid;
    }

    /*
     * Build adjacency lists using arrays.
     */
    int *head =
        malloc((size_t)vertices * sizeof(int));

    int *next = NULL;

    if (edge_count > 0) {
        next =
            malloc((size_t)edge_count * sizeof(int));
    }

    if (head == NULL ||
        (edge_count > 0 && next == NULL)) {

        free(head);
        free(next);
        return 0;
    }

    for (int i = 0; i < vertices; i++) {
        head[i] = -1;
        distance[i] = DBL_MAX;
        previous[i] = -1;
    }

    for (int i = 0; i < edge_count; i++) {
        next[i] = head[edges[i].from];
        head[edges[i].from] = i;
    }

    struct DijkstraHeap heap;

    heap.data = NULL;
    heap.size = 0;
    heap.capacity = 0;

    distance[source] = 0.0;

    if (!dijkstra_heap_push(
            &heap,
            source,
            0.0)) {

        free(head);
        free(next);
        return 0;
    }

    while (heap.size > 0) {
        struct DijkstraHeapNode node;

        dijkstra_heap_pop(
            &heap,
            &node
        );

        int u = node.vertex;

        /*
         * Ignore stale heap entries.
         */
        if (node.distance > distance[u]) {
            continue;
        }

        for (int edge_index = head[u];
             edge_index != -1;
             edge_index = next[edge_index]) {

            if (relaxations != NULL) {
                (*relaxations)++;
            }

            int v = edges[edge_index].to;

            double candidate =
                distance[u] +
                edges[edge_index].weight;

            if (candidate < distance[v]) {
                distance[v] = candidate;
                previous[v] = u;

                if (!dijkstra_heap_push(
                        &heap,
                        v,
                        candidate)) {

                    free(heap.data);
                    free(head);
                    free(next);

                    return 0;
                }
            }
        }
    }

    free(heap.data);
    free(head);
    free(next);

    return 1;
}

/* =========================================================
   PATH RECONSTRUCTION
   ========================================================= */

int reconstruct_path(
    int source,
    int target,
    const int previous[],
    int vertices,
    int path[]
) {
    if (previous == NULL ||
        path == NULL ||
        vertices <= 0 ||
        source < 0 ||
        source >= vertices ||
        target < 0 ||
        target >= vertices) {

        return 0;
    }

    int reverse_path[1000];

    if (vertices > 1000) {
        return 0;
    }

    int count = 0;
    int current = target;

    while (current != -1 &&
           count < vertices) {

        reverse_path[count++] = current;

        if (current == source) {
            break;
        }

        current = previous[current];
    }

    if (count == 0 ||
        reverse_path[count - 1] != source) {

        return 0;
    }

    for (int i = 0; i < count; i++) {
        path[i] =
            reverse_path[count - 1 - i];
    }

    return count;
}
/* =========================================================
   PRIM / KRUSKAL / K-CLUSTERING
   ========================================================= */

struct MstEdgeEntry {
    struct UndirectedEdge edge;
    int original_index;
};

static int mst_edge_compare(
    const void *left,
    const void *right
) {
    const struct MstEdgeEntry *a = left;
    const struct MstEdgeEntry *b = right;

    if (a->edge.weight < b->edge.weight) {
        return -1;
    }

    if (a->edge.weight > b->edge.weight) {
        return 1;
    }

    if (a->edge.u != b->edge.u) {
        return a->edge.u - b->edge.u;
    }

    if (a->edge.v != b->edge.v) {
        return a->edge.v - b->edge.v;
    }

    return a->original_index - b->original_index;
}

static int validate_undirected_graph(
    int vertices,
    const struct UndirectedEdge edges[],
    int edge_count
) {
    if (vertices <= 0 || edge_count < 0) {
        return 0;
    }

    if (edge_count > 0 && edges == NULL) {
        return 0;
    }

    for (int i = 0; i < edge_count; i++) {
        if (edges[i].u < 0 ||
            edges[i].u >= vertices ||
            edges[i].v < 0 ||
            edges[i].v >= vertices ||
            edges[i].u == edges[i].v) {

            return 0;
        }
    }

    return 1;
}

/* ---------------------------------------------------------
   UNION-FIND
   --------------------------------------------------------- */

static int uf_find(
    int parent[],
    int x
) {
    if (parent[x] != x) {
        parent[x] =
            uf_find(parent, parent[x]);
    }

    return parent[x];
}

static int uf_union(
    int parent[],
    int rank[],
    int a,
    int b
) {
    int root_a = uf_find(parent, a);
    int root_b = uf_find(parent, b);

    if (root_a == root_b) {
        return 0;
    }

    if (rank[root_a] < rank[root_b]) {
        parent[root_a] = root_b;
    } else if (rank[root_a] > rank[root_b]) {
        parent[root_b] = root_a;
    } else {
        parent[root_b] = root_a;
        rank[root_a]++;
    }

    return 1;
}

/* ---------------------------------------------------------
   PRIM MINIMUM SPANNING FOREST

   Array-based O(V^2 + VE) implementation.
   It deliberately supports disconnected graphs by
   restarting Prim from every unvisited component.
   --------------------------------------------------------- */

int prim_msf(
    int vertices,
    const struct UndirectedEdge edges[],
    int edge_count,
    struct UndirectedEdge forest_edges[],
    double *total_weight,
    long long *operations
) {
    if (operations != NULL) {
        *operations = 0;
    }

    if (total_weight == NULL ||
        (vertices > 1 && forest_edges == NULL) ||
        !validate_undirected_graph(
            vertices,
            edges,
            edge_count)) {

        return -1;
    }

    int *in_forest =
        calloc((size_t)vertices, sizeof(int));

    double *key =
        malloc((size_t)vertices * sizeof(double));

    int *parent =
        malloc((size_t)vertices * sizeof(int));

    if (in_forest == NULL ||
        key == NULL ||
        parent == NULL) {

        free(in_forest);
        free(key);
        free(parent);
        return -1;
    }

    for (int i = 0; i < vertices; i++) {
        key[i] = DBL_MAX;
        parent[i] = -1;
    }

    *total_weight = 0.0;
    int selected_count = 0;

    for (int start = 0; start < vertices; start++) {
        if (in_forest[start]) {
            continue;
        }

        key[start] = 0.0;

        while (1) {
            int u = -1;
            double best = DBL_MAX;

            for (int i = 0; i < vertices; i++) {
                if (!in_forest[i] &&
                    key[i] < best) {

                    best = key[i];
                    u = i;
                }
            }

            if (u == -1) {
                break;
            }

            in_forest[u] = 1;

            if (parent[u] != -1) {
                struct UndirectedEdge chosen;

                chosen.u = parent[u];
                chosen.v = u;
                chosen.weight = key[u];

                forest_edges[selected_count++] =
                    chosen;

                *total_weight += key[u];
            }

            for (int i = 0; i < edge_count; i++) {
                int v = -1;

                if (edges[i].u == u) {
                    v = edges[i].v;
                } else if (edges[i].v == u) {
                    v = edges[i].u;
                }

                if (v == -1 || in_forest[v]) {
                    continue;
                }

                if (operations != NULL) {
                    (*operations)++;
                }

                if (edges[i].weight < key[v]) {
                    key[v] = edges[i].weight;
                    parent[v] = u;
                }
            }
        }
    }

    free(in_forest);
    free(key);
    free(parent);

    return selected_count;
}

/* ---------------------------------------------------------
   KRUSKAL MINIMUM SPANNING FOREST
   --------------------------------------------------------- */

int kruskal_msf(
    int vertices,
    const struct UndirectedEdge edges[],
    int edge_count,
    struct UndirectedEdge forest_edges[],
    double *total_weight,
    long long *operations
) {
    if (operations != NULL) {
        *operations = 0;
    }

    if (total_weight == NULL ||
        (vertices > 1 && forest_edges == NULL) ||
        !validate_undirected_graph(
            vertices,
            edges,
            edge_count)) {

        return -1;
    }

    struct MstEdgeEntry *sorted = NULL;

    if (edge_count > 0) {
        sorted =
            malloc(
                (size_t)edge_count *
                sizeof(*sorted)
            );

        if (sorted == NULL) {
            return -1;
        }
    }

    int *parent =
        malloc((size_t)vertices * sizeof(int));

    int *rank =
        calloc((size_t)vertices, sizeof(int));

    if (parent == NULL || rank == NULL) {
        free(sorted);
        free(parent);
        free(rank);
        return -1;
    }

    for (int i = 0; i < vertices; i++) {
        parent[i] = i;
    }

    for (int i = 0; i < edge_count; i++) {
        sorted[i].edge = edges[i];
        sorted[i].original_index = i;
    }

    qsort(
        sorted,
        (size_t)edge_count,
        sizeof(*sorted),
        mst_edge_compare
    );

    *total_weight = 0.0;
    int selected_count = 0;

    for (int i = 0; i < edge_count; i++) {
        if (operations != NULL) {
            (*operations)++;
        }

        if (uf_union(
                parent,
                rank,
                sorted[i].edge.u,
                sorted[i].edge.v)) {

            forest_edges[selected_count++] =
                sorted[i].edge;

            *total_weight +=
                sorted[i].edge.weight;
        }
    }

    free(sorted);
    free(parent);
    free(rank);

    return selected_count;
}

/* ---------------------------------------------------------
   KRUSKAL K-CLUSTERING
   --------------------------------------------------------- */

int kruskal_k_clustering(
    int vertices,
    const struct UndirectedEdge edges[],
    int edge_count,
    int k,
    int labels[],
    long long *operations
) {
    if (operations != NULL) {
        *operations = 0;
    }

    if (labels == NULL ||
        k <= 0 ||
        k > vertices ||
        !validate_undirected_graph(
            vertices,
            edges,
            edge_count)) {

        return -1;
    }

    struct MstEdgeEntry *sorted = NULL;

    if (edge_count > 0) {
        sorted =
            malloc(
                (size_t)edge_count *
                sizeof(*sorted)
            );

        if (sorted == NULL) {
            return -1;
        }
    }

    int *parent =
        malloc((size_t)vertices * sizeof(int));

    int *rank =
        calloc((size_t)vertices, sizeof(int));

    if (parent == NULL || rank == NULL) {
        free(sorted);
        free(parent);
        free(rank);
        return -1;
    }

    for (int i = 0; i < vertices; i++) {
        parent[i] = i;
    }

    for (int i = 0; i < edge_count; i++) {
        sorted[i].edge = edges[i];
        sorted[i].original_index = i;
    }

    qsort(
        sorted,
        (size_t)edge_count,
        sizeof(*sorted),
        mst_edge_compare
    );

    int components = vertices;

    for (int i = 0;
         i < edge_count && components > k;
         i++) {

        if (operations != NULL) {
            (*operations)++;
        }

        if (uf_union(
                parent,
                rank,
                sorted[i].edge.u,
                sorted[i].edge.v)) {

            components--;
        }
    }

    /*
     * If the graph itself has more than k disconnected
     * components, no sequence of available edges can
     * reduce it to exactly k.
     */
    if (components > k) {
        free(sorted);
        free(parent);
        free(rank);
        return -1;
    }

    /*
     * Convert union-find roots into compact labels
     * 0,1,...,k-1.
     */
    int *root_to_label =
        malloc((size_t)vertices * sizeof(int));

    if (root_to_label == NULL) {
        free(sorted);
        free(parent);
        free(rank);
        return -1;
    }

    for (int i = 0; i < vertices; i++) {
        root_to_label[i] = -1;
    }

    int label_count = 0;

    for (int i = 0; i < vertices; i++) {
        int root = uf_find(parent, i);

        if (root_to_label[root] == -1) {
            root_to_label[root] =
                label_count++;
        }

        labels[i] =
            root_to_label[root];
    }

    free(root_to_label);
    free(sorted);
    free(parent);
    free(rank);

    return label_count;
}
