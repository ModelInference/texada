Incrementer provides an example of missing/implicit invariants. Without the two
calls to increment(-1) and increment(-2), Daikon only infers a one-of invariant
for increment's entry and exit instead of inferring the correct invariant (that
the method increments x by 3). This is generally a problem of the test cases we 
are provided.
