/*
 * formulainstantiator.h
 *
 *  Created on: May 20, 2014
 *      Author: clemieux
 */

#ifndef FORMULAINSTANTIATOR_H_
#define FORMULAINSTANTIATOR_H_

#include <ltlast/allnodes.hh>

namespace texada {

const spot::ltl::formula* instantiate(const spot::ltl::formula*, std::map<std::string, std::string>*);

} /* namespace texada */

#endif /* FORMULAINSTANTIATOR_H_ */
