#include <stdio.h>
#include <string.h>
#include "algorithms/sort_search.h"

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

static int arrays_equal(
    const int a[],
    const int b[],
    int n
) {
    for (int i = 0; i < n; i++) {
        if (a[i] != b[i]) {
            return 0;
        }
    }

    return 1;
}

int main(void) {
    long long comparisons;

    printf("SORT AND SEARCH TESTS\n");
    printf("========================================\n");

    /* --------------------------------------------------
       MERGE SORT TESTS
       -------------------------------------------------- */

    {
        int a[] = {5, 2, 9, 1, 5, 6};
        int expected[] = {1, 2, 5, 5, 6, 9};

        merge_sort(a, 6, &comparisons);

        check(
            arrays_equal(a, expected, 6),
            "merge sort normal input"
        );

        check(
            is_sorted(a, 6),
            "merge sort result is sorted"
        );
    }

    {
        int a[] = {-3, -10, 4, 0, -3};
        int expected[] = {-10, -3, -3, 0, 4};

        merge_sort(a, 5, &comparisons);

        check(
            arrays_equal(a, expected, 5),
            "merge sort negatives and duplicates"
        );
    }

    {
        int a[] = {1};

        merge_sort(a, 1, &comparisons);

        check(
            a[0] == 1,
            "merge sort single element"
        );
    }

    {
        merge_sort(NULL, 0, &comparisons);

        check(
            comparisons == 0,
            "merge sort empty input"
        );
    }

    /* --------------------------------------------------
       QUICK SORT TESTS
       -------------------------------------------------- */

    {
        int a[] = {5, 2, 9, 1, 5, 6};
        int expected[] = {1, 2, 5, 5, 6, 9};

        quick_sort(a, 6, &comparisons);

        check(
            arrays_equal(a, expected, 6),
            "quick sort normal input"
        );
    }

    {
        int a[] = {9, 8, 7, 6, 5, 4, 3, 2, 1};
        int expected[] = {1, 2, 3, 4, 5, 6, 7, 8, 9};

        quick_sort(a, 9, &comparisons);

        check(
            arrays_equal(a, expected, 9),
            "quick sort reverse-sorted input"
        );
    }

    {
        int a[] = {1, 2, 3, 4, 5, 6, 7, 8};

        quick_sort(a, 8, &comparisons);

        check(
            is_sorted(a, 8),
            "quick sort already-sorted adversarial input"
        );

        check(
            comparisons == 28,
            "quick sort exposes quadratic sorted-input comparisons"
        );
    }

    {
        int a[] = {4, 4, 4, 4, 4};

        quick_sort(a, 5, &comparisons);

        check(
            is_sorted(a, 5),
            "quick sort all-equal values"
        );
    }

    /* --------------------------------------------------
       ITERATIVE BINARY SEARCH
       -------------------------------------------------- */

    {
        int a[] = {-10, -3, 0, 4, 8, 12, 20};

        int index = binary_search_iterative(
            a,
            7,
            8,
            &comparisons
        );

        check(
            index == 4,
            "iterative binary search existing key"
        );
    }

    {
        int a[] = {-10, -3, 0, 4, 8, 12, 20};

        int index = binary_search_iterative(
            a,
            7,
            7,
            &comparisons
        );

        check(
            index == -1,
            "iterative binary search missing key"
        );
    }

    {
        int a[] = {42};

        check(
            binary_search_iterative(
                a,
                1,
                42,
                &comparisons
            ) == 0,
            "iterative binary search single element"
        );
    }

    {
        check(
            binary_search_iterative(
                NULL,
                0,
                5,
                &comparisons
            ) == -1,
            "iterative binary search empty input"
        );
    }

    /* --------------------------------------------------
       RECURSIVE BINARY SEARCH
       -------------------------------------------------- */

    {
        int a[] = {-10, -3, 0, 4, 8, 12, 20};

        int index = binary_search_recursive(
            a,
            7,
            -3,
            &comparisons
        );

        check(
            index == 1,
            "recursive binary search existing key"
        );
    }

    {
        int a[] = {-10, -3, 0, 4, 8, 12, 20};

        int index = binary_search_recursive(
            a,
            7,
            100,
            &comparisons
        );

        check(
            index == -1,
            "recursive binary search missing key"
        );
    }

    /* --------------------------------------------------
       CROSS-ALGORITHM ORACLE CHECK

       Merge sort and quick sort must produce exactly
       the same sorted result.
       -------------------------------------------------- */

    {
        int merge_data[] = {
            12, -5, 7, 7, 0, 99, -20, 3, 3, 1
        };

        int quick_data[] = {
            12, -5, 7, 7, 0, 99, -20, 3, 3, 1
        };

        long long merge_comparisons;
        long long quick_comparisons;

        merge_sort(
            merge_data,
            10,
            &merge_comparisons
        );

        quick_sort(
            quick_data,
            10,
            &quick_comparisons
        );

        check(
            arrays_equal(
                merge_data,
                quick_data,
                10
            ),
            "merge sort and quick sort agree"
        );
    }

    printf("========================================\n");
    printf(
        "RESULT: %d/%d tests passed\n",
        tests_passed,
        tests_run
    );

    if (tests_passed != tests_run) {
        return 1;
    }

    return 0;
}