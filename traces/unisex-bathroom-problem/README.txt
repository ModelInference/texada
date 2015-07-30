This is a placeholder for a use-case based on the unisex bathroom
problem. See the following write-up for a description of the problem:
http://ww1.ucmss.com/books/LFS/CSREA2006/PDP3757.pdf

The basic idea in this problem is that you have some state/event that
should only occur between two other states/events, and always be
occurring between those two states and events. In this case, we want
to make sure i-in-bathroom occurs only between i-entering-bathroom and
i-exiting-bathroom and is always true between i-entering-bathroom and
i-exiting-bathroom. In LTL this can be written as:

F(p) -> ((!p W q) & G((q & !r)->X(p U r)) & G((r & !q)->(!p U q)))
 (1)        (2)             (3)                       (4)

(1) if p occurs, then....

(2) p doesn't occur before q (q AP p)

(3) after a point where q occurs, p must occur until r occurs

(4) at a point where r occurs, p must no occur until q occurs (perhaps
I should make this into q R !p or !p U (!p & q)... anyway)
