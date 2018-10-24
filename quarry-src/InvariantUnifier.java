import java.io.*;
import java.util.*;
import daikon.PptMap;
import daikon.FileIO;
import daikon.PptTopLevel;
import daikon.PptSlice;
import daikon.PptSlice0;
import daikon.PptSlice1;
import daikon.PptSlice2;
import daikon.PptSlice3;
import daikon.PptName;
import daikon.inv.Invariant;
import daikon.inv.OutputFormat;

import daikon.*;
import daikon.tools.InvariantChecker.InvariantCheckProcessor;
import daikon.split.PptSplitter;
import daikon.inv.*;
import daikon.inv.filter.InvariantFilters;
//import plume.*;


import java.util.*;
import java.io.*;
import gnu.getopt.*;
import java.util.logging.Logger;
import java.util.logging.Level;



public class InvariantUnifier {
   private PptMap map;
   private Map<List<String>, VarInfo[]> varInfosPerName;

   public InvariantUnifier(PptMap map) {
     this.map = map;
     varInfosPerName = new HashMap<List<String>, VarInfo[]>();
   }

   // Add all the invs stored in invsPerSliceVars to the given slice
   private void addAllInvsToSlice(PptSlice slice, Map<List<String>, Map<String, List<Invariant>>> invsPerSliceVars){
      // get map of all invariants to add
      Map<String, List<Invariant>> pptsToInvariants = invsPerSliceVars.get(varInfosToStringList(slice));
      for (String pptName : pptsToInvariants.keySet()){
         // for any program point which is not the current one, add the ppts to the slice
         if (pptName != slice.parent.name) {
            addInvsToSlice(slice, pptsToInvariants.get(pptName));
         } 
      }
   }
   // modifies the pptMap object to have one set of invariants across all program points
   public void unifyInvariants() {
      Map<List<String>, Map<String, List<Invariant>>> invsPerSliceVars = getInvariantsBySliceVariables();
      // second loop thrhough map to assign all the invariants we want
      for (PptTopLevel ppt : map.all_ppts()){
          Iterator<PptSlice> sliceIt = ppt.views_iterator();
          Set<List<String>> missingSlices = new HashSet<List<String>>(varInfosPerName.keySet());
          // now loop through the slices
          while (sliceIt.hasNext()) {
              PptSlice slice = sliceIt.next(); 
              addAllInvsToSlice(slice,  invsPerSliceVars);
              missingSlices.remove(varInfosToStringList(slice));

          }
          // remove any slices that don't apply
          removeInvalidSlices(missingSlices, ppt);
          // if we are missing some slices at the ppt, add them 
          if (!missingSlices.isEmpty()){             
             for (List<String> varinfoNames : missingSlices){
                //  System.out.print("Adding slice: ");
                 // for (String var: varinfoNames){   
                 //     System.out.print(var + " ");
                 // }
                //  System.out.println("\n to: " + ppt.name());
                 // printPpt(ppt);
                 VarInfo[] vinfos = varInfosPerName.get(varinfoNames);
                 if (vinfos != null){
                   // create new varinfos with the correct parent
                   VarInfo[] newVinfos = new VarInfo[vinfos.length];
                   for (int i = 0 ; i <vinfos.length; i++){
                       newVinfos[i] = ppt.find_var_by_name(vinfos[i].name());//new VarInfo(vinfos[i]);
                      // newVinfos[i].ppt = ppt;
                   }
                   PptSlice newSlice;
                   switch (newVinfos.length){
                       case 1:
                          newSlice = new PptSlice1(ppt, newVinfos);
                          addAllInvsToSlice(newSlice, invsPerSliceVars);
                          newSlice.parent.addSlice(newSlice);
                          break;
                       case 2:
                          newSlice = new PptSlice2(ppt, newVinfos);
                          addAllInvsToSlice(newSlice, invsPerSliceVars);
                          newSlice.parent.addSlice(newSlice);
                          break;
                       case 3:
                          newSlice = new PptSlice3(ppt, newVinfos);
                          addAllInvsToSlice(newSlice, invsPerSliceVars);
                          newSlice.parent.addSlice(newSlice);
                          break;
                       default:
                          System.err.println("This is a problem");
                   }

                 } 
                 // this should not happen. 
                 else {
                   System.err.println("Woops, this is a problem.");
                 }
             }
          }

      }
   }

   // Removes any items from slices
   private void removeInvalidSlices (Set<List<String>> slices, PptTopLevel ppt){
      Iterator<List<String>> it = slices.iterator();
      Set<String> pptVars = new HashSet<String>();
      for (VarInfo vi: ppt.var_infos){
          pptVars.add(vi.str_name());
      }
      while (it.hasNext()){
          List<String> currentVars = it.next();
          for (String name: currentVars){
              if (!pptVars.contains(name)){
               //   System.out.print("Removing slice: ");
                  //for (String var: currentVars){   
                 //     System.out.print(var + " ");
                //  }
                //  System.out.println("\n from: ");
                //  printPpt(ppt);
                  it.remove();
                  break;
              } 
          }
      }
   }

   // Gets, for each set of slice variables (these should all be fields with name this.),
   // a map for the invariants at each program point that are contained in that slice    
   public Map<List<String>, Map<String, List<Invariant>>> getInvariantsBySliceVariables(){
      Map<List<String>, Map<String, List<Invariant>>> rtnMap = new HashMap<List<String>, Map<String, List<Invariant>>>();
      // loop through map
      for (PptTopLevel ppt: map.all_ppts()){
          // for each slice, add all the invs to rtnMap
          Iterator<PptSlice> sliceIt = ppt.views_iterator();
          while (sliceIt.hasNext()) {
              PptSlice slice = sliceIt.next(); 
              Map<String, List<Invariant>> sliceVarVal = rtnMap.get(varInfosToStringList(slice));
              if (sliceVarVal == null){
                 // if this set of variables is not yet in the return map, we create the first
                 // entry by mapping the list of variables to a map containing the current ppt
                 // pointing to the invs in this slice. 
                 Map<String, List<Invariant>> sliceMap = new HashMap<String, List<Invariant>>();
                 sliceMap.put(slice.parent.name, slice.invs);
                 rtnMap.put(varInfosToStringList(slice), sliceMap);
                 varInfosPerName.put(varInfosToStringList(slice), slice.var_infos);
              }
              else {
                // otherwise, augment the entry
                sliceVarVal.put(slice.parent.name, slice.invs);
              }
          }
      }
      // print for debugging. 
      /*for (List<String> varinfos : rtnMap.keySet()){
          for (String str : varinfos){
              System.out.print(str + ", ");
          }
          System.out.println();
          for (String ppt: rtnMap.get(varinfos).keySet()){
              System.out.println("    " + ppt + ":");
              for (Invariant inv:  rtnMap.get(varinfos).get(ppt)){
                  System.out.println("          " + inv.format()); 
              }
          }
      }*/
      return rtnMap;
   }


   // returns true if inv is in listInvs, by isSameInvariant
   public boolean inList(Invariant inv, List<Invariant> listInvs){
     for (Invariant listInv : listInvs) {
         if (inv.isSameInvariant(listInv)){
            return true;
         }
     }
     return false;
   }


   // add invariants from invs into the slice, without repetition
   private void addInvsToSlice(PptSlice slice, List<Invariant> invs){
       List<Invariant> sliceInvs = slice.invs;
       // for each inv in the list
       for (Invariant inv: invs){
           // ad the inv if it is not in the slice's invariant
           if (!inList(inv, sliceInvs)){
              inv.ppt = slice;
              slice.addInvariant(inv);
           }  
       }
   }

   // returns the names of this slice's varinfos as a list. 
   private List<String> varInfosToStringList(PptSlice slice){
      VarInfo[] varinfos = slice.var_infos;
      List<String> retList = new ArrayList<String>();
      for (int i = 0; i<varinfos.length; i++) {
          retList.add(varinfos[i].str_name());
      }
      return retList;
   }

   private void printPpt(PptTopLevel ppt){
    System.out.println("Printing ppt: " + ppt.name);
    for (PptSlice slice : ppt.views_iterable()){
         System.out.print("     "); 
         printSlice(slice);
    }
   }
   private void printSlice(PptSlice slice){
     System.out.println("Printing slice: " + slice.name());
     for (Invariant inv: slice.invs){
         System.out.println("           " + inv.format()); 
     }
   }

   
   public PptMap getPptMap(){
     return this.map;
   }
 /*
   // returns false if the invariants are not identical over all slices.
   public boolean sanityCheck(){
   for (PptTopLevel ppt : map.all_ppts()){
          Iterator<PptSlice> sliceIt = ppt.views_iterator();
          // now loop through the slices
          while (sliceIt.hasNext()) {
              PptSlice slice = sliceIt.next(); 
              for (Invariant inv: slice.invs){
                    for (PptTopLevel ppt2 : map.all_ppts()){
                       boolean containedInPpt = false;
                       Iterator<PptSlice> sliceIt2 = ppt2.views_iterator();
                       // now loop through the slices
                       while (sliceIt2.hasNext()) {
                          PptSlice slice2 = sliceIt2.next(); 
                          for (Invariant inv2: slice2.invs){
                              boolean isSameAs = inv.isSameInvariant(inv2);
                              if (isSameAs){
                                 containedInPpt = true;
                                 break;
                              }                      
                          }
                          if (containedInPpt) break;
                       }
                    
                      if (!containedInPpt) return false;//assert containedInPpt;
                   }
             
              }
          }

     }
     return true;

   }
*/
   public static void main(String [] args) {
     if (args.length != 1){
        System.out.println("Error: you must provide a .inv or .inv.gz file to the command line, and nothing else.");
     }
     
     File f= new File(args[0]);

     try {
      PptMap map = FileIO.read_serialized_pptmap(f,true);
      InvariantUnifier unifier = new InvariantUnifier(map);
      unifier.unifyInvariants();
      // uncomment line below to test if this actually unified invariants. 
      //if (!unifier.sanityCheck()) {System.out.println("This is bad!!"); return;}
     
      int invIndex = args[0].indexOf(".inv");
      // TODO: should this just be a .inv file? or just a .inv.gz file?
      File g= new File(args[0].substring(0,invIndex) + "-unified" + args[0].substring(invIndex));
      FileIO.write_serialized_pptmap(unifier.getPptMap(), g);
     } catch (IOException e) {
       e.printStackTrace();
     }
   }
}


