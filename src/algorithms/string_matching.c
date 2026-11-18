#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "algorithms/string_matching.h"

static int validate_search(
    const char *text,
    const char *pattern,
    int positions[]
) {
    if (text == NULL ||
        pattern == NULL ||
        positions == NULL) {

        return 0;
    }

    /*
     * We deliberately reject an empty pattern because
     * its matching convention differs between APIs.
     */
    if (pattern[0] == '\0') {
        return 0;
    }

    return 1;
}

/* =========================================================
   NAIVE STRING MATCHING
   ========================================================= */

int naive_string_match(
    const char *text,
    const char *pattern,
    int positions[],
    long long *comparisons
) {
    if (comparisons != NULL) {
        *comparisons = 0;
    }

    if (!validate_search(
            text,
            pattern,
            positions)) {

        return -1;
    }

    size_t n = strlen(text);
    size_t m = strlen(pattern);

    if (m > n) {
        return 0;
    }

    int count = 0;

    for (size_t i = 0; i <= n - m; i++) {
        size_t j = 0;

        while (j < m) {
            if (comparisons != NULL) {
                (*comparisons)++;
            }

            if (text[i + j] != pattern[j]) {
                break;
            }

            j++;
        }

        if (j == m) {
            positions[count++] = (int)i;
        }
    }

    return count;
}

/* =========================================================
   RABIN-KARP
   ========================================================= */

int rabin_karp_match(
    const char *text,
    const char *pattern,
    int positions[],
    long long *operations
) {
    if (operations != NULL) {
        *operations = 0;
    }

    if (!validate_search(
            text,
            pattern,
            positions)) {

        return -1;
    }

    size_t n = strlen(text);
    size_t m = strlen(pattern);

    if (m > n) {
        return 0;
    }

    const uint64_t base = 256ULL;
    const uint64_t modulus = 1000003ULL;

    uint64_t highest_power = 1;

    for (size_t i = 1; i < m; i++) {
        highest_power =
            (highest_power * base) % modulus;
    }

    uint64_t pattern_hash = 0;
    uint64_t window_hash = 0;

    for (size_t i = 0; i < m; i++) {
        pattern_hash =
            (pattern_hash * base +
             (unsigned char)pattern[i]) %
            modulus;

        window_hash =
            (window_hash * base +
             (unsigned char)text[i]) %
            modulus;

        if (operations != NULL) {
            (*operations)++;
        }
    }

    int count = 0;

    for (size_t i = 0; i <= n - m; i++) {
        if (operations != NULL) {
            (*operations)++;
        }

        if (pattern_hash == window_hash) {
            /*
             * Mandatory verification:
             * equal hashes do not prove equal strings.
             */
            size_t j = 0;

            while (j < m) {
                if (operations != NULL) {
                    (*operations)++;
                }

                if (text[i + j] != pattern[j]) {
                    break;
                }

                j++;
            }

            if (j == m) {
                positions[count++] = (int)i;
            }
        }

        if (i < n - m) {
            uint64_t outgoing =
                ((uint64_t)
                 (unsigned char)text[i] *
                 highest_power) %
                modulus;

            if (window_hash < outgoing) {
                window_hash += modulus;
            }

            window_hash -= outgoing;

            window_hash =
                (window_hash * base +
                 (unsigned char)text[i + m]) %
                modulus;
        }
    }

    return count;
}

/* =========================================================
   FINITE AUTOMATON
   ========================================================= */

static int suffix_is_prefix(
    const char *pattern,
    size_t pattern_length,
    size_t state,
    unsigned char symbol,
    size_t candidate
) {
    if (candidate == 0) {
        return 1;
    }

    /*
     * Candidate prefix ends with the new symbol.
     */
    if ((unsigned char)pattern[candidate - 1] !=
        symbol) {

        return 0;
    }

    if (candidate == 1) {
        return 1;
    }

    /*
     * Compare the preceding candidate-1 characters
     * against the suffix of the current matched prefix.
     */
    if (candidate - 1 > state) {
        return 0;
    }

    size_t start =
        state - (candidate - 1);

    for (size_t i = 0;
         i < candidate - 1;
         i++) {

        if (pattern[i] !=
            pattern[start + i]) {

            return 0;
        }
    }

    (void)pattern_length;
    return 1;
}

int finite_automaton_match(
    const char *text,
    const char *pattern,
    int positions[],
    long long *operations
) {
    if (operations != NULL) {
        *operations = 0;
    }

    if (!validate_search(
            text,
            pattern,
            positions)) {

        return -1;
    }

    size_t n = strlen(text);
    size_t m = strlen(pattern);

    if (m > n) {
        return 0;
    }

    size_t rows = m + 1U;

    if (rows > SIZE_MAX / 256U ||
        rows * 256U >
            SIZE_MAX / sizeof(size_t)) {

        return -1;
    }

    size_t *transition =
        malloc(
            rows * 256U *
            sizeof(size_t)
        );

    if (transition == NULL) {
        return -1;
    }

#define TRANS(state, symbol) \
    transition[(state) * 256U + \
               (unsigned int)(symbol)]

    /*
     * Build transition table.
     */
    for (size_t state = 0;
         state <= m;
         state++) {

        for (int symbol = 0;
             symbol < 256;
             symbol++) {

            size_t maximum =
                state + 1U < m
                    ? state + 1U
                    : m;

            size_t next_state = 0;

            for (size_t candidate = maximum;
                 candidate > 0;
                 candidate--) {

                if (operations != NULL) {
                    (*operations)++;
                }

                if (suffix_is_prefix(
                        pattern,
                        m,
                        state,
                        (unsigned char)symbol,
                        candidate)) {

                    next_state = candidate;
                    break;
                }
            }

            TRANS(state, symbol) =
                next_state;
        }
    }

    size_t state = 0;
    int count = 0;

    for (size_t i = 0; i < n; i++) {
        state = TRANS(
            state,
            (unsigned char)text[i]
        );

        if (operations != NULL) {
            (*operations)++;
        }

        if (state == m) {
            positions[count++] =
                (int)(i + 1U - m);
        }
    }

#undef TRANS

    free(transition);

    return count;
}

/* =========================================================
   KMP
   ========================================================= */

int kmp_string_match(
    const char *text,
    const char *pattern,
    int positions[],
    long long *comparisons
) {
    if (comparisons != NULL) {
        *comparisons = 0;
    }

    if (!validate_search(
            text,
            pattern,
            positions)) {

        return -1;
    }

    size_t n = strlen(text);
    size_t m = strlen(pattern);

    if (m > n) {
        return 0;
    }

    size_t *lps =
        malloc(m * sizeof(size_t));

    if (lps == NULL) {
        return -1;
    }

    lps[0] = 0;

    size_t length = 0;
    size_t i = 1;

    while (i < m) {
        if (comparisons != NULL) {
            (*comparisons)++;
        }

        if (pattern[i] == pattern[length]) {
            length++;
            lps[i] = length;
            i++;
        } else if (length != 0) {
            length = lps[length - 1];
        } else {
            lps[i] = 0;
            i++;
        }
    }

    int count = 0;

    i = 0;
    size_t j = 0;

    while (i < n) {
        if (comparisons != NULL) {
            (*comparisons)++;
        }

        if (text[i] == pattern[j]) {
            i++;
            j++;

            if (j == m) {
                positions[count++] =
                    (int)(i - j);

                /*
                 * Do not reset to zero:
                 * this preserves overlapping matches.
                 */
                j = lps[j - 1];
            }
        } else if (j != 0) {
            j = lps[j - 1];
        } else {
            i++;
        }
    }

    free(lps);
    return count;
}