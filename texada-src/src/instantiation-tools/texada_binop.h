/*
 * texada_binop.h
 * Created On: December 20, 2021
 * Author: cjbanks
 */

#ifndef TEXADA_TEXADA_BINOP_HPP
#define TEXADA_TEXADA_BINOP_HPP

#include <iostream>
#include "texadatospotformula.h"
#include <spot/misc/common.hh>
#include "texadapredecl.h"

namespace texada {
    namespace ltl {

        /**
        * Creates a mapping from binop formulas represented as spot::formula::binop to functions calls
        * in Texada that are/require binop formulas. SPOT 2.X implementation of binop are now
        * members of the spot::formula object.
        **/

        class binop : public formula {
        public:

            enum type{
                Xor,
                Implies,                   ///< Implication
                Equiv,                     ///< Equivalence
                U,                         ///< until
                R,                         ///< release (dual of until)
                W,                         ///< weak until
                M,                         ///< strong release (dual of weak until)
                EConcat,		     ///< Seq
                EConcatMarked,	     ///< Seq, Marked
                UConcat, 		     ///< Triggers
            };

            //get the first operand
            const formula* first() const;


            //get the second operand
            const formula* second() const;


            binop() : op_(), first_(), second_()
            {}

            binop(type op, const formula* first, const formula* second) :
            op_(op), first_(first), second_(second)
            {
                switch(op){
                    case type::U:
                        this->set_spot_formula(spot::formula::binop(spot::op::U, first->spot_formula(), second->spot_formula()));
                        break;
                    case type::UConcat:
                        this->set_spot_formula(spot::formula::binop(spot::op::UConcat, first->spot_formula(), second->spot_formula()));
                        break;
                    case type::EConcat:
                        this->set_spot_formula(spot::formula::binop(spot::op::EConcat, first->spot_formula(), second->spot_formula()));
                        break;
                    case type::EConcatMarked:
                        this->set_spot_formula(spot::formula::binop(spot::op::EConcatMarked, first->spot_formula(), second->spot_formula()));
                        break;
                    case type::Equiv:
                        this->set_spot_formula(spot::formula::binop(spot::op::Equiv, first->spot_formula(), second->spot_formula()));
                        break;
                    case type::Implies:
                        this->set_spot_formula(spot::formula::binop(spot::op::Implies, first->spot_formula(), second->spot_formula()));
                        break;
                    case type::Xor:
                        this->set_spot_formula(spot::formula::binop(spot::op::Xor, first->spot_formula(), second->spot_formula()));
                        break;
                    case type::W:
                        this->set_spot_formula(spot::formula::binop(spot::op::W, first->spot_formula(), second->spot_formula()));
                        break;
                    case type::R:
                        this->set_spot_formula(spot::formula::binop(spot::op::R, first->spot_formula(), second->spot_formula()));
                        break;
                    case type::M:
                        this->set_spot_formula(spot::formula::binop(spot::op::M, first->spot_formula(), second->spot_formula()));
                        break;

                }
            }


            //returns the operator of the binop formula
            type op() const;

            virtual void accept(visitor& v) const;

        protected:
            virtual ~binop();

        private:
            type op_;
            const formula* first_;
            const formula* second_;

        };
    }
}
#endif //TEXADA_TEXADA_BINOP_HPP
