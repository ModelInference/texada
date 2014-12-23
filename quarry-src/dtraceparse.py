import collections, sys, re
from itertools import izip

class DTraceParser:
   '''The DTraceParser is used to parse a Daikon-generated dtrace file into a collection of event traces.
   More specifically, the parser returns a dictionary mapping each object occuring in the dtrace file 
   with its sequence of method entries and exits.'''

   filepath = ""
   separator_str = ""

   def __init__(self, filepath):
      '''Return a DTraceParser object which is set to parse the file located at *filepath*.'''
      self.filepath = filepath
      self.set_separator("(\n){2,}")

   def parse_dtrace(self):
      '''Return a dictionary of event traces (one for each object appearing in the dtrace) parsed from the initialized dtrace file.'''
      rtn_dict = collections.defaultdict(list)
      records = self.get_records()
      for i, rec in enumerate(records):
         obj_id = ""
         if "this" in rec.data:
            obj_id = rec.data["this"]
         # if rec is a constructor entry (i.e. method name matches class name) find the object id from its corresponding exit point
         # elif rec.class_name.endswith(rec.method_name.rsplit('(')[0]):
         elif rec.method_name[0].isupper():
            for rec_j in records[i:]:
               if rec.data["this_invocation_nonce"] == rec_j.data["this_invocation_nonce"] and "this" in rec_j.data:
                  obj_id = rec_j.data["this"]
                  break
         else:
            # TODO: if rec is a non-constructor static method?
            continue
         rtn_dict[rec.class_name+obj_id].append(rec[:4])
      return rtn_dict

   def set_separator(self, regex):
      '''Set the parser's separator regex. The separator regex is used to partition the dtrace file into a list of ppt records.'''
      self.separator_str = regex

   def get_records(self):
      '''Returns a list of ppt records extracted from the initialized dtrace file'''
      # a struct representing a ppt record
      PPTRecord = collections.namedtuple("PPTRecord", ["class_name", "method_name", "ppt_type", "ppt_id", "data"])
      rtn_list = []
      with open(self.filepath) as f:
         contents = f.read()
         records = re.split(self.separator_str, contents)
         regex = re.compile("(?P<class_name>^(\w+\.)*\w+)\.(?P<method_name>\w+\(.*?\)):::(?P<ppt_type>ENTER|EXIT)(?P<ppt_id>[0-9]*)")
         for record in records:
            record = record.split("\n")
            match = regex.match(record[0])
            # if did not match regex, was not a valid ppt record; ignore and continue iteration
            if match == None:
               continue
            else:
               i = iter(record[1:])
               data = dict(izip(i, i))
               rtn_list.append(PPTRecord(match.group("class_name"),
                                         match.group("method_name"),
                                         match.group("ppt_type"),
                                         match.group("ppt_id"),
                                         data))
      return rtn_list



