import sys, getopt

def main(argv):
   # parse command line arguments
   dtracefile = ''
   invarfile = ''
   outputfile = ''
   usage_desc = 'Usage: quarry.py -d <dtracefile> -i <invarfile> -o <outputfile>'
   try:
      opts, args = getopt.getopt(argv,"hd:i:o:",["dfile=","ifile=","ofile="])
   except getopt.GetoptError:
      print usage_desc
      sys.exit(2)
   for opt, arg in opts:
      if opt == '-h':
         print usage_desc
         sys.exit()
      elif opt in ("-d", "--dfile"):
         dtracefile = arg
      elif opt in ("-i", "--ifile"):
         invarfile = arg
      elif opt in ("-o", "--ofile"):
         outputfile = arg
      else:
         print usage_desc
         sys.exit(2)

   # open input and output files
   dfile = open(dtracefile, 'r')
   ifile = open(invarfile, 'r')
   ofile = open(outputfile, 'w')

   dlog = parse_into_splog(dfile)
   emap = parse_into_event_map(ifile)
   output_data_event_trace(dlog,emap,ofile)

   # close all files
   dfile.close()
   ifile.close()
   ofile.close()


def parse_into_splog(dfile):
   rtnlog = []

   # TODO: skip ppt declarations

   # TODO: read until a blank line

      # TODO: add next line to rtnlog

   return rtnlog


def parse_into_event_map(ifile):
   rtnmap = {}

   # TODO: read until a separator line

      # TODO: read next line into x

      # TODO: if x is an entry point, then add a simple ppt/invariants mapping

      # TODO: if x is an exit point, then combine its invariants with the corresponding aggregate invariants, and use this to create a ppt/invariants mapping.

      # TODO: if x is an object point, then !!!

      # TODO: if x is a class point, then !!!

      # TODO: if x is a separator line, then break out of inner loop.

   return rtnmap


def output_data_event_trace(dlog,emap,ofile):

   # TODO: for i = 0 to size(dlog)-1

      # TODO: x_i = dlog[i]

      # TODO: invar_i = emap[x_i]

      # TODO: if invar_i is a specific exit

         # TODO: x_agg_i = x_i with the last numbers removed

         # TODO: invar_agg_i = dlog[x_agg_i]

         # TODO: invar_i.append(invar_agg_i)

      # TODO: for j = 0 to size(invar_i)-1

         # TODO: write invar_i[j] into ofile followed by line break

      # TODO: write an event termination line into ofile followed by line break

   # TODO: write a trace terminaion line into ofile

   return


if __name__ == "__main__":
   main(sys.argv[1:])


















