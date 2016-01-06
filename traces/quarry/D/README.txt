This example gives an example of intra-object naming collision. While parameters
are distinguished from fields with the prefix this., parameters to different 
methods are not. 

Running Texada with any variation of response gives that 
           "x one of { 1, 2, 3 }" responds to "x one of { 0, 1, 2 }"
however, the two x's being discussed are parameters to different methods, not
the same data value. 

Identical parameter names are fairly likely to occur "in the wild". The best
solution to this is probably renaming at the trace/invariant generation end
of things. 
