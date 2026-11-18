#include <limits.h>
#include "algorithms/divide_conquer.h"

/* =========================================================
   MAXIMUM SUBARRAY - DIVIDE AND CONQUER
   ========================================================= */

static struct SubarrayResult max_crossing(
    const int a[],
    int left,
    int mid,
    int right,
    long long *operations
) {
    long long sum = 0;
    long long best_left_sum = LLONG_MIN;
    int best_left = mid;

    for (int i = mid; i >= left; i--) {
        sum += a[i];

        if (operations != 0) {
            (*operations)++;
        }

        if (sum > best_left_sum) {
            best_left_sum = sum;
            best_left = i;
        }
    }

    sum = 0;

    long long best_right_sum = LLONG_MIN;
    int best_right = mid + 1;

    for (int i = mid + 1; i <= right; i++) {
        sum += a[i];

        if (operations != 0) {
            (*operations)++;
        }

        if (sum > best_right_sum) {
            best_right_sum = sum;
            best_right = i;
        }
    }

    struct SubarrayResult result;

    result.sum = best_left_sum + best_right_sum;
    result.left = best_left;
    result.right = best_right;

    return result;
}

static struct SubarrayResult max_subarray_range(
    const int a[],
    int left,
    int right,
    long long *operations
) {
    if (left == right) {
        struct SubarrayResult result;

        result.sum = a[left];
        result.left = left;
        result.right = right;

        return result;
    }

    int mid = left + (right - left) / 2;

    struct SubarrayResult left_result =
        max_subarray_range(
            a,
            left,
            mid,
            operations
        );

    struct SubarrayResult right_result =
        max_subarray_range(
            a,
            mid + 1,
            right,
            operations
        );

    struct SubarrayResult cross_result =
        max_crossing(
            a,
            left,
            mid,
            right,
            operations
        );

    if (left_result.sum >= right_result.sum &&
        left_result.sum >= cross_result.sum) {

        return left_result;
    }

    if (right_result.sum >= left_result.sum &&
        right_result.sum >= cross_result.sum) {

        return right_result;
    }

    return cross_result;
}

struct SubarrayResult max_subarray_dc(
    const int a[],
    int n,
    long long *operations
) {
    if (operations != 0) {
        *operations = 0;
    }

    if (a == 0 || n <= 0) {
        struct SubarrayResult invalid;

        invalid.sum = 0;
        invalid.left = -1;
        invalid.right = -1;

        return invalid;
    }

    return max_subarray_range(
        a,
        0,
        n - 1,
        operations
    );
}

/* =========================================================
   KADANE ORACLE

   Used to verify the divide-and-conquer implementation.
   It correctly handles all-negative arrays.
   ========================================================= */

struct SubarrayResult max_subarray_kadane(
    const int a[],
    int n,
    long long *operations
) {
    if (operations != 0) {
        *operations = 0;
    }

    if (a == 0 || n <= 0) {
        struct SubarrayResult invalid;

        invalid.sum = 0;
        invalid.left = -1;
        invalid.right = -1;

        return invalid;
    }

    long long current_sum = a[0];
    long long best_sum = a[0];

    int current_left = 0;
    int best_left = 0;
    int best_right = 0;

    for (int i = 1; i < n; i++) {
        if (operations != 0) {
            (*operations)++;
        }

        if (current_sum + a[i] < a[i]) {
            current_sum = a[i];
            current_left = i;
        } else {
            current_sum += a[i];
        }

        if (current_sum > best_sum) {
            best_sum = current_sum;
            best_left = current_left;
            best_right = i;
        }
    }

    struct SubarrayResult result;

    result.sum = best_sum;
    result.left = best_left;
    result.right = best_right;

    return result;
}

/* =========================================================
   FAST EXPONENTIATION

   Exponentiation by squaring.
   Overflow is detected before multiplication.
   ========================================================= */

static int safe_multiply(
    long long a,
    long long b,
    long long *result
) {
    if (result == 0) {
        return 0;
    }

    if (a == 0 || b == 0) {
        *result = 0;
        return 1;
    }

    if (a == -1 && b == LLONG_MIN) {
        return 0;
    }

    if (b == -1 && a == LLONG_MIN) {
        return 0;
    }

    if (a > 0) {
        if (b > 0) {
            if (a > LLONG_MAX / b) {
                return 0;
            }
        } else {
            if (b < LLONG_MIN / a) {
                return 0;
            }
        }
    } else {
        if (b > 0) {
            if (a < LLONG_MIN / b) {
                return 0;
            }
        } else {
            if (a != 0 && b < LLONG_MAX / a) {
                return 0;
            }
        }
    }

    *result = a * b;
    return 1;
}

int fast_power_ll(
    long long base,
    unsigned int exponent,
    long long *result,
    long long *multiplications
) {
    if (multiplications != 0) {
        *multiplications = 0;
    }

    if (result == 0) {
        return 0;
    }

    long long answer = 1;
    long long factor = base;
    unsigned int power = exponent;

    while (power > 0) {
        if (power & 1U) {
            long long next;

            if (!safe_multiply(
                    answer,
                    factor,
                    &next)) {

                return 0;
            }

            answer = next;

            if (multiplications != 0) {
                (*multiplications)++;
            }
        }

        power >>= 1U;

        if (power > 0) {
            long long next;

            if (!safe_multiply(
                    factor,
                    factor,
                    &next)) {

                return 0;
            }

            factor = next;

            if (multiplications != 0) {
                (*multiplications)++;
            }
        }
    }

    *result = answer;
    return 1;
}

/* =========================================================
   CELEBRITY PROBLEM

   Matrix is stored as a flat n x n array.

   knows[a*n + b] == 1 means person a knows person b.
   ========================================================= */

static int knows_person(
    const int *knows,
    int n,
    int a,
    int b,
    long long *checks
) {
    if (checks != 0) {
        (*checks)++;
    }

    return knows[a * n + b] != 0;
}

int find_celebrity(
    const int *knows,
    int n,
    long long *checks
) {
    if (checks != 0) {
        *checks = 0;
    }

    if (knows == 0 || n <= 0) {
        return -1;
    }

    if (n == 1) {
        return 0;
    }

    /*
     * Elimination phase.
     */
    int candidate = 0;

    for (int i = 1; i < n; i++) {
        if (knows_person(
                knows,
                n,
                candidate,
                i,
                checks)) {

            candidate = i;
        }
    }

    /*
     * Verification phase.
     */
    for (int i = 0; i < n; i++) {
        if (i == candidate) {
            continue;
        }

        /*
         * Celebrity must know nobody.
         */
        if (knows_person(
                knows,
                n,
                candidate,
                i,
                checks)) {

            return -1;
        }

        /*
         * Everybody must know celebrity.
         */
        if (!knows_person(
                knows,
                n,
                i,
                candidate,
                checks)) {

            return -1;
        }
    }

    return candidate;
}
/* =========================================================
   MATRIX MULTIPLICATION UTILITIES
   ========================================================= */

#include <stdlib.h>

static int next_power_of_two(int n) {
    int power = 1;

    while (power < n) {
        if (power > INT_MAX / 2) {
            return 0;
        }

        power *= 2;
    }

    return power;
}

static long long *allocate_matrix(int n) {
    if (n <= 0) {
        return NULL;
    }

    return calloc(
        (size_t)n * (size_t)n,
        sizeof(long long)
    );
}

/* =========================================================
   CLASSICAL MATRIX MULTIPLICATION
   ========================================================= */

int matrix_multiply_classical(
    const long long *a,
    const long long *b,
    long long *c,
    int n,
    long long *multiplications
) {
    if (multiplications != NULL) {
        *multiplications = 0;
    }

    if (a == NULL ||
        b == NULL ||
        c == NULL ||
        n <= 0) {

        return 0;
    }

    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            long long sum = 0;

            for (int k = 0; k < n; k++) {
                long long product;

                if (!safe_multiply(
                        a[i * n + k],
                        b[k * n + j],
                        &product)) {

                    return 0;
                }

                if ((product > 0 &&
                     sum > LLONG_MAX - product) ||
                    (product < 0 &&
                     sum < LLONG_MIN - product)) {

                    return 0;
                }

                sum += product;

                if (multiplications != NULL) {
                    (*multiplications)++;
                }
            }

            c[i * n + j] = sum;
        }
    }

    return 1;
}

/* =========================================================
   STRASSEN MATRIX MULTIPLICATION
   ========================================================= */

static int matrix_add(
    const long long *a,
    const long long *b,
    long long *c,
    int n
) {
    int count = n * n;

    for (int i = 0; i < count; i++) {
        if ((b[i] > 0 &&
             a[i] > LLONG_MAX - b[i]) ||
            (b[i] < 0 &&
             a[i] < LLONG_MIN - b[i])) {

            return 0;
        }

        c[i] = a[i] + b[i];
    }

    return 1;
}

static int matrix_subtract(
    const long long *a,
    const long long *b,
    long long *c,
    int n
) {
    int count = n * n;

    for (int i = 0; i < count; i++) {
        if ((b[i] < 0 &&
             a[i] > LLONG_MAX + b[i]) ||
            (b[i] > 0 &&
             a[i] < LLONG_MIN + b[i])) {

            return 0;
        }

        c[i] = a[i] - b[i];
    }

    return 1;
}

static void copy_block(
    const long long *source,
    long long *destination,
    int source_n,
    int row_offset,
    int col_offset,
    int block_n
) {
    for (int i = 0; i < block_n; i++) {
        for (int j = 0; j < block_n; j++) {
            destination[i * block_n + j] =
                source[
                    (i + row_offset) * source_n +
                    (j + col_offset)
                ];
        }
    }
}

static void write_block(
    long long *destination,
    const long long *source,
    int destination_n,
    int row_offset,
    int col_offset,
    int block_n
) {
    for (int i = 0; i < block_n; i++) {
        for (int j = 0; j < block_n; j++) {
            destination[
                (i + row_offset) * destination_n +
                (j + col_offset)
            ] = source[i * block_n + j];
        }
    }
}

static int strassen_recursive(
    const long long *a,
    const long long *b,
    long long *c,
    int n,
    long long *multiplications
) {
    if (n == 1) {
        if (!safe_multiply(a[0], b[0], &c[0])) {
            return 0;
        }

        if (multiplications != NULL) {
            (*multiplications)++;
        }

        return 1;
    }

    int h = n / 2;

    long long *a11 = allocate_matrix(h);
    long long *a12 = allocate_matrix(h);
    long long *a21 = allocate_matrix(h);
    long long *a22 = allocate_matrix(h);

    long long *b11 = allocate_matrix(h);
    long long *b12 = allocate_matrix(h);
    long long *b21 = allocate_matrix(h);
    long long *b22 = allocate_matrix(h);

    long long *m1 = allocate_matrix(h);
    long long *m2 = allocate_matrix(h);
    long long *m3 = allocate_matrix(h);
    long long *m4 = allocate_matrix(h);
    long long *m5 = allocate_matrix(h);
    long long *m6 = allocate_matrix(h);
    long long *m7 = allocate_matrix(h);

    long long *t1 = allocate_matrix(h);
    long long *t2 = allocate_matrix(h);

    long long *c11 = allocate_matrix(h);
    long long *c12 = allocate_matrix(h);
    long long *c21 = allocate_matrix(h);
    long long *c22 = allocate_matrix(h);

    if (a11 == NULL || a12 == NULL ||
        a21 == NULL || a22 == NULL ||
        b11 == NULL || b12 == NULL ||
        b21 == NULL || b22 == NULL ||
        m1 == NULL || m2 == NULL ||
        m3 == NULL || m4 == NULL ||
        m5 == NULL || m6 == NULL ||
        m7 == NULL ||
        t1 == NULL || t2 == NULL ||
        c11 == NULL || c12 == NULL ||
        c21 == NULL || c22 == NULL) {

        goto failure;
    }

    copy_block(a, a11, n, 0, 0, h);
    copy_block(a, a12, n, 0, h, h);
    copy_block(a, a21, n, h, 0, h);
    copy_block(a, a22, n, h, h, h);

    copy_block(b, b11, n, 0, 0, h);
    copy_block(b, b12, n, 0, h, h);
    copy_block(b, b21, n, h, 0, h);
    copy_block(b, b22, n, h, h, h);

    /*
     * M1 = (A11 + A22)(B11 + B22)
     */
    if (!matrix_add(a11, a22, t1, h) ||
        !matrix_add(b11, b22, t2, h) ||
        !strassen_recursive(
            t1, t2, m1, h, multiplications)) {

        goto failure;
    }

    /*
     * M2 = (A21 + A22)B11
     */
    if (!matrix_add(a21, a22, t1, h) ||
        !strassen_recursive(
            t1, b11, m2, h, multiplications)) {

        goto failure;
    }

    /*
     * M3 = A11(B12 - B22)
     */
    if (!matrix_subtract(b12, b22, t2, h) ||
        !strassen_recursive(
            a11, t2, m3, h, multiplications)) {

        goto failure;
    }

    /*
     * M4 = A22(B21 - B11)
     */
    if (!matrix_subtract(b21, b11, t2, h) ||
        !strassen_recursive(
            a22, t2, m4, h, multiplications)) {

        goto failure;
    }

    /*
     * M5 = (A11 + A12)B22
     */
    if (!matrix_add(a11, a12, t1, h) ||
        !strassen_recursive(
            t1, b22, m5, h, multiplications)) {

        goto failure;
    }

    /*
     * M6 = (A21 - A11)(B11 + B12)
     */
    if (!matrix_subtract(a21, a11, t1, h) ||
        !matrix_add(b11, b12, t2, h) ||
        !strassen_recursive(
            t1, t2, m6, h, multiplications)) {

        goto failure;
    }

    /*
     * M7 = (A12 - A22)(B21 + B22)
     */
    if (!matrix_subtract(a12, a22, t1, h) ||
        !matrix_add(b21, b22, t2, h) ||
        !strassen_recursive(
            t1, t2, m7, h, multiplications)) {

        goto failure;
    }

    /*
     * C11 = M1 + M4 - M5 + M7
     */
    if (!matrix_add(m1, m4, t1, h) ||
        !matrix_subtract(t1, m5, t2, h) ||
        !matrix_add(t2, m7, c11, h)) {

        goto failure;
    }

    /*
     * C12 = M3 + M5
     */
    if (!matrix_add(m3, m5, c12, h)) {
        goto failure;
    }

    /*
     * C21 = M2 + M4
     */
    if (!matrix_add(m2, m4, c21, h)) {
        goto failure;
    }

    /*
     * C22 = M1 - M2 + M3 + M6
     */
    if (!matrix_subtract(m1, m2, t1, h) ||
        !matrix_add(t1, m3, t2, h) ||
        !matrix_add(t2, m6, c22, h)) {

        goto failure;
    }

    write_block(c, c11, n, 0, 0, h);
    write_block(c, c12, n, 0, h, h);
    write_block(c, c21, n, h, 0, h);
    write_block(c, c22, n, h, h, h);

    free(a11);
    free(a12);
    free(a21);
    free(a22);

    free(b11);
    free(b12);
    free(b21);
    free(b22);

    free(m1);
    free(m2);
    free(m3);
    free(m4);
    free(m5);
    free(m6);
    free(m7);

    free(t1);
    free(t2);

    free(c11);
    free(c12);
    free(c21);
    free(c22);

    return 1;

failure:

    free(a11);
    free(a12);
    free(a21);
    free(a22);

    free(b11);
    free(b12);
    free(b21);
    free(b22);

    free(m1);
    free(m2);
    free(m3);
    free(m4);
    free(m5);
    free(m6);
    free(m7);

    free(t1);
    free(t2);

    free(c11);
    free(c12);
    free(c21);
    free(c22);

    return 0;
}

int matrix_multiply_strassen(
    const long long *a,
    const long long *b,
    long long *c,
    int n,
    long long *multiplications
) {
    if (multiplications != NULL) {
        *multiplications = 0;
    }

    if (a == NULL ||
        b == NULL ||
        c == NULL ||
        n <= 0) {

        return 0;
    }

    int padded_n = next_power_of_two(n);

    if (padded_n == 0) {
        return 0;
    }

    long long *padded_a = allocate_matrix(padded_n);
    long long *padded_b = allocate_matrix(padded_n);
    long long *padded_c = allocate_matrix(padded_n);

    if (padded_a == NULL ||
        padded_b == NULL ||
        padded_c == NULL) {

        free(padded_a);
        free(padded_b);
        free(padded_c);

        return 0;
    }

    /*
     * Copy original matrices into the top-left
     * corner. Remaining cells stay zero.
     */
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            padded_a[i * padded_n + j] =
                a[i * n + j];

            padded_b[i * padded_n + j] =
                b[i * n + j];
        }
    }

    int ok = strassen_recursive(
        padded_a,
        padded_b,
        padded_c,
        padded_n,
        multiplications
    );

    if (ok) {
        for (int i = 0; i < n; i++) {
            for (int j = 0; j < n; j++) {
                c[i * n + j] =
                    padded_c[i * padded_n + j];
            }
        }
    }

    free(padded_a);
    free(padded_b);
    free(padded_c);

    return ok;
}