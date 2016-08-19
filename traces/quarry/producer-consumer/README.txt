This directory contains the Quarry producer-consumer example.

FILES IN MAIN DIR =============================================================
ProducerConsumerTest.java : main source file. Fixed version. Buggy version 
changes the while statements at lines 51 and 76 to if statements.
fix.trace, bug.trace : method call only logs for runs of the fixed and buggy 
versions of the program
preamble.txt : Daikon dtrace declarations for the program

FILES IN bug-dtraces, fix-dtraces =============================================
i-only-trace.dtrace : only the trace part of dtrace i
i.dtrace : dtrace i with declarations at the top
i.trace : data-temporal trace i, pre-processing
i-processed.txt : data-temporal trace i, after processing (can be passed to 
Texada)
log-processed.txt: all data-temporal traces stitched into a log (used in paper
eval)
cat-preamble.sh : produces i.dtrace files form i-only-trace.dtrace files
make-texada-traces.sh: produces data-temporal log from i.dtrace files

