import java.io.FileInputStream;
import com.github.javaparser.ast.CompilationUnit;
import com.github.javaparser.ParseException;
import com.github.javaparser.JavaParser;

// Main class uniquefies the local variables of an inputted java 
// file and outputs the renamed version.  
public class Main {

	// Main function running Variable Renamer
    public static void main(String[] args) throws Exception {
    	if (args.length != 1){
    		System.out.println("Error: please specify input location.");
    		return;
    	}
    	
        // creates an input stream for the file to be parsed
        FileInputStream in = new FileInputStream(args[0]);

        CompilationUnit cu;
        try {
            // parse the file
            cu = JavaParser.parse(in);
        } finally {
            in.close();
        }
		
		new VariableRenamer().visit(cu,null);
		
		System.out.println(cu.toString());
    }
}