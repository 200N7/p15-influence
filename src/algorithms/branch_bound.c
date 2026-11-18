#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include "algorithms/branch_bound.h"

struct SortedBBItem {
    int weight;
    long long value;
    double ratio;
    int original_index;
};

struct BBNode {
    int level;
    int weight;
    long long value;
    double bound;
    unsigned long long mask;
};

static int compare_ratio_desc(
    const void *left,
    const void *right
) {
    const struct SortedBBItem *a = left;
    const struct SortedBBItem *b = right;

    if (a->ratio > b->ratio) {
        return -1;
    }

    if (a->ratio < b->ratio) {
        return 1;
    }

    if (a->weight < b->weight) {
        return -1;
    }

    if (a->weight > b->weight) {
        return 1;
    }

    return a->original_index - b->original_index;
}

/*
 * Fractional-knapsack relaxation.
 *
 * Since fractional knapsack can only do at least as well
 * as 0/1 knapsack from the same node, this is a valid
 * optimistic upper bound for maximization.
 */
static double calculate_bound(
    const struct SortedBBItem items[],
    int n,
    int capacity,
    const struct BBNode *node
) {
    if (node->weight > capacity) {
        return -1.0;
    }

    double bound = (double)node->value;
    int total_weight = node->weight;

    int i = node->level + 1;

    while (i < n &&
           total_weight + items[i].weight <= capacity) {

        total_weight += items[i].weight;
        bound += (double)items[i].value;
        i++;
    }

    if (i < n && total_weight < capacity) {
        int remaining = capacity - total_weight;

        bound +=
            (double)remaining * items[i].ratio;
    }

    return bound;
}

/* =========================================================
   FIFO / LIFO FRONTIER
   ========================================================= */

struct LinearFrontier {
    struct BBNode *data;
    int size;
    int capacity;
    int head;
};

static int linear_push(
    struct LinearFrontier *frontier,
    struct BBNode node
) {
    if (frontier->size >= frontier->capacity) {
        int new_capacity =
            frontier->capacity == 0
                ? 32
                : frontier->capacity * 2;

        struct BBNode *new_data =
            realloc(
                frontier->data,
                (size_t)new_capacity *
                sizeof(*new_data)
            );

        if (new_data == NULL) {
            return 0;
        }

        frontier->data = new_data;
        frontier->capacity = new_capacity;
    }

    frontier->data[frontier->size++] = node;
    return 1;
}

static int fifo_pop(
    struct LinearFrontier *frontier,
    struct BBNode *node
) {
    if (frontier->head >= frontier->size) {
        return 0;
    }

    *node = frontier->data[frontier->head++];
    return 1;
}

static int lifo_pop(
    struct LinearFrontier *frontier,
    struct BBNode *node
) {
    if (frontier->size <= 0) {
        return 0;
    }

    frontier->size--;
    *node = frontier->data[frontier->size];

    return 1;
}

/* =========================================================
   BEST-BOUND PRIORITY QUEUE
   ========================================================= */

struct BBHeap {
    struct BBNode *data;
    int size;
    int capacity;
};

static int node_better(
    struct BBNode a,
    struct BBNode b
) {
    if (a.bound > b.bound) {
        return 1;
    }

    if (a.bound < b.bound) {
        return 0;
    }

    /*
     * Tie-break toward the node with greater current value.
     */
    return a.value > b.value;
}

static void node_swap(
    struct BBNode *a,
    struct BBNode *b
) {
    struct BBNode temp = *a;
    *a = *b;
    *b = temp;
}

static int heap_push_node(
    struct BBHeap *heap,
    struct BBNode node
) {
    if (heap->size >= heap->capacity) {
        int new_capacity =
            heap->capacity == 0
                ? 32
                : heap->capacity * 2;

        struct BBNode *new_data =
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
    heap->data[i] = node;

    while (i > 0) {
        int parent = (i - 1) / 2;

        if (!node_better(
                heap->data[i],
                heap->data[parent])) {

            break;
        }

        node_swap(
            &heap->data[i],
            &heap->data[parent]
        );

        i = parent;
    }

    return 1;
}

static int heap_pop_node(
    struct BBHeap *heap,
    struct BBNode *node
) {
    if (heap->size <= 0) {
        return 0;
    }

    *node = heap->data[0];

    heap->size--;

    if (heap->size == 0) {
        return 1;
    }

    heap->data[0] = heap->data[heap->size];

    int i = 0;

    while (1) {
        int left = 2 * i + 1;
        int right = 2 * i + 2;
        int best = i;

        if (left < heap->size &&
            node_better(
                heap->data[left],
                heap->data[best])) {

            best = left;
        }

        if (right < heap->size &&
            node_better(
                heap->data[right],
                heap->data[best])) {

            best = right;
        }

        if (best == i) {
            break;
        }

        node_swap(
            &heap->data[i],
            &heap->data[best]
        );

        i = best;
    }

    return 1;
}

/* =========================================================
   SHARED BRANCH-AND-BOUND ENGINE
   ========================================================= */

long long knapsack_branch_bound(
    const struct BBItem items[],
    int n,
    int capacity,
    enum BBStrategy strategy,
    int selected[],
    long long *nodes_expanded
) {
    if (nodes_expanded != NULL) {
        *nodes_expanded = 0;
    }

    if (n < 0 ||
        n > 63 ||
        capacity < 0 ||
        strategy < BB_FIFO ||
        strategy > BB_BEST_BOUND ||
        (n > 0 &&
         (items == NULL ||
          selected == NULL))) {

        return -1;
    }

    if (n == 0) {
        return 0;
    }

    struct SortedBBItem *sorted =
        malloc(
            (size_t)n *
            sizeof(*sorted)
        );

    if (sorted == NULL) {
        return -1;
    }

    for (int i = 0; i < n; i++) {
        if (items[i].weight < 0 ||
            items[i].value < 0) {

            free(sorted);
            return -1;
        }

        selected[i] = 0;

        sorted[i].weight = items[i].weight;
        sorted[i].value = items[i].value;
        sorted[i].original_index = i;

        if (items[i].weight == 0) {
            sorted[i].ratio =
                items[i].value > 0
                    ? 1.0e300
                    : 0.0;
        } else {
            sorted[i].ratio =
                (double)items[i].value /
                (double)items[i].weight;
        }
    }

    qsort(
        sorted,
        (size_t)n,
        sizeof(*sorted),
        compare_ratio_desc
    );

    struct BBNode root;

    root.level = -1;
    root.weight = 0;
    root.value = 0;
    root.mask = 0ULL;

    root.bound =
        calculate_bound(
            sorted,
            n,
            capacity,
            &root
        );

    long long best_value = 0;
    unsigned long long best_mask = 0ULL;

    struct LinearFrontier linear = {
        NULL, 0, 0, 0
    };

    struct BBHeap heap = {
        NULL, 0, 0
    };

    int push_ok;

    if (strategy == BB_BEST_BOUND) {
        push_ok =
            heap_push_node(
                &heap,
                root
            );
    } else {
        push_ok =
            linear_push(
                &linear,
                root
            );
    }

    if (!push_ok) {
        free(sorted);
        free(linear.data);
        free(heap.data);
        return -1;
    }

    while (1) {
        struct BBNode node;
        int have_node;

        if (strategy == BB_FIFO) {
            have_node =
                fifo_pop(
                    &linear,
                    &node
                );
        } else if (strategy == BB_LIFO) {
            have_node =
                lifo_pop(
                    &linear,
                    &node
                );
        } else {
            have_node =
                heap_pop_node(
                    &heap,
                    &node
                );
        }

        if (!have_node) {
            break;
        }

        if (nodes_expanded != NULL) {
            (*nodes_expanded)++;
        }

        /*
         * Bound cannot improve current best.
         */
        if (node.bound <=
            (double)best_value) {

            continue;
        }

        int next_level = node.level + 1;

        if (next_level >= n) {
            continue;
        }

        /* ---------------------------------------------
           INCLUDE branch
           --------------------------------------------- */

        struct BBNode include = node;

        include.level = next_level;

        if (sorted[next_level].weight <=
            capacity - include.weight) {

            include.weight +=
                sorted[next_level].weight;

            if (sorted[next_level].value >
                LLONG_MAX - include.value) {

                free(sorted);
                free(linear.data);
                free(heap.data);
                return -1;
            }

            include.value +=
                sorted[next_level].value;

            include.mask |=
                1ULL <<
                (unsigned int)next_level;

            if (include.value > best_value) {
                best_value = include.value;
                best_mask = include.mask;
            }

            include.bound =
                calculate_bound(
                    sorted,
                    n,
                    capacity,
                    &include
                );

            if (include.bound >
                (double)best_value) {

                int ok;

                if (strategy ==
                    BB_BEST_BOUND) {

                    ok = heap_push_node(
                        &heap,
                        include
                    );
                } else {
                    ok = linear_push(
                        &linear,
                        include
                    );
                }

                if (!ok) {
                    free(sorted);
                    free(linear.data);
                    free(heap.data);
                    return -1;
                }
            }
        }

        /* ---------------------------------------------
           EXCLUDE branch
           --------------------------------------------- */

        struct BBNode exclude = node;

        exclude.level = next_level;

        exclude.bound =
            calculate_bound(
                sorted,
                n,
                capacity,
                &exclude
            );

        if (exclude.bound >
            (double)best_value) {

            int ok;

            if (strategy ==
                BB_BEST_BOUND) {

                ok = heap_push_node(
                    &heap,
                    exclude
                );
            } else {
                ok = linear_push(
                    &linear,
                    exclude
                );
            }

            if (!ok) {
                free(sorted);
                free(linear.data);
                free(heap.data);
                return -1;
            }
        }
    }

    /*
     * Convert mask from sorted-item positions back
     * to original item positions.
     */
    for (int i = 0; i < n; i++) {
        selected[i] = 0;
    }

    for (int i = 0; i < n; i++) {
        if (best_mask &
            (1ULL << (unsigned int)i)) {

            selected[
                sorted[i].original_index
            ] = 1;
        }
    }

    free(sorted);
    free(linear.data);
    free(heap.data);

    return best_value;
}