import sys, getopt, re

ppt_separator   = "==========================================================================="
event_separator = ".."
trace_separator = "--"

# TODOs:
# - Output texada usage.
# - Separate each object into individual traces.
# - Option for specifying the class to extract.
# - Option for including class invariants.
# - Option for including object invariants.

# Output texada usage:
# - at the end of the main method, print out a texada usage description.

# Separate each object into individual traces:
# - For each ppt, determine its "this" value.
# - Create a dictionary of ppt sequences, mapping objects to their sequences.
# - When printing the data/temp trace, iterate through the above dictionary and create a trace for each sequence.

# Option for specifying the class to extract:
# - Make an optional command line arg for specifying a specific class to extract [extract all by default].
# - For each ppt, determine the calling class.
# - If an arg was specified, when printing the data/temp trace, for each ppt sequence, check its calling class, and if it does not match the specified class, ignore.

# Option for including class invariants:
# - Make an optional command line flag for including class invariants [don't include by default].
# - Change regex in parse_into_event_map to include :::CLASS and :::OBJECT.
# - For each ppt, determine the calling class.
# - If flag is set, when printing the data/temp trace, for each ppt, find and print the corresponding class invariants.
# - Similarly for object invariants.



# Usage: quarry.py -d <dtracefile> -i <invarfile> -o <outputfile>
# Combines the ppt ordering in the dtrace file with the ppt to invariants
# mapping in the invariance output to write a Texada-processable log into
# the specified output file.
def main(argv):
   # parse command line arguments
   dfile = ''
   ifile = ''
   ofile = ''
   usage_desc = 'Usage: quarry.py -d <dtracefile> -i <invarfile> -o <outputfile>'
   
   try:
      opts, args = getopt.getopt(argv,"hd:i:o:",["dfile=","ifile=","ofile="])
   except getopt.GetoptError:
      print(usage_desc)
      sys.exit(2)
      
   for opt, arg in opts:
      if opt == '-h':
         print(usage_desc)
         sys.exit()
      elif opt in ("-d", "--dfile"):
         dfile = arg
      elif opt in ("-i", "--ifile"):
         ifile = arg
      elif opt in ("-o", "--ofile"):
         ofile = arg
      else:
         print(usage_desc)
         sys.exit(2)

   dlog = parse_into_splog(dfile)
   emap = parse_into_event_map(ifile)
   generate_data_event_trace(dlog,emap,ofile)


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

   ### An alternative implementation (harder to read but may be a bit more efficient)

   # with open(dfile) as f:
   #    for line in f:
   #       if line == '\n':
   #          try:
   #             tar_line = next(f)
   #             match = ppt.match(tar_line)
   #             if match != None:
   #                rtnlog.append(match.group(0))
   #          except StopIteration:
   #             break

   return rtnlog


# Parses a Daikon invariants output file into a map of ppts to their invariants.
def parse_into_event_map(ifile):
   rtnmap = {}
   ppt = ""
   invars = []
   # regex used to match ppt lines.
   ltype = re.compile("^(\w+\.)+\w+\(.*?\):::(ENTER|EXIT)[0-9]*")
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

   ### An alternative implementation (harder to read but may be a bit more efficient)

   # with open(ifile) as f:
   #    # skip the very first ppt separator.
   #    f.next()
   #    # read the file line by line and determine the type of the line.
   #    for line in f:
   #       # erase 
   #       line = line.replace('\n', '')
   #       # if the line is a ppt separator, then insert the current ppt and invars pair into 
   #       # the map to be returned. And reset the ppt and invars variables to be used for the
   #       # next ppt. 
   #       if (line == ppt_separator):
   #          rtnmap[ppt] = invars
   #          ppt = ""
   #          invars = []
   #       # if the line is not a ppt separator, and 
   #       elif ppt == "":
   #          ppt = line
   #       else:
   #          invars.append(line)

   # if ppt != "":
   #    rtnmap[ppt] = invars

   return rtnmap

# Given a sequence of ppts and a mapping of ppts to their invariants, print into a given output file
# a Texada-processable log consisting of the set of invariants as events and the sequence of ppts as
# the order in which these events occur.
def generate_data_event_trace(dlog,emap,ofile):

   f = open(ofile, "w")
   # regex used to match the ppt to one of two types: 
   #     (i)   method entry            (e.g. method():::ENTER) 
   #     (ii)  specific method exit    (e.g. method():::EXIT42)
   ppt_type = re.compile(":::(ENTER|EXIT)[0-9]*$")
   for key in dlog:
      for ppt in dlog[key]:
         match = ppt_type.search(ppt)
         invarlist = []
         # if the ppt was not mentioned in the invariants output, skip it.
         if ppt in emap:
            # if the ppt is a method entry, retrive its invariants.
            if match.group(0) == ":::ENTER":
               invarlist = emap[ppt]
            # otherwise the ppt is a specific method exit, so retrive both its invariants and the invariants of the corresponding aggregate exit.
            # e.g. given a ppt method()::EXIT42, its invariants consists of the union of[method()::EXIT42] and emap[method()::EXIT].
            else:
               ppt_agg = re.sub(":::EXIT[0-9]*$", ":::EXIT", ppt)
               if ppt in emap:
                  invarlist = emap[ppt] + emap[ppt_agg]
               else:
                 invarlist = emap[ppt_agg]
            # loop through the retrieved invariants and write them line by line into the outputfile followed by an event separator line.
            for invar in invarlist:
               f.write("%s\n" % invar)
      
            f.write("%s\n" % event_separator)
      # write a trace separator at the end of each trace
      f.write("%s\n" % trace_separator)

   f.close()
   return

if __name__ == "__main__":
   main(sys.argv[1:])
