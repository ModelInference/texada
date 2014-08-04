This folder contains the trace containing a OneCause property, with a as cause 
and b as effect, as well as the arguments to texada and expected instantiations 
returned from the texada run. 

The OneCause property is described the the simplified (ignoring all non x or y events,
which can be inserted throughought) regular expression y*(xyy*)*; x being the placeholder 
for the cause, and y the placeholder for the effect. An LTL formulation of this property 
type is G(x -> X(!x U y)).

MultiEffect describes a relationship where every occurrence of the cause (x) is followed by 
one or more occurrences of the effect (y). Additional cause events cannot happen before the 
effect events.

Since OneCause allows the occurrence of an effect event first , mining G(x -> X(!x U y))
on Alternating, MultiEffect, and CauseFirst traces will return the instantiation G(a -> X(!a U b)).

This property type and its relation to other property type is described in more 
detail in section 2.4.1 of J. Yang's dissertation Automatic Inference and Effective 
Application of Temporal Specifications, found at: 
http://www.cs.virginia.edu/~evans/pubs/theses/yang-abstract.html. 
