This folder contains the trace containing a MultiCause property, with a as cause 
and b as effect, as well as the arguments to texada and expected instantiations 
returned from the texada run. 

The MultiCause property is described the the simplified (ignoring all non x or y events,
which can be inserted throughought) regular expression (xx*y)*; x being the placeholder 
for the cause, and y the placeholder for the effect. An LTL formulation of this property 
type is (!y W x) & G((x -> XFy) & (y -> X(!y W x))).

MultiCause describes a relationship where the effect (y) cannot happen before the initial
cause (x), and every occurrence of the cause is followed by only one occurrence of the 
effect, unless another cause happens after the effect. However, additional cause events
may happen before the single effect event.

Since MultiCause is fairly restrictive, mining (!y W x) & G((x -> XFy) & (y -> X(!y W x)))
on all traces except Alternating will produce no instantiations; mining it on alternating
will return (!b W a) & G((a -> XFb) & (b -> X(!b W a))).

This property type and its relation to other property type is described in more 
detail in section 2.4.1 of J. Yang's dissertation Automatic Inference and Effective 
Application of Temporal Specifications, found at: 
http://www.cs.virginia.edu/~evans/pubs/theses/yang-abstract.html. 
