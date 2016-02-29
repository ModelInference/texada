import re
listedPpt = re.compile("(?P<class_name>^(\w+\.)*\w+)\.(?P<method_name>\w+\(.*?\)):::(?P<ppt_type>ENTER|EXIT)(?P<ppt_id>[0-9]*)")


# holds information for each sliced class
class DefaultClassInfoHolder:

  # obj_class_record should be a list of each line in the object class declaration
  def __init__(self, obj_class_record):
    # first initiate all fields
    self.obj_ppt_name = obj_class_record[0]
    self.obj_ppt_decls = ""
    self.all_decls = "decl-version 2.0\nvar-comparability none\n"
    # list of vars in object
    self.req_vars = []
    # list with all the program points
    self.all_ppts = []
    # list of all ppt names
    self.all_ppt_names = []
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

  # here we add a program point! 
  def add_program_point(self, ppt_record):
    name = ppt_record[0]
    if (ppt_record[1] == "this_invocation_nonce"):
      # here we remove the nonce
      ppt_record = ppt_record[3:]
    else:
      ppt_record = ppt_record[1:]
    # if there was just the invocation_nonce, not interested
    if (ppt_record == []):
      print "hear?"
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
      # else: 
         # print var_tuple[0] + " is not in " + str(req_vars_remaining)
    # if we enter this if, some of the fields were not observable
    if (req_vars_remaining != []):
       print "Pfooof"
       return  
    # add declaration if necessary in the next part
    match = listedPpt.match(name)
    new_ppt_name = ""
    # this should not happen. 
    if (match == None):
        print "wups"
        return
    else:
      new_ppt_name = match.group("class_name") + "."+ match.group("ppt_type") + match.group("ppt_id") +"."+ match.group("method_name") + ":::POINT"
    if not new_ppt_name in self.all_ppt_names:
       self.all_ppt_names.append(new_ppt_name)
       self.addPptDecl(new_ppt_name)
    dtrace_element = new_ppt_name + "\n" + dtrace_element
    self.all_ppts.append(dtrace_element + "\n")
    
    

  # add a ppt declaration for the new_ppt_name
  def addPptDecl(self, new_ppt_name):
    self.all_decls += "\nppt " + new_ppt_name + "\nppt-type point\n" + self.obj_ppt_decls

          
       
        
      
  

