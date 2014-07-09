/*
 * propertytypeminer.cpp
 *
 *  Created on: Jun 3, 2014
 *      Author: clemieux
 */

#include "propertytypeminer.h"

#include <fstream>
#include <assert.h>

#include <ltlparse/public.hh>
#include <ltlvisit/apcollect.hh>
#include <ltlvisit/tostring.hh>

#include "simpleparser.h"
#include "arrayinstantiator.h"
#include "maptracechecker.h"
#include "lineartracechecker.h"
#include "apsubbingcloner.h"

namespace texada {
using std::set;
using std::vector;
using std::string;
using std::map;
//TODO: using std::things, maybe

set<const spot::ltl::formula*> mine_lin_property_type(
		string formula_string, string trace_source) {
	return mine_property_type(formula_string, trace_source, false);
}

set<const spot::ltl::formula*> mine_map_property_type(
		string formula_string, string trace_source) {
	return mine_property_type(formula_string, trace_source, true);
}

/**
 *
 * @param formula_string the string form of the ltl formula to be mined
 * @param trace_source the input source of the trace
 * @param use_map use map miner if true, linear miner otherwise
 * @return valid instantiations of the inputted formula on inputted trace set
 */
set<const spot::ltl::formula*> mine_property_type(
		string formula_string, string trace_source, bool use_map) {

	//parse the ltl formula
	spot::ltl::parse_error_list parse_errs;
	const spot::ltl::formula* formula = spot::ltl::parse(formula_string,
			parse_errs);
	assert(parse_errs.size() == 0);

	// parse file
	std::ifstream infile(trace_source);
	simple_parser * parser = new simple_parser();
	set<vector<string_event> > vector_trace_set;
	set<map<string_event, vector<long>>>map_trace_set;
	if (use_map) {
		parser->parse_to_map(infile);
		map_trace_set = parser->return_map_trace();
	} else {
		parser->parse_to_vector(infile);
		vector_trace_set = parser->return_vec_trace();
	}
	set<string> event_set = parser->return_events();
	//done parsing file, clean up parser
	delete parser;
	parser = NULL;

	// create the set of formula's variables
	spot::ltl::atomic_prop_set * variables = spot::ltl::atomic_prop_collect(
			formula);

	// create all possible instantiations of formula
	array_instantiator instantiator = array_instantiator(event_set, *variables);
	instantiator.instantiate_array();
	vector<array_instantiator::inst_fxn> all_instants =
			instantiator.return_instantiations();

	// size of all_instant
	int instantiations_size = all_instants.size();

	// check all valid instantiations on each trace
	if (!vector_trace_set.empty()) {
	    linear_trace_checker checker;
		for (set<vector<string_event> >::iterator it =
				vector_trace_set.begin(); it != vector_trace_set.end(); it++) {
			vector<texada::string_event> current_vec = *it;
			string_event* current_trace = &current_vec[0];
			checker.check_instants_on_trace(all_instants, formula, current_trace);
		}
	} else {
		for (set<map<string_event, vector<long>>>::iterator it =
		map_trace_set.begin(); it != map_trace_set.end(); it++) {
			map<string_event, vector<long>> current_map = *it;
			map_trace_checker checker = map_trace_checker(&current_map);
			checker.check_instants_on_trace(all_instants,formula);
		}
	}

	set<const spot::ltl::formula*> return_set;

	for (int i = 0; i < instantiations_size; i++) {
		if (all_instants[i].validity) {
			const spot::ltl::formula * valid_form = instantiate(formula,
					all_instants[i].inst_map);
			return_set.insert(valid_form);
		}
	}
	return return_set;

}

} /* namespace texada */
