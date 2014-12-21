class DTraceParser:
   '''The DTraceParser is used to parse a Daikon-generated dtrace file into a collection of event traces.
   More specifically, the parser returns a dictionary mapping each object occuring in the dtrace file 
   with its sequence of method entries and exits.'''

   filepath = ""
   event_regex = ""
   separator_regex = ""
   is_slurping = True

   def __init__(self,filepath):
      '''Return a DTraceParser object which is set to parse the file located at *filepath*.'''
      # TODO

   def parse_dtrace():
      '''Return a dictionary of object event traces parsed from the parser's initialized dtrace file.'''
      # TODO

   def set_lazy():
      '''Set the parser to lazy read the dtrace file. If lazy reading is turned on, the parser will 
      read the file chunk by chunk. By default, the parser is set to non-lazy reading, and the dtrace
      file will be fully loaded to memory before being processed. It is recommended that lazy reading
      be turned if the dtrace file is very large.'''
      # TODO

   def is_lazy():
      '''Returns whether the parser is configured to lazy read.'''
      # TODO

   def set_separator(regex):
      '''Set the parser's separator regex. The separator regex is used to partition the dtrace file into a list of ppt records.'''
      # TODO

   def set_event_classes(regex):
      '''Set the parser's event regex. The event regex is used to determine the class of a given program point. By default, the class of a program point refers to the corresponding object.'''
      # TODO

   def extract_event(datablock):
      '''Returns an (event,event_class) pair from a ppt record.'''
      # TODO

   def get_list_of_records(filepath):
      '''Returns a list of ppt records partitioned from a dtrace file.'''
      # TODO