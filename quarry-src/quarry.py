import re
from argparse     import ArgumentParser
from invarparse   import InvariantParser
from dtraceparse  import DTraceParser

def main():
   ''' Returns a data-temporal log generated from combining a pair of Daikon-generated dtrace and invariants files.
   Usage: quarry.py [-h] [-o] [-c] [-e EXTRACT] <dtracefile> <invarfile> <outputfile>'''

   # process options
   argparser = ArgumentParser()
   argparser.add_argument("dfile", help="path to the Daikon-generated dtrace file.")
   argparser.add_argument("ifile", help="path to the Daikon-generated invariants file.")
   argparser.add_argument("ofile", help="path to the output file.")
   argparser.add_argument("--incl_obj",         help="include object invariances",  action="store_true")
   argparser.add_argument("--incl_cls",         help="include class invariances",   action="store_true")
   argparser.add_argument("-e", "--extract",    help="specify a specific class to extract [by default: extract all]")
   args = argparser.parse_args()

   # parse dtrace file
   dtraceparser = DTraceParser(args.dfile)
   # if args.extract:
      # TODO: configure dtraceparser to extract only the specified class
   traces = dtraceparser.parse_dtrace()

   # parse invariants file
   invariantparser = InvariantParser(args.ifile)
   invars = invariantparser.parse_invariants()

   # generate data-temp log
   generate_data_temp_log(traces, invars, args.ofile, args.incl_cls, args.incl_obj)

   # Print a description of how to process the generated data-temp log using Texada.
   texada_usage = 'How to process Quarry output using Texada:\nIn order to mine temporal properties from the outputed data-temp log, run the file with Texada using it\'s mult-prop option.\nExample usage: ./texada -l -f \'G(x -> FXy)\' --log-file path/to/data-temp-log'
   print("%s\n" % texada_usage)

def generate_data_temp_log(traces, invars, filepath, incl_cls, incl_obj):
   '''Prints a data-temporal log into the file located at *filepath*, generated from combining *dtset* and *invars*.'''
   # TODO
   with open(filepath, 'w') as f:
      for obj, trace in traces.iteritems():
         for ppt in trace:
            ppt_name = ppt[0]+"."+ppt[1]+":::"+ppt[2]+ppt[3]
            if ppt_name not in invars:
               continue
            invarlist = invars[ppt_name]
            # if the ppt is an exit point, append the invariants of the corresponding aggregate exit.
            # e.g. given a ppt method()::EXIT42, its invariants consists of the union of[method()::EXIT42] and emap[method()::EXIT].
            if ppt[2] == "EXIT":
               agg_ppt_name = ppt[0]+"."+ppt[1]+":::"+ppt[2]
               invarlist = invars[ppt_name] + invars[agg_ppt_name]
            else:
               invarlist = invars[ppt_name]
            # loop through the retrieved invariants and write them line by line into the ofile, followed by a event separator.
            for invar in invarlist:
               f.write("%s\n" % invar)
            f.write("%s\n" % "..")
         # write a trace separator at the end of each trace
         f.write("%s\n" % "--")

if __name__ == "__main__":
   main()
