#include <stdio.h>
#include "algorithms/flow_matching.h"

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

int main(void) {
    long long augmentations;

    printf("FLOW AND MATCHING TESTS - PART A\n");
    printf("========================================\n");

    /* --------------------------------------------------
       CLASSIC MAX-FLOW GRAPH
       Expected maximum flow = 23
       -------------------------------------------------- */

    {
        struct FlowEdge edges[] = {
            {0, 1, 16},
            {0, 2, 13},
            {1, 2, 10},
            {2, 1, 4},
            {1, 3, 12},
            {3, 2, 9},
            {2, 4, 14},
            {4, 3, 7},
            {3, 5, 20},
            {4, 5, 4}
        };

        long long flow[36];
        long long residual[36];

        long long max_flow = ford_fulkerson(
            6,
            edges,
            10,
            0,
            5,
            flow,
            residual,
            &augmentations
        );

        check(
            max_flow == 23,
            "Ford-Fulkerson known maximum flow"
        );

        check(
            augmentations > 0,
            "Ford-Fulkerson records augmentations"
        );

        int source_side[6];

        int ok = min_cut_source_side(
            6,
            0,
            residual,
            source_side
        );

        check(
            ok == 1 &&
            source_side[0] == 1 &&
            source_side[5] == 0,
            "min-cut separates source and sink"
        );

        long long min_cut = cut_capacity(
            6,
            edges,
            10,
            source_side
        );

        check(
            min_cut == 23,
            "known minimum cut capacity"
        );

        check(
            max_flow == min_cut,
            "max-flow equals min-cut"
        );
    }

    /* --------------------------------------------------
       NO SOURCE-TO-SINK PATH
       -------------------------------------------------- */

    {
        struct FlowEdge edges[] = {
            {0, 1, 5},
            {2, 3, 7}
        };

        long long flow[16];
        long long residual[16];

        long long result = ford_fulkerson(
            4,
            edges,
            2,
            0,
            3,
            flow,
            residual,
            &augmentations
        );

        check(
            result == 0,
            "Ford-Fulkerson disconnected source and sink"
        );

        check(
            augmentations == 0,
            "no augmentations when sink unreachable"
        );
    }

    /* --------------------------------------------------
       SINGLE BOTTLENECK
       -------------------------------------------------- */

    {
        struct FlowEdge edges[] = {
            {0, 1, 100},
            {1, 2, 3},
            {2, 3, 100}
        };

        long long flow[16];
        long long residual[16];

        long long result = ford_fulkerson(
            4,
            edges,
            3,
            0,
            3,
            flow,
            residual,
            &augmentations
        );

        check(
            result == 3,
            "Ford-Fulkerson identifies bottleneck capacity"
        );

        int source_side[4];

        min_cut_source_side(
            4,
            0,
            residual,
            source_side
        );

        check(
            cut_capacity(
                4,
                edges,
                3,
                source_side
            ) == 3,
            "min-cut exposes bottleneck"
        );
    }

    /* --------------------------------------------------
       PARALLEL EDGES
       -------------------------------------------------- */

    {
        struct FlowEdge edges[] = {
            {0, 1, 4},
            {0, 1, 6},
            {1, 2, 10}
        };

        long long flow[9];
        long long residual[9];

        check(
            ford_fulkerson(
                3,
                edges,
                3,
                0,
                2,
                flow,
                residual,
                &augmentations
            ) == 10,
            "Ford-Fulkerson combines parallel capacities"
        );
    }

    /* --------------------------------------------------
       ZERO-CAPACITY EDGE
       -------------------------------------------------- */

    {
        struct FlowEdge edges[] = {
            {0, 1, 0},
            {0, 2, 5},
            {2, 1, 5}
        };

        long long flow[9];
        long long residual[9];

        check(
            ford_fulkerson(
                3,
                edges,
                3,
                0,
                1,
                flow,
                residual,
                &augmentations
            ) == 5,
            "Ford-Fulkerson handles zero-capacity edge"
        );
    }

    /* --------------------------------------------------
       INVALID INPUT
       -------------------------------------------------- */

    {
        struct FlowEdge edges[] = {
            {0, 1, -5}
        };

        long long flow[4];
        long long residual[4];

        check(
            ford_fulkerson(
                2,
                edges,
                1,
                0,
                1,
                flow,
                residual,
                &augmentations
            ) == -1,
            "Ford-Fulkerson rejects negative capacity"
        );
    }

    {
        struct FlowEdge edges[] = {
            {0, 5, 10}
        };

        long long flow[9];
        long long residual[9];

        check(
            ford_fulkerson(
                3,
                edges,
                1,
                0,
                2,
                flow,
                residual,
                &augmentations
            ) == -1,
            "Ford-Fulkerson rejects invalid vertex"
        );
    }

    {
        long long flow[4];
        long long residual[4];

        check(
            ford_fulkerson(
                2,
                NULL,
                0,
                0,
                0,
                flow,
                residual,
                &augmentations
            ) == -1,
            "Ford-Fulkerson rejects source equal to sink"
        );
    }

    {
        check(
            min_cut_source_side(
                0,
                0,
                NULL,
                NULL
            ) == 0,
            "min-cut rejects invalid input"
        );
    }
    /* --------------------------------------------------
       BIPARTITE MATCHING
       -------------------------------------------------- */

    {
        /*
         * Perfect matching exists:
         *
         * L0 -> R0, R1
         * L1 -> R0
         * L2 -> R1, R2
         */
        struct BipartiteEdge edges[] = {
            {0, 0},
            {0, 1},
            {1, 0},
            {2, 1},
            {2, 2}
        };

        int left_match[3];
        int right_match[3];

        int size = bipartite_maximum_matching(
            3,
            3,
            edges,
            5,
            left_match,
            right_match,
            &augmentations
        );

        check(
            size == 3,
            "maximum bipartite matching finds perfect matching"
        );

        check(
            left_match[0] != -1 &&
            left_match[1] != -1 &&
            left_match[2] != -1,
            "maximum matching assigns every left vertex"
        );

        check(
            augmentations > 0,
            "maximum matching records operations"
        );
    }

    /*
     * Deliberate greedy trap.
     *
     * Input order makes greedy choose:
     * L0-R0
     *
     * Then L1 cannot use R0.
     *
     * Exact matching can rearrange:
     * L0-R1
     * L1-R0
     *
     * Therefore:
     * greedy = 1
     * exact  = 2
     */
    {
        struct BipartiteEdge edges[] = {
            {0, 0},
            {0, 1},
            {1, 0}
        };

        int exact_left[2];
        int exact_right[2];

        int greedy_left[2];
        int greedy_right[2];

        long long exact_operations;
        long long greedy_operations;

        int exact = bipartite_maximum_matching(
            2,
            2,
            edges,
            3,
            exact_left,
            exact_right,
            &exact_operations
        );

        int greedy = bipartite_greedy_matching(
            2,
            2,
            edges,
            3,
            greedy_left,
            greedy_right,
            &greedy_operations
        );

        check(
            exact == 2,
            "exact matching escapes greedy trap"
        );

        check(
            greedy == 1,
            "greedy matching exposes suboptimal result"
        );

        check(
            exact > greedy,
            "exact matching quality exceeds greedy baseline"
        );

        check(
            exact_operations > 0 &&
            greedy_operations > 0,
            "matching algorithms record operations"
        );
    }

    /* Disconnected left vertex. */
    {
        struct BipartiteEdge edges[] = {
            {0, 0},
            {1, 1}
        };

        int left_match[3];
        int right_match[2];

        int size = bipartite_maximum_matching(
            3,
            2,
            edges,
            2,
            left_match,
            right_match,
            &augmentations
        );

        check(
            size == 2 &&
            left_match[2] == -1,
            "matching handles unmatched left vertex"
        );
    }

    /* More left vertices than right vertices. */
    {
        struct BipartiteEdge edges[] = {
            {0, 0},
            {1, 0},
            {2, 1}
        };

        int left_match[3];
        int right_match[2];

        check(
            bipartite_maximum_matching(
                3,
                2,
                edges,
                3,
                left_match,
                right_match,
                &augmentations
            ) == 2,
            "matching respects smaller partition limit"
        );
    }

    /* Duplicate edges must not inflate matching size. */
    {
        struct BipartiteEdge edges[] = {
            {0, 0},
            {0, 0},
            {1, 1}
        };

        int left_match[2];
        int right_match[2];

        check(
            bipartite_maximum_matching(
                2,
                2,
                edges,
                3,
                left_match,
                right_match,
                &augmentations
            ) == 2,
            "matching handles duplicate edges"
        );
    }

    /* Empty bipartite graph. */
    {
        check(
            bipartite_maximum_matching(
                0,
                0,
                NULL,
                0,
                NULL,
                NULL,
                &augmentations
            ) == 0,
            "maximum matching handles empty graph"
        );
    }

    /* Invalid endpoint. */
    {
        struct BipartiteEdge edges[] = {
            {0, 3}
        };

        int left_match[2];
        int right_match[2];

        check(
            bipartite_maximum_matching(
                2,
                2,
                edges,
                1,
                left_match,
                right_match,
                &augmentations
            ) == -1,
            "maximum matching rejects invalid endpoint"
        );
    }

    {
        struct BipartiteEdge edges[] = {
            {-1, 0}
        };

        int left_match[2];
        int right_match[2];

        check(
            bipartite_greedy_matching(
                2,
                2,
                edges,
                1,
                left_match,
                right_match,
                &augmentations
            ) == -1,
            "greedy matching rejects invalid endpoint"
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