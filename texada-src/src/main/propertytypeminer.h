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
#include <boost/program_options.hpp>
#include <vector>
#include "../parsing/stringevent.h"

/**
 * header file for property type mining functions.
 */

namespace texada {

// mine property type with map checker
std::set<const spot::ltl::formula*> mine_map_property_type(
		std::string formula_string, std::string trace_source);

// mine property type with linear checker
std::set<const spot::ltl::formula*> mine_lin_property_type(
		std::string formula_string, std::string trace_source);

// mine property type with map checker if use_map is true,
// linear checker otherwise
std::set<const spot::ltl::formula*> mine_property_type(boost::program_options::variables_map opts);

} /* namespace texada */

#endif /* PROPERTYTYPEMINER_H_ */
