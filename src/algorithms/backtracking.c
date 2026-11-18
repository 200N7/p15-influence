#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include "algorithms/backtracking.h"

/* =========================================================
   N-QUEENS
   ========================================================= */

static void nqueens_search(
    int n,
    int row,
    int columns[],
    int used_column[],
    int used_diag1[],
    int used_diag2[],
    int first_solution[],
    int *have_first,
    long long *solutions,
    long long *nodes
) {
    if (nodes != NULL) {
        (*nodes)++;
    }

    if (row == n) {
        (*solutions)++;

        if (!*have_first) {
            for (int i = 0; i < n; i++) {
                first_solution[i] = columns[i];
            }

            *have_first = 1;
        }

        return;
    }

    for (int col = 0; col < n; col++) {
        int diag1 = row - col + n - 1;
        int diag2 = row + col;

        if (used_column[col] ||
            used_diag1[diag1] ||
            used_diag2[diag2]) {

            continue;
        }

        columns[row] = col;
        used_column[col] = 1;
        used_diag1[diag1] = 1;
        used_diag2[diag2] = 1;

        nqueens_search(
            n,
            row + 1,
            columns,
            used_column,
            used_diag1,
            used_diag2,
            first_solution,
            have_first,
            solutions,
            nodes
        );

        used_column[col] = 0;
        used_diag1[diag1] = 0;
        used_diag2[diag2] = 0;
    }
}

long long n_queens(
    int n,
    int first_solution[],
    long long *nodes
) {
    if (nodes != NULL) {
        *nodes = 0;
    }

    if (n <= 0 ||
        n > 14 ||
        first_solution == NULL) {

        return -1;
    }

    int *columns =
        malloc((size_t)n * sizeof(int));

    int *used_column =
        calloc((size_t)n, sizeof(int));

    int diagonal_count = 2 * n - 1;

    int *used_diag1 =
        calloc(
            (size_t)diagonal_count,
            sizeof(int)
        );

    int *used_diag2 =
        calloc(
            (size_t)diagonal_count,
            sizeof(int)
        );

    if (columns == NULL ||
        used_column == NULL ||
        used_diag1 == NULL ||
        used_diag2 == NULL) {

        free(columns);
        free(used_column);
        free(used_diag1);
        free(used_diag2);

        return -1;
    }

    for (int i = 0; i < n; i++) {
        first_solution[i] = -1;
    }

    long long solutions = 0;
    int have_first = 0;

    nqueens_search(
        n,
        0,
        columns,
        used_column,
        used_diag1,
        used_diag2,
        first_solution,
        &have_first,
        &solutions,
        nodes
    );

    free(columns);
    free(used_column);
    free(used_diag1);
    free(used_diag2);

    return solutions;
}

/* =========================================================
   SUBSET SUM BACKTRACKING
   ========================================================= */

static int subset_sum_search(
    const int values[],
    int n,
    int index,
    int remaining,
    int selected[],
    long long *nodes
) {
    if (nodes != NULL) {
        (*nodes)++;
    }

    if (remaining == 0) {
        return 1;
    }

    if (index == n || remaining < 0) {
        return 0;
    }

    /*
     * Include current item.
     */
    selected[index] = 1;

    if (subset_sum_search(
            values,
            n,
            index + 1,
            remaining - values[index],
            selected,
            nodes)) {

        return 1;
    }

    /*
     * Exclude current item.
     */
    selected[index] = 0;

    if (subset_sum_search(
            values,
            n,
            index + 1,
            remaining,
            selected,
            nodes)) {

        return 1;
    }

    return 0;
}

int subset_sum_backtracking(
    const int values[],
    int n,
    int target,
    int selected[],
    long long *nodes
) {
    if (nodes != NULL) {
        *nodes = 0;
    }

    if (n < 0 ||
        target < 0 ||
        (n > 0 &&
         (values == NULL ||
          selected == NULL))) {

        return -1;
    }

    if (n == 0) {
        return target == 0 ? 1 : 0;
    }

    for (int i = 0; i < n; i++) {
        if (values[i] < 0) {
            return -1;
        }

        selected[i] = 0;
    }

    int found = subset_sum_search(
        values,
        n,
        0,
        target,
        selected,
        nodes
    );

    /*
     * When no solution exists, return a clean selection.
     */
    if (!found) {
        for (int i = 0; i < n; i++) {
            selected[i] = 0;
        }
    }

    return found;
}

/* =========================================================
   0/1 KNAPSACK BACKTRACKING
   ========================================================= */

static void knapsack_search(
    const struct BacktrackItem items[],
    int n,
    int index,
    int remaining_capacity,
    long long current_value,
    int current_selected[],
    long long *best_value,
    int best_selected[],
    long long *nodes
) {
    if (nodes != NULL) {
        (*nodes)++;
    }

    if (index == n) {
        if (current_value > *best_value) {
            *best_value = current_value;

            memcpy(
                best_selected,
                current_selected,
                (size_t)n * sizeof(int)
            );
        }

        return;
    }

    /*
     * Include item if it fits.
     */
    if (items[index].weight <=
        remaining_capacity) {

        if (items[index].value <=
            LLONG_MAX - current_value) {

            current_selected[index] = 1;

            knapsack_search(
                items,
                n,
                index + 1,
                remaining_capacity -
                    items[index].weight,
                current_value +
                    items[index].value,
                current_selected,
                best_value,
                best_selected,
                nodes
            );

            current_selected[index] = 0;
        }
    }

    /*
     * Exclude item.
     */
    current_selected[index] = 0;

    knapsack_search(
        items,
        n,
        index + 1,
        remaining_capacity,
        current_value,
        current_selected,
        best_value,
        best_selected,
        nodes
    );
}

long long knapsack_backtracking(
    const struct BacktrackItem items[],
    int n,
    int capacity,
    int selected[],
    long long *nodes
) {
    if (nodes != NULL) {
        *nodes = 0;
    }

    if (n < 0 ||
        capacity < 0 ||
        (n > 0 &&
         (items == NULL ||
          selected == NULL))) {

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

    int *current_selected =
        calloc((size_t)n, sizeof(int));

    if (current_selected == NULL) {
        return -1;
    }

    long long best_value = 0;

    knapsack_search(
        items,
        n,
        0,
        capacity,
        0,
        current_selected,
        &best_value,
        selected,
        nodes
    );

    free(current_selected);

    return best_value;
}