class InvariantParser:
   '''The InvariantParser is used to parse a Daikon-generated invariants file into a dictionary 
   mapping program points (ppts) to their invariants.'''

   filepath = ""
   separator_str = ""

   def __init__(self, filepath):
      '''Return a InvariantParser object which is set to parse the file located at *filepath*.'''
      self.filepath = filepath
      self.set_separator("===========================================================================")

   def parse_invariants(self):
      '''Return a dictionary mapping ppts to sets of invariants parsed from the parser's initialized invariants file.'''
      rtn_dict = {}
      with open(self.filepath) as f:
         contents = f.read()
         for entry in contents.split(self.separator_str):
            entry = entry.strip().split("\n")
            rtn_dict[entry[0]] = entry[1:]
      return rtn_dict

   def set_separator(self, regex):
      '''Set the parser's separator regex. The separator regex is used to partition the invariants 
      file into a list of invariant records.'''
      self.separator_str = regex