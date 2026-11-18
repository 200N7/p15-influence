#include <stdio.h>
#include <limits.h>
#include "algorithms/divide_conquer.h"

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
    long long operations;

    printf("DIVIDE AND CONQUER TESTS - PART 1\n");
    printf("========================================\n");

    /* --------------------------------------------------
       MAXIMUM SUBARRAY
       -------------------------------------------------- */

    {
        int a[] = {
            -2, 1, -3, 4, -1, 2, 1, -5, 4
        };

        struct SubarrayResult dc =
            max_subarray_dc(a, 9, &operations);

        check(
            dc.sum == 6 &&
            dc.left == 3 &&
            dc.right == 6,
            "maximum subarray normal mixed input"
        );
    }

    {
        int a[] = {-8, -3, -6, -2, -5, -4};

        struct SubarrayResult dc =
            max_subarray_dc(a, 6, &operations);

        check(
            dc.sum == -2 &&
            dc.left == 3 &&
            dc.right == 3,
            "maximum subarray all-negative input"
        );
    }

    {
        int a[] = {7};

        struct SubarrayResult dc =
            max_subarray_dc(a, 1, &operations);

        check(
            dc.sum == 7 &&
            dc.left == 0 &&
            dc.right == 0,
            "maximum subarray single element"
        );
    }

    {
        struct SubarrayResult dc =
            max_subarray_dc(NULL, 0, &operations);

        check(
            dc.left == -1 &&
            dc.right == -1,
            "maximum subarray empty input"
        );
    }

    /*
     * Kadane is the O(n) oracle for our
     * O(n log n) divide-and-conquer implementation.
     */
    {
        int a[] = {
            13, -3, -25, 20, -3, -16, -23,
            18, 20, -7, 12, -5, -22, 15, -4, 7
        };

        long long dc_operations;
        long long kadane_operations;

        struct SubarrayResult dc =
            max_subarray_dc(
                a,
                16,
                &dc_operations
            );

        struct SubarrayResult kadane =
            max_subarray_kadane(
                a,
                16,
                &kadane_operations
            );

        check(
            dc.sum == kadane.sum,
            "D&C maximum subarray agrees with Kadane"
        );

        check(
            dc.sum == 43,
            "maximum subarray known oracle value"
        );
    }

    /* --------------------------------------------------
       FAST EXPONENTIATION
       -------------------------------------------------- */

    {
        long long result;
        long long multiplications;

        int ok = fast_power_ll(
            2,
            10,
            &result,
            &multiplications
        );

        check(
            ok && result == 1024,
            "fast power normal exponent"
        );

        check(
            multiplications < 10,
            "fast power uses fewer multiplications"
        );
    }

    {
        long long result;

        int ok = fast_power_ll(
            12345,
            0,
            &result,
            &operations
        );

        check(
            ok && result == 1,
            "fast power exponent zero"
        );
    }

    {
        long long result;

        int ok = fast_power_ll(
            -2,
            5,
            &result,
            &operations
        );

        check(
            ok && result == -32,
            "fast power negative base"
        );
    }

    {
        long long result;

        int ok = fast_power_ll(
            2,
            63,
            &result,
            &operations
        );

        check(
            !ok,
            "fast power detects signed overflow"
        );
    }

    {
        int ok = fast_power_ll(
            2,
            10,
            NULL,
            &operations
        );

        check(
            !ok,
            "fast power rejects null result pointer"
        );
    }

    /* --------------------------------------------------
       CELEBRITY
       -------------------------------------------------- */

    {
        /*
         * Person 2 is the celebrity.
         *
         * 0 knows 2
         * 1 knows 2
         * 3 knows 2
         * 2 knows nobody
         */
        int knows[] = {
            0, 0, 1, 0,
            0, 0, 1, 0,
            0, 0, 0, 0,
            0, 0, 1, 0
        };

        long long checks;

        int celebrity = find_celebrity(
            knows,
            4,
            &checks
        );

        check(
            celebrity == 2,
            "celebrity exists"
        );

        check(
            checks <= 3 * (4 - 1),
            "celebrity uses linear number of checks"
        );
    }

    {
        /*
         * No celebrity:
         * 0 knows 1,
         * 1 knows 2,
         * 2 knows 0.
         */
        int knows[] = {
            0, 1, 0,
            0, 0, 1,
            1, 0, 0
        };

        int celebrity = find_celebrity(
            knows,
            3,
            &operations
        );

        check(
            celebrity == -1,
            "celebrity none exists"
        );
    }

    {
        int knows[] = {0};

        check(
            find_celebrity(
                knows,
                1,
                &operations
            ) == 0,
            "celebrity single person"
        );
    }

    {
        check(
            find_celebrity(
                NULL,
                0,
                &operations
            ) == -1,
            "celebrity empty input"
        );
    }
    /* --------------------------------------------------
       MATRIX MULTIPLICATION
       -------------------------------------------------- */

    {
        long long a[] = {
            1, 2,
            3, 4
        };

        long long b[] = {
            5, 6,
            7, 8
        };

        long long expected[] = {
            19, 22,
            43, 50
        };

        long long classical[4];
        long long strassen[4];

        long long classical_mult;
        long long strassen_mult;

        int ok1 = matrix_multiply_classical(
            a,
            b,
            classical,
            2,
            &classical_mult
        );

        int ok2 = matrix_multiply_strassen(
            a,
            b,
            strassen,
            2,
            &strassen_mult
        );

        int correct = 1;

        for (int i = 0; i < 4; i++) {
            if (classical[i] != expected[i] ||
                strassen[i] != expected[i]) {

                correct = 0;
            }
        }

        check(
            ok1 && ok2 && correct,
            "2x2 classical and Strassen known result"
        );

        check(
            strassen_mult == 7,
            "Strassen uses 7 scalar multiplications for 2x2"
        );

        check(
            classical_mult == 8,
            "classical uses 8 scalar multiplications for 2x2"
        );
    }

    /*
     * 3x3 is deliberately important.
     *
     * Strassen naturally works on power-of-two sizes,
     * so our implementation must internally pad 3x3
     * to 4x4 and still return only the correct 3x3 result.
     */
    {
        long long a[] = {
            1, 2, 3,
            4, 5, 6,
            7, 8, 9
        };

        long long b[] = {
            9, 8, 7,
            6, 5, 4,
            3, 2, 1
        };

        long long classical[9];
        long long strassen[9];

        int ok1 = matrix_multiply_classical(
            a,
            b,
            classical,
            3,
            NULL
        );

        int ok2 = matrix_multiply_strassen(
            a,
            b,
            strassen,
            3,
            NULL
        );

        int same = 1;

        for (int i = 0; i < 9; i++) {
            if (classical[i] != strassen[i]) {
                same = 0;
                break;
            }
        }

        check(
            ok1 && ok2 && same,
            "Strassen padding works for 3x3 matrix"
        );
    }

    {
        long long a[] = {
            -1, 2,
            3, -4
        };

        long long b[] = {
            5, -6,
            -7, 8
        };

        long long classical[4];
        long long strassen[4];

        int ok1 = matrix_multiply_classical(
            a,
            b,
            classical,
            2,
            NULL
        );

        int ok2 = matrix_multiply_strassen(
            a,
            b,
            strassen,
            2,
            NULL
        );

        int same = 1;

        for (int i = 0; i < 4; i++) {
            if (classical[i] != strassen[i]) {
                same = 0;
            }
        }

        check(
            ok1 && ok2 && same,
            "matrix multiplication negative values"
        );
    }

    {
        long long a[] = {7};
        long long b[] = {6};

        long long classical[1];
        long long strassen[1];

        int ok1 = matrix_multiply_classical(
            a,
            b,
            classical,
            1,
            NULL
        );

        int ok2 = matrix_multiply_strassen(
            a,
            b,
            strassen,
            1,
            NULL
        );

        check(
            ok1 &&
            ok2 &&
            classical[0] == 42 &&
            strassen[0] == 42,
            "matrix multiplication 1x1 boundary"
        );
    }

    {
        long long c[1];

        check(
            !matrix_multiply_classical(
                NULL,
                NULL,
                c,
                0,
                NULL
            ),
            "classical matrix rejects invalid input"
        );

        check(
            !matrix_multiply_strassen(
                NULL,
                NULL,
                c,
                0,
                NULL
            ),
            "Strassen rejects invalid input"
        );
    }

    /*
     * Overflow must be rejected rather than silently
     * producing a corrupt matrix.
     */
    {
        long long a[] = {LLONG_MAX};
        long long b[] = {2};
        long long c[1];

        check(
            !matrix_multiply_classical(
                a,
                b,
                c,
                1,
                NULL
            ),
            "classical matrix detects multiplication overflow"
        );

        check(
            !matrix_multiply_strassen(
                a,
                b,
                c,
                1,
                NULL
            ),
            "Strassen detects multiplication overflow"
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