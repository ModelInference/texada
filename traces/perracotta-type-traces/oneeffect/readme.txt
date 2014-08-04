This folder contains the trace containing a OneEffect property, with a as cause 
and b as effect, as well as the arguments to texada and expected instantiations 
returned from the texada run. 

The OneEffect property is described the the simplified (ignoring all non x or y events,
which can be inserted throughought) regular expression y*(xx*y)*; x being the placeholder 
for the cause, and y the placeholder for the effect. An LTL formulation of this property 
type is G((x -> XFy) & (y -> X(!y W x))).

MultiCause describes a relationship where every occurrence of the cause (x) is followed by 
only one occurrence of the effect (y), unless another cause happens after the effect. However, 
additional cause events may happen before the single effect event.

Since OneEffect allows the occurrence of an effect before the cause, mining 
G((x -> XFy) & (y -> X(!y W x))) on Alternating, CauseFirst, and MultiCause traces
will return G((a -> XFb) & (b -> X(!b W a))).

This property type and its relation to other property type is described in more 
detail in section 2.4.1 of J. Yang's dissertation Automatic Inference and Effective 
Application of Temporal Specifications, found at: 
http://www.cs.virginia.edu/~evans/pubs/theses/yang-abstract.html. 
