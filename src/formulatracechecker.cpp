/*
 * formulatracechecker.cpp
 *
 *  Created on: May 30, 2014
 *      Author: clemieux
 */

#include "formulatracechecker.h"
#include "formulainstantiator.h"
#include "arraytracechecker.h"
#include "maptracechecker.h"

namespace texada {

/**
 * Checks whether the given instantitations of a formula hold on the
 * given trace -- if they do not, the validity of the function is set
 * to false. The original instantiation array is the one returned.
 * @param instantiations all instantiation function mappings
 * @param formula the LTL formula to instantiate
 * @param trace the trace to check on
 * @return updated instantiations, with invalid ones set to false
 */
std::vector<array_instantiator::inst_fxn> check_instants_on_trace(std::vector<array_instantiator::inst_fxn>& instantiations,
		const spot::ltl::formula* formula, const string_event* trace){
	array_trace_checker checker = array_trace_checker();
	int size = instantiations.size();
	for (int i=0; i<size; i++){
		// if it's invalid, ignore
		if (!(instantiations[i].validity)) continue;
		std::map<std::string, std::string> current_map = instantiations[i].inst_map;
		const spot::ltl::formula* instantiated_form =	instantiate(formula, current_map);
		instantiations[i].validity = checker.check(instantiated_form,trace);
	}
	return instantiations;

}

/**
 * Checks whether the given instantitations of a formula hold on the
 * given trace -- if they do not, the validity of the function is set
 * to false. The original instantiation array is the one returned.
 * @param instantiations all instantiation function mappings
 * @param formula the LTL formula to instantiate
 * @param trace the trace to check on
 * @return updated instantiations, with invalid ones set to false
 */
std::vector<array_instantiator::inst_fxn> check_instants_on_trace(std::vector<array_instantiator::inst_fxn>& instantiations,
		const spot::ltl::formula* formula, std::map<string_event,std::vector<long>>* trace){
	map_trace_checker checker = map_trace_checker(trace);
	int inst_size = instantiations.size();
	for (int i=0; i<inst_size; i++){
		// if it's invalid, ignore
		if (!(instantiations[i].validity)) continue;
		std::map<std::string, std::string> current_map = instantiations[i].inst_map;
		const spot::ltl::formula* instantiated_form =	instantiate(formula, current_map);
		instantiations[i].validity = checker.check_on_trace(instantiated_form);
	}
	return instantiations;

}

} /* namespace texada */
