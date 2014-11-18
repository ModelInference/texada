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
#include "../trace/event.h"
#include "../checkers/statistic.h"

/**
 * header file for property type mining functions.
 */

namespace texada {

// mine property type with map checker
std::set<std::pair<const spot::ltl::formula*, statistic>> mine_map_property_type(
        std::string formula_string, std::string trace_source);

// mine property type with linear checker
std::set<std::pair<const spot::ltl::formula*, statistic>> mine_lin_property_type(
        std::string formula_string, std::string trace_source);

// mine property type with map checker if use_map is true,
// linear checker otherwise
std::set<std::pair<const spot::ltl::formula*, statistic>> mine_property_type(boost::program_options::variables_map opts);     // need to change return type to return support and confidence information

} /* namespace texada */

#endif /* PROPERTYTYPEMINER_H_ */
