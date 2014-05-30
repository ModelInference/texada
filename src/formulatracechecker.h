/*
 * formulatracechecker.h
 *
 *  Created on: May 30, 2014
 *      Author: clemieux
 */

#ifndef FORMULATRACECHECKER_H_
#define FORMULATRACECHECKER_H_
#include "arrayinstantiator.h"

namespace texada {

	array_instantiator::inst_fxn* check_instants_on_trace(array_instantiator::inst_fxn*, int,
			const spot::ltl::formula*, const std::string*);


} /* namespace texada */
#endif /* FORMULATRACECHECKER_H_ */
