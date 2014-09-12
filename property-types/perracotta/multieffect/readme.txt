This folder contains the trace containing a MultiEffect property, with a as cause 
and b as effect, as well as the arguments to texada and expected instantiations 
returned from the texada run. 

The MultiEffect property is described the the simplified (ignoring all non x or y events,
which can be inserted throughought) regular expression (xyy*)*; x being the placeholder 
for the cause, and y the placeholder for the effect. An LTL formulation of this property 
type is (!y W x) & G(x -> X(!x U y)).

MultiEffect describes a relationship where the effect (y) cannot happen before the initial
cause (x), and every occurrence of the cause is followed by one or more occurrences 
of the effect. Additional cause eventsc annot happen before the effect events.

Since MultiEffect is fairly restrictive, mining (!y W x) & G(x -> X(!x U y))on all traces 
except Alternating will produce no instantiations; mining it on alternating will return 
(!b W a) & G(a -> X(!a U b)).

This property type and its relation to other property type is described in more 
detail in section 2.4.1 of J. Yang's dissertation Automatic Inference and Effective 
Application of Temporal Specifications, found at: 
http://www.cs.virginia.edu/~evans/pubs/theses/yang-abstract.html. 
