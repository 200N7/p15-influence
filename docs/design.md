\# P15 Design, Workflow and Correctness

\## 1. Product Architecture

P15 is an offline C11 social-network analytics application.

Main structure:

&#x20;   Input graph file

&#x20;         |

&#x20;         v

&#x20;     graph\_load()

&#x20;         |

&#x20;         v

&#x20;   Graph representation

&#x20;   - user names

&#x20;   - adjacency lists

&#x20;   - influence probabilities

&#x20;         |

&#x20;         v

&#x20;      main.c

&#x20;   menu / validation

&#x20;         |

&#x20;         v

&#x20;    influence.c

&#x20;   product-level analysis

&#x20;         |

&#x20;         v

&#x20;   reusable algorithms

&#x20;   src/algorithms/

&#x20;         |

&#x20;         v

&#x20;   formatted result

The reusable algorithm library is separated from the social-network mapping. This allows the algorithms to be tested independently from the final product.

\## 2. Main Data Structure

The social network is stored as an adjacency-list graph.

Each edge contains:

&#x20;   destination user

&#x20;   propagation probability

&#x20;   next edge pointer

The graph contains:

&#x20;   number of users

&#x20;   user names

&#x20;   adjacency-list heads

This representation is appropriate because social networks are generally sparse compared with a complete graph.

\## 3. Product Workflow

1\. Start the application with a graph dataset.

2\. Validate and load users and directed influence relationships.

3\. Display the analysis menu.

4\. User selects an analysis.

5\. Required parameters are validated.

6\. The product maps the social-media problem to an algorithm.

7\. The algorithm runs.

8\. The result and useful operation counts are displayed.

9\. The user can perform another analysis or exit.

\## 4. Integrated Product Functions

The main integration functions are:

&#x20;   most\_likely\_path()

&#x20;   rank\_influencers()

&#x20;   detect\_communities()

&#x20;   plan\_dissemination()

&#x20;   analyze\_audience\_capacity()

&#x20;   plan\_campaign\_assignments()

&#x20;   analyze\_post\_pattern()

&#x20;   analyze\_text\_compression()

\## 5. Most-Likely Influence Path

\### Problem

Given source user s and target user t, find the path maximizing the product of edge propagation probabilities.

\### Transformation

For each probability p:

&#x20;   weight = -log(p)

Since:

&#x20;   0 < p <= 1

all transformed weights are non-negative.

Also:

&#x20;   -log(p1 \* p2 \* ... \* pk)

&#x20;   =

&#x20;   -log(p1) + -log(p2) + ... + -log(pk)

Therefore maximizing the probability product is equivalent to minimizing the transformed additive cost.

\### Pseudocode

&#x20;   MOST\_LIKELY\_PATH(graph, source, target)

&#x20;       locate source and target

&#x20;       if either does not exist

&#x20;           report error

&#x20;       for every directed graph edge (u, v, p)

&#x20;           create weighted edge

&#x20;           weight = -log(p)

&#x20;       run HEAP\_DIJKSTRA from source

&#x20;       if target is unreachable

&#x20;           report no influence path

&#x20;       reconstruct source-to-target path

&#x20;       probability = exp(-distance\[target])

&#x20;       output path

&#x20;       output probability

&#x20;       output relaxation count

\### Correctness

Dijkstra is correct when all edge weights are non-negative.

Because every valid probability satisfies:

&#x20;   0 < p <= 1

we have:

&#x20;   -log(p) >= 0

Therefore the transformed graph satisfies Dijkstra's requirement.

Minimizing the sum of transformed weights minimizes the negative logarithm of the probability product, so the reconstructed shortest path is exactly the maximum-probability influence path.

\## 6. Influential-User Ranking

For each user:

&#x20;   score = sum of outgoing influence probabilities

Users are ranked by this direct-influence score.

This provides an explainable ranking: a user scores highly when their outgoing relationships have high combined direct propagation strength.

\## 7. Community Detection

\### Model

The follower network is directed, but clustering requires a relationship strength between pairs.

For each pair, use the strongest observed probability and define:

&#x20;   distance = 1 - strongest\_probability

Strong relationships therefore produce smaller distances.

\### Pseudocode

&#x20;   DETECT\_COMMUNITIES(graph, k)

&#x20;       validate k

&#x20;       convert directed relationships

&#x20;       into undirected pair strengths

&#x20;       for each pair

&#x20;           distance = 1 - strongest\_probability

&#x20;       sort relationships by increasing distance

&#x20;       initialize each user as a separate component

&#x20;       process relationships in Kruskal order

&#x20;           if endpoints are in different components

&#x20;               merge them

&#x20;           stop when exactly k components remain

&#x20;       assign component labels

&#x20;       print users grouped by label

\### Correctness

Kruskal processes edges from smallest distance to largest.

Since smaller distance represents stronger social connection, the strongest available links merge components first.

Stopping when k components remain produces the k clusters generated by single-linkage/Kruskal clustering under the chosen distance model.

\## 8. Budget-Constrained Dissemination

\### Model

Each candidate user has:

&#x20;   value = influence benefit

&#x20;   cost  = promotion cost

A user is either selected completely or not selected.

Therefore this is a 0/1 Knapsack problem.

\### Pseudocode

&#x20;   PLAN\_DISSEMINATION(users, budget)

&#x20;       compute candidate influence values

&#x20;       for each candidate

&#x20;           read/obtain integer promotion cost

&#x20;       create DP table

&#x20;       for each candidate i

&#x20;           for each budget b

&#x20;               option1 = do not select i

&#x20;               if cost\[i] <= b

&#x20;                   option2 =

&#x20;                       value\[i] +

&#x20;                       previous optimum at

&#x20;                       b - cost\[i]

&#x20;               store better option

&#x20;       reconstruct selected users

&#x20;       output selected users

&#x20;       output total value

&#x20;       output budget usage

\### Correctness

For every state, the optimal solution either excludes the current user or includes that user if affordable.

The recurrence considers both possibilities and stores the better one.

By optimal substructure, the final state contains the maximum total value achievable within the budget.

\## 9. Audience Capacity and Bottlenecks

\### Model

Probability is converted to normalized integer capacity:

&#x20;   capacity = round(probability \* 100)

\### Pseudocode

&#x20;   ANALYZE\_CAPACITY(graph, source, target)

&#x20;       validate source and target

&#x20;       build capacity network

&#x20;       initialize flow to zero

&#x20;       while augmenting path exists

&#x20;           find bottleneck capacity

&#x20;           augment flow

&#x20;           update residual network

&#x20;       maximum\_flow = total augmented flow

&#x20;       find vertices reachable from source

&#x20;       in final residual graph

&#x20;       minimum cut =

&#x20;           original edges crossing

&#x20;           reachable -> unreachable

&#x20;       output maximum flow

&#x20;       output cut edges

&#x20;       output cut capacity

&#x20;       verify:

&#x20;           maximum\_flow == minimum\_cut\_capacity

\### Correctness

Ford-Fulkerson repeatedly augments valid source-to-target flow until no augmenting path remains.

The final residual graph determines a cut separating vertices reachable from the source from the remaining vertices.

By the Max-Flow Min-Cut Theorem:

&#x20;   maximum flow = minimum cut capacity

The application explicitly verifies this equality.

\## 10. Campaign Assignment

\### Model

Users and campaigns form the two sides of a bipartite graph.

An edge means:

&#x20;   user is eligible for campaign

Each user and campaign can participate in at most one selected assignment.

\### Pseudocode

&#x20;   CAMPAIGN\_ASSIGNMENT(users, campaigns)

&#x20;       read eligibility matrix

&#x20;       exact\_result =

&#x20;           maximum bipartite matching

&#x20;       greedy\_result =

&#x20;           greedy matching on same matrix

&#x20;       output exact assignments

&#x20;       output exact matching size

&#x20;       output greedy matching size

&#x20;       quality =

&#x20;           greedy\_size / exact\_size \* 100

&#x20;       output greedy solution quality

\### Correctness

The exact implementation repeatedly searches for augmenting assignments.

Whenever an augmenting path is found, matching size increases by one.

When no augmenting path remains, the matching is maximum.

The greedy version is intentionally retained as a comparison baseline and is not assumed to be optimal.

\## 11. Post / Hashtag Search

The same text and pattern are processed by:

&#x20;   Naive

&#x20;   Rabin-Karp

&#x20;   Finite Automaton

&#x20;   KMP

\### Pseudocode

&#x20;   ANALYZE\_PATTERN(text, pattern)

&#x20;       validate non-empty inputs

&#x20;       result1 = NAIVE(text, pattern)

&#x20;       result2 = RABIN\_KARP(text, pattern)

&#x20;       result3 = FINITE\_AUTOMATON(text, pattern)

&#x20;       result4 = KMP(text, pattern)

&#x20;       compare:

&#x20;           match count

&#x20;           every match position

&#x20;       if all agree

&#x20;           report cross-check PASS

&#x20;       else

&#x20;           report disagreement

&#x20;       output operation counts

\### Correctness

Each implementation independently computes exact pattern occurrences.

The product requires all four algorithms to agree on both the number and positions of matches.

Rabin-Karp additionally verifies every hash hit before accepting it, preventing hash collisions from becoming false matches.

\## 12. Huffman Text Compression

\### Pseudocode

&#x20;   ANALYZE\_COMPRESSION(text)

&#x20;       validate text

&#x20;       count frequency of each character

&#x20;       build Huffman tree

&#x20;       from character frequencies

&#x20;       traverse tree to generate codes

&#x20;       encoded\_bits = 0

&#x20;       for each character

&#x20;           encoded\_bits +=

&#x20;               frequency \*

&#x20;               code\_length

&#x20;       original\_bits =

&#x20;           character\_count \* 8

&#x20;       verify prefix-free property

&#x20;       output:

&#x20;           frequencies

&#x20;           codes

&#x20;           original size

&#x20;           encoded size

&#x20;           reduction percentage

&#x20;           verification

\### Correctness

At each step Huffman combines the two least-frequent available nodes.

This greedy-choice property produces an optimal prefix code for the supplied symbol frequencies.

The generated code set is also checked explicitly to ensure that no code is a prefix of another.

\## 13. Validation

Graph input rejects:

&#x20;   probability <= 0

&#x20;   probability > 1

&#x20;   self relationships

&#x20;   malformed lines

&#x20;   non-numeric probabilities

&#x20;   nonexistent files

Individual analyses additionally validate their own parameters, such as source/target users, budgets, campaign counts and text inputs.

\## 14. Independent Algorithm Layer

The project keeps general algorithms under:

&#x20;   src/algorithms/

&#x20;   include/algorithms/

The application-specific mapping is kept primarily in:

&#x20;   src/influence.c

This separation improves:

\- testing;

\- reuse;

\- readability;

\- debugging;

\- comparison between algorithms.

\## 15. Build and Test Interfaces

Build:

&#x20;   bash build.sh

Automated regression:

&#x20;   bash tests/run\_all.sh

Benchmark:

&#x20;   ./benchmark

Application:

&#x20;   ./p15 <dataset>

Example:

&#x20;   ./p15 data/normal.txt

\## 16. Correctness Strategy

Correctness is supported using several independent forms of evidence:

1\. Known-output unit tests.

2\. Boundary and invalid-input tests.

3\. Cross-algorithm agreement.

4\. Reconstruction checks.

5\. Max-flow/min-cut equality.

6\. Exact-vs-greedy counterexamples.

7\. Normal/shock scenario comparison.

8\. Deterministic stress testing.

9\. Full automated regression suite.

Current automated result:

&#x20;   268 / 268 tests passed

\## 17. Developer Notes

The graph layer should remain responsible only for graph storage/loading.

Reusable algorithms should remain domain-independent wherever practical.

Social-network interpretation belongs in the integration layer.

New features should:

1\. validate input;

2\. call an existing reusable algorithm or add a separately testable one;

3\. expose measurable output;

4\. add regression tests before final integration.

This structure prevents the menu/UI layer from becoming tightly coupled to individual algorithm implementations.

