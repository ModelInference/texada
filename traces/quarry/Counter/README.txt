This 'Counter' example demonstrates the difficulty with checking data-temporal
properties on traces consisting of sequences of data invariants instead of data
values. Looking at the data trace for each test, we should observe that 
size >= 1 is always followed by size == 0.

However, when checking against the data-temporal trace (trace.txt), this 
instance of the property type G(x->XFy) does not show up; size == 0 is only a
data invariant at creation, not in the remove() function's exit. As such, we
fail to find G("size >= 1" -> XF"size == 0"). Our interpretation of data-
temporal properties checked over the data-temporal trace is thus a bit counter-
intuitive. For example, the current output includes the instantiation:

                      G("size == 0" -> XF "this.size >= 1")

which would not hold if checked over the data values (where "size == 0" always
occurs at our terminal trace events), but holds over the data-temporal (i.e.
data invariant) trace. 


