/*
 * arrayinstantitator.cpp
 *
 *  Created on: May 21, 2014
 *      Author: clemieux
 */

#include "arrayinstantitator.h"
#include <math.h>

namespace texada {

/**
 * Create instantiator, and fill the array of instantiations
 * @param events events in all traces inputted
 * @param ltlevents set of atomic propositions to be replaced
 */
array_instantiator::array_instantiator(std::set<std::string>* events,
		std::set<const spot::ltl::atomic_prop*> *ltlevents) :
			formula_vars(ltlevents) {
	//setting up some things
	length = formula_vars->size();
	int k = events->size();
	int array_size = pow(k,length);
	return_array = new inst_fxn[array_size];
	//TODO: finish this
	//next I iterate through the array multiple times
	int i = 0;
	for (std::set<const spot::ltl::atomic_prop*>::iterator
			formula_it=formula_vars->begin(); formula_it!=formula_vars->end();
			++formula_it){
		//we really should never get here
		if (i >= length) break;
		const spot::ltl::atomic_prop* event_var = *formula_it;
		std::string name = event_var->name();
		traverse_and_fill(name, i);
	}

}

array_instantiator::~array_instantiator() {
	// TODO Auto-generated destructor stub
	// TODO I probably shouldn't delete that new array since it's intended
	// to be used??
}

void array_instantiator::traverse_and_fill(std::string formula_event, int i){

}

} /* namespace texada */
