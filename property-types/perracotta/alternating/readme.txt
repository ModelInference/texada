This folder contains the trace containing an Alternating property,
with 'a' as cause and 'b' as effect, as well as the arguments to
texada and expected instantiations returned from the texada run.

Alternating describes a relationship where every occurrence of the
cause (x) is followed by only one occurrence of the effect (y) until
another cause occurs. The effect cannot happen before the cause.

The alternating property is describes the simplified regular
expression (xy)* (ignoring all non x or y events, which can be
inserted throughought). In this expression x is a placeholder for the
cause, and y is the placeholder for the effect. An LTL formulation of
this property type is (!y W x) & G((x -> X(!x U y)) & (y -> X(!y W x))).

Due to the strictness of alternating, mining (!y W x) & G((x -> X(!x U y)) & (y -> X(!y W x)))
on any of the other perracotta type traces will return no instantiations.

This property type and its relation to other property type is described in more 
detail in section 2.4.1 of J. Yang's dissertation Automatic Inference and Effective 
Application of Temporal Specifications, found at: 
http://www.cs.virginia.edu/~evans/pubs/theses/yang-abstract.html. 
