#!/usr/bin/env bash

set -e

CC=gcc
FLAGS="-std=c11 -Wall -Wextra -Wpedantic -Iinclude"

echo "========================================"
echo " P15 FULL AUTOMATED TEST SUITE"
echo "========================================"

run_test() {
    name="$1"
    source_file="$2"
    algorithm_file="$3"
    extra_flags="${4:-}"

    echo
    echo "----------------------------------------"
    echo "Building $name"
    echo "----------------------------------------"

    $CC $FLAGS \
        "$source_file" \
        "$algorithm_file" \
        -o "$name" \
        $extra_flags

    echo "Running $name"
    echo

    ./"$name"
}

run_test \
    test_sort_search \
    tests/test_sort_search.c \
    src/algorithms/sort_search.c

run_test \
    test_divide_conquer \
    tests/test_divide_conquer.c \
    src/algorithms/divide_conquer.c \
    "-lm"

run_test \
    test_greedy \
    tests/test_greedy.c \
    src/algorithms/greedy.c \
    "-lm"

run_test \
    test_dp \
    tests/test_dp.c \
    src/algorithms/dp.c

run_test \
    test_backtracking \
    tests/test_backtracking.c \
    src/algorithms/backtracking.c

run_test \
    test_branch_bound \
    tests/test_branch_bound.c \
    src/algorithms/branch_bound.c

run_test \
    test_flow_matching \
    tests/test_flow_matching.c \
    src/algorithms/flow_matching.c

run_test \
    test_number \
    tests/test_number.c \
    src/algorithms/number.c

run_test \
    test_string_matching \
    tests/test_string_matching.c \
    src/algorithms/string_matching.c

echo
echo "========================================"
echo " ALL ALGORITHM TEST SUITES COMPLETED"
echo "========================================"