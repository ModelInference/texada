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

std::set<const spot::ltl::formula*> mine_property_type(std::string formula_string, std::string trace_source){

	//parse the ltl formula
	spot::ltl::parse_error_list pel;
	const spot::ltl::formula* formula = spot::ltl::parse(formula_string, pel);
	//std::cout << "## Number of parser errors: "<<pel.size() << "\n";
	//TODO: get some way for the user to check this.

	// currently just using simple parser, assumedly could replace this by a
	// more complex parser once we have one
	std::ifstream infile(trace_source);
	simple_parser parser =  simple_parser();
	std::set<std::vector<string_event> >  trace_set = parser.parse(infile);
	std::set<std::string>  event_set = parser.return_events();

	spot::ltl::atomic_prop_set * variables = spot::ltl::atomic_prop_collect(formula);
	//std::cout << "## Number of variables: " <<variables->size() << "\n";
	//create the instantiation array
	array_instantiator instantiator = array_instantiator(event_set, *variables);
	instantiator.instantiate_array();
	std::vector<array_instantiator::inst_fxn> instantiations = instantiator.return_instantiations();

	//debugging stuff below
	/*
	for (std::map<std::string, std::string>::iterator it =instantiations[0].inst_map.begin();
			it !=instantiations[0].inst_map.end(); it++){
		std::cout << it->first << " -> " << it->second << "\n";
	}*/

	//number of events
	int k = event_set.size();
	//number of bindings
	int n = variables->size();
	//size of instnatiations
	int size = instantiations.size();

	//## debugging
	int numvalid;

	for(std::set<std::vector<string_event> >::iterator it = trace_set.begin();
			it !=trace_set.end(); it++){
		std::vector<texada::string_event> current_vec = *it;
		texada::string_event* current_trace = &current_vec[0];
		check_instants_on_trace(instantiations,formula,current_trace);
		numvalid =0;
		for (int i = 0; i <size; i++){
			if (instantiations[i].validity){
				std::cout << to_string(instantiate(formula,instantiations[i].inst_map))<< "\n";
				numvalid++;
			}
		}
		std::cout << "Numvalid: "<< numvalid << "\n";
	}

	std::set<const spot::ltl::formula*>  return_set;



	for (int i = 0; i <size; i++){
		if (instantiations[i].validity){
			const spot::ltl::formula * valid_form =
					instantiate(formula, instantiations[i].inst_map);
			return_set.insert(valid_form);
		}
	}
    return return_set;


}



} /* namespace texada */
