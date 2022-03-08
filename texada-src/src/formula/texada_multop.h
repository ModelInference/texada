/*
 * texada_multop.cpp
 * Created On: December 20, 2021
 * Author: cjbanks
 */


#ifndef TEXADA_TEXADA_MULTOP_H
#define TEXADA_TEXADA_MULTOP_H

#include <iostream>
#include "texadatospotformula.h"
#include <spot/misc/common.hh>
#include <utility>
#include "texadapredecl.h"

namespace texada {
    namespace ltl {

        /**
        * Creates a mapping from multop formulas in spot::formula::multop to functions calls
        * in Texada that rely on multop formulas. SPOT 2.X implementation of multop is now a
        * member of the spot::formula object.
        **/
        class multop : public formula {
        public:
            enum type{
                //              // n-ary operators
                Or,                        ///< (omega-Rational) Or
                And,                       ///< (omega-Rational) And
                OrRat,                     ///< Rational Or
                AndRat,                    ///< Rational And
                AndNLM,                    ///< Non-Length-Matching Rational-And
                Concat,                    ///< Concatenation
                Fusion,                    ///< Fusion
            };

            //list of formulae
            typedef std::vector<const formula*> vec;

            //Get the number of children
            unsigned size() const;


            //Get the nth children
            const formula* nth(unsigned n) const;


            //object constructor
            multop(type op, vec& v)  : op_(op), children_(v)
            {
                std::vector<spot::formula> child_vector;

                for (auto child: v){
                    child_vector.push_back(child->spot_formula());
                }
                switch(op){
                    case type::And:
                        this->set_spot_formula(spot::formula::multop(spot::op::And, child_vector));
                        break;
                    case type::AndNLM:
                        this->set_spot_formula(spot::formula::multop(spot::op::AndNLM, child_vector));
                        break;
                    case type::AndRat:
                        this->set_spot_formula(spot::formula::multop(spot::op::AndRat, child_vector));
                        break;
                    case type::Fusion:
                        this->set_spot_formula(spot::formula::multop(spot::op::Fusion, child_vector));
                        break;
                    case type::Concat:
                        this->set_spot_formula(spot::formula::multop(spot::op::Concat, child_vector));
                        break;
                    case type::Or:
                        this->set_spot_formula(spot::formula::multop(spot::op::Or, child_vector));
                        break;
                    case type::OrRat:
                        this->set_spot_formula(spot::formula::multop(spot::op::OrRat, child_vector));
                        break;
                }
            }


            //return the operator
            type op() const;

            void accept(visitor& v) const override;

            //operator type
            type op_;

            //children of formula
            vec children_;

        protected:
            virtual ~multop();


        };
    }
}
#endif //TEXADA_TEXADA_MULTOP_H
