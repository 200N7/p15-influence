#include <limits.h>
#include "algorithms/number.h"

static long long absolute_ll(long long value) {
    if (value >= 0) {
        return value;
    }

    /*
     * LLONG_MIN cannot be represented as positive
     * signed long long.
     */
    if (value == LLONG_MIN) {
        return LLONG_MAX;
    }

    return -value;
}

long long gcd_ll(long long a, long long b) {
    a = absolute_ll(a);
    b = absolute_ll(b);

    while (b != 0) {
        long long temp = a % b;
        a = b;
        b = temp;
    }

    return a;
}

long long extended_gcd(
    long long a,
    long long b,
    long long *x,
    long long *y
) {
    if (x == 0 || y == 0) {
        return -1;
    }

    long long old_r = a;
    long long r = b;

    long long old_s = 1;
    long long s = 0;

    long long old_t = 0;
    long long t = 1;

    while (r != 0) {
        long long quotient = old_r / r;

        long long next_r =
            old_r - quotient * r;

        old_r = r;
        r = next_r;

        long long next_s =
            old_s - quotient * s;

        old_s = s;
        s = next_s;

        long long next_t =
            old_t - quotient * t;

        old_t = t;
        t = next_t;
    }

    if (old_r < 0) {
        old_r = -old_r;
        old_s = -old_s;
        old_t = -old_t;
    }

    *x = old_s;
    *y = old_t;

    return old_r;
}

int modular_inverse(
    long long a,
    long long m,
    long long *inverse
) {
    if (inverse == 0 || m <= 1) {
        return 0;
    }

    a %= m;

    if (a < 0) {
        a += m;
    }

    long long x;
    long long y;

    long long gcd =
        extended_gcd(
            a,
            m,
            &x,
            &y
        );

    if (gcd != 1) {
        return 0;
    }

    x %= m;

    if (x < 0) {
        x += m;
    }

    *inverse = x;

    return 1;
}

static int safe_multiply_positive(
    long long a,
    long long b,
    long long *result
) {
    if (a < 0 || b < 0 || result == 0) {
        return 0;
    }

    if (a != 0 &&
        b > LLONG_MAX / a) {

        return 0;
    }

    *result = a * b;
    return 1;
}

/*
 * Overflow-safe modular multiplication using
 * repeated doubling.
 *
 * Computes:
 *
 *     (a * b) mod m
 *
 * without requiring a*b to fit in long long.
 */
static long long modular_multiply(
    long long a,
    long long b,
    long long m,
    long long *operations
) {
    long long result = 0;

    a %= m;
    b %= m;

    if (a < 0) {
        a += m;
    }

    if (b < 0) {
        b += m;
    }

    while (b > 0) {
        if (operations != 0) {
            (*operations)++;
        }

        if (b & 1LL) {
            if (result >= m - a) {
                result =
                    result - (m - a);
            } else {
                result += a;
            }
        }

        b >>= 1;

        if (b > 0) {
            if (a >= m - a) {
                a =
                    a - (m - a);
            } else {
                a += a;
            }
        }
    }

    return result;
}

int chinese_remainder(
    const long long remainders[],
    const long long moduli[],
    int count,
    long long *result,
    long long *product,
    long long *operations
) {
    if (operations != 0) {
        *operations = 0;
    }

    if (remainders == 0 ||
        moduli == 0 ||
        result == 0 ||
        product == 0 ||
        count <= 0) {

        return 0;
    }

    /*
     * Validate moduli and pairwise coprimality.
     */
    for (int i = 0; i < count; i++) {
        if (moduli[i] <= 1) {
            return 0;
        }

        for (int j = i + 1;
             j < count;
             j++) {

            if (operations != 0) {
                (*operations)++;
            }

            if (gcd_ll(
                    moduli[i],
                    moduli[j]) != 1) {

                return 0;
            }
        }
    }

    long long total_product = 1;

    for (int i = 0; i < count; i++) {
        long long next_product;

        if (!safe_multiply_positive(
                total_product,
                moduli[i],
                &next_product)) {

            return 0;
        }

        total_product = next_product;
    }

    long long answer = 0;

    for (int i = 0; i < count; i++) {
        long long modulus = moduli[i];

        long long normalized =
            remainders[i] % modulus;

        if (normalized < 0) {
            normalized += modulus;
        }

        long long partial_product =
            total_product / modulus;

        long long inverse;

        if (!modular_inverse(
                partial_product % modulus,
                modulus,
                &inverse)) {

            return 0;
        }

        long long term =
            modular_multiply(
                normalized,
                partial_product,
                total_product,
                operations
            );

        term =
            modular_multiply(
                term,
                inverse,
                total_product,
                operations
            );

        /*
         * answer = (answer + term) mod product
         * without overflowing.
         */
        if (answer >=
            total_product - term) {

            answer =
                answer -
                (total_product - term);
        } else {
            answer += term;
        }
    }

    *result = answer;
    *product = total_product;

    return 1;
}