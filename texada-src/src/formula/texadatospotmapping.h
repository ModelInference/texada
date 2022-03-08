/*
 * texadatospotmapping.h
 *
 * Created on: December 3, 2021
 * Author: cjbanks
 */

#ifndef TEXADATOSPOTMAPPING_H_
#define TEXADATOSPOTMAPPING_H_

#include <memory>
#include <vector>
#include <map>
#include "texadatospotformula.h"
#include "texada_atomicprop.h"
#include "texada_constant.h"
#include "texada_environment.h"
#include "texada_constant.h"
#include "texada_bunop.h"
#include "texada_unop.h"
#include "texada_binop.h"
#include "texada_multop.h"
#include <spot/misc/common.hh>
#include <spot/tl/apcollect.hh>
#include <spot/tl/defaultenv.hh>
#include <spot/tl/formula.hh>
#include <spot/tl/parse.hh>
#include <spot/tl/print.hh>
#include <spot/tl/nenoform.hh>
#include <spot/misc/location.hh>
#include <spot/tl/simplify.hh>

/**
 * Maps SPOT 2.X bindings to existing Texada functions. This functions as a wrapper for 
 * SPOT to increase compatibility of Texada to future updates.
 *
 * includes functionality for parsing given formulas into SPOT formulas and generating a Texada formula base class
 * for inheriting formula types.
 */

namespace texada {	
		namespace ltl {
	
		// clone visitor wrapping
		class clone_visitor : public visitor {
			public:
                clone_visitor();
                ~clone_visitor() override = default;

                const formula* result() const;

				void visit(const atomic_prop* ap);
                void visit(const unop* uo);
                void visit(const binop* bo);
                void visit(const multop* mo);
                void visit(const constant* c);
                void visit(const bunop* c);
                void visit(const formula* f);



            virtual const formula* recurse(const formula* f);

			protected:
				const formula* result_;

		};
         
	 
		//define atomic_prop_set as the string set of atomic propositions     
		typedef std::set<const atomic_prop*> atomic_prop_set;


		//collect atomic propositions from formula
		atomic_prop_set* atomic_prop_collect(const formula* f, atomic_prop_set* texada_prop_set = 0);


		//default environment wrapper
		class default_environment : public environment {

			public:
                ~default_environment() override;
                const formula* require(const std::string& prop_str) override;
				virtual const std::string& name() const;


    			/// Get the sole instance of spot::default_environment.
                static default_environment& instance();

			protected:
				default_environment();
		};

		// print SPOT formula
		std::string to_string(const formula* node);

		//parse error list as texada type
		typedef std::pair<spot::location, std::string> one_parse_error;
		typedef std::list<one_parse_error> parse_error_list;

		//create formula and cast to formula class
		formula* recurse_form(const spot::formula& spot_formula);

		//parse ltl_string into formula
		formula* parse(const std::string& ltl_string, parse_error_list& error_list);
}
}
	
#endif /*TEXADATOSPOTMAPPING_H_ */
			



