/*
 * texada_binop.h
 * Created On: December 20, 2021
 * Author: cjbanks
 */

#include "texada_binop.h"
#include "texadavisitor.h"

//binop member functions
namespace texada {
    namespace ltl {

        const formula* binop::first() const
        {

            return first_;
        }

        const formula* binop::second() const
        {
            return second_;
        }


        binop::type binop::op() const
        {
            spot::formula f_temp = formula::spot_formula();
                    if (f_temp.is(spot::op::Xor))
                        return binop::type::Xor;
                    if (f_temp.is(spot::op::Implies))
                        return binop::type::Implies;
                    if (f_temp.is(spot::op::Equiv))
                        return binop::type::Equiv;
                    if (f_temp.is(spot::op::U))
                        return binop::type::U;
                    if (f_temp.is(spot::op::R))
                        return binop::type::R;
                    if (f_temp.is(spot::op::W) )
                        return binop::type::W;
                    if (f_temp.is(spot::op::M) )
                        return binop::type::M;
                    if (f_temp.is(spot::op::EConcat) )
                        return binop::type::EConcat;
                    if (f_temp.is(spot::op::EConcatMarked) )
                        return binop::type::EConcatMarked;
                    if (f_temp.is(spot::op::UConcat) )
                        return binop::type::UConcat;
        }

        void binop::accept(visitor& v) const
        {
            v.visit(this);
        }

        binop::~binop() {
            first()->destroy();
            second()->destroy();
        }

    }
}