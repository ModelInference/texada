This directory contains the source code and resulting traces for an
implementation that solves the dining philosophers problem in Java.
The implementation is by Stephen J. Hartley.

To use:

1. Compile from ./ with:
$ ant

2. Run the implementation from ./ with:
$ ./run.sh


You should see output like this:
==
0 is THINKING
..
1 is THINKING
..
2 is THINKING
..
3 is THINKING
..
4 is THINKING
..
==
0 is HUNGRY
..
1 is THINKING
..
2 is THINKING
..
3 is THINKING
..
4 is THINKING
..
==
