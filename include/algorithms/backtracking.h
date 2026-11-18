#ifndef BACKTRACKING_H
#define BACKTRACKING_H

struct BacktrackItem {
    int weight;
    long long value;
};

/*
 * N-Queens.
 *
 * Counts all solutions for an n x n board.
 * first_solution[row] receives the column of the queen
 * for one valid solution.
 *
 * Returns number of solutions.
 * Returns -1 for invalid input.
 *
 * n is deliberately limited to 14 to keep the
 * exponential search bounded for this workbench.
 */
long long n_queens(
    int n,
    int first_solution[],
    long long *nodes
);

/*
 * Exact subset-sum decision using backtracking.
 *
 * values must contain non-negative integers.
 *
 * selected[i] receives 1 for one subset producing target.
 *
 * Returns:
 *   1 solution exists
 *   0 no solution
 *  -1 invalid input
 */
int subset_sum_backtracking(
    const int values[],
    int n,
    int target,
    int selected[],
    long long *nodes
);

/*
 * Exact 0/1 knapsack using backtracking.
 *
 * selected[i] receives one optimal item set.
 *
 * Returns optimal value.
 * Returns -1 for invalid input.
 */
long long knapsack_backtracking(
    const struct BacktrackItem items[],
    int n,
    int capacity,
    int selected[],
    long long *nodes
);

#endif