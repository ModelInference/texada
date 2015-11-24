strange-support-log.txt gives an example of where (as of 2015-11-24) Texada's
support, confidence algorithms give results that are comparable. The log 
consists of a single trace, which starts with

c
b
c

and is followed by 10 instances of

a
b
b
b
b
b
b
b
b
b
b

On this log,  we see that running Texada with -l -f 'G(x->XFy)' --print-stats
gives

G(a -> XFb)
   support: 10
   support potential: 10
   confidence: 1
G(c -> XFa)
   support: 2
   support potential: 2
   confidence: 1
G(c -> XFb)
   support: 2
   support potential: 2
   confidence: 1

and running Texada with  -l -f 'G(x->XG!y)' --print-stats gives

G(a -> XG!c)
   support: 595
   support potential: 595
   confidence: 1

It is not clear whether G(a -> XG!c) should be so much more supported than G(a -> XFb).


