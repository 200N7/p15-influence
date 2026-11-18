#!/usr/bin/env bash

set -e

CC=gcc
FLAGS="-std=c11 -Wall -Wextra -Wpedantic -Iinclude"

echo "========================================"
echo " BUILDING P15 APPLICATION"
echo "========================================"

$CC $FLAGS \
    src/main.c \
    src/graph.c \
    src/influence.c \
    src/algorithms/greedy.c \
    src/algorithms/dp.c \
    src/algorithms/flow_matching.c \
    src/algorithms/string_matching.c \
    -o p15 \
    -lm

echo "[PASS] Main application built"

echo
echo "========================================"
echo " BUILDING BENCHMARK SUITE"
echo "========================================"

$CC $FLAGS \
    bench/benchmark.c \
    src/algorithms/sort_search.c \
    src/algorithms/greedy.c \
    src/algorithms/dp.c \
    src/algorithms/string_matching.c \
    -o benchmark \
    -lm

echo "[PASS] Benchmark suite built"

echo
echo "========================================"
echo " BUILD COMPLETE"
echo "========================================"
echo "Run application: ./p15 data/normal.txt"
echo "Run benchmarks:  ./benchmark"
echo "Run all tests:   bash tests/run_all.sh"