/*
 * texadavisitor.h
 * Created On: June 24, 2021
 * Author: cjbanks
 */

#ifndef TEXADAVISITOR_H_
#define TEXADAVISITOR_H_
#include "texadapredecl.h"

namespace texada {
	namespace ltl {
   
	       	struct visitor     {
		 	virtual ~visitor() {}
		 	virtual void visit(const atomic_prop* node) = 0;
		 	virtual void visit(const binop* node) = 0;
		 	virtual void visit(const unop* node) = 0; 
		 	virtual void visit(const constant* node) = 0;
		  	virtual void visit(const multop* node) = 0;  
		    virtual void visit(const bunop* node) = 0;

            //added since ref formula is abstract class
            virtual void visit (const formula* node) = 0;
		};
		
		/// \brief Formula visitor that cannot modify the formula.     
		/////     /// Writing visitors is the prefered way 
		//    /// to traverse a formula, since it doesn't   
		//      /// involve any cast.     ///     
		//      /// If you want to modify the visited formula, inherit from
		//           /// spot::ltl:visitor instead.     

	}
}
#endif /*TEXADAVISITOR_H_*/
