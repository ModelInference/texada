/* texada_bunop.h
 * Created On: December 14, 2021
 * Author: cjbanks
 */

#include "texada_bunop.h"
#include "texadavisitor.h"
#include "texadatospotformula.h"

namespace texada {
    namespace ltl {
        const formula* bunop::one_star_ = nullptr;

        bunop::bunop(type op, const formula* child, unsigned min, unsigned max)
                : op_(op), child_(child), min_(min), max_(max)
        {

            assert(is.sere_formula);
            is.boolean = false;
            is.ltl_formula = false;
            is.psl_formula = false;
            is.eventual = false;
            is.universal = false;
            is.syntactic_safety = false;
            is.syntactic_guarantee = false;
            is.syntactic_obligation = false;
            is.syntactic_recurrence = false;
            is.syntactic_persistence = false;

            switch (op)
            {
                case Star:
                    this->set_spot_formula(spot::formula::bunop(spot::op::Star,child->spot_formula(),min, max));
                    break;
                case FStar:
                    this->set_spot_formula(spot::formula::bunop(spot::op::FStar,child->spot_formula(),min, max));
                    break;

            }
        }


        bunop::~bunop()
        {
            // one_star_ should never get deleted.  Otherwise, that means it
            // has been destroyed too much, or not cloned enough.
            assert(this != one_star_);
            child()->destroy();
        }

        void bunop::accept(visitor &v) const {

            v.visit(this);
        }

    }
    }