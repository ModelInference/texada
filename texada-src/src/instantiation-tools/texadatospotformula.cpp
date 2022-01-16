/*
 * texadatospotformula.cpp
 *
 * Created on: December 9, 2021
 * Author: cjbanks
 */

#include "texadatospotformula.h"



namespace texada {
                namespace ltl {

      //formula member functions
        const formula* formula::clone() const
        {
            this->ref_();
            return this;
        }


        formula::type formula::kind() const
        {
            if (f_.kind() == spot::op::ap){
                return formula::type::AtomicProp;
                }
            if (f_.kind() == spot::op::ff)
                return formula::type::False;
            if (f_.kind() == spot::op::tt)
                return formula::type::True;
            if (f_.kind() == spot::op::eword)
                return formula::type::EmptyWord;
            if (f_.is_constant()) {
                return formula::type::Constant;
            }
            if (f_.kind() == spot::op::Not || f_.kind() == spot::op::X || f_.kind() == spot::op::F || f_.kind() == spot::op::G || f_.kind() == spot::op::Closure || f_.kind() == spot::op::NegClosure || f_.kind() == spot::op::NegClosureMarked) {
                return formula::type::UnOp;
            }
            if (f_.kind() == spot::op::Xor || f_.kind() == spot::op::Implies || f_.kind() == spot::op::Equiv || f_.kind() == spot::op::U || f_.kind() == spot::op::R || f_.kind() == spot::op::W || f_.kind() == spot::op::M || f_.kind() == spot::op::EConcat || f_.kind() == spot::op::EConcatMarked || f_.kind() == spot::op::UConcat)
                return formula::type::BinOp;
            if (f_.kind() == spot::op::Star || f_.kind() == spot::op::FStar)
                return formula::type::BUnOp;
            if (f_.kind() == spot::op::Or || f_.kind() == spot::op::OrRat || f_.kind() == spot::op::And || f_.kind() == spot::op::AndRat || f_.kind() == spot::op::AndNLM || f_.kind() == spot::op::Concat || f_.kind() == spot::op::Fusion)
                return formula::type::MultOp;
        }


        formula::type formula::val() const
        {
            if (f_.is_ff())
                return formula::type::False;
            if (f_.is_tt())
                return formula::type::True;
            if (f_.is_eword())
                return formula::type::EmptyWord;
        }

        void formula::accept(visitor& v) const
        {
          v.visit(this);
        }

        void formula::set_spot_formula(spot::formula f_temp) {
            f_ = f_temp;
        }

        std::size_t formula::hash() const
        {
            std::hash<spot::formula> formula_hash;
            return formula_hash(f_);

        }

        //returns spot formula
        spot::formula formula::spot_formula() const {
            return f_;
        }

        void formula::destroy() const
        {
            //SPOT destroys formula object
            if (this->unref_()) {
                    printf("delete this formula. \n");
                    delete this;
            }
        }

        void formula::ref_() const
        {
            ++ref_counter_;
        }

        bool formula::unref_() const
        {
            // Not reference counted by default.
            return !--ref_counter_;
        }

        unsigned formula::ref_count_() const {
            return ref_counter_;
        }




                }
}
