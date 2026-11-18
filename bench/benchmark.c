#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>

#include "algorithms/sort_search.h"
#include "algorithms/greedy.h"
#include "algorithms/dp.h"
#include "algorithms/string_matching.h"

static double elapsed_ms(
    clock_t start,
    clock_t end
) {
    return
        1000.0 *
        (double)(end - start) /
        (double)CLOCKS_PER_SEC;
}

static unsigned int benchmark_state = 123456789U;

static unsigned int benchmark_random(void) {
    benchmark_state =
        benchmark_state * 1664525U +
        1013904223U;

    return benchmark_state;
}

static void benchmark_sorting(void) {
    const int sizes[] = {
        100,
        1000,
        5000,
        10000
    };

    const int size_count =
        (int)(sizeof(sizes) / sizeof(sizes[0]));

    printf("\nSORTING BENCHMARK\n");
    printf(
        "------------------------------------------------------------\n"
    );

    printf(
        "%-8s %-8s %-14s %-12s %-8s\n",
        "N",
        "Method",
        "Comparisons",
        "Time(ms)",
        "Sorted"
    );

    for (int s = 0; s < size_count; s++) {
        int n = sizes[s];

        int *original = malloc(
            (size_t)n * sizeof(int)
        );

        int *merge_data = malloc(
            (size_t)n * sizeof(int)
        );

        int *quick_data = malloc(
            (size_t)n * sizeof(int)
        );

        if (original == NULL ||
            merge_data == NULL ||
            quick_data == NULL) {

            free(original);
            free(merge_data);
            free(quick_data);

            printf("sorting benchmark allocation failed\n");
            return;
        }

        for (int i = 0; i < n; i++) {
            original[i] =
                (int)(benchmark_random() % 100000U);
        }

        memcpy(
            merge_data,
            original,
            (size_t)n * sizeof(int)
        );

        memcpy(
            quick_data,
            original,
            (size_t)n * sizeof(int)
        );

        long long merge_comparisons = 0;
        long long quick_comparisons = 0;

        clock_t start = clock();

        merge_sort(
            merge_data,
            n,
            &merge_comparisons
        );

        clock_t end = clock();

        double merge_time =
            elapsed_ms(start, end);

        start = clock();

        quick_sort(
            quick_data,
            n,
            &quick_comparisons
        );

        end = clock();

        double quick_time =
            elapsed_ms(start, end);

        printf(
            "%-8d %-8s %-14lld %-12.3f %-8s\n",
            n,
            "Merge",
            merge_comparisons,
            merge_time,
            is_sorted(merge_data, n)
                ? "PASS"
                : "FAIL"
        );

        printf(
            "%-8d %-8s %-14lld %-12.3f %-8s\n",
            n,
            "Quick",
            quick_comparisons,
            quick_time,
            is_sorted(quick_data, n)
                ? "PASS"
                : "FAIL"
        );

        free(original);
        free(merge_data);
        free(quick_data);
    }
}
static void benchmark_dijkstra(void) {
    const int sizes[] = {
        50,
        100,
        250,
        500
    };

    const int size_count =
        (int)(sizeof(sizes) / sizeof(sizes[0]));

    printf("\nDIJKSTRA BENCHMARK\n");
    printf(
        "-----------------------------------------------------------------\n"
    );

    printf(
        "%-8s %-10s %-14s %-12s %-10s\n",
        "V",
        "Method",
        "Relaxations",
        "Time(ms)",
        "Agree"
    );

    for (int s = 0; s < size_count; s++) {
        int vertices = sizes[s];

        /*
         * Deterministic sparse connected directed graph.
         *
         * Every vertex i has:
         *   i -> i+1
         * and, when possible:
         *   i -> i+2
         *
         * Additional deterministic edges make the graph
         * less trivial while keeping it sparse.
         */
        int max_edges = vertices * 4;

        struct WeightedEdge *edges = malloc(
            (size_t)max_edges * sizeof(*edges)
        );

        double *array_distance = malloc(
            (size_t)vertices * sizeof(double)
        );

        double *heap_distance = malloc(
            (size_t)vertices * sizeof(double)
        );

        int *array_previous = malloc(
            (size_t)vertices * sizeof(int)
        );

        int *heap_previous = malloc(
            (size_t)vertices * sizeof(int)
        );

        if (edges == NULL ||
            array_distance == NULL ||
            heap_distance == NULL ||
            array_previous == NULL ||
            heap_previous == NULL) {

            free(edges);
            free(array_distance);
            free(heap_distance);
            free(array_previous);
            free(heap_previous);

            printf("Dijkstra benchmark allocation failed\n");
            return;
        }

        int edge_count = 0;

        for (int i = 0; i < vertices; i++) {
            if (i + 1 < vertices) {
                edges[edge_count].from = i;
                edges[edge_count].to = i + 1;
                edges[edge_count].weight =
                    1.0 + (double)(i % 7) / 10.0;

                edge_count++;
            }

            if (i + 2 < vertices) {
                edges[edge_count].from = i;
                edges[edge_count].to = i + 2;
                edges[edge_count].weight =
                    1.5 + (double)(i % 5) / 10.0;

                edge_count++;
            }

            if (i + 5 < vertices) {
                edges[edge_count].from = i;
                edges[edge_count].to = i + 5;
                edges[edge_count].weight =
                    2.0 + (double)(i % 9) / 10.0;

                edge_count++;
            }
        }

        long long array_relaxations = 0;
        long long heap_relaxations = 0;

        clock_t start = clock();

        int array_result = dijkstra_array(
            vertices,
            edges,
            edge_count,
            0,
            array_distance,
            array_previous,
            &array_relaxations
        );

        clock_t end = clock();

        double array_time =
            elapsed_ms(start, end);

        start = clock();

        int heap_result = dijkstra_heap(
            vertices,
            edges,
            edge_count,
            0,
            heap_distance,
            heap_previous,
            &heap_relaxations
        );

        end = clock();

        double heap_time =
            elapsed_ms(start, end);

        int agree =
            array_result == 1 &&
            heap_result == 1;

        if (agree) {
            for (int i = 0; i < vertices; i++) {
                if (fabs(
                        array_distance[i] -
                        heap_distance[i]
                    ) > 1e-9) {

                    agree = 0;
                    break;
                }
            }
        }

        printf(
            "%-8d %-10s %-14lld %-12.3f %-10s\n",
            vertices,
            "Array",
            array_relaxations,
            array_time,
            agree ? "PASS" : "FAIL"
        );

        printf(
            "%-8d %-10s %-14lld %-12.3f %-10s\n",
            vertices,
            "Heap",
            heap_relaxations,
            heap_time,
            agree ? "PASS" : "FAIL"
        );

        free(edges);
        free(array_distance);
        free(heap_distance);
        free(array_previous);
        free(heap_previous);
    }
}
static void benchmark_knapsack(void) {
    const int sizes[] = {
        10,
        50,
        100,
        250
    };

    const int size_count =
        (int)(sizeof(sizes) / sizeof(sizes[0]));

    printf("\nKNAPSACK BENCHMARK\n");
    printf(
        "--------------------------------------------------------------------------\n"
    );

    printf(
        "%-6s %-12s %-14s %-14s %-12s\n",
        "N",
        "Method",
        "Value",
        "Operations",
        "Time(ms)"
    );

    for (int s = 0; s < size_count; s++) {
        int n = sizes[s];
        int capacity = n * 5;

        struct FractionalItem *fractional_items =
            malloc(
                (size_t)n *
                sizeof(*fractional_items)
            );

        struct KnapsackItem *dp_items =
            malloc(
                (size_t)n *
                sizeof(*dp_items)
            );

        double *fractions =
            malloc(
                (size_t)n *
                sizeof(double)
            );

        int *selected =
            malloc(
                (size_t)n *
                sizeof(int)
            );

        if (fractional_items == NULL ||
            dp_items == NULL ||
            fractions == NULL ||
            selected == NULL) {

            free(fractional_items);
            free(dp_items);
            free(fractions);
            free(selected);

            printf(
                "knapsack benchmark allocation failed\n"
            );
            return;
        }

        for (int i = 0; i < n; i++) {
            int weight =
                1 +
                (int)(benchmark_random() % 10U);

            int value =
                10 +
                (int)(benchmark_random() % 91U);

            fractional_items[i].weight =
                (double)weight;

            fractional_items[i].value =
                (double)value;

            dp_items[i].weight = weight;
            dp_items[i].value = value;
        }

        long long greedy_operations = 0;
        long long dp_operations = 0;

        clock_t start = clock();

        double fractional_value =
            fractional_knapsack(
                fractional_items,
                n,
                (double)capacity,
                fractions,
                &greedy_operations
            );

        clock_t end = clock();

        double greedy_time =
            elapsed_ms(start, end);

        start = clock();

        long long exact_value =
            knapsack_dp(
                dp_items,
                n,
                capacity,
                selected,
                &dp_operations
            );

        end = clock();

        double dp_time =
            elapsed_ms(start, end);

        printf(
            "%-6d %-12s %-14.2f %-14lld %-12.3f\n",
            n,
            "Fractional",
            fractional_value,
            greedy_operations,
            greedy_time
        );

        printf(
            "%-6d %-12s %-14lld %-14lld %-12.3f\n",
            n,
            "0/1-DP",
            exact_value,
            dp_operations,
            dp_time
        );

        free(fractional_items);
        free(dp_items);
        free(fractions);
        free(selected);
    }

    printf(
        "Note: Fractional and 0/1 knapsack solve different "
        "problem variants; values are not expected to match.\n"
    );
}
static void benchmark_string_matching(void) {
    const int sizes[] = {
        100,
        1000,
        5000,
        10000
    };

    const int size_count =
        (int)(sizeof(sizes) / sizeof(sizes[0]));

    const char *pattern = "aaaaab";

    printf("\nSTRING MATCHING BENCHMARK\n");
    printf(
        "--------------------------------------------------------------------------\n"
    );

    printf(
        "%-8s %-18s %-14s %-12s %-10s\n",
        "N",
        "Method",
        "Operations",
        "Time(ms)",
        "Agree"
    );

    for (int s = 0; s < size_count; s++) {
        int n = sizes[s];

        char *text = malloc(
            (size_t)n + 1U
        );

        int *naive_positions = malloc(
            ((size_t)n + 1U) * sizeof(int)
        );

        int *rk_positions = malloc(
            ((size_t)n + 1U) * sizeof(int)
        );

        int *fa_positions = malloc(
            ((size_t)n + 1U) * sizeof(int)
        );

        int *kmp_positions = malloc(
            ((size_t)n + 1U) * sizeof(int)
        );

        if (text == NULL ||
            naive_positions == NULL ||
            rk_positions == NULL ||
            fa_positions == NULL ||
            kmp_positions == NULL) {

            free(text);
            free(naive_positions);
            free(rk_positions);
            free(fa_positions);
            free(kmp_positions);

            printf(
                "string benchmark allocation failed\n"
            );
            return;
        }

        /*
         * Adversarial repetitive input:
         * mostly 'a', with one 'b' at the end.
         *
         * This forces naive matching to repeatedly
         * re-check long prefixes while KMP can reuse
         * prefix information.
         */
        for (int i = 0; i < n; i++) {
            text[i] = 'a';
        }

        text[n - 1] = 'b';
        text[n] = '\0';

        long long naive_ops = 0;
        long long rk_ops = 0;
        long long fa_ops = 0;
        long long kmp_ops = 0;

        clock_t start = clock();

        int naive_count = naive_string_match(
            text,
            pattern,
            naive_positions,
            &naive_ops
        );

        clock_t end = clock();

        double naive_time =
            elapsed_ms(start, end);

        start = clock();

        int rk_count = rabin_karp_match(
            text,
            pattern,
            rk_positions,
            &rk_ops
        );

        end = clock();

        double rk_time =
            elapsed_ms(start, end);

        start = clock();

        int fa_count = finite_automaton_match(
            text,
            pattern,
            fa_positions,
            &fa_ops
        );

        end = clock();

        double fa_time =
            elapsed_ms(start, end);

        start = clock();

        int kmp_count = kmp_string_match(
            text,
            pattern,
            kmp_positions,
            &kmp_ops
        );

        end = clock();

        double kmp_time =
            elapsed_ms(start, end);

        int agree =
            naive_count >= 0 &&
            naive_count == rk_count &&
            naive_count == fa_count &&
            naive_count == kmp_count;

        if (agree) {
            for (int i = 0; i < naive_count; i++) {
                if (naive_positions[i] != rk_positions[i] ||
                    naive_positions[i] != fa_positions[i] ||
                    naive_positions[i] != kmp_positions[i]) {

                    agree = 0;
                    break;
                }
            }
        }

        printf(
            "%-8d %-18s %-14lld %-12.3f %-10s\n",
            n,
            "Naive",
            naive_ops,
            naive_time,
            agree ? "PASS" : "FAIL"
        );

        printf(
            "%-8d %-18s %-14lld %-12.3f %-10s\n",
            n,
            "Rabin-Karp",
            rk_ops,
            rk_time,
            agree ? "PASS" : "FAIL"
        );

        printf(
            "%-8d %-18s %-14lld %-12.3f %-10s\n",
            n,
            "Finite-Automaton",
            fa_ops,
            fa_time,
            agree ? "PASS" : "FAIL"
        );

        printf(
            "%-8d %-18s %-14lld %-12.3f %-10s\n",
            n,
            "KMP",
            kmp_ops,
            kmp_time,
            agree ? "PASS" : "FAIL"
        );

        free(text);
        free(naive_positions);
        free(rk_positions);
        free(fa_positions);
        free(kmp_positions);

    }
}
int main(void) {
    printf(
        "========================================\n"
    );
    printf(
        " P15 ALGORITHM BENCHMARK SUITE\n"
    );
    printf(
        "========================================\n"
    );

    benchmark_sorting();
    benchmark_dijkstra();
    benchmark_knapsack();
    benchmark_string_matching();

    printf(
    "\nNOTE: Operation counts are the primary comparison metric.\n"
);
    printf(
    "Very small elapsed times may appear as 0.000 ms because "
    "of platform clock resolution.\n"
);

    return 0;
}