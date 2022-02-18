/*
 * texada_unop.cpp
 * Created On: December 20, 2021
 * Author: cjbanks
 */

#include <iostream>
#include "texada_unop.h"
#include "texadavisitor.h"

namespace texada {
	namespace ltl {

		//unop member functions
		const formula* unop::child() const
		{
		    if (child_ == nullptr){
                printf("child doesnt exist const \n");
                spot::formula f_temp = formula::spot_formula();
                const static formula child_temp = formula(f_temp.get_child_of(f_temp.kind()));
                return &child_temp;
            }
		    else{
                return child_;
		    }
		}



		unop::type unop::op() const
		{
			spot::formula f_temp = formula::f_;
			if (f_temp.is(spot::op::Not))
				return unop::type::Not;
			else if (f_temp.is(spot::op::X))
				return unop::type::X;
			else if (f_temp.is(spot::op::F))
				return unop::type::F;
			else if (f_temp.is(spot::op::G))
				return unop::type::G;
			else if (f_temp.is(spot::op::Closure))
				return unop::type::Closure;
			else if (f_temp.is(spot::op::NegClosure))
				return unop::type::NegClosure;
			else if (f_temp.is(spot::op::NegClosureMarked))
				return unop::type::NegClosureMarked;

            return F;
        }


		void unop::accept(visitor& v) const
		{
			v.visit(this);
		}

        unop::~unop()
        {
            child()->destroy();
        }

	}
}

