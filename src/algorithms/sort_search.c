#include <stdlib.h>
#include "algorithms/sort_search.h"

/* ---------------------------------------------------------
   ITERATIVE BINARY SEARCH
   Precondition: array must be sorted in ascending order.
   Returns index of key, or -1 when not found.
   --------------------------------------------------------- */

int binary_search_iterative(
    const int a[],
    int n,
    int key,
    long long *comparisons
) {
    if (comparisons != NULL) {
        *comparisons = 0;
    }

    if (a == NULL || n <= 0) {
        return -1;
    }

    int left = 0;
    int right = n - 1;

    while (left <= right) {
        int mid = left + (right - left) / 2;

        if (comparisons != NULL) {
            (*comparisons)++;
        }

        if (a[mid] == key) {
            return mid;
        }

        if (comparisons != NULL) {
            (*comparisons)++;
        }

        if (a[mid] < key) {
            left = mid + 1;
        } else {
            right = mid - 1;
        }
    }

    return -1;
}

/* ---------------------------------------------------------
   RECURSIVE BINARY SEARCH
   --------------------------------------------------------- */

static int binary_search_recursive_range(
    const int a[],
    int left,
    int right,
    int key,
    long long *comparisons
) {
    if (left > right) {
        return -1;
    }

    int mid = left + (right - left) / 2;

    if (comparisons != NULL) {
        (*comparisons)++;
    }

    if (a[mid] == key) {
        return mid;
    }

    if (comparisons != NULL) {
        (*comparisons)++;
    }

    if (a[mid] < key) {
        return binary_search_recursive_range(
            a,
            mid + 1,
            right,
            key,
            comparisons
        );
    }

    return binary_search_recursive_range(
        a,
        left,
        mid - 1,
        key,
        comparisons
    );
}

int binary_search_recursive(
    const int a[],
    int n,
    int key,
    long long *comparisons
) {
    if (comparisons != NULL) {
        *comparisons = 0;
    }

    if (a == NULL || n <= 0) {
        return -1;
    }

    return binary_search_recursive_range(
        a,
        0,
        n - 1,
        key,
        comparisons
    );
}

/* ---------------------------------------------------------
   MERGE SORT
   --------------------------------------------------------- */

static void merge_ranges(
    int a[],
    int temp[],
    int left,
    int mid,
    int right,
    long long *comparisons
) {
    int i = left;
    int j = mid + 1;
    int k = left;

    while (i <= mid && j <= right) {
        if (comparisons != NULL) {
            (*comparisons)++;
        }

        /*
         * <= keeps merge sort stable.
         */
        if (a[i] <= a[j]) {
            temp[k++] = a[i++];
        } else {
            temp[k++] = a[j++];
        }
    }

    while (i <= mid) {
        temp[k++] = a[i++];
    }

    while (j <= right) {
        temp[k++] = a[j++];
    }

    for (i = left; i <= right; i++) {
        a[i] = temp[i];
    }
}

static void merge_sort_range(
    int a[],
    int temp[],
    int left,
    int right,
    long long *comparisons
) {
    if (left >= right) {
        return;
    }

    int mid = left + (right - left) / 2;

    merge_sort_range(
        a,
        temp,
        left,
        mid,
        comparisons
    );

    merge_sort_range(
        a,
        temp,
        mid + 1,
        right,
        comparisons
    );

    merge_ranges(
        a,
        temp,
        left,
        mid,
        right,
        comparisons
    );
}

void merge_sort(
    int a[],
    int n,
    long long *comparisons
) {
    if (comparisons != NULL) {
        *comparisons = 0;
    }

    if (a == NULL || n <= 1) {
        return;
    }

    int *temp = malloc((size_t)n * sizeof(int));

    if (temp == NULL) {
        return;
    }

    merge_sort_range(
        a,
        temp,
        0,
        n - 1,
        comparisons
    );

    free(temp);
}

/* ---------------------------------------------------------
   QUICK SORT

   First-element pivot is intentionally used here because
   it gives us a clear worst-case demonstration on already
   sorted input, as required for complexity analysis.
   --------------------------------------------------------- */

static int partition_first_pivot(
    int a[],
    int low,
    int high,
    long long *comparisons
) {
    int pivot = a[low];
    int i = low + 1;

    for (int j = low + 1; j <= high; j++) {
        if (comparisons != NULL) {
            (*comparisons)++;
        }

        if (a[j] < pivot) {
            int temp = a[i];
            a[i] = a[j];
            a[j] = temp;
            i++;
        }
    }

    int pivot_position = i - 1;

    int temp = a[low];
    a[low] = a[pivot_position];
    a[pivot_position] = temp;

    return pivot_position;
}

static void quick_sort_range(
    int a[],
    int low,
    int high,
    long long *comparisons
) {
    if (low >= high) {
        return;
    }

    int pivot = partition_first_pivot(
        a,
        low,
        high,
        comparisons
    );

    quick_sort_range(
        a,
        low,
        pivot - 1,
        comparisons
    );

    quick_sort_range(
        a,
        pivot + 1,
        high,
        comparisons
    );
}

void quick_sort(
    int a[],
    int n,
    long long *comparisons
) {
    if (comparisons != NULL) {
        *comparisons = 0;
    }

    if (a == NULL || n <= 1) {
        return;
    }

    quick_sort_range(
        a,
        0,
        n - 1,
        comparisons
    );
}

/* ---------------------------------------------------------
   Utility used by tests and benchmark validation.
   --------------------------------------------------------- */

int is_sorted(
    const int a[],
    int n
) {
    if (a == NULL) {
        return n <= 0;
    }

    for (int i = 1; i < n; i++) {
        if (a[i - 1] > a[i]) {
            return 0;
        }
    }

    return 1;
}