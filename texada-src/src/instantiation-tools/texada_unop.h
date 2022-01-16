/*
 * texada_unop.cpp
 * Created On: December 20, 2021
 * Author: cjbanks
 */



#ifndef TEXADA_UNOP_H
#define TEXADA_UNOP_H

#include <iostream>
#include <map>
#include "texadatospotformula.h"
#include <spot/misc/common.hh>
#include "texadapredecl.h"


namespace texada {
	namespace ltl {
        /**
        * Creates a mapping from unop formulas in spot::formula::unop to function calls
        * in Texada that rely on unop formulas. SPOT 2.X implementation of unop is now a
        * member of the spot::formula object.
        **/
	    class unop : public formula {
	    public:
	        enum type{
			  // unary operators
                G,                         ///< Globally
                F,                         ///< Eventually
                X,                         ///< Next
                Not,                       ///< Negation
			  Finish, //does not exist in Spot 2.X
                          Closure,                   ///< PSL Closure
                          NegClosure,                ///< Negated PSL Closure
                          NegClosureMarked,          ///< marked version of the Negated PSL Closure
			  };

            //return sole operand of this operator
            const formula* child() const;

            //empty constructor
            unop() : op_(), child_()
            {}


            //additional constructors of unop object
            unop(type op, const formula* child):
            op_(op), child_(child)
                {
                switch(op) {
                    case type::Not:
                        this->set_spot_formula(spot::formula::unop(spot::op::Not, child->spot_formula()));
                        break;
                    case type::X:
                        this->set_spot_formula(spot::formula::unop(spot::op::X, child->spot_formula()));
                        break;
                    case type::NegClosureMarked:
                        this->set_spot_formula(spot::formula::unop(spot::op::NegClosureMarked, child->spot_formula()));
                        break;
                    case type::NegClosure:
                        this->set_spot_formula(spot::formula::unop(spot::op::NegClosure, child->spot_formula()));
                        break;
                    case type::Closure:
                        this->set_spot_formula(spot::formula::unop(spot::op::Closure, child->spot_formula()));
                        break;
                    case type::G:
                        this->set_spot_formula(spot::formula::unop(spot::op::G, child->spot_formula()));
                        break;
                    case type::F:
                        this->set_spot_formula(spot::formula::unop(spot::op::F, child->spot_formula()));
                        break;

                }
                }


            //return operator
            type op() const;

			virtual void accept(visitor& v) const;

        protected:
            virtual ~unop();


	    private:
            type op_;
            const formula* child_;

	    };
	}
}
#endif //TEXADA_UNOP_H
