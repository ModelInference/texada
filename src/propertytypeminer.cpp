/*
 * propertytypeminer.cpp
 *
 *  Created on: Jun 3, 2014
 *      Author: clemieux
 */

#include "propertytypeminer.h"
#include <fstream>
#include <ltlparse/public.hh>
#include "simpleparser.h"
#include <ltlvisit/apcollect.hh>
#include "arrayinstantiator.h"
#include "formulatracechecker.h"
#include "formulainstantiator.h"
#include "ltlvisit/tostring.hh"

namespace texada {

void mine_property_type(std::string formula_string, std::string trace_source){

	//parse the ltl formula
	spot::ltl::parse_error_list pel;
	const spot::ltl::formula* formula = spot::ltl::parse(formula_string, pel);

	// currently just using simple parser, assumedly could replace this by a
	// more complex parser once we have one
	std::ifstream infile(trace_source);
	simple_parser * parser = new simple_parser();
	std::set<std::vector<string_event> > * trace_set = parser->parse(infile);
	std::set<std::string> * event_set = parser->return_events();
	delete parser;

	spot::ltl::atomic_prop_set * variables = spot::ltl::atomic_prop_collect(formula);
	//create the instantiation array
	array_instantiator * instantiator = new array_instantiator(event_set, variables);
	instantiator->instantiate_array();
	array_instantiator::inst_fxn* instantiations = instantiator->return_instantiations();
	delete instantiator;

	//number of events
	int k = event_set->size();
	//number of bindings
	int n = variables->size();

	for(std::set<std::vector<string_event> >::iterator it = trace_set->begin();
			it !=trace_set->end(); it++){
		std::vector<texada::string_event> current_vec = *it;
		texada::string_event* current_trace = &current_vec[0];
		check_instants_on_trace(instantiations,pow(k,n),formula,current_trace);
	}


	//TODO: some other method to output
	std::cout << "Valid instantiations: \n";
	for (int i = 0; i <pow(k,n);i++){
		if ((instantiations+i)->validity){
			const spot::ltl::formula * valid_form =
					instantiate(formula, &((instantiations+i)->inst_map));
			std::cout << spot::ltl::to_string(valid_form) << "\n";
		}
	}
	std::cout << "If no instantiations are listed, no valid instantiations found. \n";

    std::cout << "Deleting event set... \n";
	delete event_set;
    std::cout << "Deleted event set. Deleting variables... \n";
	delete variables;
	std::cout << instantiations;
}

class property_type_miner {
public:
	property_type_miner();
	virtual ~property_type_miner();
};


} /* namespace texada */
