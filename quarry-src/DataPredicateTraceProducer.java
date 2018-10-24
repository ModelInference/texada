import daikon.*;
import daikon.chicory.Runtime.TerminationMessage;
import daikon.split.PptSplitter;
import daikon.inv.*;
import daikon.inv.binary.BinaryInvariant;
import daikon.inv.filter.InvariantFilters;
import daikon.inv.ternary.TernaryInvariant;
import daikon.inv.unary.UnaryInvariant;
import plume.*;

import java.util.*;
import java.io.*;
import gnu.getopt.*;
import java.util.logging.Logger;
import java.util.logging.Level;

/*>>>
import org.checkerframework.checker.nullness.qual.*;
*/

/**
 * InvariantChecker reads an invariant file and trace file.  It prints errors
 * for any invariants that are violated by the trace file.
 */
public class DataPredicateTraceProducer {
  private DataPredicateTraceProducer() { throw new Error("do not instantiate"); }

  public static final Logger debug
    = Logger.getLogger ("daikon.tools.InvariantChecker");

  public static final Logger debug_detail
    = Logger.getLogger ("daikon.tools.InvariantCheckerDetail");


  private static final String output_SWITCH = "output";
  private static final String dir_SWITCH = "dir";
  private static final String conf_SWITCH = "conf";
  private static final String filter_SWITCH = "filter";
  private static final String verbose_SWITCH = "verbose";

  private static String usage =
    UtilMDE.joinLines(
      "Usage: java DataPredicateTraceProducer [OPTION]... <inv_file> "
        + "<dtrace_file>",
      "  -h, --" + Daikon.help_SWITCH,
      "      Display this usage message",
      "  --" + output_SWITCH + " output file",
      "  --" + conf_SWITCH,
      "      Checks only invariants that are above the default confidence level",
      "  --" + filter_SWITCH,
      "      Checks only invariants that are not filtered by the default filters",
      "  --" + dir_SWITCH + " directory with invariant and dtrace files",
      "      We output how many invariants failed for each invariant file. We check for failure against any sample in any dtrace file.",
      "  --" + verbose_SWITCH + " print all failing samples",
      "  --" + Daikon.config_option_SWITCH + " config_var=val",
      "      Sets the specified configuration variable.  ",
      "  --" + Daikon.debugAll_SWITCH,
      "      Turns on all debug flags (voluminous output)",
      "  --" + Daikon.debug_SWITCH + " logger",
      "      Turns on the specified debug logger",
      "  --" + Daikon.track_SWITCH + " class<var1,var2,var3>@ppt",
      "      Print debug info on the specified invariant class, vars, and ppt"
      );

  public static List<String> dtrace_files = new ArrayList<String>();
  static PrintStream output_stream = System.out;
  static int error_cnt = 0;
  static int sample_cnt = 0;

  static /*@Nullable*/ File dir_file; //Yoav added
  static boolean doFilter;
  static boolean doConf;
  static boolean quiet = true;
  static HashSet<Invariant> failedInvariants = new HashSet<Invariant>(); //Yoav added
  static HashSet<Invariant> testedInvariants = new HashSet<Invariant>(); //Yoav added
  static HashSet<Invariant> activeInvariants = new HashSet<Invariant>(); //Yoav added
  static LinkedHashSet<String> outputComma = new LinkedHashSet<String>(); //Yoav added

 // static HashMap<Object, HashMap<Integer, HashSet<String>>> quarryPrintMap= new HashMap<Object, HashMap<Integer, HashSet<String>>>(); //Caroline added
  static TreeMap<Integer, HashSet<String>> quarryPrintMap = new TreeMap<Integer, HashSet<String>>();
  static HashSet<String> worthPrintingSomewhere = new HashSet<String>();
  static boolean augmentPpts = false;
  static boolean augmentEquals = false;
  
  public static void main(String[] args)
    throws FileNotFoundException, StreamCorruptedException,
           OptionalDataException, IOException, ClassNotFoundException {
    try {
      if (args.length==0) {
          throw new TerminationMessage(usage);
      }
      mainHelper(args);
    } catch (TerminationMessage e) {
      System.err.println(e.getMessage());
      System.exit(1);
    }
    // Any exception other than TerminationMessage gets propagated.
    // This simplifies debugging by showing the stack trace.
  }

  /**
   * This does the work of main, but it never calls System.exit, so it
   * is appropriate to be called progrmmatically.
   * Termination of the program with a message to the user is indicated by
   * throwing TerminationMessage.
   * @see #main(String[])
   * @see daikon.chicory.Runtime.TerminationMessage
   **/
  public static void mainHelper(final String[] args)
    throws FileNotFoundException, StreamCorruptedException,
           OptionalDataException, IOException, ClassNotFoundException {
    daikon.LogHelper.setupLogs(daikon.LogHelper.INFO);

    LongOpt[] longopts = new LongOpt[] {
      new LongOpt(Daikon.config_option_SWITCH, LongOpt.REQUIRED_ARGUMENT, null, 0),
      new LongOpt(output_SWITCH, LongOpt.REQUIRED_ARGUMENT, null, 0),
      new LongOpt(dir_SWITCH, LongOpt.REQUIRED_ARGUMENT, null, 0),
      new LongOpt(conf_SWITCH, LongOpt.NO_ARGUMENT, null, 0),
      new LongOpt(filter_SWITCH, LongOpt.NO_ARGUMENT, null, 0),
      new LongOpt(verbose_SWITCH, LongOpt.NO_ARGUMENT, null, 0),
      new LongOpt(Daikon.debugAll_SWITCH, LongOpt.NO_ARGUMENT, null, 0),
      new LongOpt(Daikon.debug_SWITCH, LongOpt.REQUIRED_ARGUMENT, null, 0),
      new LongOpt(Daikon.ppt_regexp_SWITCH, LongOpt.REQUIRED_ARGUMENT, null, 0),
      new LongOpt(Daikon.track_SWITCH, LongOpt.REQUIRED_ARGUMENT, null, 0),
      new LongOpt("augment-ppts", LongOpt.NO_ARGUMENT, null, 0),
      new LongOpt("augment-equals", LongOpt.NO_ARGUMENT, null, 0)
    };
    Getopt g = new Getopt("DataPredicateTraceProducer", args, "h", longopts);
    int c;
    while ((c = g.getopt()) != -1) {
      switch(c) {
      case 0:
        // got a long option
        String option_name = longopts[g.getLongind()].getName();
        if (Daikon.help_SWITCH.equals(option_name)) {
          System.out.println(usage);
          throw new TerminationMessage();
        } else if (conf_SWITCH.equals (option_name)) {
          doConf = true;
        } else if (filter_SWITCH.equals (option_name)) {
          doFilter = true;
        } else if (verbose_SWITCH.equals (option_name)) {
          quiet = false;
        } else if (option_name.equals("augment-ppts")){
          augmentPpts = true;
        } else if (option_name.equals("augment-equals")){
          augmentEquals = true;
        } else if (dir_SWITCH.equals (option_name)) {
          dir_file = new File (Daikon.getOptarg(g));
          if (!dir_file.exists() || !dir_file.isDirectory())
             throw new TerminationMessage ("Error reading the directory "+dir_file);

        } else if (output_SWITCH.equals (option_name)) {
          File output_file = new File (Daikon.getOptarg(g));
          output_stream = new PrintStream (new FileOutputStream (output_file));
        } else if (Daikon.config_option_SWITCH.equals(option_name)) {
          String item = Daikon.getOptarg(g);
          daikon.config.Configuration.getInstance().apply(item);
          break;
        } else if (Daikon.debugAll_SWITCH.equals(option_name)) {
          Global.debugAll = true;
        } else if (Daikon.debug_SWITCH.equals(option_name)) {
          LogHelper.setLevel(Daikon.getOptarg(g), LogHelper.FINE);
        } else if (Daikon.track_SWITCH.equals (option_name)) {
          LogHelper.setLevel("daikon.Debug", LogHelper.FINE);
          String error = Debug.add_track (Daikon.getOptarg(g));
          if (error != null) {
            throw new TerminationMessage ("Error parsing track argument '"
                                + Daikon.getOptarg(g) + "' - " + error);
          }
        } else {
          throw new RuntimeException("Unknown long option received: " +
                                     option_name);
        }
        break;
      case 'h':
        System.out.println(usage);
        throw new TerminationMessage();
      case '?':
        break; // getopt() already printed an error
      default:
        System.out.println("getopt() returned " + c);
        break;
      }
    }


    {
    File inv_file = null;

    // Loop through each filename specified
    for (int i=g.getOptind(); i<args.length; i++) {

      // Get the file and make sure it exists
      File file = new File(args[i]);
      if (! file.exists()) {
        throw new Error("File " + file + " not found.");
      }

      // These aren't "endsWith()" because there might be a suffix on the end
      // (eg, a date).
      String filename = file.toString();
      if (filename.indexOf(".inv") != -1) {
        if (inv_file != null) {
          throw new TerminationMessage ("multiple inv files specified" + Global.lineSep + usage);
        }
        inv_file = file;
      } else if (filename.indexOf(".dtrace") != -1) {
        dtrace_files.add(filename);
      } else {
        throw new Error("Unrecognized argument: " + file);
      }
    }
    if (dir_file==null) {
      if (inv_file == null) {
          throw new TerminationMessage ("No inv file specified" + Global.lineSep + usage);
      }
      checkInvariants(inv_file);
      return;
    }
    }

    // Yoav additions:
    File[] filesInDir = dir_file.listFiles();
    if (filesInDir == null || filesInDir.length==0)
          throw new TerminationMessage("The directory "+dir_file+" is empty"+ Global.lineSep + usage);
    ArrayList<File> invariants = new ArrayList<File>();
    for (File f: filesInDir)
       if (f.toString().indexOf(".inv") != -1) invariants.add(f);
    if (invariants.size()==0)
          throw new TerminationMessage("Did not find any invariant files in the directory "+dir_file+ Global.lineSep + usage);
    ArrayList<File> dtraces = new ArrayList<File>();
    for (File f: filesInDir)
       if (f.toString().indexOf(".dtrace") != -1) dtraces.add(f);
    if (dtraces.size()==0)
          throw new TerminationMessage("Did not find any dtrace files in the directory "+dir_file+ Global.lineSep + usage);

    System.out.println("Collecting data for invariants files "+invariants+" and dtrace files "+dtraces);

    dtrace_files.clear();
    for (File dtrace: dtraces) {
      dtrace_files.add(dtrace.toString());
    }


    String commaLine = "";
    for (File inFile : invariants) {
      String name = inFile.getName().replace(".inv","").replace(".gz","");
      commaLine += ","+name;
    }
    outputComma.add(commaLine);

    commaLine = "";
    for (File inFile : invariants) {
      File inv_file = inFile;
      failedInvariants.clear();
      testedInvariants.clear();
      error_cnt = 0;

      output_stream = new PrintStream (new FileOutputStream (inFile.toString().replace(".inv","").replace(".gz","")+".false-positives.txt"));
      checkInvariants(inv_file);
      output_stream.close();

      int failedCount = failedInvariants.size();
      int testedCount = testedInvariants.size();
      String percent = toPercentage(failedCount, testedCount);
      commaLine += ","+percent;
    }
    outputComma.add(commaLine);

    System.out.println();
    for (String output : outputComma)
      System.out.println(output);
  }
  private static String toPercentage(int portion, int total) {
    double s = portion * 100;
    return String.format("%.2f",s /total)+"%";
  }
  private static void checkInvariants(File inv_file) throws IOException {
    // Read the invariant file
    PptMap ppts = FileIO.read_serialized_pptmap (inv_file, true );

    //Yoav: make sure we have unique invariants
    InvariantFilters fi = InvariantFilters.defaultFilters();
    //Set<String> allInvariantsStr = new HashSet<String>();
    Set<Invariant> allInvariants = new HashSet<Invariant>();
    for (PptTopLevel ppt : ppts.all_ppts()){
     // System.out.println("============================================================");
     // System.out.println(ppt.name);
      for (PptSlice slice : ppt.views_iterable()) {
        for (Invariant inv : slice.invs) {
        //	System.out.println(inv.format());
         // System.out.println("at:   " + inv.format());
          if (doConf &&
              inv.getConfidence()<Invariant.dkconfig_confidence_limit) {
             System.out.printf ("inv ignored (conf): %s:%s\n", inv.ppt.name(),
                               inv.format());
            continue;
          }

          if (doFilter && fi.shouldKeep(inv)==null) {
             System.out.printf ("inv ignored (filter): %s:%s\n",
                                 inv.ppt.name(), inv.format());
            continue;
          }
          activeInvariants.add(inv);

          //String n = invariant2str(ppt, inv);
          //if (!allInvariants.contains(inv) && allInvariantsStr.contains(n)) throw new TerminationMessage("Two invariants have the same ppt.name+inv.rep:"+n);
          allInvariants.add(inv);
          //allInvariantsStr.add(n);
        }
      }
    }

    // Read and process the data trace files
    FileIO.Processor processor = new InvariantCheckProcessor();

    Daikon.FileIOProgress progress = new Daikon.FileIOProgress();
    progress.start();
    progress.clear();
    FileIO.read_data_trace_files (dtrace_files, ppts, processor, false);
    progress.shouldStop = true;
    //System.out.println ();
    //System.out.printf ("%s: %,d errors found in %,d samples (%s)\n", inv_file,
    //                   error_cnt, sample_cnt,
    //                   toPercentage (error_cnt, sample_cnt));
    int failedCount = failedInvariants.size();
    int testedCount = testedInvariants.size();
    String percent = toPercentage(failedCount, testedCount);
    //System.out.println(inv_file+": "+failedCount+" false positives, out of "+testedCount+", which is "+percent+".");
    if (false) {
      for (Invariant inv : failedInvariants) {
        System.out.printf ("+%s:%s\n", inv.ppt.name(), inv.format());
      }
    }
    for (Integer lineNum : quarryPrintMap.keySet()){
    		for (String inv :quarryPrintMap.get(lineNum)){
    			if (inv.startsWith("!!!") || worthPrintingSomewhere.contains(inv))
    				System.out.println(inv);
    		}
    		System.out.println("..");
    	   	
    }
    System.out.println("--");
    
  }

  /** Class to track matching ppt and its values. */
  static final class EnterCall {

    public PptTopLevel ppt;
    public ValueTuple vt;

    public EnterCall (PptTopLevel ppt, ValueTuple vt) {

      this.ppt = ppt;
      this.vt = vt;
    }
  }

  public static class InvariantCheckProcessor extends FileIO.Processor {

    Map<Integer,EnterCall> call_map = new LinkedHashMap<Integer,EnterCall>();

    /**
     * process the sample by checking it against each existing invariant
     * and issuing an error if any invariant is falsified or weakened.
     */
    /*@RequiresNonNull("FileIO.data_trace_state")*/
    public void process_sample (PptMap all_ppts, PptTopLevel ppt,
                                ValueTuple vt, /*@Nullable*/ Integer nonce) {

      debug.fine ("processing sample from: " + ppt.name);

      // Add orig and derived variables
      assert vt.vals != null : "@AssumeAssertion(nullness): bug: Checker Framework bug:  vals is a non-null array, but is reported as nullable";
      FileIO.compute_orig_variables(ppt, vt.vals, vt.mods, nonce);
      FileIO.compute_derived_variables(ppt, vt.vals, vt.mods);

      // Intern the sample
      vt = new ValueTuple(vt.vals, vt.mods);

      // If this is an enter point, just remember it for later
      if (ppt.ppt_name.isEnterPoint()) {
        assert nonce != null : "@AssumeAssertion(nullness): nonce exists for enter & exit points";
        if (dir_file!=null) {
          //Yoav: I had to do a hack to handle the case that several dtrace files are concatenated together,
          // and Sung's dtrace files have unterminated calls, and when concatenating two files you can have the same nonce.
          // So I have to remove the nonce found from the call_map
          call_map.remove(nonce);
        } else
          assert call_map.get (nonce) == null;
        call_map.put (nonce, new EnterCall (ppt, vt));
        debug.fine ("Skipping enter sample");
        return;
      }

      // If this is an exit point, process the saved enter point
      if (ppt.ppt_name.isExitPoint()) {
        assert nonce != null : "@AssumeAssertion(nullness): nonce exists for enter & exit points";
        EnterCall ec = call_map.get (nonce);
        if (ec != null) {
          call_map.remove (nonce);
          debug.fine ("Processing enter sample from " + ec.ppt.name);
          add (ec.ppt, ec.vt, all_ppts);
        } else { // didn't find the enter
          if (!quiet)
            System.out.printf ("couldn't find enter for nonce %d at ppt %s\n",
                               nonce, ppt.name());
          return;
        }
      }

      add (ppt, vt, all_ppts);
    }

    /*@RequiresNonNull("FileIO.data_trace_state")*/
    private void add (PptTopLevel ppt, ValueTuple vt, PptMap all_ppts) {
      // Add the sample to any splitters
      if (ppt.has_splitters()) {
        assert ppt.splitters != null; // because ppt.has_splitters() = true
        for (PptSplitter ppt_split : ppt.splitters) {
          PptConditional ppt_cond = ppt_split.choose_conditional (vt);
          if (ppt_cond != null)
            add (ppt_cond, vt, all_ppts);
          else
            debug.fine (": sample doesn't pick conditional");
        }
      }

      // if this is a numbered exit, apply to the combined exit as well
      if (!(ppt instanceof PptConditional)
          && ppt.ppt_name.isNumberedExitPoint()) {
        PptTopLevel parent = all_ppts.get (ppt.ppt_name.makeExit());
        if (parent != null) {
          parent.get_missingOutOfBounds (ppt, vt);
          add (parent, vt, all_ppts);
        }
      }

      // If the point has no variables, skip it
      if (ppt.var_infos.length == 0)
        return;

      // We should have received sample here before, or there is nothing
      // to check.
      // Yoav added: It can be that the different dtrace and inv files have different program points
      if (false && ppt.num_samples() <= 0)
        assert ppt.num_samples() > 0
        : "ppt " + ppt.name
                            + " has 0 samples and "
                            + ppt.var_infos.length + " variables";


      // CARO ADDED the below three lines		
      Integer lineNum = new Integer(FileIO.get_linenum());
      // first boolean is augment with equality predicates, second is augment with ppts
      // !!! edit next line for different behavior
	  HashSet<String> insertSet = initializeSet(ppt.var_infos, vt, ppt.name);
	  quarryPrintMap.put(lineNum, insertSet);
      
	  // Loop through each slice
      slice_loop:
      for (PptSlice slice : ppt.views_iterable()) {
        if (debug_detail.isLoggable (Level.FINE))
          debug_detail.fine (": processing slice " + slice + "vars: "
                           + Debug.toString (slice.var_infos, vt));

        // If any variables are missing, skip this slice
        for (int j = 0; j < slice.var_infos.length; j++) {
          VarInfo v = slice.var_infos[j];
          int mod = vt.getModified (v);
          if (v.isMissing (vt)) {
            if (debug_detail.isLoggable (Level.FINE))
              debug_detail.fine (": : Skipping slice, " + v.name()
                               + " missing");
            continue slice_loop;
          }
          if (v.missingOutOfBounds()) {
            if (debug_detail.isLoggable (Level.FINE))
              debug.fine (": : Skipping slice, " + v.name()
                          + " out of bounds");
            continue slice_loop;
          }
        }
        

		
		// CARO ADDED  
		TreeSet<String> sliceVnames = new TreeSet<String>();
		for (VarInfo vinfo: slice.var_infos){
			sliceVnames.add(vinfo.str_name());
		}
		String sliceVnameString = "???";
		for (String name: sliceVnames)
			sliceVnameString += name + "; ";
		sliceVnameString += "???";
		// done caro adding

        // Loop through each invariant
        for (Invariant inv : slice.invs) {
          if (debug_detail.isLoggable (Level.FINE))
            debug_detail.fine (": : Processing invariant: " + inv);
          if (!inv.isActive()) {
            if (debug_detail.isLoggable (Level.FINE))
              debug_detail.fine (": : skipped non-active " + inv);
            continue;
          }

          //Yoav added
          if (!activeInvariants.contains(inv)) {
            // System.out.printf ("skipping invariant %s:%s\n", inv.ppt.name(),
            //                   inv.format());
            continue;
          }
          //String invRep = invariant2str(ppt, inv);
          //testedInvariants.add(inv);
          Invariant newinv = inv.clone();

          InvariantStatus status;

          // Caroline Modified
          if (slice instanceof PptSlice1) {

              VarInfo v = slice.var_infos[0];
              UnaryInvariant unary_inv = (UnaryInvariant) inv;
              status = unary_inv.check (vt.getValue(v), vt.getModified(v), 1);

            } else if (slice instanceof PptSlice2) {

              VarInfo v1 = slice.var_infos[0];
              VarInfo v2 = slice.var_infos[1];
              BinaryInvariant bin_inv = (BinaryInvariant) inv;
              status = bin_inv.check_unordered (vt.getValue(v1), vt.getValue(v2),
                                              vt.getModified(v1), 1);


            } else {
                VarInfo v1 = slice.var_infos[0];
                VarInfo v2 = slice.var_infos[1];
                VarInfo v3 = slice.var_infos[2];
                assert (inv instanceof TernaryInvariant)
                  : "invariant '" + inv.format() + "' in slice "
                                     + ppt.name() + " is not ternary";
                TernaryInvariant ternary_inv = (TernaryInvariant) inv;
                status = ternary_inv.check (vt.getValue(v1), vt.getValue(v2),
                                          vt.getValue(v3), vt.getModified(v1), 1);
            }
          // end caroline modified
          
          if (status != InvariantStatus.NO_CHANGE) {
            if (!quiet) {
              output_stream.println("Next was " + status);
              output_stream.println ("At ppt " + ppt.name + ", Invariant '"
                                     + inv.format() + "' invalidated by sample "
                                     + Debug.toString (slice.var_infos, vt)
                                     + "at line " + FileIO.get_linenum() + " in file "
                                     + FileIO.data_trace_state.filename);
            }

          } else {
        	  String invFormat = inv.format();   

      			if (inv.isWorthPrinting()){
      				worthPrintingSomewhere.add(sliceVnameString + invFormat);
      			}
      			
        		HashSet<String> lineNumMapping = quarryPrintMap.get(lineNum);
        		if (lineNumMapping != null){
       				lineNumMapping.add(sliceVnameString + inv.format());       			
        		} else {
        			System.out.println("NOPE NOPE NOPE");

        		}

        	 // System.err.println("Line " + FileIO.get_linenum() +":("+ ppt.name +") %" +inv.format()+ "% 1");   
          }
        }
      }
		
//		if ( quarryPrintMap.get(lineNum) == null){
//			HashSet<String> insertSet = initializeSet(slice.var_infos, vt);
//			insertSet.add(vnameString);
//			insertSet.add("!!!pptname; ");
//			insertSet.add("???pptname; ???pptname = " + ppt.name);
//			quarryPrintMap.put(lineNum, insertSet);
//			
//		}
    }
  }

  private static HashSet<String> initializeSet(VarInfo[] vinfos, ValueTuple vt, String pptname){
	  HashSet<String> returnSet = new HashSet<String>();
	  if (augmentPpts){
		  returnSet.add("!!!pptname; ");
	      returnSet.add("???pptname; ???pptname == " + pptname);
	      worthPrintingSomewhere.add("???pptname; ???pptname == " + pptname);
	  }
	  String vNameString = "!!!";
	  TreeSet<String> allNames = new TreeSet<String>();
	  for (VarInfo vinfo: vinfos){
		  String name = vinfo.str_name();
		  allNames.add(name);
		  if (augmentEquals){
			  String val;
			  String equalityOp = " == " ;
			  if (vinfo.file_rep_type == ProglangType.HASHCODE){
				  equalityOp = " hashcode= ";
			  }
			  if (vt.getValue(vinfo) != null){
				 val = vt.getValue(vinfo).toString();
				 if (vt.getValue(vinfo) instanceof String){
					 val = "\"" + val + "\"";
				 }			     
			  } else{
				 val = "null";
			  }
			  returnSet.add("???" + name + "; ???" + name + equalityOp + val);	
			  worthPrintingSomewhere.add("???" + name + "; ???" + name + equalityOp + val);
		  }  
	 }
	  for (String name: allNames){
		  vNameString += name + "; ";
	  }
	  returnSet.add(vNameString);
	  return returnSet;
  }
  private static String invariant2str(PptTopLevel ppt, Invariant inv) {
    return ppt.name + " == " + inv.repr() + inv.getClass() + inv.varNames() + ": " + inv.format();
  }

}

