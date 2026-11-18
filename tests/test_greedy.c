#include <stdio.h>
#include <string.h>
#include <math.h>
#include "algorithms/greedy.h"

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

static int nearly_equal(double a, double b) {
    return fabs(a - b) < 1e-9;
}

static const char *find_code(
    const struct HuffmanCode codes[],
    int count,
    unsigned char symbol
) {
    for (int i = 0; i < count; i++) {
        if (codes[i].symbol == symbol) {
            return codes[i].code;
        }
    }

    return NULL;
}

int main(void) {
    long long operations;

    printf("GREEDY ALGORITHM TESTS - PART A\n");
    printf("========================================\n");

    /* --------------------------------------------------
       ACTIVITY SELECTION
       -------------------------------------------------- */

    {
        struct Activity activities[] = {
            {1, 4},
            {3, 5},
            {0, 6},
            {5, 7},
            {3, 9},
            {5, 9},
            {6, 10},
            {8, 11},
            {8, 12},
            {2, 14},
            {12, 16}
        };

        int selected[11];

        int count = activity_selection(
            activities,
            11,
            selected,
            &operations
        );

        check(
            count == 4,
            "activity selection known optimum"
        );

        int valid = 1;

        for (int i = 1; i < count; i++) {
            int previous = selected[i - 1];
            int current = selected[i];

            if (activities[current].start <
                activities[previous].finish) {

                valid = 0;
            }
        }

        check(
            valid,
            "activity selection produces non-overlapping schedule"
        );
    }

    {
        struct Activity activities[] = {
            {0, 1}
        };

        int selected[1];

        check(
            activity_selection(
                activities,
                1,
                selected,
                &operations
            ) == 1 &&
            selected[0] == 0,
            "activity selection single activity"
        );
    }

    {
        check(
            activity_selection(
                NULL,
                0,
                NULL,
                &operations
            ) == 0,
            "activity selection empty input"
        );
    }

    {
        struct Activity activities[] = {
            {5, 2}
        };

        int selected[1];

        check(
            activity_selection(
                activities,
                1,
                selected,
                &operations
            ) == -1,
            "activity selection rejects invalid interval"
        );
    }

    /* --------------------------------------------------
       INTERVAL PARTITIONING
       -------------------------------------------------- */

    {
        struct Activity activities[] = {
            {0, 4},
            {1, 3},
            {2, 5},
            {4, 7},
            {5, 8}
        };

        int channels[5];

        int count = interval_partition(
            activities,
            5,
            channels,
            &operations
        );

        check(
            count == 3,
            "interval partitioning equals maximum overlap"
        );

        int valid = 1;

        for (int i = 0; i < 5; i++) {
            for (int j = i + 1; j < 5; j++) {
                if (channels[i] == channels[j]) {
                    int overlap =
                        activities[i].start <
                            activities[j].finish &&
                        activities[j].start <
                            activities[i].finish;

                    if (overlap) {
                        valid = 0;
                    }
                }
            }
        }

        check(
            valid,
            "interval partitioning has no channel conflicts"
        );
    }

    {
        check(
            interval_partition(
                NULL,
                0,
                NULL,
                &operations
            ) == 0,
            "interval partitioning empty input"
        );
    }

    {
        struct Activity activities[] = {
            {10, 5}
        };

        int channels[1];

        check(
            interval_partition(
                activities,
                1,
                channels,
                &operations
            ) == -1,
            "interval partitioning rejects invalid interval"
        );
    }

    /* --------------------------------------------------
       FRACTIONAL KNAPSACK
       -------------------------------------------------- */

    {
        struct FractionalItem items[] = {
            {60.0, 10.0},
            {100.0, 20.0},
            {120.0, 30.0}
        };

        double fractions[3];

        double value = fractional_knapsack(
            items,
            3,
            50.0,
            fractions,
            &operations
        );

        check(
            nearly_equal(value, 240.0),
            "fractional knapsack known optimum"
        );

        check(
            nearly_equal(fractions[0], 1.0) &&
            nearly_equal(fractions[1], 1.0) &&
            nearly_equal(
                fractions[2],
                2.0 / 3.0
            ),
            "fractional knapsack selects correct fractions"
        );
    }

    {
        struct FractionalItem items[] = {
            {10.0, 2.0},
            {20.0, 4.0}
        };

        double fractions[2];

        check(
            nearly_equal(
                fractional_knapsack(
                    items,
                    2,
                    0.0,
                    fractions,
                    &operations
                ),
                0.0
            ),
            "fractional knapsack zero capacity"
        );
    }

    {
        struct FractionalItem items[] = {
            {10.0, 0.0}
        };

        double fractions[1];

        check(
            fractional_knapsack(
                items,
                1,
                10.0,
                fractions,
                &operations
            ) < 0.0,
            "fractional knapsack rejects zero weight"
        );
    }

    {
        struct FractionalItem items[] = {
            {-10.0, 5.0}
        };

        double fractions[1];

        check(
            fractional_knapsack(
                items,
                1,
                10.0,
                fractions,
                &operations
            ) < 0.0,
            "fractional knapsack rejects negative value"
        );
    }

    /* --------------------------------------------------
       HUFFMAN CODING
       -------------------------------------------------- */

    {
        unsigned long frequencies[256] = {0};

        frequencies[(unsigned char)'a'] = 45;
        frequencies[(unsigned char)'b'] = 13;
        frequencies[(unsigned char)'c'] = 12;
        frequencies[(unsigned char)'d'] = 16;
        frequencies[(unsigned char)'e'] = 9;
        frequencies[(unsigned char)'f'] = 5;

        struct HuffmanCode codes[256];

        int count = huffman_build_codes(
            frequencies,
            codes,
            &operations
        );

        check(
            count == 6,
            "Huffman creates code for each used symbol"
        );

        check(
            huffman_is_prefix_free(
                codes,
                count
            ),
            "Huffman codes are prefix-free"
        );

        int all_present =
            find_code(codes, count, 'a') != NULL &&
            find_code(codes, count, 'b') != NULL &&
            find_code(codes, count, 'c') != NULL &&
            find_code(codes, count, 'd') != NULL &&
            find_code(codes, count, 'e') != NULL &&
            find_code(codes, count, 'f') != NULL;

        check(
            all_present,
            "Huffman preserves all symbols"
        );

        const char *code_a =
            find_code(codes, count, 'a');

        const char *code_f =
            find_code(codes, count, 'f');

        check(
            code_a != NULL &&
            code_f != NULL &&
            strlen(code_a) <= strlen(code_f),
            "higher-frequency Huffman symbol is not longer than rare symbol"
        );
    }

    {
        unsigned long frequencies[256] = {0};

        frequencies[(unsigned char)'X'] = 100;

        struct HuffmanCode codes[256];

        int count = huffman_build_codes(
            frequencies,
            codes,
            &operations
        );

        const char *code =
            find_code(codes, count, 'X');

        check(
            count == 1 &&
            code != NULL &&
            strcmp(code, "0") == 0,
            "Huffman single-symbol boundary case"
        );

        check(
            huffman_is_prefix_free(
                codes,
                count
            ),
            "single-symbol Huffman code is prefix-free"
        );
    }

    {
        unsigned long frequencies[256] = {0};
        struct HuffmanCode codes[256];

        check(
            huffman_build_codes(
                frequencies,
                codes,
                &operations
            ) == 0,
            "Huffman empty frequency table"
        );
    }

    {
        check(
            huffman_build_codes(
                NULL,
                NULL,
                &operations
            ) == -1,
            "Huffman rejects null input"
        );
    }

    /*
     * Deliberately invalid codes:
     * "0" is a prefix of "01".
     */
    {
        struct HuffmanCode bad_codes[2];

        bad_codes[0].symbol = 'A';
        strcpy(bad_codes[0].code, "0");

        bad_codes[1].symbol = 'B';
        strcpy(bad_codes[1].code, "01");

        check(
            !huffman_is_prefix_free(
                bad_codes,
                2
            ),
            "prefix-free validator catches invalid code set"
        );
    }
    /* --------------------------------------------------
       DIJKSTRA - ARRAY VS BINARY HEAP
       -------------------------------------------------- */

    {
        struct WeightedEdge edges[] = {
            {0, 1, 4.0},
            {0, 2, 1.0},
            {2, 1, 2.0},
            {1, 3, 1.0},
            {2, 3, 5.0},
            {3, 4, 3.0}
        };

        double array_distance[5];
        double heap_distance[5];

        int array_previous[5];
        int heap_previous[5];

        long long array_relaxations;
        long long heap_relaxations;

        int ok_array = dijkstra_array(
            5,
            edges,
            6,
            0,
            array_distance,
            array_previous,
            &array_relaxations
        );

        int ok_heap = dijkstra_heap(
            5,
            edges,
            6,
            0,
            heap_distance,
            heap_previous,
            &heap_relaxations
        );

        check(
            ok_array == 1 &&
            ok_heap == 1,
            "Dijkstra normal graph succeeds"
        );

        int same = 1;

        for (int i = 0; i < 5; i++) {
            if (!nearly_equal(
                    array_distance[i],
                    heap_distance[i])) {

                same = 0;
            }
        }

        check(
            same,
            "array and heap Dijkstra agree"
        );

        check(
            nearly_equal(array_distance[0], 0.0) &&
            nearly_equal(array_distance[1], 3.0) &&
            nearly_equal(array_distance[2], 1.0) &&
            nearly_equal(array_distance[3], 4.0) &&
            nearly_equal(array_distance[4], 7.0),
            "Dijkstra known shortest distances"
        );

        int path[5];

        int path_length = reconstruct_path(
            0,
            4,
            heap_previous,
            5,
            path
        );

        check(
            path_length == 5 &&
            path[0] == 0 &&
            path[1] == 2 &&
            path[2] == 1 &&
            path[3] == 3 &&
            path[4] == 4,
            "Dijkstra reconstructs shortest path"
        );

        check(
            array_relaxations > 0 &&
            heap_relaxations > 0,
            "Dijkstra records relaxation operations"
        );
    }

    /* Zero-weight edges are valid for Dijkstra. */
    {
        struct WeightedEdge edges[] = {
            {0, 1, 0.0},
            {1, 2, 0.0},
            {0, 2, 5.0}
        };

        double distance[3];
        int previous[3];

        int ok = dijkstra_heap(
            3,
            edges,
            3,
            0,
            distance,
            previous,
            &operations
        );

        check(
            ok == 1 &&
            nearly_equal(distance[2], 0.0),
            "Dijkstra handles zero-weight edges"
        );
    }

    /* Disconnected vertex must remain unreachable. */
    {
        struct WeightedEdge edges[] = {
            {0, 1, 2.0},
            {1, 2, 3.0}
        };

        double distance[4];
        int previous[4];

        int ok = dijkstra_array(
            4,
            edges,
            2,
            0,
            distance,
            previous,
            &operations
        );

        int path[4];

        int path_length = reconstruct_path(
            0,
            3,
            previous,
            4,
            path
        );

        check(
            ok == 1 &&
            path_length == 0,
            "Dijkstra handles unreachable target"
        );
    }

    /* Source to itself should be distance zero. */
    {
        struct WeightedEdge edges[] = {
            {0, 1, 5.0}
        };

        double distance[2];
        int previous[2];

        int ok = dijkstra_heap(
            2,
            edges,
            1,
            0,
            distance,
            previous,
            &operations
        );

        int path[2];

        int path_length = reconstruct_path(
            0,
            0,
            previous,
            2,
            path
        );

        check(
            ok == 1 &&
            nearly_equal(distance[0], 0.0) &&
            path_length == 1 &&
            path[0] == 0,
            "Dijkstra source equals target"
        );
    }

    /*
     * Dijkstra must reject negative edges.
     * This is an important correctness condition.
     */
    {
        struct WeightedEdge edges[] = {
            {0, 1, -1.0}
        };

        double distance[2];
        int previous[2];

        check(
            dijkstra_array(
                2,
                edges,
                1,
                0,
                distance,
                previous,
                &operations
            ) == -1,
            "array Dijkstra rejects negative edge"
        );

        check(
            dijkstra_heap(
                2,
                edges,
                1,
                0,
                distance,
                previous,
                &operations
            ) == -1,
            "heap Dijkstra rejects negative edge"
        );
    }

    /* Invalid vertex in an edge must be rejected. */
    {
        struct WeightedEdge edges[] = {
            {0, 5, 1.0}
        };

        double distance[3];
        int previous[3];

        check(
            dijkstra_heap(
                3,
                edges,
                1,
                0,
                distance,
                previous,
                &operations
            ) == 0,
            "Dijkstra rejects invalid edge vertex"
        );
    }

    /* Invalid source must be rejected. */
    {
        double distance[3];
        int previous[3];

        check(
            dijkstra_array(
                3,
                NULL,
                0,
                3,
                distance,
                previous,
                &operations
            ) == 0,
            "Dijkstra rejects invalid source"
        );
    }

    /*
     * Equal-cost alternatives:
     * correctness depends on optimal distance,
     * not which equally optimal predecessor is chosen.
     */
    {
        struct WeightedEdge edges[] = {
            {0, 1, 1.0},
            {0, 2, 1.0},
            {1, 3, 1.0},
            {2, 3, 1.0}
        };

        double array_distance[4];
        double heap_distance[4];

        int array_previous[4];
        int heap_previous[4];

        int ok1 = dijkstra_array(
            4,
            edges,
            4,
            0,
            array_distance,
            array_previous,
            &operations
        );

        int ok2 = dijkstra_heap(
            4,
            edges,
            4,
            0,
            heap_distance,
            heap_previous,
            &operations
        );

        check(
            ok1 == 1 &&
            ok2 == 1 &&
            nearly_equal(array_distance[3], 2.0) &&
            nearly_equal(heap_distance[3], 2.0),
            "Dijkstra handles equal-cost shortest paths"
        );
    }
    /* --------------------------------------------------
       PRIM / KRUSKAL / K-CLUSTERING
       -------------------------------------------------- */

    {
        /*
         * Connected undirected graph.
         *
         * Expected MST weight = 10
         */
        struct UndirectedEdge edges[] = {
            {0, 1, 1.0},
            {0, 2, 4.0},
            {1, 2, 2.0},
            {1, 3, 5.0},
            {2, 3, 3.0},
            {2, 4, 7.0},
            {3, 4, 4.0}
        };

        struct UndirectedEdge prim_edges[4];
        struct UndirectedEdge kruskal_edges[4];

        double prim_weight;
        double kruskal_weight;

        long long prim_operations;
        long long kruskal_operations;

        int prim_count = prim_msf(
            5,
            edges,
            7,
            prim_edges,
            &prim_weight,
            &prim_operations
        );

        int kruskal_count = kruskal_msf(
            5,
            edges,
            7,
            kruskal_edges,
            &kruskal_weight,
            &kruskal_operations
        );

        check(
            prim_count == 4 &&
            kruskal_count == 4,
            "Prim and Kruskal select V-1 edges on connected graph"
        );

        check(
            nearly_equal(prim_weight, 10.0) &&
            nearly_equal(kruskal_weight, 10.0),
            "Prim and Kruskal known MST weight"
        );

        check(
            nearly_equal(
                prim_weight,
                kruskal_weight
            ),
            "Prim and Kruskal MST weights agree"
        );

        check(
            prim_operations > 0 &&
            kruskal_operations > 0,
            "MST algorithms record operations"
        );
    }

    /*
     * Disconnected graph.
     *
     * Both algorithms should produce a minimum
     * spanning forest rather than fail.
     */
    {
        struct UndirectedEdge edges[] = {
            {0, 1, 1.0},
            {1, 2, 2.0},
            {3, 4, 4.0}
        };

        struct UndirectedEdge prim_edges[4];
        struct UndirectedEdge kruskal_edges[4];

        double prim_weight;
        double kruskal_weight;

        int prim_count = prim_msf(
            5,
            edges,
            3,
            prim_edges,
            &prim_weight,
            &operations
        );

        int kruskal_count = kruskal_msf(
            5,
            edges,
            3,
            kruskal_edges,
            &kruskal_weight,
            &operations
        );

        check(
            prim_count == 3 &&
            kruskal_count == 3,
            "Prim and Kruskal produce forest on disconnected graph"
        );

        check(
            nearly_equal(prim_weight, 7.0) &&
            nearly_equal(kruskal_weight, 7.0),
            "minimum spanning forest known weight"
        );
    }

    /*
     * Three deliberately separated groups:
     *
     * {0,1,2}
     * {3,4,5}
     * {6,7}
     *
     * Weak cross-group edges have very high distance,
     * so stopping Kruskal at k=3 should recover them.
     */
    {
        struct UndirectedEdge edges[] = {
            {0, 1, 0.10},
            {1, 2, 0.20},
            {0, 2, 0.30},

            {3, 4, 0.10},
            {4, 5, 0.20},
            {3, 5, 0.25},

            {6, 7, 0.10},

            {2, 3, 0.90},
            {5, 6, 0.95}
        };

        int labels[8];

        int count = kruskal_k_clustering(
            8,
            edges,
            9,
            3,
            labels,
            &operations
        );

        check(
            count == 3,
            "Kruskal clustering produces requested 3 communities"
        );

        int correct_groups =
            labels[0] == labels[1] &&
            labels[1] == labels[2] &&

            labels[3] == labels[4] &&
            labels[4] == labels[5] &&

            labels[6] == labels[7] &&

            labels[0] != labels[3] &&
            labels[0] != labels[6] &&
            labels[3] != labels[6];

        check(
            correct_groups,
            "Kruskal clustering recovers expected communities"
        );
    }

    /*
     * k = V means every vertex remains its own cluster.
     */
    {
        struct UndirectedEdge edges[] = {
            {0, 1, 1.0},
            {1, 2, 2.0}
        };

        int labels[3];

        int count = kruskal_k_clustering(
            3,
            edges,
            2,
            3,
            labels,
            &operations
        );

        check(
            count == 3 &&
            labels[0] != labels[1] &&
            labels[0] != labels[2] &&
            labels[1] != labels[2],
            "Kruskal clustering k equals number of vertices"
        );
    }

    /*
     * k = 1 on a connected graph should produce
     * one community.
     */
    {
        struct UndirectedEdge edges[] = {
            {0, 1, 1.0},
            {1, 2, 1.0},
            {2, 3, 1.0}
        };

        int labels[4];

        int count = kruskal_k_clustering(
            4,
            edges,
            3,
            1,
            labels,
            &operations
        );

        check(
            count == 1 &&
            labels[0] == labels[1] &&
            labels[1] == labels[2] &&
            labels[2] == labels[3],
            "Kruskal clustering k equals one"
        );
    }

    /*
     * A disconnected graph with three components
     * cannot be reduced to k=2 because no connecting
     * edges exist.
     */
    {
        struct UndirectedEdge edges[] = {
            {0, 1, 1.0},
            {2, 3, 1.0}
        };

        int labels[5];

        check(
            kruskal_k_clustering(
                5,
                edges,
                2,
                2,
                labels,
                &operations
            ) == -1,
            "Kruskal clustering rejects impossible k"
        );
    }

    /* Invalid k values. */
    {
        int labels[3];

        check(
            kruskal_k_clustering(
                3,
                NULL,
                0,
                0,
                labels,
                &operations
            ) == -1,
            "Kruskal clustering rejects k zero"
        );

        check(
            kruskal_k_clustering(
                3,
                NULL,
                0,
                4,
                labels,
                &operations
            ) == -1,
            "Kruskal clustering rejects k greater than V"
        );
    }

    /* Self-loops are rejected by our MST graph model. */
    {
        struct UndirectedEdge edges[] = {
            {0, 0, 1.0}
        };

        struct UndirectedEdge forest[1];
        double weight;

        check(
            prim_msf(
                2,
                edges,
                1,
                forest,
                &weight,
                &operations
            ) == -1,
            "MST rejects self-loop input"
        );

        check(
            kruskal_msf(
                2,
                edges,
                1,
                forest,
                &weight,
                &operations
            ) == -1,
            "Kruskal rejects self-loop input"
        );
    }

    /*
     * Negative edge weights are valid for an MST.
     * Unlike Dijkstra, MST algorithms do not require
     * non-negative edge weights.
     */
    {
        struct UndirectedEdge edges[] = {
            {0, 1, -2.0},
            {1, 2, 1.0},
            {0, 2, 5.0}
        };

        struct UndirectedEdge prim_edges[2];
        struct UndirectedEdge kruskal_edges[2];

        double prim_weight;
        double kruskal_weight;

        int p = prim_msf(
            3,
            edges,
            3,
            prim_edges,
            &prim_weight,
            &operations
        );

        int k = kruskal_msf(
            3,
            edges,
            3,
            kruskal_edges,
            &kruskal_weight,
            &operations
        );

        check(
            p == 2 &&
            k == 2 &&
            nearly_equal(prim_weight, -1.0) &&
            nearly_equal(kruskal_weight, -1.0),
            "MST correctly handles negative edge weights"
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