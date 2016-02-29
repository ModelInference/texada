import sys

class InvariantParser:
   '''
   The InvariantParser is used to parse a Daikon-generated invariants file into a
      dictionary mapping program points (ppts) to their invariants.
   '''

   # Path of the invariants file to be parsed.
   filepath = ""
   # String used to match entry separating lines in the invariants file.
   separator_str = ""


   def __init__(self, filepath):
      '''
      Return a InvariantParser object which is set to parse the file located at
         *filepath*.
      '''
      self.filepath = filepath
      self.separator_str = "==========================================================================="


   def parse_invariants(self):
      '''
      Return a dictionary mapping ppts (strings) to sets of invariants (lists of
         strings) parsed from the initialized invariants file.
      '''
      rtn_dict = {}
      with open(self.filepath) as f:
         contents = f.read()
         for entry in contents.split(self.separator_str):
            entry = entry.strip().split("\n")
            rtn_dict[entry[0]] = entry[1:]
      return rtn_dict