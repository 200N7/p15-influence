#ifndef DIVIDE_CONQUER_H
#define DIVIDE_CONQUER_H

struct SubarrayResult {
    long long sum;
    int left;
    int right;
};

struct SubarrayResult max_subarray_dc(
    const int a[],
    int n,
    long long *operations
);

struct SubarrayResult max_subarray_kadane(
    const int a[],
    int n,
    long long *operations
);

int fast_power_ll(
    long long base,
    unsigned int exponent,
    long long *result,
    long long *multiplications
);

int find_celebrity(
    const int *knows,
    int n,
    long long *checks
);

/*
 * Matrices are stored as flat row-major n x n arrays.
 *
 * Returns 1 on success, 0 for invalid input or
 * arithmetic/allocation failure.
 */
int matrix_multiply_classical(
    const long long *a,
    const long long *b,
    long long *c,
    int n,
    long long *multiplications
);

/*
 * Strassen multiplication.
 *
 * Arbitrary positive n is supported by internally
 * padding matrices to the next power of two.
 */
int matrix_multiply_strassen(
    const long long *a,
    const long long *b,
    long long *c,
    int n,
    long long *multiplications
);

#endif