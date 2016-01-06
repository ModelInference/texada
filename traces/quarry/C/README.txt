This example demontrates intra-class name collision issues. In particular,
it shows collision between two objects of the same class. If Quarry were to
separate traces solely by object, we would see the alternating invariant with
between this.x == 1 and this.x == 2 -- but inside a single object, x stays
constant after the first call to setToX().

Quarry currently splits traces over objects, which does not cause this problem.
However, splitting traces over objects could prove problematic for large 
applications where there are many different 
