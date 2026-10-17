\# Problem Statement



Social media platforms want to know which users matter most and how a post spreads from one person to another. This project is a tool that answers that on a follower network.



User: a social media analyst planning a campaign.



Input: a list of users, who follows whom, and a number between 0 and 1 on each follow showing how likely a post is to pass along it. Each user also has a cost to promote, and there is a total budget.



Output: a ranking of users, the most likely path between two users, and the best set of users to promote within the budget.



Constraints: the spread number must be between 0 and 1, a user cannot follow themselves, and the budget is a whole number.



Success: the results match a slow brute-force check on small inputs, and the running time grows on larger inputs the way the theory says it should.

