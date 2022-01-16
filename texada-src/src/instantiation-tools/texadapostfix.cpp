#include "texadapostfix.h"
#include "texada_allnodes.h"

namespace texada {
	namespace ltl {

		void    postfix_visitor::visit(const atomic_prop* ap)
		{
			doit(ap);
		}

		void    postfix_visitor::visit(const unop* uo)     
		{       
			uo->child()->accept(*this);       
			doit(uo);     
		}     
		
		void    postfix_visitor::visit(const binop* bo)
	   	{       
			bo->first()->accept(*this); 	
		  	bo->second()->accept(*this); 
		  	doit(bo);     
		}
	       
		void     postfix_visitor::visit(const multop* mo)     
		{       
			unsigned s = mo->size();   
		    	for (unsigned i = 0; i < s; ++i) 	
				mo->nth(i)->accept(*this);   
		    	doit(mo);     
		}    
	       
		void     postfix_visitor::visit(const bunop* so)  
	     	{       
			doit(so);
	   	}  
	     
		void     postfix_visitor::visit(const constant* c)   
	      	{    
		     	doit(c);
	   	}

        void     postfix_visitor::visit(const formula* f)
        {
            doit(f);
        }
	   	
		void     postfix_visitor::doit(const atomic_prop* ap)   
	      	{      
		       	doit_default(ap);   
	      	}    
	       
		void     postfix_visitor::doit(const unop* uo)  
	     	{ 
		  	doit_default(uo);
	   	}     
		
		void     postfix_visitor::doit(const binop* bo) 
	    	{  
		   	doit_default(bo); 
	    	} 
	    	
		void     postfix_visitor::doit(const multop* mo)   
	      	{
		 	doit_default(mo); 
	    	}

	   	
		void     postfix_visitor::doit(const bunop* so)  
	     	{
		 	doit_default(so); 
	    	}
	   
		void     postfix_visitor::doit(const constant* c) 
	    	{
		 	doit_default(c);   
	      	}

        void     postfix_visitor::doit(const formula* f)
        {
            doit_default(f);
        }
	   
		void     postfix_visitor::doit_default(const formula* f) 
	    	{ 
		  	(void)f;    
		     	// Dummy implementation that does nothing.  
		}



	}
}

