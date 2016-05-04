#regexes
import re
#to parse input args (is this really necessary?
from argparse     import ArgumentParser


#some global regexes


objPpt = re.compile("ppt (?P<class_name>(\w+\.)*\w+):::OBJECT")   
declPpt = re.compile("ppt (?P<class_name>(\w*\.)*\w*)\.(?P<method_name>\w+\(.*?\))?:::(?P<ppt_type>ENTER|EXIT)(?P<ppt_id>[0-9]*)")   
listedPpt = re.compile("(?P<class_name>^(\w*\.)*\w*)\.(?P<method_name>\w+\(.*?\)):::(?P<ppt_type>ENTER|EXIT)(?P<ppt_id>[0-9]*)")
parent = re.compile("^parent parent (?P<parent_name>(\w+\.)*\w+:::OBJECT)")
parentLine = re.compile("^\s+parent.*")
variableDecl=re.compile("(?P<whitespace>\s*)variable (?P<var_name>.*)")
# this is a hack based on the fact I only ever see one arg in
# these functions
functionArgs=re.compile("(?P<whitespace>\s*)function-args (?P<var_name>.*)")
enclosingVar=re.compile("(?P<whitespace>\s*)enclosing-var (?P<var_name>.*)")
pptType = re.compile("(?P<whitespace>\s*)ppt-type .*")

# this preprocessor adds method names to return value names.
# eventually it should do some bucketing as well.

def main():
   # process options
   argparser = ArgumentParser()
   argparser.add_argument("dfile", help="path to the Daikon front end-generated dtrace file.")
   args = argparser.parse_args()
   
   # processes dtrace, fills obj_ppt_name_to_cih and each class info holder. 
   process_dtrace(args.dfile)

   return
   
def remove_parent_lines_from_list(the_list):
   return [value for value in the_list if parentLine.match(value) == None]

# renames according to...
# 1) starts with "this": add class_name_this...
# 2) contains class name already: do not rename, static var
# 3) otherwise: add class_name-method_name
# 4) if size() or size()-1, rename the insides
def rename(varname, method_name, class_name):
    sizeRegex = re.compile("size\((?P<var_name>.*)\)(?P<post>.*)")
    match_size = sizeRegex.match(varname)
    if (match_size != None):
       if (match_size.group("post")):
          return "size(" + rename(match_size.group("var_name"), method_name, class_name) + ")" + match_size.group("post")
       else:
          return "size(" + rename(match_size.group("var_name"), method_name, class_name) + ")"
    if (varname.startswith("this")):
       return class_name + "_" + varname
    if (varname.startswith(class_name)):
       return varname
    return class_name + "." + method_name + "_" + varname
    
# Processes a dtrace to do the bucketed version (in this case, strips non-field values)
def process_dtrace(filename):
   # a struct representing a ppt record.
   # see https://docs.python.org/2/library/collections.html#collections.namedtuple
   #  for documentation regarding namedtuple.
   with open(filename) as f:
      contents = f.read()
      records = re.split("(\n){2,}", contents)
      all_recs = []
      for record in records:
         split_record = record.split("\n")
         match_declPpt = declPpt.match(split_record[0])
         match_objPpt = objPpt.match(split_record[0])
         match_listedPpt = listedPpt.match(split_record[0])         
         # if we are in a declaration
         if (match_declPpt != None):
            class_name = match_declPpt.group("class_name")
            method_name =  match_declPpt.group("method_name")[:match_declPpt.group("method_name").find("(")]
            # change name
            split_record[0] = "ppt " + match_declPpt.group("class_name")+ "." + match_declPpt.group("method_name") + match_declPpt.group("ppt_type")
            if (match_declPpt.group("ppt_type") == "EXIT"):
               split_record[0] += match_declPpt.group("ppt_id")
            split_record[0] += ":::POINT"
            # loop to remove parent declaration
            for i in range(1, len(split_record)):
               match_parent = parent.match(split_record[i])
               if (match_parent != None):
                  del split_record[i]
                  break;
            # loop to add name to return, args, change ppt-type
            for i in range(1, len(split_record)):
               match_ppttype = pptType.match(split_record[i])
               match_var = variableDecl.match(split_record[i])
               match_enclosing = enclosingVar.match(split_record[i])
               match_fxnargs = functionArgs.match(split_record[i])
               # rename returns here
               if (match_ppttype != None):
                  split_record[i] = "ppt-type point" 
                  if (match_ppttype.group("whitespace")):
                     split_record[i] =  match_ppttype.group("whitespace")+split_record[i]
               elif (match_var != None):
                  if (match_var.group("whitespace")): 
                     split_record[i] = match_var.group("whitespace") + "variable " + rename(match_var.group("var_name"), method_name, class_name) 
                  else: 
                     split_record[i] = "variable " + rename(match_var.group("var_name"), method_name, class_name)
                  
               elif (match_enclosing != None):
                  split_record[i] = match_enclosing.group("whitespace") + "enclosing-var " + rename(match_enclosing.group("var_name"), method_name, class_name)
               elif (match_fxnargs != None):
                  split_record[i] = match_fxnargs.group("whitespace") + "function-args " + rename(match_fxnargs.group("var_name"), method_name, class_name)
            split_record = remove_parent_lines_from_list(split_record) 
         #ignore objects
         elif (match_objPpt != None):
             continue
         # if we are in the dtrace part
         elif (match_listedPpt != None):
            class_name = match_listedPpt.group("class_name")
            method_name =  match_listedPpt.group("method_name")[:match_listedPpt.group("method_name").find("(")]
            split_record[0] = match_listedPpt.group("class_name")+ "." + match_listedPpt.group("method_name") + match_listedPpt.group("ppt_type")
            if (match_listedPpt.group("ppt_type") == "EXIT"):
               split_record[0] += match_listedPpt.group("ppt_id")
            split_record[0] += ":::POINT"
            for i in range(1, len(split_record)):
               if (split_record[i] == "this_invocation_nonce"):
                   del split_record[i]
                   del split_record[i]
                   break;
            for i in range(1, len(split_record)):
               if (i % 3 == 1):
                  split_record[i] = rename(split_record[i], method_name, class_name) 
         
         all_recs.append(split_record) 
      period_index = filename.rfind(".")
      new_file_name = filename[:period_index] + "-processed" + filename[period_index:]      
      g = open(new_file_name, 'w')
      for split_record in all_recs:
         for i in range(0, len(split_record)):
            g.write(split_record[i] + "\n")
      g.close()            

   return

if __name__ == "__main__":
   main()
