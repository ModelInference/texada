/* texada_constant.h
 * Created On: December 20, 2021
 * Author: cjbanks
 */
#ifndef TEXADA_TEXADA_CONSTANT_H
#define TEXADA_TEXADA_CONSTANT_H

#include <iostream>
#include "texadatospotmapping.h"
#include "spot/tl/formula.hh"


namespace texada {
    namespace ltl {

        /**
         * Creates a mapping from constant formula objects implemented in spot as spot::formula calls to Texada.
         */

        class constant : public formula
        {
        public:
            enum type {False, True, EmptyWord};

            constant(): val_(){}

            explicit constant(type val)
                    : val_(val)
            {
                switch (val)
                {
                    case constant::True:
                        this->set_spot_formula(spot::formula::tt());
                        break;
                    case constant::False:
                        this->set_spot_formula(spot::formula::ff());
                        break;
                    case constant::EmptyWord:
                        this->set_spot_formula(spot::formula::eword());
                        break;
                }
            }


            virtual void accept(visitor& v) const;

            type val() const
            {
                return val_;
            }


            const char* val_name() const;

            std::string dump() const;

            /// Get the sole instance of spot::ltl::constant::constant(True).
            static constant* true_instance() { return &true_instance_; }
            /// Get the sole instance of spot::ltl::constant::constant(False).
            static constant* false_instance() { return &false_instance_; }
            /// Get the sole instance of spot::ltl::constant::constant(EmptyWord).
            static constant* empty_word_instance() { return &empty_word_instance_; }


        protected:
            type val_;

        private:
            static constant true_instance_;
            static constant false_instance_;
            static constant empty_word_instance_;
        };

        /// \brief Cast \a f into a constant.
        ///
        /// Cast \a f into a constant iff it is a constant instance.
        /// Return 0 otherwise.  This is faster than \c dynamic_cast.

        inline
        const constant*
        is_constant(const formula* f)
        {
            if (!f->spot_formula().is_constant())
                return 0;
            return static_cast<const constant*>(f);
        }




    }


}
#endif //TEXADA_TEXADA_CONSTANT_H
