#include <stdio.h>
#include "algorithms/backtracking.h"

static int tests_run = 0;
static int tests_passed = 0;

static void check(int condition, const char *name) {
    tests_run++;

    if (condition) {
        tests_passed++;
        printf("[PASS] %s\n", name);
    } else {
        printf("[FAIL] %s\n", name);
    }
}

static int valid_queen_solution(
    const int solution[],
    int n
) {
    for (int i = 0; i < n; i++) {
        if (solution[i] < 0 ||
            solution[i] >= n) {
            return 0;
        }

        for (int j = i + 1; j < n; j++) {
            if (solution[i] == solution[j]) {
                return 0;
            }

            int row_difference = j - i;
            int column_difference =
                solution[j] - solution[i];

            if (column_difference < 0) {
                column_difference =
                    -column_difference;
            }

            if (row_difference ==
                column_difference) {
                return 0;
            }
        }
    }

    return 1;
}

int main(void) {
    long long nodes;

    printf("BACKTRACKING TESTS\n");
    printf("========================================\n");

    /* --------------------------------------------------
       N-QUEENS
       -------------------------------------------------- */

    {
        int solution[8];

        long long count = n_queens(
            8,
            solution,
            &nodes
        );

        check(
            count == 92,
            "N-Queens n=8 has exactly 92 solutions"
        );

        check(
            valid_queen_solution(solution, 8),
            "N-Queens reconstructs valid solution"
        );

        check(
            nodes > 0,
            "N-Queens records search nodes"
        );
    }

    {
        int solution[4];

        check(
            n_queens(
                4,
                solution,
                &nodes
            ) == 2,
            "N-Queens n=4 has exactly 2 solutions"
        );
    }

    {
        int solution[1];

        check(
            n_queens(
                1,
                solution,
                &nodes
            ) == 1 &&
            solution[0] == 0,
            "N-Queens single queen"
        );
    }

    {
        int solution[3];

        check(
            n_queens(
                3,
                solution,
                &nodes
            ) == 0,
            "N-Queens n=3 has no solution"
        );
    }

    {
        int solution[15];

        check(
            n_queens(
                15,
                solution,
                &nodes
            ) == -1,
            "N-Queens rejects instance above safety limit"
        );
    }

    {
        check(
            n_queens(
                8,
                NULL,
                &nodes
            ) == -1,
            "N-Queens rejects null output"
        );
    }

    /* --------------------------------------------------
       SUBSET SUM
       -------------------------------------------------- */

    {
        int values[] = {
            3, 34, 4, 12, 5, 2
        };

        int selected[6];

        int found = subset_sum_backtracking(
            values,
            6,
            9,
            selected,
            &nodes
        );

        int sum = 0;

        for (int i = 0; i < 6; i++) {
            if (selected[i]) {
                sum += values[i];
            }
        }

        check(
            found == 1 && sum == 9,
            "subset sum finds valid target subset"
        );

        check(
            nodes > 0,
            "subset sum records search nodes"
        );
    }

    {
        int values[] = {
            3, 34, 4, 12, 5, 2
        };

        int selected[6];

        check(
            subset_sum_backtracking(
                values,
                6,
                30,
                selected,
                &nodes
            ) == 0,
            "subset sum reports impossible target"
        );

        int clean = 1;

        for (int i = 0; i < 6; i++) {
            if (selected[i] != 0) {
                clean = 0;
            }
        }

        check(
            clean,
            "subset sum clears selection when impossible"
        );
    }

    {
        int values[] = {
            5, 10
        };

        int selected[2];

        check(
            subset_sum_backtracking(
                values,
                2,
                0,
                selected,
                &nodes
            ) == 1,
            "subset sum target zero"
        );
    }

    {
        check(
            subset_sum_backtracking(
                NULL,
                0,
                0,
                NULL,
                &nodes
            ) == 1,
            "subset sum empty set reaches zero"
        );
    }

    {
        check(
            subset_sum_backtracking(
                NULL,
                0,
                5,
                NULL,
                &nodes
            ) == 0,
            "subset sum empty set cannot reach positive target"
        );
    }

    {
        int values[] = {
            1, -2, 3
        };

        int selected[3];

        check(
            subset_sum_backtracking(
                values,
                3,
                3,
                selected,
                &nodes
            ) == -1,
            "subset sum rejects negative values"
        );
    }

    /* --------------------------------------------------
       KNAPSACK BACKTRACKING
       -------------------------------------------------- */

    {
        struct BacktrackItem items[] = {
            {10, 60},
            {20, 100},
            {30, 120}
        };

        int selected[3];

        long long value =
            knapsack_backtracking(
                items,
                3,
                50,
                selected,
                &nodes
            );

        check(
            value == 220,
            "backtracking knapsack known optimum"
        );

        check(
            selected[0] == 0 &&
            selected[1] == 1 &&
            selected[2] == 1,
            "backtracking knapsack reconstructs optimal items"
        );

        check(
            nodes > 0,
            "backtracking knapsack records nodes"
        );
    }

    {
        struct BacktrackItem items[] = {
            {10, 60},
            {20, 100},
            {30, 120}
        };

        int selected[3];

        check(
            knapsack_backtracking(
                items,
                3,
                0,
                selected,
                &nodes
            ) == 0,
            "backtracking knapsack zero capacity"
        );
    }

    {
        struct BacktrackItem items[] = {
            {0, 50},
            {5, 10}
        };

        int selected[2];

        check(
            knapsack_backtracking(
                items,
                2,
                5,
                selected,
                &nodes
            ) == 60,
            "backtracking knapsack handles zero-weight item"
        );
    }

    {
        struct BacktrackItem items[] = {
            {10, 100},
            {20, 200}
        };

        int selected[2];

        check(
            knapsack_backtracking(
                items,
                2,
                5,
                selected,
                &nodes
            ) == 0,
            "backtracking knapsack all items overweight"
        );
    }

    {
        struct BacktrackItem items[] = {
            {-1, 100}
        };

        int selected[1];

        check(
            knapsack_backtracking(
                items,
                1,
                10,
                selected,
                &nodes
            ) == -1,
            "backtracking knapsack rejects negative weight"
        );
    }

    {
        struct BacktrackItem items[] = {
            {1, -100}
        };

        int selected[1];

        check(
            knapsack_backtracking(
                items,
                1,
                10,
                selected,
                &nodes
            ) == -1,
            "backtracking knapsack rejects negative value"
        );
    }

    {
        check(
            knapsack_backtracking(
                NULL,
                0,
                10,
                NULL,
                &nodes
            ) == 0,
            "backtracking knapsack empty item set"
        );
    }

    printf("========================================\n");
    printf(
        "RESULT: %d/%d tests passed\n",
        tests_passed,
        tests_run
    );

    return tests_passed == tests_run ? 0 : 1;
}