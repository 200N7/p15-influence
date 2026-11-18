#include <stdio.h>
#include <limits.h>
#include "algorithms/number.h"

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

    printf("NUMBER REPRESENTATION / CRT TESTS\n");
    printf("========================================\n");

    /* --------------------------------------------------
       GCD
       -------------------------------------------------- */

    check(
        gcd_ll(48, 18) == 6,
        "GCD normal input"
    );

    check(
        gcd_ll(-48, 18) == 6,
        "GCD handles negative input"
    );

    check(
        gcd_ll(0, 7) == 7,
        "GCD handles zero"
    );

    /* --------------------------------------------------
       EXTENDED EUCLIDEAN ALGORITHM
       -------------------------------------------------- */

    {
        long long x;
        long long y;

        long long gcd = extended_gcd(
            240,
            46,
            &x,
            &y
        );

        check(
            gcd == 2,
            "extended GCD returns correct gcd"
        );

        check(
            240 * x + 46 * y == gcd,
            "extended GCD satisfies Bezout identity"
        );
    }

    /* --------------------------------------------------
       MODULAR INVERSE
       -------------------------------------------------- */

    {
        long long inverse;

        int ok = modular_inverse(
            3,
            11,
            &inverse
        );

        check(
            ok == 1 && inverse == 4,
            "modular inverse known result"
        );
    }

    {
        long long inverse;

        check(
            modular_inverse(
                6,
                9,
                &inverse
            ) == 0,
            "modular inverse rejects non-coprime values"
        );
    }

    {
        long long inverse;

        int ok = modular_inverse(
            -3,
            11,
            &inverse
        );

        check(
            ok == 1 &&
            ((-3 * inverse) % 11 + 11) % 11 == 1,
            "modular inverse handles negative value"
        );
    }

    /* --------------------------------------------------
       CHINESE REMAINDER THEOREM
       -------------------------------------------------- */

    {
        /*
         * x = 2 (mod 3)
         * x = 3 (mod 5)
         * x = 2 (mod 7)
         *
         * Unique solution:
         * x = 23 (mod 105)
         */
        long long remainders[] = {
            2, 3, 2
        };

        long long moduli[] = {
            3, 5, 7
        };

        long long result;
        long long product;

        int ok = chinese_remainder(
            remainders,
            moduli,
            3,
            &result,
            &product,
            &operations
        );

        check(
            ok == 1 &&
            result == 23 &&
            product == 105,
            "CRT known solution 23 modulo 105"
        );

        check(
            operations > 0,
            "CRT records operations"
        );

        int satisfies = 1;

        for (int i = 0; i < 3; i++) {
            if (result % moduli[i] !=
                remainders[i]) {

                satisfies = 0;
            }
        }

        check(
            satisfies,
            "CRT result satisfies every congruence"
        );
    }

    /* Negative remainder normalization. */
    {
        long long remainders[] = {
            -1, 3
        };

        long long moduli[] = {
            5, 7
        };

        long long result;
        long long product;

        int ok = chinese_remainder(
            remainders,
            moduli,
            2,
            &result,
            &product,
            &operations
        );

        check(
            ok == 1 &&
            result >= 0 &&
            result < product &&
            result % 5 == 4 &&
            result % 7 == 3,
            "CRT normalizes negative remainder"
        );
    }

    /* Single congruence. */
    {
        long long remainders[] = {
            17
        };

        long long moduli[] = {
            5
        };

        long long result;
        long long product;

        int ok = chinese_remainder(
            remainders,
            moduli,
            1,
            &result,
            &product,
            &operations
        );

        check(
            ok == 1 &&
            result == 2 &&
            product == 5,
            "CRT single modulus"
        );
    }

    /* Pairwise coprimality is required. */
    {
        long long remainders[] = {
            1, 2
        };

        long long moduli[] = {
            6, 8
        };

        long long result;
        long long product;

        check(
            chinese_remainder(
                remainders,
                moduli,
                2,
                &result,
                &product,
                &operations
            ) == 0,
            "CRT rejects non-coprime moduli"
        );
    }

    /* Modulus must be greater than one. */
    {
        long long remainders[] = {
            0, 1
        };

        long long moduli[] = {
            1, 5
        };

        long long result;
        long long product;

        check(
            chinese_remainder(
                remainders,
                moduli,
                2,
                &result,
                &product,
                &operations
            ) == 0,
            "CRT rejects invalid modulus"
        );
    }

    /*
     * Pairwise-coprime moduli whose product cannot fit
     * inside signed long long must be rejected safely.
     */
    {
        long long remainders[] = {
            1, 1
        };

        long long moduli[] = {
            3037000493LL,
            3037000507LL
        };

        long long result;
        long long product;

        check(
            chinese_remainder(
                remainders,
                moduli,
                2,
                &result,
                &product,
                &operations
            ) == 0,
            "CRT rejects modulus-product overflow"
        );
    }

    /* Null/empty input. */
    {
        long long result;
        long long product;

        check(
            chinese_remainder(
                NULL,
                NULL,
                0,
                &result,
                &product,
                &operations
            ) == 0,
            "CRT rejects empty input"
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