import re
import random 
# holds information for each sliced class
class RandomClassInfoHolder:

  # obj_class_record should be a list of each line in the object class declaration
  def __init__(self, obj_class_record, bucketsize):
    # first initiate all fields
    self.obj_ppt_name = obj_class_record[0]
    self.obj_ppt_decls = ""
    self.all_decls = "decl-version 2.0\nvar-comparability none\n"
    # list of vars in object
    self.req_vars = []
    # todo: change to something else
    self.bucket_size = bucketsize
    self.ppt_count = 0
    # list with all the program points
    self.all_ppts = []
    # dictionary of bucket number to the number of ppts in that bucket so far 
    self.buck_num_to_num_ppts = {}
    # now do some stuff
    
    omit_next = True
    # create the ppt_declaration
    for line in obj_class_record[1:]:
       # here, check if we're at the start of a variable declaration
       variableDeclStart = re.compile("^( *)variable (?P<var_name>.+)")
       match = variableDeclStart.match(line)
       # if we are, check if it's a field   
       if (match != None):
          # if it is a field, it starts with this, so we add it to the declaration. 
          if (match.group("var_name").startswith("this")):
              self.obj_ppt_decls += line + "\n"
              omit_next = False
              # also put this variable in req_vars
              self.req_vars.append(match.group("var_name"))
          # we will not include non-field variables. Sorry!
          else:
              omit_next = True
       else:
          # add the other parts of the declaration to the declaration
          if (not omit_next):
             self.obj_ppt_decls += line + "\n"

  # store the program point for now
  def store_program_point(self, ppt_record):
     self.all_ppts.append(ppt_record)
     self.ppt_count += 1

  # now make everything into nice buckets
  def bucket_all(self):
     # round up the number of buckets. Cute solution from 
     # http://stackoverflow.com/questions/2356501/how-do-you-round-up-a-number-in-python
     # The second part evaluates to true if there is a reminder, which will then add a 1. CUTE!
     number_of_buckets = (self.ppt_count / self.bucket_size) + (self.ppt_count % self.bucket_size > 0)
     # double check the math for debugging
     print "Round up " + str(self.ppt_count) + " divided by " + str(self.bucket_size) + " is " + str(number_of_buckets)
     # fill up the dictionary
     for i in range(number_of_buckets):
        self.buck_num_to_num_ppts[i] = 0
     # now go through each ppt
     for i in range(len(self.all_ppts)):
        self.add_program_point(i);
     

    
  # here we add a program point! 
  def add_program_point(self, i):
    ppt_record = self.all_ppts[i]
    name = ppt_record[0]
    if (ppt_record[1] == "this_invocation_nonce"):
      # here we remove the nonce
      ppt_record = ppt_record[3:]
    else:
      ppt_record = ppt_record[1:]
    # if there was just the invocation_nonce, not interested
    if (ppt_record == []):
      return
    # make tuples of (varname, value, the third number in dtraces)
    variable_val_list = zip(ppt_record[::3], ppt_record[1::3], ppt_record[2::3])
    req_vars_remaining = list(self.req_vars)
    dtrace_element = ""
    # go through the variables listed at this program point
    for var_tuple in variable_val_list:
       if var_tuple[0] in req_vars_remaining:
          req_vars_remaining.remove(var_tuple[0])
          dtrace_element += var_tuple[0] + "\n" + var_tuple[1] + "\n" + var_tuple[2] + "\n"
    # if we enter this if some of the fields were not observable. ignore the ppt in that case
    if (req_vars_remaining != []):
       return  
    # choose a random bucket number
    buck_num = random.choice(list(self.buck_num_to_num_ppts.keys()))
    # here's the new ppt name! 
    new_ppt_name = "Bucket" + str(buck_num) + ":::POINT"
    # incremement the number of ppts in the bucket
    self.buck_num_to_num_ppts[buck_num] = self.buck_num_to_num_ppts[buck_num] + 1
    num_ppts_in_bucket = self.buck_num_to_num_ppts[buck_num]
    # add the new ppt name to the dtrace element
    dtrace_element = new_ppt_name + "\n" + dtrace_element
    # replace the element in all_ppts with something printable
    self.all_ppts[i] = dtrace_element + "\n"
    # if this is the first item in the bucket, we need to add it as a new declaration
    if (num_ppts_in_bucket == 1):
       self.addPptDecl(new_ppt_name)
    # if we have added everything we can to the bucket, we remove it from the possible choices
    elif (num_ppts_in_bucket == self.bucket_size):
       del self.buck_num_to_num_ppts[buck_num]
       
    
    

  # add a ppt declaration for the new_ppt_name
  def addPptDecl(self, new_ppt_name):
    self.all_decls += "\nppt " + new_ppt_name + "\nppt-type point\n" + self.obj_ppt_decls

          
       
        
      
  

