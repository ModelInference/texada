/*
 * texadatospotformula.h
 *
 * Created on: December 9, 2021
 * Author: cjbanks
 */

#ifndef TEXADATOSPOTFORMULA_H_
#define TEXADATOSPOTFORMULA_H_

#include <set>
#include <iostream>
#include "texadatospotrefformula.h"
#include "texadapredecl.h"
#include "texadavisitor.h"
#include <spot/tl/formula.hh>
#include <spot/misc/common.hh>

namespace texada {
                namespace ltl {

		typedef std::set<const atomic_prop*> atomic_prop_set;

		class formula : public refformula {

		public:
		    //formula type
		    enum type
		            {
		        Constant,
		        AtomicProp,
		        UnOp,
                BinOp,
                MultOp,
                BUnOp,
		        AutomatOp,
		        True,
		        False,
		        EmptyWord,
		        };



            //formula constructors
		    formula() : f_(), ref_counter_(0)
		    {}


		    formula(const formula& f) : f_(f.f_), ref_counter_(0)
		    {
		        //f_type = kind();
		    }

		    explicit formula(spot::formula& ff) : f_(ff), ref_counter_(0)
		    {}

		    explicit formula(const spot::formula &ff) : f_(ff), ref_counter_(0)
		    {}

		    explicit formula(std::string& f_str) : f_(spot::formula::ap(f_str)), ref_counter_(0)
		    {}

		    //~formula();


            //copied from Spot 1.99
            struct ltl_prop
            {
                // All properties here should be expressed in such a a way
                // that property(f && g) is just property(f)&property(g).
                // This allows us to compute all properties of a compound
                // formula in one operation.
                //
                // For instance we do not use a property that says "has
                // temporal operator", because it would require an OR between
                // the two arguments.  Instead we have a property that
                // says "no temporal operator", and that one is computed
                // with an AND between the arguments.
                //
                // Also choose a name that makes sense when prefixed with
                // "the formula is".
                bool boolean:1;		   // No temporal operators.
                bool sugar_free_boolean:1; // Only AND, OR, and NOT operators.
                bool in_nenoform:1;	   // Negative Normal Form.
                bool syntactic_si:1;	   // LTL-X or siPSL
                bool sugar_free_ltl:1;	   // No F and G operators.
                bool ltl_formula:1;	   // Only LTL operators.
                bool psl_formula:1;	   // Only PSL operators.
                bool sere_formula:1;	   // Only SERE operators.
                bool finite:1;		   // Finite SERE formulae, or Bool+X forms.
                bool eventual:1;	   // Purely eventual formula.
                bool universal:1;	   // Purely universal formula.
                bool syntactic_safety:1;   // Syntactic Safety Property.
                bool syntactic_guarantee:1;   // Syntactic Guarantee Property.
                bool syntactic_obligation:1;  // Syntactic Obligation Property.
                bool syntactic_recurrence:1;  // Syntactic Recurrence Property.
                bool syntactic_persistence:1; // Syntactic Persistence Property.
                bool not_marked:1;	   // No occurrence of EConcatMarked.
                bool accepting_eword:1;	   // Accepts the empty word.
                bool lbt_atomic_props:1;   // Use only atomic propositions like p42.
                bool spin_atomic_props:1;  // Use only spin-compatible atomic props.
            };
            union
            {
                // Use an unsigned for fast computation of all properties.
                unsigned props;
                ltl_prop is;
            };

            bool is_boolean() const{
                return is.boolean;
            }



            //clones the formula object and increments reference counter by 1
            const formula* clone() const;

            //return formula type
		    formula::type kind() const;

            //return if formula (constant) is True, False, or Empty
		    formula::type val() const;

		    //hash function wrapper
		    std::size_t hash() const;

            //returns SPOT formula object of formula
		    spot::formula spot_formula() const;

		    //dummy destroy function
		    void destroy() const;

            //used in visitors for formula traversal
		    virtual void accept(visitor& v) const;

            //sets the private variable f_, SPOT formula
		    void set_spot_formula(spot::formula f_temp);

		    //define vector of formula
		    //list of formulae
		    typedef std::vector<const formula*> vec;

            /**
             * Comparison variables for formula class
             *
             */
            bool operator<(const formula& other) const noexcept
            {


                spot::formula in_formula = other.f_;

                if (SPOT_UNLIKELY(!in_formula))
                    return false;
                if (SPOT_UNLIKELY(!this->f_))
                    return true;
                if (this->f_.id() < in_formula.id())
                    return true;
                if (this->f_.id() > in_formula.id())
                    return false;

                return &this->f_ < &in_formula;;

            }


            bool operator<=(const formula& other) const noexcept
            {
                spot::formula in_formula = other.f_;
                return &this->f_ == &in_formula || &this->f_ < &in_formula;
            }

            bool operator>(const formula& other) const noexcept
            {
                spot::formula in_formula = other.f_;
                return !(&this->f_ <= &in_formula);
            }

            bool operator>=(const formula& other) const noexcept
            {
                spot::formula in_formula = other.f_;
                return !(&this->f_ < &in_formula);

            }

            bool operator==(const formula& other) const noexcept
            {
                spot::formula in_formula = other.f_;
                return &in_formula == &this->f_;
            }

            bool operator==(std::nullptr_t) const noexcept
            {
                return &this->f_ == nullptr;
            }

            bool operator!=(const formula& other) const noexcept
            {
                spot::formula in_formula = other.f_;
                return &in_formula != &this->f_;
            }


            bool operator!=(std::nullptr_t) const noexcept
            {
                return &this->f_ != nullptr;
            }

            operator bool() const
            {
                return &this->f_ != nullptr;
            }



        protected:
            //reference counting help in refformula in Spot 1.2.6

            /// \brief increment reference counter if any
            virtual void ref_() const;
            /// \brief decrement reference counter if any, return true when
            /// the instance must be deleted (usually when the counter hits 0).
            virtual bool unref_() const;


            spot::formula f_;

            //number of references to this formula
            unsigned ref_count_() const;

        private:
            mutable unsigned ref_counter_;
            //SPOT formula wrapper maintains

        };


				
		//automatop formula
		class automatop : public formula {
			public:
			private:

		};

		}
}

#endif /*TEXADATOSPOTFORMULA_H_ */
