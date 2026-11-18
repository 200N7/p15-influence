#ifndef NUMBER_H
#define NUMBER_H

/*
 * Greatest common divisor.
 */
long long gcd_ll(long long a, long long b);

/*
 * Extended Euclidean algorithm.
 *
 * Finds x and y such that:
 *
 *     ax + by = gcd(a,b)
 *
 * Returns gcd(a,b).
 */
long long extended_gcd(
    long long a,
    long long b,
    long long *x,
    long long *y
);

/*
 * Modular inverse.
 *
 * Finds inverse of a modulo m.
 *
 * Returns:
 *   1 if inverse exists
 *   0 otherwise
 */
int modular_inverse(
    long long a,
    long long m,
    long long *inverse
);

/*
 * Chinese Remainder Theorem.
 *
 * Solves:
 *
 * x = remainder[i] (mod modulus[i])
 *
 * All moduli must be > 1 and pairwise coprime.
 *
 * result receives the unique solution in:
 *
 *     0 <= result < product
 *
 * product receives the product of all moduli.
 *
 * Returns:
 *   1 success
 *   0 invalid/non-coprime/overflow
 */
int chinese_remainder(
    const long long remainders[],
    const long long moduli[],
    int count,
    long long *result,
    long long *product,
    long long *operations
);

#endif