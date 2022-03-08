/*
 * texada_multop.cpp
 * Created On: December 20, 2021
 * Author: cjbanks
 */

#include "texada_multop.h"
#include "texadavisitor.h"

namespace texada {
    namespace ltl {

        //multop member functions
        unsigned multop::size() const   
        {
            if (children_.empty()){
                spot::formula form = formula::spot_formula();
                return form.size();
            }
            return children_.size();
        }

        const formula* multop::nth(unsigned n) const
        {
            vec children = children_;
            return children[n];
        }


        multop::type multop::op() const
        {
            spot::formula form = formula::spot_formula();
            if (form.is(spot::op::Or))
                return multop::type::Or;
            else if (form.is(spot::op::OrRat))
                return multop::type::OrRat;
            else if (form.is(spot::op::And))
                return multop::type::And;
            else if (form.is(spot::op::AndRat) )
                return multop::type::AndRat;
            else if (form.is(spot::op::AndNLM))
                return multop::type::AndNLM;
            else if (form.is(spot::op::Concat))
                return multop::type::Concat;
            else //fusion
                return multop::type::Fusion;
        }

    void multop::accept(visitor& v) const
    {
        v.visit(this);
    }

    multop::~multop() {
        for (auto c : children_){
            c->destroy();
        }
        }

    }
}