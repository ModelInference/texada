#regexes
import re
#to parse input args (is this really necessary?
from argparse     import ArgumentParser
from RandomClassInfoHolder import RandomClassInfoHolder
#to call daikon with
import subprocess
import collections

#some global regexes

objPpt = re.compile("ppt (?P<class_name>(\w+\.)*\w+):::OBJECT")   
declPpt = re.compile("ppt (?P<class_name>(\w+\.)*\w+)\.(?P<method_name>\w+\(.*?\))?:::(?P<ppt_type>ENTER|EXIT)(?P<ppt_id>[0-9]*)")   
listedPpt = re.compile("(?P<class_name>^(\w+\.)*\w+)\.(?P<method_name>\w+\(.*?\)):::(?P<ppt_type>ENTER|EXIT)(?P<ppt_id>[0-9]*)")
parent = re.compile("^parent parent (?P<parent_name>(\w+\.)*\w+:::OBJECT)")
ppt_name_to_parent = {}
obj_ppt_name_to_cih = {}

def main():
   # process options
   argparser = ArgumentParser()
   argparser.add_argument("bucketsize", type=int, help="the bucket size")
   argparser.add_argument("dfile", help="path to the Daikon front end-generated dtrace file.")
   args = argparser.parse_args()
   
   # processes dtrace, fills obj_ppt_name_to_cih and each class info holder. 
   process_dtrace(args.dfile, args.bucketsize)
   cih_dict = obj_ppt_name_to_cih
   print cih_dict
   for key in cih_dict:
     name = key.split(":")[0]
     filename = name + "-" + str(args.bucketsize) + "-bucket-random.dtrace"
     f = open(filename, 'w')
     # get the actual class info holder, which contains all we want to write
     cih = cih_dict[key]
     f.write(cih.all_decls +"\n")
     for ppt in cih.all_ppts:
       f.write(ppt)
     f.close()

   return
   

# Processes a dtrace to do the bucketed version
def process_dtrace(filename, bucketsize):
   # a struct representing a ppt record.
   # see https://docs.python.org/2/library/collections.html#collections.namedtuple
   #  for documentation regarding namedtuple.
   with open(filename) as f:
      contents = f.read()
      records = re.split("(\n){2,}", contents)
      for record in records:
         split_record = record.split("\n")
         match_declPpt = declPpt.match(split_record[0])
         match_listedPpt = listedPpt.match(split_record[0])
         match_objPpt = objPpt.match(split_record[0])
         # if we are in a declaration
         if (match_declPpt != None):
            # if this is a ppt with no parent, ignore
            if (len(split_record) < 3):
               continue
            parent_match = parent.match(split_record[2])
            # the thing above should always exist?
            if (parent_match != None):
              ppt_name_to_parent[split_record[0][4:]] = parent_match.group("parent_name")
            else:
              continue
         elif (match_objPpt != None):
           obj_ppt_name_to_cih[split_record[0][4:]] = RandomClassInfoHolder(split_record, bucketsize)
         # if we are in the dtrace part
         elif (match_listedPpt != None):
            # this ignores static methods
            if split_record[0] in ppt_name_to_parent:
               cur_cih = obj_ppt_name_to_cih[ppt_name_to_parent[split_record[0]]] 
               cur_cih.store_program_point(split_record)
            
         else:
          continue
      for key in obj_ppt_name_to_cih:
         cih = obj_ppt_name_to_cih[key]
         cih.bucket_all()

   return

if __name__ == "__main__":
   main()
