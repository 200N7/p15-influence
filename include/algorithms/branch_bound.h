#ifndef BRANCH_BOUND_H
#define BRANCH_BOUND_H

struct BBItem {
    int weight;
    long long value;
};

enum BBStrategy {
    BB_FIFO = 0,
    BB_LIFO = 1,
    BB_BEST_BOUND = 2
};

/*
 * Exact 0/1 knapsack using Branch and Bound.
 *
 * The upper bound is computed using the fractional-knapsack
 * relaxation after sorting items by value/weight ratio.
 *
 * strategy:
 *   BB_FIFO       -> queue / breadth-first exploration
 *   BB_LIFO       -> stack / depth-first exploration
 *   BB_BEST_BOUND -> max-priority exploration by upper bound
 *
 * selected[i] receives one optimal solution using original
 * item indices.
 *
 * nodes_expanded receives the number of search nodes removed
 * from the frontier and processed.
 *
 * Returns optimal value.
 * Returns -1 for invalid input/allocation failure.
 */
long long knapsack_branch_bound(
    const struct BBItem items[],
    int n,
    int capacity,
    enum BBStrategy strategy,
    int selected[],
    long long *nodes_expanded
);

#endif