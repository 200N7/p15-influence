\# Testing and Reliability Report

\## 1. Testing Strategy

P15 uses automated tests at two levels:

\- algorithm-level tests for individual implementations;

\- application/scenario tests for integrated social-network behaviour.

The test design covers normal, boundary, invalid, adversarial and stress conditions.

The complete raw automated-test output is stored in:

&#x20;   docs/evidence/test\_results.txt

\## 2. Automated Test Summary

| Test Suite | Passed | Total |

|---|---:|---:|

| Sorting and Searching | 17 | 17 |

| Divide and Conquer | 27 | 27 |

| Greedy Algorithms | 52 | 52 |

| Dynamic Programming | 55 | 55 |

| Backtracking | 25 | 25 |

| Branch and Bound | 20 | 20 |

| Flow and Matching | 28 | 28 |

| Number / CRT | 17 | 17 |

| String Matching | 27 | 27 |

| \*\*Total\*\* | \*\*268\*\* | \*\*268\*\* |

Final result:

&#x20;   268 / 268 tests passed

\## 3. Representative Expected-vs-Actual Tests

| Area | Test | Expected | Actual | Result |

|---|---|---|---|---|

| Sorting | Sort unsorted integer input | Increasing order | Increasing order | PASS |

| Sorting | Already sorted input | Remains sorted | Remains sorted | PASS |

| Binary Search | Existing target | Correct index | Correct index | PASS |

| Binary Search | Missing target | Not found | Not found | PASS |

| Dijkstra | Valid non-negative graph | Correct shortest distances | Correct distances | PASS |

| Dijkstra | Negative edge | Reject input | Rejected | PASS |

| Dijkstra | Array vs heap | Same distances | Same distances | PASS |

| Kruskal | Known weighted graph | Correct MSF/MST weight | Correct weight | PASS |

| 0/1 Knapsack | Known instance | Known optimum | Optimum matched | PASS |

| Bellman-Ford | Reachable negative cycle | Detect cycle | Detected | PASS |

| Floyd-Warshall | Known graph | Correct all-pairs distances | Correct distances | PASS |

| LCS | Known strings | Known LCS | LCS matched | PASS |

| N-Queens | Valid board size | Valid placement/count behaviour | Matched | PASS |

| Subset Sum | Solvable target | Solution found | Found | PASS |

| Branch and Bound | Known knapsack | Exact optimum | Optimum matched | PASS |

| Max Flow | Known network | Known maximum flow | Flow matched | PASS |

| Min Cut | Flow network | Cut capacity = max flow | Equality verified | PASS |

| Bipartite Matching | Adversarial eligibility | Exact matching = 2 | 2 | PASS |

| Greedy Matching | Same adversarial case | May be suboptimal | Matching = 1 | PASS |

| CRT | Compatible congruences | Valid combined solution | Matched | PASS |

| CRT | Invalid/incompatible input | Reject/fail safely | Handled | PASS |

| Naive Search | Known pattern positions | Correct positions | Matched | PASS |

| Rabin-Karp | Same text/pattern | Same positions | Matched | PASS |

| Finite Automaton | Same text/pattern | Same positions | Matched | PASS |

| KMP | Same text/pattern | Same positions | Matched | PASS |

| String Search | Empty pattern | Reject | Rejected | PASS |

| Huffman | Generated codes | Prefix-free codes | Prefix-free verification PASS | PASS |

The table summarizes representative checks. The raw test evidence contains the complete automated results.

\## 4. Cross-Algorithm Correctness Checks

Several modules use independent implementations to reduce the chance of accepting an incorrect result.

\### Dijkstra

Array-based Dijkstra and binary-heap Dijkstra are run on the same graphs.

Expected:

&#x20;   identical shortest-path distances

Observed:

&#x20;   PASS

\### String Matching

Naive, Rabin-Karp, Finite Automaton and KMP are given the same text and pattern.

Expected:

&#x20;   identical match count and positions

Observed:

&#x20;   PASS

\### Maximum Flow / Minimum Cut

The integrated application checks:

&#x20;   maximum flow = minimum cut capacity

Observed on tested scenarios:

&#x20;   PASS

\### Exact vs Greedy Matching

The exact algorithm is intentionally compared with a greedy baseline.

Adversarial example:

&#x20;   Exact maximum matching = 2

&#x20;   Greedy matching        = 1

This test is useful because it verifies not only correctness but also demonstrates why the exact algorithm is necessary when assignment quality matters.

\## 5. Input Validation Tests

The project checks invalid conditions including:

\- malformed graph lines;

\- probabilities outside `(0, 1]`;

\- self relationships;

\- nonexistent users;

\- invalid source/target combinations;

\- empty patterns;

\- null text where tested;

\- negative graph edges for Dijkstra;

\- incompatible or invalid number-theory inputs;

\- invalid capacities and problem dimensions.

The goal is controlled failure rather than undefined behaviour.

\## 6. Scenario Tests

\### Normal Scenario

File:

&#x20;   data/normal.txt

Purpose:

Evaluate the application under ordinary social-network relationships.

Checks include:

\- influence path;

\- community structure;

\- dissemination capacity;

\- network ranking and planning behaviour.

\### Viral Shock Scenario

File:

&#x20;   data/shock.txt

Purpose:

Use the same population with stronger cross-community influence relationships.

This tests whether the algorithms respond meaningfully when network conditions change.

Observed behaviour includes:

\- different most-likely influence paths;

\- substantially increased dissemination capacity;

\- changed community structure.

\### Stress Scenario

File:

&#x20;   data/stress.txt

Generated deterministically by:

&#x20;   bench/generate\_stress.c

Size:

&#x20;   500 users

&#x20;   2000 directed relationships

The application successfully loads and analyses this larger graph.

Example integrated checks include:

&#x20;   User000 -> User499

&#x20;   Dijkstra edge relaxations: 2000

and:

&#x20;   Maximum dissemination capacity: 172

&#x20;   Minimum cut capacity: 172

&#x20;   Ford-Fulkerson augmentations: 131

The equality of maximum flow and minimum cut remains satisfied.

\## 7. Adversarial Testing

Adversarial tests are included to expose weaknesses that ordinary inputs may hide.

Examples include:

\- repetitive text for string matching;

\- greedy matching counterexample;

\- unreachable graph targets;

\- negative-edge rejection;

\- invalid/empty inputs;

\- boundary capacities;

\- cases with one distinct Huffman symbol;

\- graph structures designed to exercise alternative paths.

These cases are important because passing only normal examples does not establish reliability.

\## 8. Regression Testing

All algorithm tests can be rerun with:

&#x20;   bash tests/run\_all.sh

This allows a modification to one module to be checked against the existing test suite.

A successful regression run currently ends with:

&#x20;   RESULT: 27/27 tests passed

&#x20;   ALL ALGORITHM TEST SUITES COMPLETED

across the final string-matching suite, with all preceding suites also passing for a combined:

&#x20;   268/268

\## 9. Reproducibility

Build:

&#x20;   bash build.sh

Run all automated tests:

&#x20;   bash tests/run\_all.sh

Run comparative benchmark:

&#x20;   ./benchmark

Run scenarios:

&#x20;   ./p15 data/normal.txt

&#x20;   ./p15 data/shock.txt

&#x20;   ./p15 data/stress.txt

Raw automated-test evidence:

&#x20;   docs/evidence/test\_results.txt

\## 10. Testing Limitation

Passing the current tests does not prove absence of every possible defect.

The suite provides strong evidence through:

\- known expected outputs;

\- independent algorithm agreement;

\- invalid-input handling;

\- adversarial cases;

\- regression testing;

\- scenario testing;

\- stress testing.

Additional future testing could include fuzz testing, larger randomized graph oracles, memory-analysis tools and repeated performance trials on multiple machines.

