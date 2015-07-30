This directory contains the source code and resulting traces for an
implementation that solves the dining philosophers problem in Java.
The implementation is by Stephen J. Hartley.

To use:

1. Compile from ./ with:
$ ant

2. Run the implementation from ./ with:
$ ./run.sh

You should see output like this:

0 is THINKING
1 is THINKING
2 is THINKING
3 is THINKING
4 is THINKING
..
0 is HUNGRY
1 is THINKING
2 is THINKING
3 is THINKING
4 is THINKING
..


============== RUNNING TEXADA TO CONFIRM SOLUTION PROPERTIES ===============

Several args files are provided which can be given to texada with the -c
or --config-file option in order to confirm some expected solution properties:

 args-check-exclusive-access.txt 
 This args file provides arguments to check the "adjacent philosophers never
 eat at the same time" or equivalently "only one philosopher can access the
 same utensil at the same time" property. 

 args-eventually-eat-together(-phil*).txt
 These args files provide arguments to check that eventually non-adjacent
 philosophers eat together. the (-phil*) versions output only the properties
 involving the given philospher eating, and are easier for manual examination.


