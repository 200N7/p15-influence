# P15 - Social Network Influence Analysis

A C11 Design and Analysis of Algorithms project that applies multiple algorithmic techniques to social-network influence analysis.

The system models users as vertices and directed influence relationships as edges with propagation probabilities.

## Main Features

The application provides:

1. Display network
2. Most likely influence path
3. Rank influential users
4. Detect communities
5. Plan high-value dissemination
6. Audience capacity and bottleneck analysis
7. Campaign assignment
8. Post / hashtag search
9. Huffman text compression

## Algorithms Used in the Application

### Most Likely Influence Path
Uses binary-heap Dijkstra.

Each influence probability `p` is transformed into:

    cost = -log(p)

Minimizing the transformed path cost therefore maximizes the product of influence probabilities.

### Community Detection
Uses Kruskal-based single-linkage clustering.

For an undirected relationship:

    distance = 1 - strongest influence probability

Stronger relationships are therefore joined earlier.

### Budget-Constrained Dissemination
Uses exact 0/1 Knapsack dynamic programming to select influential users while remaining within a campaign budget.

### Audience Capacity and Bottlenecks
Uses Ford-Fulkerson maximum flow and minimum cut.

For this model:

    capacity = round(probability * 100)

The minimum cut identifies bottleneck relationships, and the program verifies that maximum flow equals minimum cut.

### Campaign Assignment
Uses exact bipartite maximum matching.

The result is also compared with a greedy matching baseline to demonstrate cases where greedy selection is suboptimal.

### Post / Hashtag Search
Compares four exact string-matching algorithms:

- Naive
- Rabin-Karp
- Finite Automaton
- KMP

All four results are cross-checked for agreement.

### Huffman Compression
Builds Huffman codes from character frequencies and reports:

- original size
- encoded size
- compression percentage
- generated codes
- prefix-free verification

## Algorithm Library

The repository also contains independently tested implementations of:

- Merge Sort and Quick Sort
- Iterative and Recursive Binary Search
- Maximum Subarray
- Fast Exponentiation
- Celebrity Problem
- Classical and Strassen Matrix Multiplication
- Activity Selection
- Interval Partitioning
- Fractional Knapsack
- Prim and Kruskal
- Matrix Chain Multiplication
- Longest Common Subsequence
- Optimal BST
- Bellman-Ford
- Floyd-Warshall
- Held-Karp TSP
- N-Queens
- Subset Sum
- Backtracking Knapsack
- FIFO, LIFO and Best-Bound Branch and Bound
- Chinese Remainder Theorem
- GCD, Extended GCD and Modular Inverse

## Testing

Nine automated algorithm test suites are included.

Current result:

    Sort/Search             17/17
    Divide and Conquer      27/27
    Greedy                  52/52
    Dynamic Programming     55/55
    Backtracking            25/25
    Branch and Bound        20/20
    Flow and Matching       28/28
    Number / CRT            17/17
    String Matching         27/27
    --------------------------------
    TOTAL                  268/268

Tests cover normal inputs, boundary cases, invalid inputs, known oracle results, reconstruction, adversarial cases and cross-algorithm agreement.

Run all tests:

    bash tests/run_all.sh

## Benchmarking

`bench/benchmark.c` provides comparative experiments for:

- Merge Sort vs Quick Sort
- Array Dijkstra vs Heap Dijkstra
- Fractional Knapsack vs 0/1 Knapsack
- Naive vs Rabin-Karp vs Finite Automaton vs KMP

Operation counts are used as the primary comparison metric. Timing is also reported, although very small executions may show 0.000 ms because of platform clock resolution.

Run:

    ./benchmark

## Evaluation Scenarios

### Normal Scenario
`data/normal.txt`

A nine-user network with three relatively stable communities and weak cross-community influence.

For Alice -> Grace:

    Most likely influence probability: 0.0207
    Maximum dissemination capacity: 34

### Viral Shock Scenario
`data/shock.txt`

The same users are subjected to strong cross-community relationships representing a viral event.

For Alice -> Grace:

    Most likely influence probability: 0.8370
    Maximum dissemination capacity: 227

The community structure also changes significantly.

### Stress Scenario
`data/stress.txt`

A deterministic stress network containing:

    500 users
    2000 directed relationships

Example User000 -> User499 results:

    Dijkstra edge relaxations: 2000
    Maximum flow: 172
    Minimum cut: 172
    Ford-Fulkerson augmentations: 131

The dataset can be reproduced using `bench/generate_stress.c`.

## Input Format

Each relationship is stored as:

    source_user target_user probability

Example:

    Alice Bob 0.82

Rules:

- usernames contain no spaces
- `0 < probability <= 1`
- self-loops are rejected
- malformed lines are rejected
- lines beginning with `#` are comments

## Build and Run

Build the application and benchmark:

    bash build.sh

Run the application:

    ./p15 data/normal.txt

Other available datasets:

    ./p15 data/tiny.txt
    ./p15 data/communities.txt
    ./p15 data/shock.txt
    ./p15 data/stress.txt

Run the benchmark:

    ./benchmark

Run all tests:

    bash tests/run_all.sh

## Project Structure

    src/                 Main application
    src/algorithms/      Algorithm implementations
    include/             Header files
    include/algorithms/  Algorithm APIs
    tests/               Automated tests
    bench/               Benchmark and stress generator
    data/                Evaluation datasets
    docs/                Problem and input specifications
    build.sh             Reproducible build script

## Key Assumptions

- Influence relationships are directed.
- Edge probabilities represent likelihood of information propagation.
- Community detection ignores direction and uses the strongest relationship between a pair.
- Campaign costs are integer values.
- Flow capacity is a normalized analytical value derived from probability.
- Campaign eligibility is entered separately because follower relationships do not imply campaign eligibility.

## Reproducibility

A complete verification workflow is:

    bash build.sh
    bash tests/run_all.sh
    ./benchmark
    ./p15 data/normal.txt
    ./p15 data/shock.txt
    ./p15 data/stress.txt