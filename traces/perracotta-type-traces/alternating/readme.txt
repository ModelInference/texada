This folder contains the trace containing an Alternating property, with a as cause and b 
as effect, as well as the arguments to texada and expected instantiations returned from 
the texada run. 

The alternating property is described the the simplified (ignoring all non x or y events,
which can be inserted throughought) regular expression (xy)*; x being the placeholder 
for the cause, and y the placeholder for the effect. An LTL formulation of this property 
type is (!y W x) & G((x -> X(!x U y)) & (y -> X(!y W x))).

Alternating describes a relationship where every occurrence of the cause (x) is followed 
by only one occurrence of the effect (y) until another cause occurs. The effect cannot 
happen before the cause. 

Due to the strictness of alternating, mining (!y W x) & G((x -> X(!x U y)) & (y -> X(!y W x)))
on any of the other perracotta type traces will return no instantiations.

This property type and its relation to other property type is described in more 
detail in section 2.4.1 of J. Yang's dissertation Automatic Inference and Effective 
Application of Temporal Specifications, found at: 
http://www.cs.virginia.edu/~evans/pubs/theses/yang-abstract.html. 
