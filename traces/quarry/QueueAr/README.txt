This folder contains the source code for the Daikon test program QueueAr
and the trace data-temporal trace which was used to extract invariants
for the Quarry SRC ICSE poster and presentation. 

========= INFERRING DATA-TEMPORAL PROPERTIES FROM PROVIDED TRACE ==============

From this trace (QueueAr152526237_Log.txt) we can use Texada to infer the
three properties discussed in the ICSE SRC Research Abstract, "Mining
Temporal Properties of Data Invariants":

1) G"this.back <= size(this.theArray[])-1"
   this property holds with confidence 0.99765. It can be mined with the
   Texada args:
   -l -f 'G(x)' --conf-threshold 0.95 --use-global-thresholds --parse-mult-prop 
   on the trace in this directory, that is 
   /TEXADA_LOCATION/traces/quarry/QueueAr/QueueAr152526237_Log.txt

2) !"this.currentSize <= size(this.theArray[])-1" U "this.back <= size(this.theArray[])-1"
   !"this.currentSize <= size(this.theArray[])-1" U "this.front <= size(this.theArray[])-1"
   These properties hold with confidence 1 and 0.98995, respectively -- the 
   trace generation mechanism has changed since the initial prototype discussed 
   in "Mining Temporal Properties of Data Invariants", and the second has lower
   confidence on this new trace. Texada args for inference:
   -l -f '!"this.currentSize <= size(this.theArray[])-1" U y' --parse-mult-prop 
   -e 'this.currentSize <= size(this.theArray[])-1' --conf-threshold 0.98 
   --use-global-thresholds
   on the trace in this directory, that is 
   /TEXADA_LOCATION/traces/quarry/QueueAr/QueueAr152526237_Log.txt
   (running without --conf-threshold 0.98 and --use-global-thresholds will not 
   output the second property, relating currentSize and front, but will be much
   faster. For even quicker output use the -m option instead of -l, without
   the threshold options). 

=========== REPRODUCING QUARRY TRACE ==========================================

Alternately, you can run Quarry end-to-end on QueueAr to produce the log in 
this directory as well as additional logs to analyze. Currently Quarry logs
are split by object, and the log in this directory captures behaviour for 
the object DataStructures.QueueAr152526237.

Steps to produce log:

1) Compile QueueArTester and get invariants for the entire log.
   Follow instructions here (http://plse.cs.washington.edu/daikon/download/doc/daikon.html#Detecting-invariants-in-Java-programs)
   to compile QueueArTester and run Daikon on it, obtaining a .dtrace.gz and .inv.gz file. 
   Retain the textual invariant output (either from command line or by using Daikon's
   tools to print out invariants from a .inv file)

2) Split log into traces.
   We achieved this by running Chicory to produce a .dtrace.gz file on each
   individual test in QueueArTester.java (i.e. every method call in run()). 
   In more detail, for every method call in run(), We commented every other
   call in run() and re-compiled QueueArTester.java to effectively run only
   one test at the time. We saved each .dtrace.gz file for every test.

3) Use quarry.py to produce data-temporal traces.
   For each .dtrace file produced, we ran (in the quarry-src dir)
   $ python quarry.py -l TESTJ.dtrace ALLINVS.txt prefix.txt
   (where TESTJ.dtrace is the .dtrace for each test and ALLINVS.txt 
   was the file produced in 1; prefix.txt is optional and gives a 
   prefix to the output files; -l creates a separate log for each object). 
   This produces a trace for each object in the .dtrace file, named
   prefixOBJECTNAME.txt. We retained only the files for the object
   DataStructures.QueueAr152526237. If no such object exists, it is
   the QueueAr object whose data-temporal trace is the longest for 
   the first test.  

4) Concatenate data-temporal traces for matching objects.
   QueueAr152526237_Log.txt was created by concatenating the traces
   produced for QueueAr152526237 for each test in QueueArTester 
   in step 3. Each trace must be separated by --; this should appear
   at the end of each trace produced in 3 by default. You can now
   infer temporal properties on this trace using Texada. 
   


