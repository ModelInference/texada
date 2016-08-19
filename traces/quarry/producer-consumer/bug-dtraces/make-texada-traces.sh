#!/bin/bash
array=( 1 2 3 4 5 6 7 8 9 10 11 12)
arraydtrace=( 1.dtrace 2.dtrace 3.dtrace 4.dtrace 5.dtrace 6.dtrace 7.dtrace 8.dtrace 9.dtrace 10.dtrace 11.dtrace 12.dtrace)
java daikon.Daikon --config ~/quarry-temp/no-ternary-config.txt "${arraydtrace[@]}" > DaikonOut.txt
mv 1.inv.gz Invariants.inv.gz
java InvariantTransplanter Invariants.inv.gz
j=0
for i in "${array[@]}"
do
   java daikon.tools.InvariantChecker Invariants-unified.inv.gz $i.dtrace > $i-trace.txt
   ~/quarry-temp/cleanupscript.sh $i-trace.txt
   python ../../trace-preprocessor.py $i-trace.txt
   processedarray[j]=$i-trace-processed.txt
   ((j++))
done
cat ${processedarray[@]} > log-processed.txt



