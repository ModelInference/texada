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

