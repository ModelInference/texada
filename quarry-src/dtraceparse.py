import collections, sys, re
from itertools import izip

class DTraceParser:
   '''
   The DTraceParser is used to parse a Daikon-generated dtrace file into a collection
      of event traces. More specifically, the parser returns a dictionary mapping each
      object occuring in the dtrace file with its sequence of method entries and exits.
   '''

   # Path of the dtrace file to be parsed.
   filepath = ""
   # String used to match entry separating lines in the dtrace file.
   separator_str = ""


   def __init__(self, filepath):
      '''
      Return a DTraceParser object which is set to parse the file located at *filepath*.
      '''
      self.filepath = filepath
      self.separator_str = "(\n){2,}"


   def parse_dtrace(self):
      '''
      Return a dictionary of event traces (one for each object appearing in the dtrace)
         parsed from the initialized dtrace file.
      '''
      rtn_dict = collections.defaultdict(list)
      records = self.get_records()
      for i, rec in enumerate(records):
         obj_id = ""
         # the object id corresponding to an event is held in the field 'this'
         if "this" in rec.data:
            obj_id = rec.data["this"]
         # note that constructor entries do not contain an object id, so if rec is 
         #  a constructor entry (i.e. method name matches class name) find the 
         #  object id from its corresponding exit point.
         elif rec.class_name.endswith(rec.method_name.rsplit('(')[0]):
            for rec_j in records[i:]:
               # the field 'this_invocation_nonce'
               #  is an integer value matching method entries to method exits, so 
               #  in order to find the exit point corresponding to a given constructor
               #  entry, we look for a matching 'this_invocation_nonce' value.
               if rec.data["this_invocation_nonce"] == rec_j.data["this_invocation_nonce"] and "this" in rec_j.data:
                  obj_id = rec_j.data["this"]
                  break
         else:
            # TODO: if rec is a non-constructor static method?
            continue
         # append to the list mapped under the current record's class name + object
         #  id, a subset of the record. More specifically, append a 4-tuple cosisting
         #  of the first four fields of the record (i.e. class_name, method_name,
         #  ppt_type, ppt_id), throwing away the data.
         rtn_dict[rec.class_name+obj_id].append(rec[:4])
      return rtn_dict


   def set_separator(self, regex):
      '''
      Set the parser's separator regex. The separator regex is used to partition the
         dtrace file into a list of ppt records.
      '''
      self.separator_str = regex


   def get_records(self):
      '''
      Returns a list of ppt records extracted from the initialized dtrace file.
      '''
      # a struct representing a ppt record.
      # see https://docs.python.org/2/library/collections.html#collections.namedtuple
      #  for documentation regarding namedtuple.
      PPTRecord = collections.namedtuple("PPTRecord", ["class_name", "method_name", "ppt_type", "ppt_id", "data"])
      rtn_list = []
      with open(self.filepath) as f:
         contents = f.read()
         records = re.split(self.separator_str, contents)
         regex = re.compile("(?P<class_name>^(\w+\.)*\w+)\.(?P<method_name>\w+\(.*?\)):::(?P<ppt_type>ENTER|EXIT)(?P<ppt_id>[0-9]*)")
         for record in records:
            record = record.split("\n")
            match = regex.match(record[0])
            # if did not match regex, was not a valid ppt record; ignore and continue
            #  iteration
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



