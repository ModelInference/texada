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
array_instantitator::array_instantitator(std::set<std::string>* events,
		std::set<const spot::ltl::atomic_prop*> *ltlevents) :
			formula_vars(ltlevents) {
	length = formula_vars->size();
	int k = events->size();
	int array_size = pow(k,length);
	return_array = new inst_fxn[array_size];
	//TODO: finish this

}

array_instantitator::~array_instantitator() {
	// TODO Auto-generated destructor stub
	// TODO I probably shouldn't delete that new array since it's intended
	// to be used??
}


} /* namespace texada */
