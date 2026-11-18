#ifndef STRING_MATCHING_H
#define STRING_MATCHING_H

/*
 * All search functions return the number of matches.
 *
 * positions receives starting indices of matches and
 * must have room for at least text_length + 1 integers.
 *
 * Overlapping matches are included.
 *
 * Returns -1 for invalid input.
 */

int naive_string_match(
    const char *text,
    const char *pattern,
    int positions[],
    long long *comparisons
);

/*
 * Rabin-Karp uses a rolling hash, but every hash hit
 * is verified character-by-character to eliminate
 * false positives caused by collisions.
 */
int rabin_karp_match(
    const char *text,
    const char *pattern,
    int positions[],
    long long *operations
);

/*
 * Finite-automaton string matching.
 *
 * Uses the full unsigned-char alphabet (256 symbols).
 */
int finite_automaton_match(
    const char *text,
    const char *pattern,
    int positions[],
    long long *operations
);

/*
 * Knuth-Morris-Pratt string matching.
 */
int kmp_string_match(
    const char *text,
    const char *pattern,
    int positions[],
    long long *comparisons
);

#endif