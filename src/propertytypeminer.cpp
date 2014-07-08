/*
 * propertytypeminer.cpp
 *
 *  Created on: Jun 3, 2014
 *      Author: clemieux
 */

#include "propertytypeminer.h"

#include <fstream>

#include <ltlparse/public.hh>
#include <ltlvisit/apcollect.hh>
#include <ltlvisit/tostring.hh>

#include "simpleparser.h"
#include "arrayinstantiator.h"
#include "formulatracechecker.h"
#include "formulainstantiator.h"


namespace texada {

//TODO: using std::things, maybe
//TODO: refactor mine_map_property_type into this

std::set<const spot::ltl::formula*> mine_property_type(std::string formula_string, std::string trace_source){

	//parse the ltl formula
	//TODO: call pel something else
	spot::ltl::parse_error_list pel;
	const spot::ltl::formula* formula = spot::ltl::parse(formula_string, pel);
	//std::cout << "## Number of parser errors: "<<pel.size() << "\n";
	//TODO: get some way for the user to check this.

	// parse file
	std::ifstream infile(trace_source);
	simple_parser parser =  simple_parser();
	std::set<std::vector<string_event> >  trace_set = parser.parse(infile);
	std::set<std::string>  event_set = parser.return_events();

	// create the set of formula's variables
	spot::ltl::atomic_prop_set * variables = spot::ltl::atomic_prop_collect(formula);
	//std::cout << "## Number of variables: " <<variables->size() << "\n";
	//create the instantiation array

	// create an array of formula instantiations
	//TODO: rename array_inst to instances_pool or something
	array_instantiator instantiator = array_instantiator(event_set, *variables);
	instantiator.instantiate_array();
	std::vector<array_instantiator::inst_fxn> instantiations = instantiator.return_instantiations();



	//size of instantiations, TODO rename
	int size = instantiations.size();

	//## debugging
	//int numvalid;

	// check all valid instantiations on each trace
	for(std::set<std::vector<string_event> >::iterator it = trace_set.begin();
			it !=trace_set.end(); it++){
		std::vector<texada::string_event> current_vec = *it;
		texada::string_event* current_trace = &current_vec[0];
		check_instants_on_trace(instantiations,formula,current_trace);
		//## the rest is debugging
		/*numvalid =0;
		for (int i = 0; i <size; i++){
			if (instantiations[i].validity){
				std::cout << to_string(instantiate(formula,instantiations[i].inst_map))<< "\n";
				numvalid++;
			}
		}
		std::cout << "Numvalid: "<< numvalid << "\n";*/
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
