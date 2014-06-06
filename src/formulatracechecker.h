/*
 * formulatracechecker.h
 *
 *  Created on: May 30, 2014
 *      Author: clemieux
 */

#ifndef FORMULATRACECHECKER_H_
#define FORMULATRACECHECKER_H_
#include "arrayinstantiator.h"
#include "stringevent.h"

namespace texada {

std::vector<array_instantiator::inst_fxn> check_instants_on_trace(std::vector<array_instantiator::inst_fxn>&,
			const spot::ltl::formula*, const string_event*);


} /* namespace texada */
#endif /* FORMULATRACECHECKER_H_ */
