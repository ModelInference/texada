import sys, getopt, re

ppt_separator = "==========================================================================="
event_separator = ".."
trace_separator = "--"

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


def parse_into_splog(dfile):
   rtnlog = []
   
   ppt = re.compile("^(\w+\.)+\w+\(.*?\):::(ENTER|EXIT)[0-9]*")

   with open(dfile) as f:
      for line in f:
         match = ppt.match(line)
         if match != None:
            rtnlog.append(match.group(0))

#   with open(dfile) as f:
#      for line in f:
#         if line == '\n':
#            try:
#               tar_line = next(f)
#               match = ppt.match(tar_line)
#               if match != None:
#                  rtnlog.append(match.group(0))
#            except StopIteration:
#               break

   return rtnlog


def parse_into_event_map(ifile):
   rtnmap = {}
   ppt = ""
   invars = []
   
   with open(ifile) as f:
      f.next()
      for line in f:
         line = line.replace('\n', '')
         if (line == ppt_separator):
            rtnmap[ppt] = invars
            ppt = ""
            invars = []
         elif ppt == "":
            ppt = line
         else:
            invars.append(line)

   if ppt != "":
      rtnmap[ppt] = invars

   return rtnmap


def generate_data_event_trace(dlog,emap,ofile):

   f = open(ofile, "w")
   ppt_type = re.compile(":::(ENTER|EXIT)[0-9]*$")
   for ppt in dlog:
      match = ppt_type.search(ppt)
      invarlist = []
      
      if match.group(0) == ":::ENTER":
         invarlist = emap[ppt]
      else:
         ppt_agg = re.sub(":::EXIT[0-9]*$", ":::EXIT", ppt)
         if ppt in emap:
            invarlist = emap[ppt] + emap[ppt_agg]
         else:
            invarlist = emap[ppt_agg]
         
      for invar in invarlist:
         f.write("%s\n" % invar)
      
      f.write("%s\n" % event_separator)

   f.write(trace_separator)
   f.close()
   return


if __name__ == "__main__":
   main(sys.argv[1:])


















