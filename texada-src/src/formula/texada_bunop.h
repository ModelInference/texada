/* texada_bunop.h
 * Created On: December 14, 2021
 * Author: cjbanks
 */
#ifndef TEXADA_TEXADA_BUNOP_H
#define TEXADA_TEXADA_BUNOP_H
#include <iostream>
#include <map>
#include "texadatospotmapping.h"
#include "spot/tl/formula.hh"
#include "texadapredecl.h"

namespace texada {
    namespace ltl {
        /**
        * Creates a mapping from bunop formulas represented as spot::formula::bunop to functions calls
        * in Texada that are/require bunop formulas. SPOT 2.X implementation of bunop are now
        * members of the spot::formula object.
        **/


        class bunop : public formula {
        public:
            enum type { Star, FStar };

            static const unsigned unbounded = -1U;

            //static const formula* instance(type op,
            //                               const formula* child,
            //                               unsigned min = 0,
            //                               unsigned max = unbounded);


            void accept(visitor& v) const override;

            /// Get the sole operand of this operator.
            const formula* child() const
            {
                return child_;
            }

            /// Minimum number of repetition.
            unsigned min() const
            {
                return min_;
            }

            /// Minimum number of repetition.
            unsigned max() const
            {
                return max_;
            }

            /// \brief A string representation of the operator.
            ///
            /// For instance "[*2..]".
            std::string format() const;

            /// Get the type of this operator.
            type op() const
            {
                return op_;
            }


            bunop(type op, const formula* child, unsigned min, unsigned max);

        protected:
            typedef std::tuple<type, const formula*, unsigned, unsigned> key;
            typedef std::map<key, const bunop*> map;
            static map instances;


            virtual ~bunop();


        private:
            type op_;
            const formula* child_;
            unsigned min_;
            unsigned max_;
            static const formula* one_star_;



        };


    }
}
#endif //TEXADA_TEXADA_BUNOP_H
