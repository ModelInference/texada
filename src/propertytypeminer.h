/*
 * propertytypeminer.h
 *
 *  Created on: Jun 3, 2014
 *      Author: clemieux
 */

#ifndef PROPERTYTYPEMINER_H_
#define PROPERTYTYPEMINER_H_

#include <ltlast/predecl.hh>

#include <set>
#include <vector>
#include "stringevent.h"

namespace texada {

std::set<const spot::ltl::formula*> mine_map_property_type(
		std::string formula_string, std::string trace_source);

std::set<const spot::ltl::formula*> mine_lin_property_type(
		std::string formula_string, std::string trace_source);

std::set<const spot::ltl::formula*> mine_property_type(
		std::string formula_string, std::string trace_source, bool use_map);

} /* namespace texada */

#endif /* PROPERTYTYPEMINER_H_ */
