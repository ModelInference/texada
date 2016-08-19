#regexes
import re
#to parse input args (is this really necessary?
from argparse     import ArgumentParser


# keep all the observed scopes so that we don't generate
observed_scopes = set()



def main():
   # process options
   argparser = ArgumentParser()
   argparser.add_argument("file", help="path to the InvariantChecker-generated trace")
   args = argparser.parse_args()
   # processes dtrace, fills obj_ppt_name_to_cih and each class info holder. 
   process_trace(args.file)

   return
   


# Processes a trace to add possible scopes
def process_trace(filename):
    # a struct representing a ppt record.
    # see https://docs.python.org/2/library/collections.html#collections.namedtuple
    #  for documentation regarding namedtuple.
    with open(filename) as f:
      # gather all possible scope     
      for line in f: 
         quest_index = line.find("???")
         if (quest_index != -1):
            second_quest_index = line.find("???", quest_index + 3)
            observed_scopes.add(line[quest_index + 3: second_quest_index])
    with open(filename) as f:    
      period_index = filename.rfind(".")
      new_file_name = filename[:period_index] + "-processed" + filename[period_index:]  
      g = open(new_file_name, 'w')
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
            g.write(list_to_string(sublist))
            g.write("\n")

       else:
        g.write(line)        
      g.close()  
    return

# get all ordered sublists of to_sublist
def all_ordered_sublists(to_sublist):
   retlist = []
   for i in range(1, len(to_sublist) + 1):
      retlist += all_ordered_sublists_n(to_sublist, i)
   return retlist

# get all ordered sublists of to_sublist of length n
def all_ordered_sublists_n(to_sublist, n):
  length = len(to_sublist)
  if (n > length) or (n <= 0):
     return [[]]
  else:
    retlist = []
    # produce a list of positions to produce each sublist in order 
    iterlist = []
    for i in range(0,n):
      iterlist.append(i)
    # now go through all iterations. 
    while True:
        add_if_observed(retlist, to_sublist, iterlist)
        if not next_arrangement(iterlist, len(to_sublist)):
           break
  return retlist

# updates iterlist to be the next arrangement, returning false when 
# the last arrangement has been reached
def next_arrangement(iterlist, length):
  if (iterlist[0] == length - len(iterlist)):
     return False
  else:
     for i in reversed(range(0, len(iterlist))):
         j = len(iterlist) - i 
         if (iterlist[i] < length - j):
            inc_by_1(iterlist, i)
            return True
  return False

# increment position i by 1, set any later positions to be the 
# ones directly following  
def inc_by_1(iterlist, i):    
  iterlist[i] = iterlist[i] + 1
  for j in range (i + 1, len(iterlist)):
     iterlist[j] = iterlist[j-1] + 1   
   

# if the sublist designated by iterlist is in observed_scopes, add to retlist
def add_if_observed(retlist, to_sublist, iterlist):
   sublist = []
   sublist_string = ""
   for index in iterlist:
      sublist.append(to_sublist[index])
   sublist_string = list_to_string(sublist)
   if sublist_string in observed_scopes:
      retlist.append(sublist)

# turns list into a single string, appending elements with " "
def list_to_string(list):
  retstring = ""
  for i in range(0, len(list)):
    retstring += list[i].rstrip() + " "
  return retstring

if __name__ == "__main__":
   main()
