#include <stdio.h>
#include <string.h>
#include "algorithms/string_matching.h"

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

static int arrays_equal(
    const int a[],
    const int b[],
    int count
) {
    for (int i = 0; i < count; i++) {
        if (a[i] != b[i]) {
            return 0;
        }
    }

    return 1;
}

/*
 * Run all four algorithms and verify that they produce
 * exactly the same number of matches and positions.
 */
static int all_algorithms_agree(
    const char *text,
    const char *pattern
) {
    int naive[256];
    int rk[256];
    int fa[256];
    int kmp[256];

    long long operations;

    int n1 = naive_string_match(
        text,
        pattern,
        naive,
        &operations
    );

    int n2 = rabin_karp_match(
        text,
        pattern,
        rk,
        &operations
    );

    int n3 = finite_automaton_match(
        text,
        pattern,
        fa,
        &operations
    );

    int n4 = kmp_string_match(
        text,
        pattern,
        kmp,
        &operations
    );

    if (n1 < 0 ||
        n1 != n2 ||
        n1 != n3 ||
        n1 != n4) {

        return 0;
    }

    return
        arrays_equal(naive, rk, n1) &&
        arrays_equal(naive, fa, n1) &&
        arrays_equal(naive, kmp, n1);
}

int main(void) {
    long long operations;

    printf("STRING MATCHING TESTS\n");
    printf("========================================\n");

    /* --------------------------------------------------
       NORMAL MULTIPLE MATCHES
       -------------------------------------------------- */

    {
        const char *text =
            "ABABDABACDABABCABAB";

        const char *pattern =
            "ABABCABAB";

        int positions[32];

        int count = kmp_string_match(
            text,
            pattern,
            positions,
            &operations
        );

        check(
            count == 1 &&
            positions[0] == 10,
            "KMP known match position"
        );

        check(
            all_algorithms_agree(
                text,
                pattern
            ),
            "all algorithms agree on normal input"
        );
    }

    /* --------------------------------------------------
       OVERLAPPING MATCHES
       -------------------------------------------------- */

    {
        const char *text = "aaaaa";
        const char *pattern = "aaa";

        int positions[16];

        int count = naive_string_match(
            text,
            pattern,
            positions,
            &operations
        );

        check(
            count == 3 &&
            positions[0] == 0 &&
            positions[1] == 1 &&
            positions[2] == 2,
            "naive finds overlapping matches"
        );

        check(
            all_algorithms_agree(
                text,
                pattern
            ),
            "all algorithms preserve overlapping matches"
        );
    }

    /* --------------------------------------------------
       BEGINNING AND END
       -------------------------------------------------- */

    {
        const char *text =
            "TEST middle TEST";

        const char *pattern =
            "TEST";

        int positions[32];

        int count = rabin_karp_match(
            text,
            pattern,
            positions,
            &operations
        );

        check(
            count == 2 &&
            positions[0] == 0 &&
            positions[1] == 12,
            "Rabin-Karp finds beginning and end matches"
        );

        check(
            all_algorithms_agree(
                text,
                pattern
            ),
            "all algorithms agree at text boundaries"
        );
    }

    /* --------------------------------------------------
       NO MATCH
       -------------------------------------------------- */

    {
        check(
            all_algorithms_agree(
                "social network analysis",
                "xyz"
            ),
            "all algorithms agree when pattern absent"
        );

        int positions[32];

        check(
            kmp_string_match(
                "social network analysis",
                "xyz",
                positions,
                &operations
            ) == 0,
            "string matching reports no match"
        );
    }

    /* --------------------------------------------------
       PATTERN LONGER THAN TEXT
       -------------------------------------------------- */

    {
        check(
            all_algorithms_agree(
                "abc",
                "abcdef"
            ),
            "all algorithms handle pattern longer than text"
        );
    }

    /* --------------------------------------------------
       SINGLE CHARACTER
       -------------------------------------------------- */

    {
        const char *text = "banana";
        const char *pattern = "a";

        int positions[16];

        int count = finite_automaton_match(
            text,
            pattern,
            positions,
            &operations
        );

        check(
            count == 3 &&
            positions[0] == 1 &&
            positions[1] == 3 &&
            positions[2] == 5,
            "finite automaton handles single-character pattern"
        );

        check(
            all_algorithms_agree(
                text,
                pattern
            ),
            "all algorithms agree on single-character pattern"
        );
    }

    /* --------------------------------------------------
       PATTERN EQUALS TEXT
       -------------------------------------------------- */

    {
        check(
            all_algorithms_agree(
                "#viral",
                "#viral"
            ),
            "all algorithms handle pattern equal to text"
        );

        int positions[16];

        check(
            naive_string_match(
                "#viral",
                "#viral",
                positions,
                &operations
            ) == 1 &&
            positions[0] == 0,
            "whole-text match occurs at zero"
        );
    }

    /* --------------------------------------------------
       ADVERSARIAL REPETITIVE INPUT

       Important for comparing naive search with KMP.
       -------------------------------------------------- */

    {
        const char *text =
            "aaaaaaaaaaaaaaaaaaaaaaaaab";

        const char *pattern =
            "aaaaab";

        check(
            all_algorithms_agree(
                text,
                pattern
            ),
            "all algorithms agree on adversarial repetitive input"
        );

        int naive_positions[64];
        int kmp_positions[64];

        long long naive_comparisons;
        long long kmp_comparisons;

        int n1 = naive_string_match(
            text,
            pattern,
            naive_positions,
            &naive_comparisons
        );

        int n2 = kmp_string_match(
            text,
            pattern,
            kmp_positions,
            &kmp_comparisons
        );

        check(
            n1 == n2 &&
            arrays_equal(
                naive_positions,
                kmp_positions,
                n1
            ),
            "naive and KMP return identical adversarial result"
        );

        check(
            kmp_comparisons <
            naive_comparisons,
            "KMP uses fewer comparisons on repetitive adversarial input"
        );
    }

    /* --------------------------------------------------
       POST / HASHTAG-LIKE TEXT
       -------------------------------------------------- */

    {
        const char *text =
            "#ai #daa #ai #network #ai";

        const char *pattern =
            "#ai";

        int positions[32];

        int count = kmp_string_match(
            text,
            pattern,
            positions,
            &operations
        );

        check(
            count == 3,
            "string matching finds repeated hashtag"
        );

        check(
            all_algorithms_agree(
                text,
                pattern
            ),
            "all algorithms agree on hashtag-like text"
        );
    }

    /* --------------------------------------------------
       OPERATION COUNTERS
       -------------------------------------------------- */

    {
        int positions[32];

        naive_string_match(
            "abababab",
            "abab",
            positions,
            &operations
        );

        check(
            operations > 0,
            "naive records comparisons"
        );

        rabin_karp_match(
            "abababab",
            "abab",
            positions,
            &operations
        );

        check(
            operations > 0,
            "Rabin-Karp records operations"
        );

        finite_automaton_match(
            "abababab",
            "abab",
            positions,
            &operations
        );

        check(
            operations > 0,
            "finite automaton records operations"
        );

        kmp_string_match(
            "abababab",
            "abab",
            positions,
            &operations
        );

        check(
            operations > 0,
            "KMP records comparisons"
        );
    }

    /* --------------------------------------------------
       INVALID INPUT
       -------------------------------------------------- */

    {
        int positions[16];

        check(
            naive_string_match(
                "abc",
                "",
                positions,
                &operations
            ) == -1,
            "naive rejects empty pattern"
        );

        check(
            rabin_karp_match(
                "abc",
                "",
                positions,
                &operations
            ) == -1,
            "Rabin-Karp rejects empty pattern"
        );

        check(
            finite_automaton_match(
                "abc",
                "",
                positions,
                &operations
            ) == -1,
            "finite automaton rejects empty pattern"
        );

        check(
            kmp_string_match(
                "abc",
                "",
                positions,
                &operations
            ) == -1,
            "KMP rejects empty pattern"
        );
    }

    {
        int positions[16];

        check(
            kmp_string_match(
                NULL,
                "abc",
                positions,
                &operations
            ) == -1,
            "string matching rejects null text"
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