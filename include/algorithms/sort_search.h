#ifndef SORT_SEARCH_H
#define SORT_SEARCH_H

int binary_search_iterative(
    const int a[],
    int n,
    int key,
    long long *comparisons
);

int binary_search_recursive(
    const int a[],
    int n,
    int key,
    long long *comparisons
);

void merge_sort(
    int a[],
    int n,
    long long *comparisons
);

void quick_sort(
    int a[],
    int n,
    long long *comparisons
);

int is_sorted(
    const int a[],
    int n
);

#endif