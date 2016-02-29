
'''
Returns a data-temporal log generated from combining a pair of Daikon-generated
   dtrace and invariants files.
Usage: quarry.py [-h] [-o] [-c] [-l] [-e EXTRACT] <dtracefile> <invarfile> <outputfile>
In the case that -l option is given, <outputfile> is used as a prefix for the object files.
'''

import re
from argparse     import ArgumentParser
from invarparse   import InvariantParser
from dtraceparse  import DTraceParser

def main():
   '''
   Method which orchestrates everything.
   '''
   # process options
   argparser = ArgumentParser()
   argparser.add_argument("dfile", help="path to the Daikon-generated dtrace file.")
   argparser.add_argument("ifile", help="path to the Daikon-generated invariants file.")
   argparser.add_argument("ofile", help="path to the output file.")
   argparser.add_argument("--incl_obj",         help="include object invariances",  action="store_true")
   argparser.add_argument("--incl_cls",         help="include class invariances",   action="store_true")
   argparser.add_argument("-e", "--extract",    help="specify a specific class to extract [by default: extract all]")
   argparser.add_argument("-l", "--split_to_logs", help="splits objects into seperate logs instead of traces", action="store_true")
   argparser.add_argument("-p", "--pp_names", help="includes program point names preceded by !!! in the trace", action="store_true")
   args = argparser.parse_args()

   # parse dtrace file
   dtraceparser = DTraceParser(args.dfile)
   if args.extract:
      dtraceparser.set_target_class(args.extract)
   traces = dtraceparser.parse_dtrace()

   # parse invariants file
   invariantparser = InvariantParser(args.ifile)
   invars = invariantparser.parse_invariants()

   # generate data-temp log
   if args.split_to_logs:
     generate_data_temp_logs(traces, invars, args.ofile, args.incl_cls, args.incl_obj, args.pp_names)
   else:
     generate_data_temp_log(traces, invars, args.ofile, args.incl_cls, args.incl_obj, args.pp_names)
   
   # Print a description of how to process the generated data-temp log using
   # Texada.
   texada_usage = 'How to process Quarry output using Texada:\nIn order to mine temporal properties from the outputed data-temp log, run the file with Texada using it\'s mult-prop option.\nExample usage: ./texada -l -f \'G(x -> FXy)\' --log-file ' + args.ofile
   print("%s\n" % texada_usage)


def generate_data_temp_log(traces, invars, filepath, incl_cls, incl_obj, pp_names):
   '''
   Prints a data-temporal log into the file located at *filepath*, generated from
      combining *dtset* and *invars*.
   '''
   with open(filepath, 'w') as f:
      for obj, trace in traces.iteritems():
         write_trace_to_f(f, obj, trace, incl_cls, incl_obj, invars, pp_names)


def generate_data_temp_logs(traces, invars, filepath, incl_cls, incl_obj, pp_names):
   '''
   Prints a data-temporal log into the file located at filepathprefix[classname]filepathextension,
     for each object generated from combining *dtset* and *invars*.
   '''
   lastdot = filepath.rfind(".")
   extension = ".txt"
   if (lastdot != -1):
     extension = filepath[lastdot:]
     filestart = filepath[:lastdot]
   else:
     filestart = filepath
   for obj, trace in traces.iteritems():
      with open(filestart +  str(obj).strip(".") + extension , 'w') as f:
         write_trace_to_f(f, obj, trace, incl_cls, incl_obj, invars, pp_names)

def write_trace_to_f(f, obj, trace, incl_cls, incl_obj, invars, pp_names):
   for ppt in trace:
      ppt_name = ppt[0]+"."+ppt[1]+":::"+ppt[2]+ppt[3]
      if (pp_names):
         f.write("!!!" + ppt_name + "\n")
      invarlist = []
      if (ppt_name in invars):
         invarlist = invars[ppt_name]
      # if the ppt is an exit point, append the invariants of the
      # corresponding aggregate exit.
      # e.g. given a ppt method()::EXIT42, its invariants consists of
      # the union of[method()::EXIT42] and emap[method()::EXIT].
      if ppt[2] == "EXIT":
         agg_ppt_name = ppt[0]+"."+ppt[1]+":::"+ppt[2]
         invarlist = invarlist +invars[agg_ppt_name]
      # if configured to include class invariants, find and append 
      # onto invarlist the class invariants corresponding to this ppt.
      if incl_cls:
         cls_name = ppt[0]+":::"+"CLASS"
         if cls_name in invars:
            invarlist = invarlist + invars[cls_name]
      # if configured to include object invariants, find and append onto
      # invarlist the object invariants corresponding to this ppt.
      if incl_obj:
         obj_name = ppt[0]+":::"+"OBJECT"
         if obj_name in invars:
             invarlist = invarlist + invars[obj_name]
      # loop through the retrieved invariants and write them line by line into 
      # the ofile, followed by a event separator.
      for invar in invarlist:
               f.write("%s\n" % invar)
      f.write("%s\n" % "..")
   # write a trace separator at the end of each trace
   f.write("%s\n" % "--")

   

if __name__ == "__main__":
   main()
