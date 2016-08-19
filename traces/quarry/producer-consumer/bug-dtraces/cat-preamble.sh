#!/bin/bash
array=( 1 2 3 4 5 6 7 8 9 10 11 12)
for i in "${array[@]}"
do
   mv $i.dtrace $i-only-trace.dtrace
   cat ../preamble.txt $i-only-trace.dtrace > $i.dtrace
done
