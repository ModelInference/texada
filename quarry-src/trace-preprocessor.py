#regexes
import re
#to parse input args (is this really necessary?
from argparse     import ArgumentParser


#some global regexes


objPpt = re.compile("ppt (?P<class_name>(\w+\.)*\w+):::OBJECT")   
declPpt = re.compile("ppt (?P<class_name>(\w+\.)*\w+)\.(?P<method_name>\w+\(.*?\))?:::(?P<ppt_type>ENTER|EXIT)(?P<ppt_id>[0-9]*)")   
listedPpt = re.compile("(?P<class_name>^(\w+\.)*\w+)\.(?P<method_name>\w+\(.*?\)):::(?P<ppt_type>ENTER|EXIT)(?P<ppt_id>[0-9]*)")
parent = re.compile("^parent parent (?P<parent_name>(\w+\.)*\w+:::OBJECT)")
parentLine = re.compile("^\s+parent.*")
returnDecl = re.compile("\s*variable return")
returnVal = re.compile("return")
argsDecl = re.compile("\s*variable arg(?P<arg_name>.*)")
argsVal =re.compile("arg(?P<arg_name>.*)")
argsInLine = re.compile("(?P<preamble>.*) arg(?P<arg_name>.*)")
pptType = re.compile("ppt-type .*")
memo_map = {}

# this preprocessor adds method names to return value names.
# eventually it should do some bucketing as well.

def main():
   # process options
   argparser = ArgumentParser()
   argparser.add_argument("file", help="path to the InvariantChecker-generated trace")
   args = argparser.parse_args()
   
   # processes dtrace, fills obj_ppt_name_to_cih and each class info holder. 
   process_trace(args.file)

   return
   
def remove_parent_lines_from_list(the_list):
   return [value for value in the_list if parentLine.match(value) == None]


# Processes a trace to add possible scopes
def process_trace(filename):
   # a struct representing a ppt record.
   # see https://docs.python.org/2/library/collections.html#collections.namedtuple
   #  for documentation regarding namedtuple.
   with open(filename) as f:
      period_index = filename.rfind(".")
      new_file_name = filename[:period_index] + "-processed" + filename[period_index:]  
      g = open(new_file_name, 'w')    
      i = 0
      for line in f:   
         bang_index = line.find("!!!")
         if (bang_index != -1):
            new_line = line[bang_index+3:]
           # if not (new_line.rstrip() in memo_map):
           #     memo_map[new_line.rstrip()]=all_ordered_sublists(new_line.rstrip().split(' '))
           # for sublist in memo_map[new_line.rstrip()]:
            for sublist in all_ordered_sublists(new_line.rstrip().split(' ')):
              if (len(sublist) != 0):
                 g.write("!!!")
                 for i in range(0, len(sublist)):
                    g.write(sublist[i].rstrip() + " ")
                 g.write("\n")

         else:
            g.write(line)        
      g.close()  
   return

# get all ordered sublists of to_sublist
def all_ordered_sublists(to_sublist):
   if (len(to_sublist) == 0):
       return [[]]
   else:
     retlist = []
     sublist_iteration = all_ordered_sublists(to_sublist[1:])
     for sublist in sublist_iteration:
         retlist.append(sublist)
         retlist.append([to_sublist[0]] + sublist)
     return retlist
   

if __name__ == "__main__":
   main()
