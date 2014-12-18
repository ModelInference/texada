import sys, re
from argparse import ArgumentParser

ppt_separator   = "==========================================================================="
event_separator = ".."
trace_separator = "--"

# Usage: quarry.py <dtracefile> <invarfile> <outputfile>
# Combines the ppt ordering in the dtrace file with the ppt to invariants
# mapping in the invariance output to write a Texada-processable log into
# the specified output file.
def main():
   # parse command line arguments
   parser = ArgumentParser()
   # positional arguments
   parser.add_argument("dtracefile", help="name of file containing temporal information of program execution")
   parser.add_argument("invarsfile", help="name of file containing daikon-produced invariants of program")
   parser.add_argument("outputfile", help="name of file to write the generated data-event traces")
   # optional arguments
   parser.add_argument("-o", "--obj_invars", help="include object invariances",  action="store_true")
   parser.add_argument("-c", "--cls_invars", help="include class invariances",   action="store_true")
   parser.add_argument("-e", "--extract",    help="specify a specific class to extract [by default: extract all]")
   
   # organize command-line arguments
   args = parser.parse_args()
   dtrace = args.dtracefile
   invars = args.invarsfile
   ofile = args.outputfile
   incl_obj = args.obj_invars
   incl_cls = args.cls_invars
   class_to_extract = args.extract

   # generate a set of ppt sequences from the specified dtrace file.
   dlog = parse_into_splog(dtrace)
   # generate a mapping of ppts to invariances from the specified invariance file.
   emap = parse_into_event_map(invars)
   # combine the generated ppt sequence set and event map into a set of data-temporal traces, and write this into the specified output file.
   generate_data_event_trace(dlog, emap, ofile, incl_obj, incl_cls)
   # Print a description of how to use the generated data-event trace with Texada.
   texada_usage = '\nHow to process Quarry output using Texada:\nIn order to mine temporal properties from the outputed data-temp log, run the file with Texada using it\'s mult-prop option.\nExample usage: ./texada -l -f \'G(x -> FXy)\' --log-file path/to/data-temp-log'
   print("%s\n" % texada_usage)


# Parses a Daikon dtrace file into a sequence of its ppts.
def parse_into_splog(dfile):
   rtnlog = {}
   # regex used to match ppt lines.
   ppt = re.compile("^(\w+\.)*(?P<class_name>\w+)\.(?P<method_name>\w+)\(.*?\):::(ENTER|EXIT)[0-9]*")
   # read the file line by line and append ppt lines into the returning list
   with open(dfile) as f:
      for line in f:
         match = ppt.match(line)
         # TODO: keep entries to constructors and check subsequent ppts for matching nonce value. When a match is found,
         # append the constructor entry to the front of the corresponding ppt sequence.

         # Check whether the given line specifies the beginning of a data trace record.
         if match != None:
            calling_class = match.group('class_name')
            this_invocation_nonce = -1
            this = -1
            # Iterate through the given data trace record and determine the values of the following fields:
            #  - this_invocation_nonce
            #  - this (i.e. the calling object id)
            line = ""
            while line != "\n":
               line = next(f)
               if line == "this_invocation_nonce":
                  this_invocation_nonce = next(f)
               elif line == "this":
                  this = line
            # append the 
            if calling_class in rtnlog:
               rtnlog[calling_class].append(match.group(0))
            else:
               rtnlog[calling_class] = [match.group(0)]

   return rtnlog


# Parses a Daikon invariants output file into a map of ppts to their invariants.
def parse_into_event_map(ifile):
   rtnmap = {}
   ppt = ""
   invars = []
   # regex used to match ppt lines.
   ltype = re.compile("^(\w+\.)+\w+\(.*?\):::(ENTER|EXIT|OBJECT|CLASS)[0-9]*")
   # read the file line by line checking for ppt lines
   with open(ifile) as f:
      for line in f:
         match = ltype.match(line)
         # if line is a ddt, read the following lines until a ppt separator,
         # and for each line, add it to the correponding list of invariants.
         # note that a blank line may be used as a ppt separator at the end
         # of a daikon invariants file.
         if match != None:
            ppt = match.group(0)
            invars = []
            for line_i in f:
               line_i = line_i.replace('\n', '')
               if line_i == ppt_separator or line_i == "":
                  break
               else:
                  invars.append(line_i)
            rtnmap[ppt] = invars

   return rtnmap

# Given a sequence of ppts and a mapping of ppts to their invariants, print into a given output file
# a Texada-processable log consisting of the set of invariants as events and the sequence of ppts as
# the order in which these events occur.
def generate_data_event_trace(dlog,emap,ofile,incl_obj,incl_cls):

   f = open(ofile, "w")
   # regex used to match the ppt to one of two types: 
   #     (i)   method entry            (e.g. method():::ENTER) 
   #     (ii)  specific method exit    (e.g. method():::EXIT42)
   regex = re.compile(":::(?P<ppt_type>ENTER|EXIT)[0-9]*$")
   for key in dlog:
      for ppt in dlog[key]:
         match = regex.search(ppt)
         invarlist = []
         # if the ppt was not mentioned in the invariants output, skip it.
         if ppt in emap:
            invarlist = emap[ppt]
            # if the ppt is an exit point, append the invariants of the corresponding aggregate exit.
            # e.g. given a ppt method()::EXIT42, its invariants consists of the union of[method()::EXIT42] and emap[method()::EXIT].
            if match.group('ppt_type') == "EXIT":
               ppt_agg = re.sub(":::EXIT[0-9]*$", ":::EXIT", ppt)
               invarlist = emap[ppt] + emap[ppt_agg] # TODO: why is this different from invarlist += emap[ppt_agg]?
            # loop through the retrieved invariants and write them line by line into the outputfile followed by an event separator line.
            for invar in invarlist:
               f.write("%s\n" % invar)
      
            f.write("%s\n" % event_separator)
      # write a trace separator at the end of each trace
      f.write("%s\n" % trace_separator)

   f.close()
   return

if __name__ == "__main__":
   main()
