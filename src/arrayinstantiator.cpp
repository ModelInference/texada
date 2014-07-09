/*
 * arrayinstantitator.cpp
 *
 *  Created on: May 21, 2014
 *      Author: clemieux
 */

#include "arrayinstantiator.h"
#include <math.h>

namespace texada {

/**
 * Create instantiator, and fill the array of instantiations
 * @param events events in all traces inputted
 * @param ltlevents set of atomic propositions to be replaced
 */
array_instantiator::array_instantiator(std::set<std::string>& events_,
		spot::ltl::atomic_prop_set ltlevents) :
		formula_vars(ltlevents), unique_events(&events_) {
	f_var_size = formula_vars.size();
	// We are creating a vector with the exact size required to store
	// all instantiations:
	// Each event can appear at each formula variable, so the total
	// size is: # of unique events ^ # of variable in formula
	inst_pool = std::vector<inst_fxn>(pow(unique_events->size(), f_var_size));

}

array_instantiator::~array_instantiator() {
	// TODO Auto-generated destructor stub

}
/**
 * Places all instantiations into the array.
 */
void array_instantiator::instantiate_array() {
	// rename k again
	int num_unique_events = unique_events->size();

	// We must produce all permutations with replacement of length f_var_size
	// of the events. We go through the entire return array multiple times
	// in order to construct all permutations. Each pass adds a "level"
	// of mapping to each map. For the simple example with two bindings
	// x and y and three events a, b, c, the process goes like this:
	// INITIAL ARRAY:
	// |____|____|____|____|____|____|____|____|____|
	// FIRST PASS, level 0:
	// |x->a|x->b|x->c|x->a|x->b|x->c|x->a|x->b|x->c|
	// SECOND PASS, level `:
	// |x->a|x->b|x->c|x->a|x->b|x->c|x->a|x->b|x->c|
	// |y->a|y->a|y->a|y->b|y->b|y->b|y->c|y->c|y->c|
	// at this point, the iteration terminates.

	// "level" of the pass, i.e. how deep in formula_vars we are

	int lvl = 0;
	for (std::set<const spot::ltl::atomic_prop*>::iterator formula_it =
			formula_vars.begin(); formula_it != formula_vars.end();
			++formula_it) {
		assert(lvl < f_var_size);
		// inst_fxn has a string->string map, so get name of atomic prop
		const spot::ltl::atomic_prop* event_var = *formula_it;
		std::string name = event_var->name();
		// now traverse the array to fill this level
		traverse_and_fill(name, lvl, num_unique_events);
		lvl++;
	}
}
/**
 * Traverse and fill the inst_fxn array with the correct mapping, according
 * to how "deep" we are in iteration through the formula variables
 * @param formula_event Name of the event to be the first argument of map
 * @param i depth of iteration through formula variables.
 * @param k number of events
 */
void array_instantiator::traverse_and_fill(std::string formula_event, int lvl,
		int num_unique_events) {
	//indicates when to continue iteration through events
	int switch_var = pow(num_unique_events, lvl);
	// find array size
	//begin with the first event
	std::set<std::string>::iterator event_iterator = unique_events->begin();
	for (std::vector<inst_fxn>::iterator inst_pool_it = inst_pool.begin();
			inst_pool_it != inst_pool.end(); inst_pool_it++) {
		// if we are at switching point, iterate to next event
		if (switch_var == 0) {
			++event_iterator;
			// if we've finished passing through the events,
			// go back to the start
			if (event_iterator == unique_events->end())
				event_iterator = unique_events->begin();
			// and reset the switch to the top value
			switch_var = pow(num_unique_events, lvl);
		}
		inst_pool_it->inst_map.insert(
				std::pair<std::string, std::string>(formula_event,
						*event_iterator));
		switch_var--;

	}
}

std::vector<array_instantiator::inst_fxn> array_instantiator::return_instantiations() const {
	return inst_pool;
}

} /* namespace texada */
