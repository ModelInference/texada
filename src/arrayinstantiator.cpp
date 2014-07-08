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
			formula_vars(ltlevents), events(events_){
	length = formula_vars.size();
	//todo: comment on math, why
	int k = events.size();
	size = pow(k,length);
	return_array = std::vector<inst_fxn>(size);

}

array_instantiator::~array_instantiator() {
	// TODO Auto-generated destructor stub

}
/**
 * Places all instantiations into the array.
 */
void array_instantiator::instantiate_array(){
	// rename k again
	int k = events.size();

	//TODO: rename length variable to something else
	//TODO: e
	// We must produce all permutations with replacement of length length
	// of the events. We go through the entire return array multiple times
	// in order to construct all permutations. Each pass adds a "level"
	// of mapping to each map. For the simple example with two bindings
	// x and y and three events a, b, c, the process goes like this:
	// INITIAL ARRAY:
	// |____|____|____|____|____|____|____|____|____|
	// FIRST PASS, i=0:
	// |x->a|x->b|x->c|x->a|x->b|x->c|x->a|x->b|x->c|
	// SECOND PASS, i=1:
	// |x->a|x->b|x->c|x->a|x->b|x->c|x->a|x->b|x->c|
	// |y->a|y->a|y->a|y->b|y->b|y->b|y->c|y->c|y->c|
	// at this point, the iteration terminates.

	// i is the "level" of the pass, i.e. how deep in formula_vars we are
	// TODO: rename i to level
	int i = 0;
	for (std::set<const spot::ltl::atomic_prop*>::iterator
			formula_it=formula_vars.begin(); formula_it!=formula_vars.end();
			++formula_it){
		//we really should never get to here, as formula_vars ends at length
		//TODO: assert i<length
		if (i >= length) break;
		// since inst_fxn has a string->string map, we obtain the name
		// of the atomic_prop the iterator is pointing to.
		const spot::ltl::atomic_prop* event_var = *formula_it;
		std::string name = event_var->name();
		// now traverse the array to fill this level
		traverse_and_fill(name, i, k);
		i++;
	}
}
/**
 * Traverse and fill the inst_fxn array with the correct mapping, according
 * to how "deep" we are in iteration through the formula variables
 * @param formula_event Name of the event to be the first argument of map
 * @param i depth of iteration through formula variables.
 * @param k number of events
 */
void array_instantiator::traverse_and_fill(std::string formula_event, int i,
		int k){
	//indicates when to continue iteration through events
	int switch_var = pow(k,i);
	// find array size
	//begin with the first event
	std::set<std::string>::iterator event_iterator = events.begin();
	for (int j = 0; j < return_array.size() ; j++){
		// if we are at switching point, iterate to next event
		if (switch_var == 0){
			++event_iterator;
			// if we've finished passing through the events,
			// go back to the start
			if (event_iterator == events.end()) event_iterator = events.begin();
			// and reset the switch to the top value
			switch_var = pow(k,i);
		}
		return_array[j].inst_map.insert(std::pair<std::string,
				std::string>(formula_event,*event_iterator));
		switch_var--;

	}
}

std::vector<array_instantiator::inst_fxn>  array_instantiator::return_instantiations() const{
	return return_array;
}

} /* namespace texada */
