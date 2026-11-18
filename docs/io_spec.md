\# Input Format



The application reads a social-network graph from a text file.



Each non-comment line has three fields:



&#x20;   source\_user target\_user probability



Example:



&#x20;   Alice Bob 0.82

&#x20;   Bob Carol 0.76



This means there is a directed influence relationship from the first user to the second user.



\## Rules



\- Usernames are single words without spaces.

\- Probability must satisfy `0 < p <= 1`.

\- A user cannot have a relationship to itself.

\- Lines beginning with `#` are comments and are ignored.

\- Each relationship line must contain exactly three fields.

\- Probability must be numeric.

\- Empty or nonexistent input files are rejected.



\## Available Datasets



&#x20;   data/tiny.txt

&#x20;   data/communities.txt

&#x20;   data/normal.txt

&#x20;   data/shock.txt

&#x20;   data/stress.txt



`normal.txt` represents ordinary network behaviour.



`shock.txt` uses the same population with stronger cross-community relationships to model a viral event.



`stress.txt` is a deterministic 500-user, 2000-edge network used for scalability testing.

