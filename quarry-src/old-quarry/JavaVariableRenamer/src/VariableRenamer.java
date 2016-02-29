import java.util.HashMap;

import com.github.javaparser.ast.body.ConstructorDeclaration;
import com.github.javaparser.ast.body.MethodDeclaration;
import com.github.javaparser.ast.body.VariableDeclaratorId;
import com.github.javaparser.ast.expr.AssignExpr;
import com.github.javaparser.ast.expr.FieldAccessExpr;
import com.github.javaparser.ast.expr.NameExpr;
import com.github.javaparser.ast.visitor.VoidVisitorAdapter;
import com.github.javaparser.ast.expr.MethodCallExpr;

// Renames local variables in a class' methods (note: does not
// rename fields). Extends the javaparser visitor class. 
public class VariableRenamer extends VoidVisitorAdapter<Object> {
	
	private int count = 0;
	HashMap<String,String> varmap;
	
	@Override
	public void visit (ConstructorDeclaration d, Object arg){
		varmap = new HashMap<String,String>();
		LocalVariableRenamer renamer = new LocalVariableRenamer();
	    renamer.visit(d,null);
	}
	
	@Override
	public void visit (MethodDeclaration d, Object arg){
		varmap = new HashMap<String,String>();
		LocalVariableRenamer renamer = new LocalVariableRenamer();
	    renamer.visit(d,null);
	}

    
    
    private class LocalVariableRenamer extends VoidVisitorAdapter<Object>{
    	
        @Override
    	public void visit(VariableDeclaratorId d, Object arg){
        	String prefix = "name" + count + "_";
        	varmap.put(d.getName(), prefix);
        	d.setName(prefix + d.getName());
    		count++;
    		
    	}
        
        
        @Override 
        public void visit(NameExpr d, Object arg){
                if (varmap.containsKey(d.getName())){
                	d.setName(varmap.get(d.getName())+ d.getName());
                }
        	}
    	
    	
    }
    	
    
}