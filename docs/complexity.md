\# Complexity Analysis and Algorithm Mapping

\## 1. Purpose

P15 is a Social Network Influence and Community Analysis Tool implemented in C11.

The project uses algorithms from different Design and Analysis of Algorithms paradigms both as reusable implementations and as parts of the final social-network application.

The main application maps algorithmic problems to social-media tasks such as influence propagation, community detection, campaign optimization, capacity analysis, assignment, text search and compression.

\---

\## 2. Application-to-Algorithm Mapping

| Product Requirement | Algorithm / Technique | Purpose |

|---|---|---|

| Most likely influence path | Binary-Heap Dijkstra | Find the route having maximum combined propagation probability |

| Influential-user ranking | Sorting / direct influence scoring | Rank users according to outgoing influence |

| Community detection | Kruskal k-clustering | Form communities from strongest social relationships |

| Budget-constrained dissemination | 0/1 Knapsack DP | Select users maximizing influence under campaign budget |

| Audience capacity | Ford-Fulkerson | Compute maximum dissemination capacity |

| Bottleneck detection | Minimum Cut | Identify relationships limiting total capacity |

| Campaign assignment | Bipartite Maximum Matching | Maximize valid user-to-campaign assignments |

| Assignment baseline | Greedy Matching | Compare heuristic quality against exact matching |

| Post / hashtag search | Naive, Rabin-Karp, Finite Automaton, KMP | Compare exact pattern-search strategies |

| Text compression | Huffman Coding | Compress repeated social-media text using prefix codes |

| Stress/scenario evaluation | Graph algorithms + generated datasets | Measure correctness and scalability under changing conditions |

\---

\## 3. Sorting and Searching

\### Iterative Binary Search

Time:

\- Best case: O(1)

\- Average case: O(log n)

\- Worst case: O(log n)

Auxiliary space:

\- O(1)

Binary search repeatedly halves the remaining search interval.

\### Recursive Binary Search

Time:

\- O(log n)

Recurrence:

&#x20;   T(n) = T(n/2) + O(1)

Using the Master Theorem:

&#x20;   a = 1

&#x20;   b = 2

&#x20;   f(n) = O(1)

Therefore:

&#x20;   T(n) = O(log n)

Auxiliary space:

&#x20;   O(log n)

because recursive calls remain on the call stack.

\### Merge Sort

Recurrence:

&#x20;   T(n) = 2T(n/2) + O(n)

Using the Master Theorem:

&#x20;   a = 2

&#x20;   b = 2

&#x20;   n^(log\_b a) = n

Therefore:

&#x20;   T(n) = O(n log n)

Time:

\- Best: O(n log n)

\- Average: O(n log n)

\- Worst: O(n log n)

Auxiliary space:

&#x20;   O(n)

for the temporary merge storage, plus logarithmic recursion depth.

\### Quick Sort

Average recurrence:

&#x20;   T(n) = 2T(n/2) + O(n)

giving:

&#x20;   O(n log n)

Worst-case recurrence:

&#x20;   T(n) = T(n - 1) + O(n)

Expanding:

&#x20;   T(n)

&#x20;   = n + (n-1) + (n-2) + ... + 1

&#x20;   = O(n^2)

Time:

\- Best: O(n log n)

\- Average: O(n log n)

\- Worst: O(n^2)

Auxiliary space:

\- Average recursion: O(log n)

\- Worst recursion: O(n)

\---

\## 4. Divide and Conquer

\### Maximum Subarray

The divide-and-conquer form divides the array into left and right halves and also evaluates a crossing solution.

Recurrence:

&#x20;   T(n) = 2T(n/2) + O(n)

Therefore:

&#x20;   T(n) = O(n log n)

Auxiliary space:

&#x20;   O(log n)

for recursion.

\### Fast Exponentiation

The exponent is approximately halved at every recursive/iterative stage.

Recurrence:

&#x20;   T(n) = T(n/2) + O(1)

Therefore:

&#x20;   T(n) = O(log n)

Auxiliary space depends on implementation style; recursive implementation requires O(log n) call-stack space, while an iterative form requires O(1).

\### Celebrity Problem

Candidate elimination removes impossible candidates, followed by verification.

Time:

&#x20;   O(n)

Auxiliary space:

&#x20;   O(1)

excluding the input relation representation.

\### Classical Matrix Multiplication

For two n x n matrices:

&#x20;   T(n) = O(n^3)

Auxiliary result storage:

&#x20;   O(n^2)

\### Strassen Matrix Multiplication

Strassen replaces eight recursive multiplications with seven.

Recurrence:

&#x20;   T(n) = 7T(n/2) + O(n^2)

Using the Master Theorem:

&#x20;   a = 7

&#x20;   b = 2

Therefore:

&#x20;   T(n) = O(n^(log2 7))

and:

&#x20;   log2(7) approximately 2.807

Hence:

&#x20;   T(n) = O(n^2.807)

This improves asymptotic multiplication count compared with classical O(n^3), although temporary matrices and recursion introduce additional overhead.

\---

\## 5. Greedy Algorithms

\### Activity Selection

After activities are ordered by finishing time, compatible activities are selected greedily.

If sorting is included:

&#x20;   O(n log n)

Selection scan:

&#x20;   O(n)

Auxiliary space depends on the sorting/temporary representation.

\### Interval Partitioning

Activities are processed according to scheduling order and assigned to compatible channels.

The exact implementation uses explicit comparisons between candidate intervals/channels, so empirical operation counts are used in addition to asymptotic analysis.

General goal:

&#x20;   minimize the number of simultaneous resources

The greedy invariant is that a new resource is introduced only when no existing resource can accept the activity.

\### Fractional Knapsack

Items are ordered by:

&#x20;   value / weight

Time including sorting:

&#x20;   O(n log n)

Selection after sorting:

&#x20;   O(n)

Unlike 0/1 Knapsack, fractions are permitted. Therefore the greedy choice is optimal for this problem variant.

\### Huffman Coding

For k distinct symbols, repeated extraction/combination of the lowest-frequency nodes constructs the prefix tree.

With a priority queue:

&#x20;   O(k log k)

Code generation requires traversal of the resulting tree.

Auxiliary space:

&#x20;   O(k)

The generated codes are also explicitly checked for the prefix-free property.

\---

\## 6. Dijkstra Shortest Path

Two implementations are included for comparison.

\### Array-Based Dijkstra

Vertex selection by scanning an array costs O(V) for each of V vertices.

Time:

&#x20;   O(V^2 + E)

which is normally written as:

&#x20;   O(V^2)

for the dominant term.

Auxiliary space:

&#x20;   O(V + E)

for distances, predecessor information and graph representation.

\### Binary-Heap Dijkstra

Using a binary heap:

&#x20;   O((V + E) log V)

For a connected sparse graph this is commonly expressed as:

&#x20;   O(E log V)

Auxiliary space:

&#x20;   O(V + E)

\### Social-Network Transformation

The application wants to maximize:

&#x20;   p1 \* p2 \* ... \* pk

for probabilities on a path.

Dijkstra minimizes additive path cost, so each probability p is transformed into:

&#x20;   w = -log(p)

Then:

&#x20;   -log(p1 \* p2 \* ... \* pk)

&#x20;   =

&#x20;   -log(p1) - log(p2) - ... - log(pk)

Because:

&#x20;   0 < p <= 1

every transformed weight is non-negative.

Therefore Dijkstra can correctly minimize transformed cost, which is equivalent to maximizing the original probability product.

\---

\## 7. Minimum Spanning Trees and Community Detection

\### Prim Minimum Spanning Forest

The project contains a Prim implementation for minimum spanning forest construction.

For an array-based vertex-selection implementation, the principal bound is:

&#x20;   O(V^2 + E)

Auxiliary space:

&#x20;   O(V + E)

\### Kruskal Minimum Spanning Forest

Edges are sorted first:

&#x20;   O(E log E)

Disjoint-set operations are nearly constant amortized time.

Therefore:

&#x20;   O(E log E)

Auxiliary space:

&#x20;   O(V + E)

\### Kruskal k-Clustering

The social network converts a relationship strength p into:

&#x20;   distance = 1 - strongest\_probability

Strong relationships therefore receive smaller distances.

Kruskal processes relationships from smallest distance upward and stops when exactly k connected components remain.

Dominant complexity:

&#x20;   O(E log E)

The resulting disjoint-set components become the detected communities.

\---

\## 8. Dynamic Programming

\### 0/1 Knapsack

Let:

&#x20;   n = number of items

&#x20;   W = integer budget/capacity

Time:

&#x20;   O(nW)

Auxiliary space depends on the DP representation. A full table requires:

&#x20;   O(nW)

while a one-dimensional optimized table can use:

&#x20;   O(W)

The project uses this algorithm for exact budget-constrained dissemination planning.

\### Matrix Chain Multiplication

For n matrices:

&#x20;   O(n^3)

Auxiliary space:

&#x20;   O(n^2)

The DP considers every possible split point for every matrix interval.

\### Longest Common Subsequence

For sequences of lengths m and n:

&#x20;   O(mn)

Auxiliary space for the standard reconstruction table:

&#x20;   O(mn)

\### Optimal Binary Search Tree

The standard interval DP evaluates possible roots for each interval.

Time:

&#x20;   O(n^3)

Auxiliary space:

&#x20;   O(n^2)

\### Bellman-Ford

Each edge can be relaxed for up to V-1 rounds.

Time:

&#x20;   O(VE)

Auxiliary space:

&#x20;   O(V)

excluding graph storage.

Unlike Dijkstra, Bellman-Ford supports negative edge weights and can detect reachable negative cycles.

\### Floyd-Warshall

Three nested vertex loops produce:

&#x20;   O(V^3)

Auxiliary space:

&#x20;   O(V^2)

It computes all-pairs shortest paths.

\### Held-Karp Travelling Salesperson DP

State is represented by:

&#x20;   subset + final vertex

Number of subset states grows exponentially.

Time:

&#x20;   O(n^2 \* 2^n)

Auxiliary space:

&#x20;   O(n \* 2^n)

Therefore this implementation is intentionally used only for bounded/small TSP instances.

\---

\## 9. Backtracking

\### N-Queens

The algorithm explores valid queen placements row by row.

Worst-case search is exponential, commonly bounded by:

&#x20;   O(n!)

with pruning reducing the practical search space.

Auxiliary space:

&#x20;   O(n)

for placement state and recursion, excluding any explicit output collection.

\### Subset Sum Backtracking

Each item can conceptually be included or excluded.

Worst-case recurrence:

&#x20;   T(n) = 2T(n - 1) + O(1)

Therefore:

&#x20;   T(n) = O(2^n)

Auxiliary recursion depth:

&#x20;   O(n)

\### Backtracking Knapsack

The same include/exclude search structure gives exponential worst-case behaviour:

&#x20;   O(2^n)

Pruning may reduce practical work but does not change the exponential worst-case class.

\---

\## 10. Branch and Bound

The project compares three exact 0/1 Knapsack search orders:

\- FIFO Branch and Bound

\- LIFO Branch and Bound

\- Best-Bound Branch and Bound

All may still require exponential work in the worst case:

&#x20;   O(2^n)

Branch-and-bound improves practical performance by calculating an optimistic upper bound and discarding states that cannot improve the current best solution.

The three variants differ primarily in which live state is explored next.

This makes them useful for comparing:

\- node expansion order;

\- pruning effectiveness;

\- number of explored states;

\- exact solution quality.

All three must return the same optimal value for the same valid instance.

\---

\## 11. Maximum Flow and Minimum Cut

\### Ford-Fulkerson

For integer capacities, the classic Ford-Fulkerson bound is:

&#x20;   O(E \* F)

where:

&#x20;   E = number of edges

&#x20;   F = value of the maximum flow

The social-network model converts probability into integer analytical capacity:

&#x20;   capacity = round(probability \* 100)

The residual network is used to identify the source side of the minimum cut.

The application verifies:

&#x20;   maximum flow = minimum cut capacity

which provides an important correctness cross-check.

\---

\## 12. Bipartite Matching

\### Exact Maximum Matching

Users and campaigns form a bipartite graph.

An augmenting-path implementation has a typical bound of:

&#x20;   O(VE)

for repeated augmenting searches in this implementation style.

The result is exact.

\### Greedy Matching

A greedy baseline accepts locally available assignments without global reconsideration.

Its practical cost is lower, but it is not guaranteed to find the maximum matching.

The project includes an adversarial example where:

&#x20;   Exact matching size = 2

&#x20;   Greedy matching size = 1

Therefore:

&#x20;   Greedy quality = 50% of optimum

This demonstrates that faster/local decisions can sacrifice solution quality.

\---

\## 13. Number-Theory Algorithms

\### Euclidean GCD

Recurrence:

&#x20;   T(a, b) = T(b, a mod b) + O(1)

The number of iterations is logarithmic in the smaller operand.

Time:

&#x20;   O(log min(a, b))

Auxiliary space:

\- O(1) iterative

\- O(log min(a,b)) if implemented recursively

\### Extended Euclidean Algorithm

Time:

&#x20;   O(log min(a, b))

It additionally determines coefficients satisfying Bezout's identity.

\### Modular Inverse

The modular inverse uses the Extended Euclidean Algorithm.

Time:

&#x20;   O(log m)

for modulus m, ignoring machine-integer arithmetic cost.

\### Chinese Remainder Theorem

For k congruences, the implementation combines modular arithmetic with GCD/modular-inverse operations.

Its arithmetic operation count is polynomial in the number of congruences and logarithmic arithmetic subroutines, while actual bit complexity depends on operand sizes.

The implementation also validates compatibility/overflow conditions.

\---

\## 14. String Matching

Let:

&#x20;   n = text length

&#x20;   m = pattern length

\### Naive Matching

Worst case:

&#x20;   O((n - m + 1)m)

or simply:

&#x20;   O(nm)

Auxiliary space:

&#x20;   O(1)

excluding output positions.

\### Rabin-Karp

Rolling hashing allows expected linear scanning.

Expected:

&#x20;   O(n + m)

Worst case with many hash collisions:

&#x20;   O(nm)

The implementation verifies every hash hit before reporting a match, preserving exact correctness.

\### Finite-Automaton Matching

After transition preprocessing, the text scan is:

&#x20;   O(n)

The preprocessing cost depends on alphabet size and transition-table construction.

For alphabet size sigma, a standard representation requires approximately:

&#x20;   O(m \* sigma)

table space.

\### Knuth-Morris-Pratt

Prefix/LPS preprocessing:

&#x20;   O(m)

Text scan:

&#x20;   O(n)

Total:

&#x20;   O(n + m)

Auxiliary space:

&#x20;   O(m)

for the prefix table.

\---

\## 15. Comparative Analysis

\### Merge Sort vs Quick Sort

Merge Sort guarantees:

&#x20;   O(n log n)

but requires additional merge storage.

Quick Sort usually performs well in-place and has average:

&#x20;   O(n log n)

but can degrade to:

&#x20;   O(n^2)

for poor partition behaviour.

The benchmark compares actual comparison counts on identical generated arrays.

\### Array Dijkstra vs Heap Dijkstra

Array Dijkstra:

&#x20;   O(V^2)

Heap Dijkstra:

&#x20;   O((V + E) log V)

The heap version is preferable for large sparse social networks.

Both implementations are cross-checked for identical shortest-path distances.

\### Fractional vs 0/1 Knapsack

Fractional Knapsack:

&#x20;   greedy

&#x20;   O(n log n)

0/1 Knapsack:

&#x20;   dynamic programming

&#x20;   O(nW)

They solve different variants, so their objective values are not expected to match.

The comparison demonstrates how allowing fractions changes both the optimal strategy and computational cost.

\### String-Matching Comparison

Naive:

&#x20;   simple but potentially O(nm)

Rabin-Karp:

&#x20;   efficient rolling hash with collision verification

Finite Automaton:

&#x20;   preprocessing cost in exchange for a linear scan

KMP:

&#x20;   deterministic O(n + m) using prefix information

All four implementations must agree on the number and positions of matches.

\---

\## 16. Empirical Evidence

The benchmark runner records operation counts and elapsed time.

Operation counts are treated as the primary empirical metric because very short executions can measure as 0.000 ms on the available Windows clock.

Observed trends include:

\- sorting comparison counts increase with input size;

\- Dijkstra implementations agree on computed distances;

\- 0/1 Knapsack DP operations increase with both item count and capacity;

\- string-matching algorithms show substantially different operation counts on repetitive inputs;

\- all compared exact string-matching methods return identical match positions.

The project also contains three application-level evaluation conditions:

\### Normal

&#x20;   data/normal.txt

Stable social structure with relatively weak cross-community influence.

\### Viral Shock

&#x20;   data/shock.txt

The same population receives stronger cross-community relationships.

This changes influence paths, flow capacity and community structure.

\### Stress

&#x20;   data/stress.txt

Contains:

&#x20;   500 users

&#x20;   2000 directed relationships

The stress dataset exercises the integrated graph algorithms on a substantially larger input.

\---

\## 17. Complexity Classes

\### Polynomial-Time Components

Examples in P include:

\- binary search;

\- merge sort;

\- shortest paths with Dijkstra for non-negative weights;

\- Bellman-Ford;

\- Floyd-Warshall;

\- minimum spanning tree algorithms;

\- maximum flow;

\- bipartite matching;

\- exact string matching;

\- Huffman coding.

These algorithms have polynomial-time solutions for the problem forms implemented in this project.

\### NP Problems

A decision problem is in NP when a proposed solution can be verified in polynomial time.

Examples related to the project include:

\- Subset Sum decision;

\- Travelling Salesperson decision;

\- 0/1 Knapsack decision.

\### NP-Complete Problems

Subset Sum decision is NP-complete.

The decision form asks:

&#x20;   Is there a subset whose sum equals the target?

A proposed subset can be checked in polynomial time, but no polynomial-time algorithm is known for all instances.

The decision form of 0/1 Knapsack is also NP-complete.

\### NP-Hard Problems

Optimization versions can be NP-hard even when their decision counterparts are NP-complete.

Examples:

\- optimization Travelling Salesperson Problem;

\- optimization 0/1 Knapsack.

Held-Karp and exact search methods are therefore restricted to bounded input sizes.

\---

\## 18. Reduction Mapping

A useful relationship in this project is:

&#x20;   Subset Sum <=p 0/1 Knapsack Decision

Consider a Subset Sum instance containing numbers:

&#x20;   a1, a2, ..., an

with target:

&#x20;   T

Create a 0/1 Knapsack decision instance where each item i has:

&#x20;   weight\_i = ai

&#x20;   value\_i  = ai

and set:

&#x20;   capacity = T

&#x20;   required value = T

If a subset sums exactly to T, selecting those items has total weight T and total value T.

Conversely, any selection satisfying weight at most T and value at least T must, because weight and value are identical, have total exactly T.

Therefore a solution to this constructed Knapsack decision instance corresponds to a solution of the original Subset Sum instance.

The transformation requires only linear work in the number of input items, so it is a polynomial-time reduction.

This connects the backtracking Subset Sum module with the exact Knapsack modules and explains why exact general solutions may require exponential work.

\---

\## 19. Why Multiple Paradigms Are Used

No single algorithmic paradigm is best for every social-network problem.

Greedy methods are useful when a locally optimal choice has the required optimal-substructure property, such as fractional knapsack or Huffman coding.

Dynamic programming is useful when overlapping subproblems can be reused, such as 0/1 knapsack, LCS and Held-Karp.

Divide and conquer reduces a problem into smaller independent instances, as in merge sort and Strassen multiplication.

Backtracking explores a decision space while rejecting invalid partial solutions.

Branch and bound additionally uses objective bounds to discard states that cannot improve the best known solution.

Graph algorithms model influence paths, communities, capacities and assignments directly.

The final product therefore demonstrates algorithm selection based on problem structure rather than applying one technique everywhere.

\---

\## 20. Production Strategy

For the final social-network product:

\- use binary-heap Dijkstra for sparse influence-path analysis;

\- use Kruskal clustering for explainable relationship-based communities;

\- use 0/1 Knapsack DP when campaign budget is moderate and exact selection is required;

\- use Ford-Fulkerson/min-cut for the normalized capacity model;

\- use exact bipartite matching when assignment quality matters;

\- use KMP when deterministic linear exact pattern matching is preferred;

\- use Huffman coding when compression is justified by repeated symbol frequencies.

Exponential algorithms such as Held-Karp, backtracking and branch-and-bound should remain bounded educational/analytical modules rather than unrestricted large-scale production operations.

The benchmark and scenario results are used together with asymptotic complexity when selecting an algorithm; neither theoretical complexity nor a single timing measurement is treated as sufficient on its own.

