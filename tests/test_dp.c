#include <stdio.h>
#include <string.h>
#include <float.h>
#include "algorithms/dp.h"

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

static int is_subsequence(
    const char *small,
    const char *large
) {
    int i = 0;
    int j = 0;

    while (small[i] != '\0' &&
           large[j] != '\0') {

        if (small[i] == large[j]) {
            i++;
        }

        j++;
    }

    return small[i] == '\0';
}

int main(void) {
    long long operations;

    printf("DYNAMIC PROGRAMMING TESTS - PART A\n");
    printf("========================================\n");

    /* --------------------------------------------------
       0/1 KNAPSACK
       -------------------------------------------------- */

    {
        struct KnapsackItem items[] = {
            {10, 60},
            {20, 100},
            {30, 120}
        };

        int selected[3];

        long long value = knapsack_dp(
            items,
            3,
            50,
            selected,
            &operations
        );

        check(
            value == 220,
            "0/1 knapsack known optimum"
        );

        check(
            selected[0] == 0 &&
            selected[1] == 1 &&
            selected[2] == 1,
            "0/1 knapsack reconstructs optimal items"
        );

        check(
            operations > 0,
            "0/1 knapsack records operations"
        );
    }

    /*
     * This is useful later when comparing with greedy
     * value/weight-ratio selection.
     *
     * Greedy would first take item 0, but the optimum
     * is items 1 and 2.
     */
    {
        struct KnapsackItem items[] = {
            {10, 60},
            {20, 100},
            {30, 120}
        };

        int selected[3];

        long long value = knapsack_dp(
            items,
            3,
            50,
            selected,
            &operations
        );

        check(
            value == 220,
            "0/1 knapsack greedy-counterexample optimum"
        );
    }

    {
        struct KnapsackItem items[] = {
            {5, 10},
            {10, 20}
        };

        int selected[2];

        long long value = knapsack_dp(
            items,
            2,
            0,
            selected,
            &operations
        );

        check(
            value == 0 &&
            selected[0] == 0 &&
            selected[1] == 0,
            "0/1 knapsack zero capacity"
        );
    }

    {
        struct KnapsackItem items[] = {
            {10, 100},
            {20, 200}
        };

        int selected[2];

        long long value = knapsack_dp(
            items,
            2,
            5,
            selected,
            &operations
        );

        check(
            value == 0,
            "0/1 knapsack all items overweight"
        );
    }

    {
        struct KnapsackItem items[] = {
            {5, 10},
            {5, 20}
        };

        int selected[2];

        long long value = knapsack_dp(
            items,
            2,
            10,
            selected,
            &operations
        );

        check(
            value == 30 &&
            selected[0] == 1 &&
            selected[1] == 1,
            "0/1 knapsack exact capacity"
        );
    }

    /*
     * Zero-weight positive-value item is legal in
     * this model and may be selected once.
     */
    {
        struct KnapsackItem items[] = {
            {0, 50},
            {5, 10}
        };

        int selected[2];

        long long value = knapsack_dp(
            items,
            2,
            5,
            selected,
            &operations
        );

        check(
            value == 60 &&
            selected[0] == 1 &&
            selected[1] == 1,
            "0/1 knapsack zero-weight item"
        );
    }

    {
        struct KnapsackItem items[] = {
            {-1, 10}
        };

        int selected[1];

        check(
            knapsack_dp(
                items,
                1,
                10,
                selected,
                &operations
            ) == -1,
            "0/1 knapsack rejects negative weight"
        );
    }

    {
        struct KnapsackItem items[] = {
            {1, -10}
        };

        int selected[1];

        check(
            knapsack_dp(
                items,
                1,
                10,
                selected,
                &operations
            ) == -1,
            "0/1 knapsack rejects negative value"
        );
    }

    {
        check(
            knapsack_dp(
                NULL,
                0,
                10,
                NULL,
                &operations
            ) == 0,
            "0/1 knapsack empty item set"
        );
    }

    /* --------------------------------------------------
       MATRIX-CHAIN MULTIPLICATION
       -------------------------------------------------- */

    {
        /*
         * Standard textbook dimensions:
         *
         * A1 30x35
         * A2 35x15
         * A3 15x5
         * A4 5x10
         * A5 10x20
         * A6 20x25
         *
         * Known optimum = 15125.
         */
        int dimensions[] = {
            30, 35, 15, 5, 10, 20, 25
        };

        int split[36];

        long long cost = matrix_chain_order(
            dimensions,
            6,
            split,
            &operations
        );

        check(
            cost == 15125,
            "matrix-chain known textbook optimum"
        );

        check(
            operations > 0,
            "matrix-chain records operations"
        );
    }

    {
        int dimensions[] = {
            10, 20
        };

        int split[1];

        check(
            matrix_chain_order(
                dimensions,
                1,
                split,
                &operations
            ) == 0,
            "matrix-chain single matrix"
        );
    }

    {
        int dimensions[] = {
            10, 20, 30
        };

        int split[4];

        check(
            matrix_chain_order(
                dimensions,
                2,
                split,
                &operations
            ) == 6000,
            "matrix-chain two matrices"
        );
    }

    /*
     * Rectangular dimensions matter in P15 because
     * user/community/topic matrices need not be square.
     */
    {
        int dimensions[] = {
            5, 10, 3, 12, 5
        };

        int split[16];

        check(
            matrix_chain_order(
                dimensions,
                4,
                split,
                &operations
            ) == 405,
            "matrix-chain rectangular matrices"
        );
    }

    {
        int dimensions[] = {
            10, 0, 20
        };

        int split[4];

        check(
            matrix_chain_order(
                dimensions,
                2,
                split,
                &operations
            ) == -1,
            "matrix-chain rejects zero dimension"
        );
    }

    {
        int split[1];

        check(
            matrix_chain_order(
                NULL,
                1,
                split,
                &operations
            ) == -1,
            "matrix-chain rejects null dimensions"
        );
    }

    /* --------------------------------------------------
       LONGEST COMMON SUBSEQUENCE
       -------------------------------------------------- */

    {
        char output[16];

        int length = lcs_sequence(
            "ABCBDAB",
            "BDCABA",
            output,
            &operations
        );

        check(
            length == 4,
            "LCS known length"
        );

        check(
            (int)strlen(output) == 4 &&
            is_subsequence(output, "ABCBDAB") &&
            is_subsequence(output, "BDCABA"),
            "LCS reconstructs valid subsequence"
        );

        check(
            operations > 0,
            "LCS records operations"
        );
    }

    {
        char output[16];

        int length = lcs_sequence(
            "ALGORITHM",
            "ALGORITHM",
            output,
            &operations
        );

        check(
            length == 9 &&
            strcmp(output, "ALGORITHM") == 0,
            "LCS identical strings"
        );
    }

    {
        char output[8];

        int length = lcs_sequence(
            "",
            "ABC",
            output,
            &operations
        );

        check(
            length == 0 &&
            strcmp(output, "") == 0,
            "LCS empty string"
        );
    }

    {
        char output[8];

        int length = lcs_sequence(
            "ABC",
            "XYZ",
            output,
            &operations
        );

        check(
            length == 0 &&
            strcmp(output, "") == 0,
            "LCS no common characters"
        );
    }

    {
        char output[16];

        int length = lcs_sequence(
            "AAAAAB",
            "AAACAB",
            output,
            &operations
        );

        check(
            length == 5 &&
            is_subsequence(output, "AAAAAB") &&
            is_subsequence(output, "AAACAB"),
            "LCS repeated-character case"
        );
    }

    {
        char output[4];

        check(
            lcs_sequence(
                NULL,
                "ABC",
                output,
                &operations
            ) == -1,
            "LCS rejects null input"
        );
    }
    /* --------------------------------------------------
       OPTIMAL BINARY SEARCH TREE
       -------------------------------------------------- */

    {
        long long frequencies[] = {
            34, 8, 50
        };

        int roots[9];

        long long cost = optimal_bst(
            frequencies,
            3,
            roots,
            &operations
        );

        check(
            cost == 142,
            "optimal BST known minimum cost"
        );

        check(
            roots[0 * 3 + 2] == 2,
            "optimal BST reconstructs expected root"
        );

        check(
            operations > 0,
            "optimal BST records operations"
        );
    }

    {
        long long frequencies[] = {
            25
        };

        int roots[1];

        check(
            optimal_bst(
                frequencies,
                1,
                roots,
                &operations
            ) == 25 &&
            roots[0] == 0,
            "optimal BST single key"
        );
    }

    {
        check(
            optimal_bst(
                NULL,
                0,
                NULL,
                &operations
            ) == 0,
            "optimal BST empty key set"
        );
    }

    {
        long long frequencies[] = {
            10, -1, 20
        };

        int roots[9];

        check(
            optimal_bst(
                frequencies,
                3,
                roots,
                &operations
            ) == -1,
            "optimal BST rejects negative frequency"
        );
    }

    /* --------------------------------------------------
       BELLMAN-FORD
       -------------------------------------------------- */

    {
        /*
         * Standard graph containing negative edges
         * but no negative cycle.
         */
        struct DPEdge edges[] = {
            {0, 1, 6.0},
            {0, 2, 7.0},
            {1, 2, 8.0},
            {1, 3, 5.0},
            {1, 4, -4.0},
            {2, 3, -3.0},
            {2, 4, 9.0},
            {3, 1, -2.0},
            {4, 0, 2.0},
            {4, 3, 7.0}
        };

        double distance[5];
        int previous[5];

        int result = bellman_ford(
            5,
            edges,
            10,
            0,
            distance,
            previous,
            &operations
        );

        check(
            result == 1,
            "Bellman-Ford accepts negative edges"
        );

        check(
            distance[0] == 0.0 &&
            distance[1] == 2.0 &&
            distance[2] == 7.0 &&
            distance[3] == 4.0 &&
            distance[4] == -2.0,
            "Bellman-Ford known shortest distances"
        );

        check(
            operations > 0,
            "Bellman-Ford records relaxations"
        );
    }

    /*
     * Reachable negative cycle:
     *
     * 1 -> 2 -> 1 has total weight -2.
     */
    {
        struct DPEdge edges[] = {
            {0, 1, 1.0},
            {1, 2, -2.0},
            {2, 1, 0.0}
        };

        double distance[3];
        int previous[3];

        check(
            bellman_ford(
                3,
                edges,
                3,
                0,
                distance,
                previous,
                &operations
            ) == -1,
            "Bellman-Ford detects reachable negative cycle"
        );
    }

    /*
     * Negative cycle exists, but it is unreachable
     * from source 0. It must not invalidate the
     * source-specific shortest paths.
     */
    {
        struct DPEdge edges[] = {
            {0, 1, 2.0},
            {2, 3, -5.0},
            {3, 2, 1.0}
        };

        double distance[4];
        int previous[4];

        check(
            bellman_ford(
                4,
                edges,
                3,
                0,
                distance,
                previous,
                &operations
            ) == 1,
            "Bellman-Ford ignores unreachable negative cycle"
        );
    }

    {
        struct DPEdge edges[] = {
            {0, 5, 1.0}
        };

        double distance[3];
        int previous[3];

        check(
            bellman_ford(
                3,
                edges,
                1,
                0,
                distance,
                previous,
                &operations
            ) == 0,
            "Bellman-Ford rejects invalid vertex"
        );
    }

    {
        double distance[3];
        int previous[3];

        check(
            bellman_ford(
                3,
                NULL,
                0,
                3,
                distance,
                previous,
                &operations
            ) == 0,
            "Bellman-Ford rejects invalid source"
        );
    }
    /* --------------------------------------------------
       FLOYD-WARSHALL
       -------------------------------------------------- */

    {
        struct DPEdge edges[] = {
            {0, 1, 3.0},
            {0, 3, 7.0},
            {1, 0, 8.0},
            {1, 2, 2.0},
            {2, 0, 5.0},
            {2, 3, 1.0},
            {3, 0, 2.0}
        };

        double distance[16];
        int next[16];

        int result = floyd_warshall(
            4,
            edges,
            7,
            distance,
            next,
            &operations
        );

        check(
            result == 1,
            "Floyd-Warshall normal graph succeeds"
        );

        check(
            distance[0 * 4 + 2] == 5.0 &&
            distance[0 * 4 + 3] == 6.0 &&
            distance[1 * 4 + 3] == 3.0 &&
            distance[3 * 4 + 2] == 7.0,
            "Floyd-Warshall known shortest distances"
        );

        check(
            operations > 0,
            "Floyd-Warshall records operations"
        );

        int path[5];

        int path_length = floyd_reconstruct_path(
            0,
            3,
            next,
            4,
            path
        );

        check(
            path_length == 4 &&
            path[0] == 0 &&
            path[1] == 1 &&
            path[2] == 2 &&
            path[3] == 3,
            "Floyd-Warshall reconstructs shortest path"
        );
    }

    /*
     * Parallel edges: the lighter edge must win.
     */
    {
        struct DPEdge edges[] = {
            {0, 1, 10.0},
            {0, 1, 2.0}
        };

        double distance[4];
        int next[4];

        int result = floyd_warshall(
            2,
            edges,
            2,
            distance,
            next,
            &operations
        );

        check(
            result == 1 &&
            distance[1] == 2.0,
            "Floyd-Warshall handles parallel edges"
        );
    }

    /*
     * Negative edges are allowed when there is no
     * negative cycle.
     */
    {
        struct DPEdge edges[] = {
            {0, 1, 4.0},
            {0, 2, 5.0},
            {1, 2, -2.0}
        };

        double distance[9];
        int next[9];

        int result = floyd_warshall(
            3,
            edges,
            3,
            distance,
            next,
            &operations
        );

        check(
            result == 1 &&
            distance[0 * 3 + 2] == 2.0,
            "Floyd-Warshall handles negative edge"
        );
    }

    /*
     * Negative cycle must be detected.
     */
    {
        struct DPEdge edges[] = {
            {0, 1, 1.0},
            {1, 2, -3.0},
            {2, 0, 1.0}
        };

        double distance[9];
        int next[9];

        check(
            floyd_warshall(
                3,
                edges,
                3,
                distance,
                next,
                &operations
            ) == -1,
            "Floyd-Warshall detects negative cycle"
        );
    }

    /*
     * Unreachable pair must not produce a fake path.
     */
    {
        struct DPEdge edges[] = {
            {0, 1, 1.0}
        };

        double distance[9];
        int next[9];
        int path[4];

        int result = floyd_warshall(
            3,
            edges,
            1,
            distance,
            next,
            &operations
        );

        int path_length = floyd_reconstruct_path(
            0,
            2,
            next,
            3,
            path
        );

        check(
            result == 1 &&
            path_length == 0,
            "Floyd-Warshall handles unreachable pair"
        );
    }

    /* --------------------------------------------------
       HELD-KARP TSP
       -------------------------------------------------- */

    {
        /*
         * Classic 4-city symmetric TSP.
         * Optimal tour cost = 80.
         */
        double distance[] = {
             0.0, 10.0, 15.0, 20.0,
            10.0,  0.0, 35.0, 25.0,
            15.0, 35.0,  0.0, 30.0,
            20.0, 25.0, 30.0,  0.0
        };

        int tour[5];

        double cost = held_karp_tsp(
            distance,
            4,
            0,
            tour,
            &operations
        );

        check(
            cost == 80.0,
            "Held-Karp known optimal TSP cost"
        );

        check(
            tour[0] == 0 &&
            tour[4] == 0,
            "Held-Karp tour starts and ends at source"
        );

        int seen[4] = {0};

        for (int i = 0; i < 4; i++) {
            if (tour[i] >= 0 &&
                tour[i] < 4) {

                seen[tour[i]]++;
            }
        }

        check(
            seen[0] == 1 &&
            seen[1] == 1 &&
            seen[2] == 1 &&
            seen[3] == 1,
            "Held-Karp visits every vertex exactly once"
        );

        check(
            operations > 0,
            "Held-Karp records operations"
        );
    }

    {
        double distance[] = {
            0.0
        };

        int tour[2];

        double cost = held_karp_tsp(
            distance,
            1,
            0,
            tour,
            &operations
        );

        check(
            cost == 0.0 &&
            tour[0] == 0 &&
            tour[1] == 0,
            "Held-Karp single vertex"
        );
    }

    /*
     * Missing return edge makes a complete tour impossible.
     */
    {
        double distance[] = {
            0.0,     1.0, DBL_MAX,
            DBL_MAX, 0.0, 1.0,
            DBL_MAX, DBL_MAX, 0.0
        };

        int tour[4];

        check(
            held_karp_tsp(
                distance,
                3,
                0,
                tour,
                &operations
            ) < 0.0,
            "Held-Karp detects impossible tour"
        );
    }

    {
        double distance[] = {
            0.0, -1.0,
            1.0,  0.0
        };

        int tour[3];

        check(
            held_karp_tsp(
                distance,
                2,
                0,
                tour,
                &operations
            ) < 0.0,
            "Held-Karp rejects negative distance"
        );
    }

    {
        double distance[] = {0.0};
        int tour[2];

        check(
            held_karp_tsp(
                distance,
                21,
                0,
                tour,
                &operations
            ) < 0.0,
            "Held-Karp rejects instance larger than limit"
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