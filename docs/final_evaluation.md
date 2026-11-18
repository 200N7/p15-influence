\# P15 Final Evaluation and Reflection

\## 1. Project Summary

P15 is an offline Social Network Influence and Community Analysis Tool written in C11.

It combines reusable DAA implementations with an integrated social-media analytics application.

The product supports:

\- network display;

\- most-likely influence paths;

\- influential-user ranking;

\- community detection;

\- budget-constrained dissemination;

\- audience-capacity and bottleneck analysis;

\- campaign assignment;

\- comparative post/hashtag searching;

\- Huffman text compression.

\## 2. Final Verification

Automated algorithm tests:

&#x20;   268 / 268 passed

Evidence:

&#x20;   docs/evidence/test\_results.txt

Comparative benchmark:

&#x20;   docs/evidence/benchmark\_results.txt

Environment:

&#x20;   Windows 11

&#x20;   13th Gen Intel Core i5-13450HX

&#x20;   GCC 6.3.0

Environment evidence:

&#x20;   docs/evidence/environment.txt

Scenario evidence:

&#x20;   docs/evidence/scenario\_results.txt

\## 3. Normal vs Viral-Shock Evaluation

The normal and shock datasets use the same nine-user population but different relationship strengths.

For Alice -> Grace:

| Metric | Normal | Viral Shock |

|---|---:|---:|

| Most-likely influence probability | 0.0207 | 0.8370 |

| Dijkstra relaxations | 20 | 25 |

| Maximum dissemination capacity | 34 | 227 |

| Minimum cut capacity | 34 | 227 |

| Ford-Fulkerson augmentations | 3 | 8 |

Normal most-likely path:

&#x20;   Alice -> Bob -> Emma -> Frank -> Grace

Shock most-likely path:

&#x20;   Alice -> Emma -> Grace

Normal communities for k = 3:

&#x20;   Alice, Bob, Carol

&#x20;   David, Emma, Frank

&#x20;   Grace, Henry, Iris

Shock communities for k = 3:

&#x20;   Alice, Bob, Emma, Frank, Grace, Henry

&#x20;   Carol, David

&#x20;   Iris

Interpretation:

The shock does not merely increase one numerical score. It changes the preferred propagation route, increases total dissemination capacity and reorganizes the community structure.

This demonstrates that the product responds to structural changes in the network.

\## 4. Stress Evaluation

The deterministic stress scenario contains:

&#x20;   500 users

&#x20;   2000 directed relationships

For User000 -> User499:

&#x20;   Influence probability: 0.0503

&#x20;   Dijkstra relaxations: 2000

Capacity analysis:

&#x20;   Maximum flow: 172

&#x20;   Minimum cut: 172

&#x20;   Ford-Fulkerson augmentations: 131

The equality:

&#x20;   maximum flow = minimum cut

continues to hold on the larger scenario.

The dataset can be regenerated using:

&#x20;   bench/generate\_stress.c

\## 5. Comparative Findings

\### Merge Sort vs Quick Sort

Both algorithms correctly sort the generated benchmark inputs.

Merge Sort provides predictable O(n log n) behaviour.

Quick Sort is typically efficient but has an O(n^2) worst case.

The benchmark records actual comparison counts rather than relying only on theoretical complexity.

\### Array vs Heap Dijkstra

Both implementations are checked for agreement.

Array Dijkstra is appropriate for smaller/dense graphs.

Heap Dijkstra is the preferred product implementation for sparse social networks because of its better asymptotic behaviour.

\### Fractional vs 0/1 Knapsack

Fractional Knapsack permits partial items and supports a greedy optimum.

0/1 Knapsack requires whole-item decisions and uses dynamic programming for exact budget planning.

The algorithms therefore demonstrate how a small change in problem constraints changes the appropriate design paradigm.

\### String Matching

Naive, Rabin-Karp, Finite Automaton and KMP return identical match positions.

Their operation counts differ significantly.

This demonstrates that algorithms can be equally correct while having different computational behaviour.

\### Exact vs Greedy Matching

A deliberate adversarial case gives:

&#x20;   Exact maximum matching = 2

&#x20;   Greedy matching = 1

The greedy solution therefore achieves:

&#x20;   50% of optimum

This provides a concrete example where a locally convenient choice is not globally optimal.

\## 6. Improvements After the Initial Version

The project progressed from a basic graph-analysis skeleton into a complete algorithmic workbench.

Major improvements include:

\- binary-heap Dijkstra integration;

\- Kruskal community clustering;

\- exact budget planning;

\- max-flow/min-cut bottleneck analysis;

\- campaign matching with greedy comparison;

\- four-way string matching;

\- Huffman compression;

\- automated regression testing;

\- comparative benchmarking;

\- normal/shock scenario analysis;

\- deterministic stress testing;

\- reproducible build and test scripts;

\- expanded documentation and validation.

Git history provides timestamped evidence of this progression.

\## 7. Limitations

The project intentionally uses simplified analytical models.

\### Influence Probability

Propagation probabilities are treated as fixed edge values.

Real social-media influence can depend on:

\- time;

\- content;

\- user activity;

\- repeated exposure;

\- recommendation systems;

\- external events.

\### Independence Assumption

The most-likely path model multiplies edge probabilities and therefore assumes a simplified independent-propagation interpretation.

\### Direct Influence Ranking

The ranking primarily represents direct outgoing influence and is not intended to replace advanced centrality measures such as PageRank or learned influence models.

\### Community Model

Community detection converts directed relationships into an undirected strongest-tie distance.

This is explainable and suitable for the project, but it loses some direction information.

\### Capacity Model

The conversion:

&#x20;   capacity = round(probability \* 100)

is an analytical normalization.

It should not be interpreted as physical network bandwidth or a guaranteed real-world audience count.

\### 0/1 Knapsack

The pseudo-polynomial O(nW) DP is practical only when the integer budget remains moderate.

\### Exact Exponential Algorithms

Held-Karp, backtracking and branch-and-bound modules are intended for bounded instances.

They should not be applied without limits to very large inputs.

\### Benchmark Timing

Very small executions may appear as:

&#x20;   0.000 ms

because of the available platform clock resolution.

Operation counts are therefore used as the primary comparison metric.

\## 8. Possible Future Improvements

Possible extensions include:

\- time-varying influence probabilities;

\- larger real or anonymized datasets;

\- additional centrality measures;

\- stronger community-detection models;

\- repeated benchmark trials with high-resolution timing;

\- memory profiling;

\- randomized/fuzz testing;

\- CSV/JSON result export;

\- interactive network visualization;

\- richer desktop UI.

These are extensions rather than requirements for correctness of the current product.

\## 9. Ethical Considerations

Social-network analysis can reveal patterns about users and relationships.

A production system should therefore consider:

\- user privacy;

\- informed data use;

\- anonymization;

\- bias in influence metrics;

\- transparency of ranking decisions;

\- misuse of targeting or profiling.

The included project datasets are controlled demonstration data rather than private production-user records.

\## 10. Recommended Product Strategy

For the integrated product:

\- Heap Dijkstra is preferred for sparse influence-path analysis.

\- Kruskal clustering provides explainable relationship-based communities.

\- 0/1 Knapsack provides exact moderate-budget campaign selection.

\- Ford-Fulkerson and min-cut expose capacity and bottleneck structure.

\- Exact bipartite matching should be preferred when assignment quality matters.

\- KMP is a strong deterministic exact-search choice.

\- Huffman coding is appropriate when observed text frequencies justify compression.

The final selection of an algorithm should consider correctness, input characteristics, asymptotic complexity and measured behaviour together.

\## 11. Demonstration Script

A concise live demonstration can be performed as follows.

\### Step 1 - Build

Run:

&#x20;   bash build.sh

Explain that the application and benchmark are built offline using GCC with warning flags enabled.

\### Step 2 - Automated Reliability

Run:

&#x20;   bash tests/run\_all.sh

Show that all nine algorithm suites pass:

&#x20;   268 / 268

\### Step 3 - Normal Social Network

Run:

&#x20;   ./p15 data/normal.txt

Demonstrate:

&#x20;   Most likely influence path

&#x20;   Alice -> Grace

Then demonstrate:

&#x20;   Community detection

&#x20;   k = 3

Then:

&#x20;   Audience capacity

&#x20;   Alice -> Grace

Explain the Dijkstra, Kruskal and max-flow mappings.

\### Step 4 - Viral Shock

Run:

&#x20;   ./p15 data/shock.txt

Repeat Alice -> Grace.

Highlight the change from:

&#x20;   probability 0.0207 -> 0.8370

&#x20;   capacity 34 -> 227

Show that the community structure also changes.

\### Step 5 - Algorithm Counterexample

Use campaign assignment with the prepared eligibility case where:

&#x20;   exact matching = 2

&#x20;   greedy matching = 1

Explain why greedy selection is not always globally optimal.

\### Step 6 - String-Matching Comparison

Search a repetitive post/pattern.

Show that:

&#x20;   Naive

&#x20;   Rabin-Karp

&#x20;   Finite Automaton

&#x20;   KMP

all agree on match positions while operation counts differ.

\### Step 7 - Stress Scenario

Run:

&#x20;   ./p15 data/stress.txt

Explain:

&#x20;   500 users

&#x20;   2000 relationships

Use User000 -> User499 to demonstrate that the integrated graph analysis remains operational on the larger deterministic dataset.

\### Step 8 - Benchmark

Run:

&#x20;   ./benchmark

Explain that operation counts are the primary empirical comparison because small elapsed times can fall below clock resolution.

\## 12. Likely Live Modification Tasks

The evaluator may request a small unseen modification.

Examples that should be understood and practiced:

\- change a validation bound;

\- add a new graph edge and predict its effect;

\- trace Dijkstra relaxation;

\- explain why -log(p) is used;

\- change campaign budget;

\- add a new invalid-input test;

\- modify k in community detection;

\- explain a max-flow augmentation;

\- trace KMP prefix behaviour;

\- diagnose a malformed input;

\- explain why greedy matching fails on the counterexample.

The goal is not memorization of code. The student should understand how data moves through the implementation and why each algorithm is used.

\## 13. Final Conclusion

P15 demonstrates that different social-media analytics tasks require different algorithmic paradigms.

The project combines:

\- reusable implementations;

\- product-level integration;

\- correctness testing;

\- theoretical analysis;

\- empirical comparison;

\- adversarial evidence;

\- controlled scenario evaluation;

\- stress testing;

\- reproducible build and execution.

The final product remains intentionally explainable so that every major algorithmic decision can be traced and defended.

