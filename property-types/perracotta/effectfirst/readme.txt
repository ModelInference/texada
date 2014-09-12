This folder contains the trace containing an EffectFirst property, with a as cause 
and b as effect, as well as the arguments to texada and expected instantiations 
returned from the texada run. 

The EffectFirst property is described the the simplified (ignoring all non x or y events,
which can be inserted throughought) regular expression y*(xy)*; x being the placeholder 
for the cause, and y the placeholder for the effect. An LTL formulation of this property 
type is G((x -> X(!x U y)) & (y -> X(!y W x))).

EffectFirst describes a relationship where the effect (y) may happen initally before a 
cause (x), but every occurrence of the cause is followed by exactly one occurrence of
the effect before the next occurrence of the cause, and no cause events can happen
between a cause event and its corresponding effect.

Since EffectFirst if fairly restrictive, mining G((x -> X(!x U y)) & (y -> X(!y W x))) 
on all traces except Alternating will produce no instantiations; mining it on alternating
will return G((a -> X(!a U b)) & (b -> X(!b W a))).

This property type and its relation to other property type is described in more 
detail in section 2.4.1 of J. Yang's dissertation Automatic Inference and Effective 
Application of Temporal Specifications, found at: 
http://www.cs.virginia.edu/~evans/pubs/theses/yang-abstract.html. 
