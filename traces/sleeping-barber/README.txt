This directory contains the source code and resulting traces for an
implementation that solves the sleeping barber problem in Java.

To use:

1. Compile from ./ with:
$ ant

2. Run the implementation from ./ with:
$ ./run.sh


You should see output like this (number of customers and number of times
they wish to get their hair cut can be adjusted in the source):

Barber is Checking
Customer1 is Chilling
Customer0 is Exiting
Customer2 is Waiting
..
Barber is CuttingHair
Customer1 is Chilling
Customer0 is Exiting
Customer2 is GettingHairCut
..



================= USING TEXADA TO CONFIRM SOLUTION PROPERTIES =================

This folder contains several args files that can be used to confirm solution
properties for the sleeping barber implementation given in src. 

 args-excluded-events(-barber).txt
 The file without the -barber provides the formula which can be used to confirm
   (1) No two customers are getting their hair cut at the same time
   (2) The barber is not sleeping while customers are waiting
 The -barber file will provide easier-to-process output to confirm the second
 property. (Note that in our implementation, the barber "takes a mini-nap" 
 between finishing to cut hair and checking the waiting room, in the sense that
 his state is set to Sleeping. As such, it may be possible to see a violation 
 of this property with different settings). 
 
 args-eventually-hair-cut.txt
 This file provides args to check which events eventually happy over all traces
 in order to confirm all customers get a haircut.

 args-exactly-two-haircuts.txt
 This file provides args to check that each customer gets exactly two haircuts.
 Note that in the output "Customer4 is GettingHairCut" does not appear in a valid
 binding; this is because in the second trace of the log, Customer4 gets a 
 haircut while no logging event happen, so "Customer4 is GettingHairCut" is never
 logged. But we see the customer does get a haircut since the entering->waiting->
 exiting cycle occurs. 
 
 args-fifo.txt
 This file provides args to check that the first-in-first-out property is 
 demonstrated by the traces; if Customer i gets to sit in the waiting room
 before Customer j, then Customer i will get a haircut before Customer j.
 It is recommended to filter the output of this property to see this fifo. On 
 a system on which sed is available, the command 
 sed -n '/(!"Customer\([0-9]\) is Waiting" W "Customer\([0-9]\) is Waiting") -> (!"Customer\1 is GettingHairCut" W "Customer\2 is GettingHairCut")/p'
 will filter the Texada output properly. It is future work to build in regex-
 restricted variables as an option to Texada, which will reduce the need
 to filter output. 
