#include <stdio.h>
#include "algorithms/branch_bound.h"

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

static int selection_valid(
    const struct BBItem items[],
    int n,
    int capacity,
    const int selected[],
    long long expected_value
) {
    int weight = 0;
    long long value = 0;

    for (int i = 0; i < n; i++) {
        if (selected[i]) {
            weight += items[i].weight;
            value += items[i].value;
        }
    }

    return weight <= capacity &&
           value == expected_value;
}

int main(void) {
    long long nodes;

    printf("BRANCH AND BOUND TESTS\n");
    printf("========================================\n");

    /* --------------------------------------------------
       KNOWN OPTIMUM - ALL THREE STRATEGIES
       -------------------------------------------------- */

    {
        struct BBItem items[] = {
            {10, 60},
            {20, 100},
            {30, 120}
        };

        int fifo_selected[3];
        int lifo_selected[3];
        int best_selected[3];

        long long fifo_nodes;
        long long lifo_nodes;
        long long best_nodes;

        long long fifo = knapsack_branch_bound(
            items,
            3,
            50,
            BB_FIFO,
            fifo_selected,
            &fifo_nodes
        );

        long long lifo = knapsack_branch_bound(
            items,
            3,
            50,
            BB_LIFO,
            lifo_selected,
            &lifo_nodes
        );

        long long best = knapsack_branch_bound(
            items,
            3,
            50,
            BB_BEST_BOUND,
            best_selected,
            &best_nodes
        );

        check(
            fifo == 220,
            "FIFO branch-and-bound known optimum"
        );

        check(
            lifo == 220,
            "LIFO branch-and-bound known optimum"
        );

        check(
            best == 220,
            "best-bound branch-and-bound known optimum"
        );

        check(
            fifo == lifo && lifo == best,
            "all branch-and-bound strategies agree"
        );

        check(
            selection_valid(
                items,
                3,
                50,
                fifo_selected,
                fifo
            ),
            "FIFO reconstructs valid optimal selection"
        );

        check(
            selection_valid(
                items,
                3,
                50,
                lifo_selected,
                lifo
            ),
            "LIFO reconstructs valid optimal selection"
        );

        check(
            selection_valid(
                items,
                3,
                50,
                best_selected,
                best
            ),
            "best-bound reconstructs valid optimal selection"
        );

        check(
            fifo_nodes > 0 &&
            lifo_nodes > 0 &&
            best_nodes > 0,
            "branch-and-bound records expanded nodes"
        );
    }

    /* --------------------------------------------------
       GREEDY COUNTEREXAMPLE

       Ratio greedy takes 10/60 first, but the exact
       optimum is the 20/100 + 30/120 combination.
       -------------------------------------------------- */

    {
        struct BBItem items[] = {
            {10, 60},
            {20, 100},
            {30, 120}
        };

        int selected[3];

        check(
            knapsack_branch_bound(
                items,
                3,
                50,
                BB_BEST_BOUND,
                selected,
                &nodes
            ) == 220,
            "B&B defeats greedy-ratio counterexample"
        );
    }

    /* --------------------------------------------------
       ZERO CAPACITY
       -------------------------------------------------- */

    {
        struct BBItem items[] = {
            {5, 10},
            {10, 20}
        };

        int selected[2];

        check(
            knapsack_branch_bound(
                items,
                2,
                0,
                BB_FIFO,
                selected,
                &nodes
            ) == 0,
            "B&B zero capacity"
        );
    }

    /* --------------------------------------------------
       ZERO-WEIGHT POSITIVE-VALUE ITEM
       -------------------------------------------------- */

    {
        struct BBItem items[] = {
            {0, 50},
            {5, 10}
        };

        int selected[2];

        long long value = knapsack_branch_bound(
            items,
            2,
            5,
            BB_BEST_BOUND,
            selected,
            &nodes
        );

        check(
            value == 60,
            "B&B handles zero-weight item"
        );

        check(
            selection_valid(
                items,
                2,
                5,
                selected,
                60
            ),
            "B&B zero-weight selection is valid"
        );
    }

    /* --------------------------------------------------
       ALL ITEMS OVERWEIGHT
       -------------------------------------------------- */

    {
        struct BBItem items[] = {
            {10, 100},
            {20, 200}
        };

        int selected[2];

        check(
            knapsack_branch_bound(
                items,
                2,
                5,
                BB_LIFO,
                selected,
                &nodes
            ) == 0,
            "B&B all items overweight"
        );
    }

    /* --------------------------------------------------
       EXACT CAPACITY
       -------------------------------------------------- */

    {
        struct BBItem items[] = {
            {5, 10},
            {5, 20}
        };

        int selected[2];

        check(
            knapsack_branch_bound(
                items,
                2,
                10,
                BB_BEST_BOUND,
                selected,
                &nodes
            ) == 30,
            "B&B exact capacity"
        );
    }

    /* --------------------------------------------------
       MORE COMPLEX ORACLE
       -------------------------------------------------- */

    {
        struct BBItem items[] = {
            {2, 40},
            {3, 50},
            {5, 100},
            {4, 60}
        };

        int fifo_selected[4];
        int lifo_selected[4];
        int best_selected[4];

        long long fifo = knapsack_branch_bound(
            items,
            4,
            8,
            BB_FIFO,
            fifo_selected,
            &nodes
        );

        long long lifo = knapsack_branch_bound(
            items,
            4,
            8,
            BB_LIFO,
            lifo_selected,
            &nodes
        );

        long long best = knapsack_branch_bound(
            items,
            4,
            8,
            BB_BEST_BOUND,
            best_selected,
            &nodes
        );

        check(
            fifo == 150 &&
            lifo == 150 &&
            best == 150,
            "all B&B strategies agree on second optimum"
        );
    }

    /* --------------------------------------------------
       INVALID INPUTS
       -------------------------------------------------- */

    {
        struct BBItem items[] = {
            {-1, 10}
        };

        int selected[1];

        check(
            knapsack_branch_bound(
                items,
                1,
                10,
                BB_FIFO,
                selected,
                &nodes
            ) == -1,
            "B&B rejects negative weight"
        );
    }

    {
        struct BBItem items[] = {
            {1, -10}
        };

        int selected[1];

        check(
            knapsack_branch_bound(
                items,
                1,
                10,
                BB_FIFO,
                selected,
                &nodes
            ) == -1,
            "B&B rejects negative value"
        );
    }

    {
        struct BBItem items[] = {
            {1, 10}
        };

        int selected[1];

        check(
            knapsack_branch_bound(
                items,
                1,
                10,
                (enum BBStrategy)99,
                selected,
                &nodes
            ) == -1,
            "B&B rejects invalid strategy"
        );
    }

    {
        check(
            knapsack_branch_bound(
                NULL,
                0,
                10,
                BB_FIFO,
                NULL,
                &nodes
            ) == 0,
            "B&B empty item set"
        );
    }

    {
        struct BBItem item = {1, 1};
        int selected[1];

        check(
            knapsack_branch_bound(
                &item,
                64,
                10,
                BB_FIFO,
                selected,
                &nodes
            ) == -1,
            "B&B rejects instance above bitmask limit"
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