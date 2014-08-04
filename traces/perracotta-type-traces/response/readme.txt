This folder contains the trace containing a Response property, with a as cause and 
b as effect, as well as the arguments to texada and expected instantiations 
returned from the texada run. 

The Response property is described by the simplified (ignoring all non x or y events,
which can be inserted throughought) regular expression y*(xx*yy*)*; x being the 
placeholder for the cause, and y the placeholder for the effect. The strict LTL
formulation of this property type is G(x->XFy).

Reponse describes a relationship where every occurrence of the cause (x) will eventually
result in an occurrence of the effect (y). Effect events can happen before the initial
cause event.

Due to the generality of Response, mining G(x->XFy) on any of the other perracotta
type traces should return G(a->XFb), amongst other instantiations.

This property type and its relation to other property type is described in more 
detail in section 2.4.1 of J. Yang's dissertation Automatic Inference and Effective 
Application of Temporal Specifications, found at: 
http://www.cs.virginia.edu/~evans/pubs/theses/yang-abstract.html. 
