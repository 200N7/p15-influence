\# Input format



Graph file: one line per follow, in the form



&#x20;   user\_a user\_b p



\- user\_a follows user\_b

\- p is the chance a post passes along that follow, a number above 0 and up to 1

\- user names are single words with no spaces

\- lines starting with # are comments and are skipped



Example: data/tiny.txt



\# Invalid input (the loader must reject these)



\- p less than or equal to 0, or greater than 1

\- a user following themselves (user\_a equal to user\_b)

\- a line with fewer or more than 3 fields

\- p that is not a number

\- an empty file or a file that does not exist

