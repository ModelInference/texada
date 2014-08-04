This folder contains the trace containing a CauseFirst property, with a as cause 
and b as effect, as well as the arguments to texada and expected instantiations 
returned from the texada run. 

The causefirst property is described the the simplified (ignoring all non x or y events,
which can be inserted throughought) regular expression (xx*yy*)*; x being the placeholder 
for the cause, and y the placeholder for the effect. An LTL formulation of this property 
type is (!y W x) & G(x -> XFy).

Causefirst describes a relationship where any occurrence of the cause (x) is eventually
followed by an occurrence of effect (y), but in which the effect cannot happen before the 
cause. 

Since  the only restriction beyond response that CauseFirst provides is that the cause
must be first and Response, EffectFirst, OneEffect, and OneCause allow the effect to happen 
before the cause,  mining (!y W x) & G(x -> XFy) on the Alternating, MultiEffect, and 
MultiCause traces will return (!b W a) & G(a -> XFb).

This property type and its relation to other property type is described in more 
detail in section 2.4.1 of J. Yang's dissertation Automatic Inference and Effective 
Application of Temporal Specifications, found at: 
http://www.cs.virginia.edu/~evans/pubs/theses/yang-abstract.html. 
