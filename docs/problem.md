\# Problem Statement



Social-media networks contain users connected by directed relationships with different probabilities of information propagation. A social-media analyst needs to understand how information spreads, which users are influential, how communities are structured, and how limited campaign resources should be allocated.



P15 models the network as a directed weighted graph:



\- each user is a vertex;

\- each follow/influence relationship is a directed edge;

\- each edge stores a propagation probability between 0 and 1.



The system supports several analysis tasks:



\- find the most likely influence path between two users;

\- rank influential users;

\- detect structural communities;

\- select high-value users under a campaign budget;

\- measure dissemination capacity and identify bottlenecks;

\- assign eligible users to campaigns;

\- search posts or hashtags using multiple string-matching algorithms;

\- analyse text compression using Huffman coding.



The project also contains independent implementations of sorting, searching, divide-and-conquer, greedy, dynamic-programming, backtracking, branch-and-bound, graph, flow, matching, number-theory and string-matching algorithms.



Correctness is evaluated using automated tests, known results, cross-algorithm comparisons and adversarial cases. Performance behaviour is studied using operation counts, comparative benchmarks and normal, viral-shock and stress scenarios.

