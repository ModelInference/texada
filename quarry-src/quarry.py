import sys, re
from argparse import ArgumentParser
#from invarparse import InvariantParser
#from dtraceparse import DTraceParser

def main():
   ''' Returns a data-temporal log generated from combining a pair of Daikon-generated dtrace and invariants files.
   Usage: quarry.py [-h] [-o] [-c] [-l] [-e EXTRACT] <dtracefile> <invarfile> <outputfile>'''

   # process options
   argparser = ArgumentParser()
   argparser.add_argument("dfile", help="path to the Daikon-generated dtrace file.")
   argparser.add_argument("ifile", help="path to the Daikon-generated invariants file.")
   argparser.add_argument("ofile", help="path to the output file.")
   argparser.add_argument("--incl_obj",         help="include object invariances",  action="store_true")
   argparser.add_argument("--incl_cls",         help="include class invariances",   action="store_true")
   argparser.add_argument("-l", "--lazy_read",  help="lazy read dtrace file [recommended if file is too big]", action="store_true")
   argparser.add_argument("-e", "--extract",    help="specify a specific class to extract [by default: extract all]")
   args = argparser.parse_args()

   # parse dtrace file
   dtraceparser = DTraceParser(args.dfile)
   if args.lazy_read:
      dtraceparser.set_lazy()
   if args.extract:
      # TODO: configure dtraceparser to extract only the specified class
   traces = parse_dtrace()

   # parse invariants file
   invariantparser = InvariantParser(args.ifile)
   invars = parse_invariants()

   # generate data-temp log
   generate_data_temp_log(traces, invars, ofile, args.incl_cls, args.incl_obj)

   # Print a description of how to process the generated data-temp log using Texada.
   texada_usage = '\nHow to process Quarry output using Texada:\nIn order to mine temporal properties from the outputed data-temp log, run the file with Texada using it\'s mult-prop option.\nExample usage: ./texada -l -f \'G(x -> FXy)\' --log-file path/to/data-temp-log'
   print("%s\n" % texada_usage)



def generate_data_temp_log(traces, invars, filepath, incl_cls, incl_obj):
   '''Prints a data-temporal log into the file located at *filepath*, generated from combining *dtset* and *invars*.'''
   # TODO

if __name__ == "__main__":
   main()