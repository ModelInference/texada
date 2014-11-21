import sys, getopt

def main(argv):
   # parse command line arguments
   dtracefile = ''
   invarfile = ''
   outputfile = ''
   usage_desc = 'Usage: dtt-generator.py -d <dtracefile> -i <invarfile> -o <outputfile>'
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

   # TODO: add each ppt to rtnlog

   return rtnlog


def parse_into_event_map(ifile):
   rtnmap = {}

   # TODO: for each program point p in ifile, insert a mapping of
   # p to its invariants into rtnmap.

   return rtnmap


def output_data_event_trace(dlog,emap,ofile):

   # TODO: iterate over dlog, and for each string in dlog
   # find its invariants [i0,...,ik] under emap. Then write
   # each of i0,...,ik to the ofile, followed by the event
   # termination line (i.e. '..'). 

   return


if __name__ == "__main__":
   main(sys.argv[1:])



























