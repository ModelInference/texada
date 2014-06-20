/*
 * tempmappropminer.cpp
 *
 *  Created on: Jun 18, 2014
 *      Author: clemieux
 */

#include "tempmappropminer.h"
#include "propertytypeminer.h"

#include <fstream>

#include <ltlparse/public.hh>
#include <ltlvisit/apcollect.hh>
#include <ltlvisit/tostring.hh>

#include "simpleparser.h"
#include "arrayinstantiator.h"
#include "maptracechecker.h"
#include "formulainstantiator.h"


namespace texada {

std::set<const spot::ltl::formula*> mine_map_property_type(std::string formula_string, std::string trace_source){

	//parse the ltl formula
	spot::ltl::parse_error_list pel;
	const spot::ltl::formula* formula = spot::ltl::parse(formula_string, pel);
	//std::cout << "## Number of parser errors: "<<pel.size() << "\n";
	//TODO: get some way for the user to check this.

	// currently just using simple parser, assumedly could replace this by a
	// more complex parser once we have one
	std::ifstream infile(trace_source);
	simple_parser parser =  simple_parser();
	std::set<std::map<string_event,std::vector<long>> >  trace_set = parser.parse_to_map(infile);
	std::set<std::string>  event_set = parser.return_events();

	spot::ltl::atomic_prop_set * variables = spot::ltl::atomic_prop_collect(formula);
	//std::cout << "## Number of variables: " <<variables->size() << "\n";
	//create the instantiation array
	array_instantiator instantiator = array_instantiator(event_set, *variables);
	instantiator.instantiate_array();
	std::vector<array_instantiator::inst_fxn> instantiations = instantiator.return_instantiations();


	//size of instnatiations
	int size = instantiations.size();



	for(std::set<std::map<string_event,std::vector<long>> >::iterator it = trace_set.begin();
			it !=trace_set.end(); it++){
		std::map<string_event,std::vector<long>> current_trace = *it;
		map_trace_checker checker = map_trace_checker(current_trace);
		int size = instantiations.size();
		for (int i=0; i<size; i++){
				// if it's invalid, ignore
				if (!(instantiations[i].validity)) continue;
				std::map<std::string, std::string> current_map = instantiations[i].inst_map;
				const spot::ltl::formula* instantiated_form =	instantiate(formula, current_map);
				instantiations[i].validity = checker.check_on_trace(instantiated_form);
			}
	}

	std::set<const spot::ltl::formula*>  return_set;



	for (int i = 0; i <size; i++){
		if (instantiations[i].validity){
			const spot::ltl::formula * valid_form =
					instantiate(formula, instantiations[i].inst_map);
			//std::cout << spot::ltl::to_string(valid_form) << "\n";
			return_set.insert(valid_form);
		}
	}
    return return_set;


}



} /* namespace texada */
