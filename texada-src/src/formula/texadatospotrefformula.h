/*
 * texadatospotrefformula.h
 * Created On: December 20, 2021
 * Author: cjbanks
 */
#ifndef TEXADA_TEXADATOSPOTREFFORMULA_H
#define TEXADA_TEXADATOSPOTREFFORMULA_H

/** SPOT formula wrapping. This is the main wrapper for mapping logic formulas to SPOT
 * formulas. SPOT implements formulas as a formula object. In addition to this, formula
 * types are instances of formula objects in SPOT 2.9. In order to maintain inheritance the virtual class is refformula,
 * The formula object inherits from this class and all other formula types inherit from the formula class.
 **/
#include "texadavisitor.h"

#include <iostream>
#include <spot/tl/formula.hh>
#include <spot/misc/common.hh>


namespace texada {
    namespace ltl {

        class refformula {


        public:

            refformula() : f_()
            {
            }


            //wrap spot formula in this class
            refformula(spot::formula& ff) : f_(ff)
            {
            }

            refformula(const spot::formula& ff) : f_(ff)
            {
            }


            /// Destroy a formula.
            ~refformula()
            {
            }

            virtual void accept(visitor& v) const = 0;


        private:
            //SPOT formula wrapper maintains
            spot::formula f_;

        };
    }
}
#endif //TEXADA_TEXADATOSPOTREFFORMULA_H
