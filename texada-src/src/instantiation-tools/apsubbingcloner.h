/*
 * apsubbingcloner.h
 *
 *  Created on: May 20, 2014
 *      Author: clemieux
 */

#ifndef APSUBBINGCLONER_H_
#define APSUBBINGCLONER_H_

#include <ltlast/allnodes.hh>

/**
 * Used to instantiate a property type given an instantiation function.
 * I.e. returns a clone of the formula representing the property type
 * and replaces each atomic prop with the one indicated in the inputted map
 */

namespace texada {

const spot::ltl::formula* instantiate(const spot::ltl::formula*, std::map<std::string, std::string>&);

} /* namespace texada */

#endif /* FORMULAINSTANTIATOR_H_ */
