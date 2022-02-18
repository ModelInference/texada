/*
 * texadapostfix.h
 * Created On: June 24, 2021
 * Author: cjbanks
 */

#ifndef TEXADAPOSTFIX_H_
#define TEXADAPOSTFIX_H_

#include "texadatospotformula.h"
#include "texadapredecl.h"
#include "texadavisitor.h"

namespace texada {
namespace ltl {

	//dummy class. SPOT 2.X does not support/rely on visitors for formula traversal. This class was primarily copied
    // from the SPOT 1.2.6 codebase to add visitor traversal in order to maintain compatibility with Texada.

	class postfix_visitor : public visitor{
		public:
			postfix_visitor(){}
			virtual ~postfix_visitor(){}
			void visit(const atomic_prop* ap);
			void visit(const unop* uo);
			void visit(const binop* bo);
			void visit(const multop* mo);  
		   	//void visit(automatop* c);
		 	void visit(const constant* c); 
		  	void visit(const bunop* so);
            void visit(const formula* f);


			virtual void doit(const atomic_prop* ap);   
            virtual void doit(const unop* uo);
		 	virtual void doit(const binop* bo);
		 	virtual void doit(const multop* mo); 
		  	//virtual void doit(automatop* mo);  
		   	virtual void doit(const constant* c);  
		   	virtual void doit(const bunop* so);
            virtual void doit(const formula* f);
		   	virtual void doit_default(const formula* f);
			
		};


}
}

#endif /*TEXADAPOSTFIX_H_*/
