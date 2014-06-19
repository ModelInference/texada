/*
 * tempmappropminer.h
 *
 *  Created on: Jun 18, 2014
 *      Author: clemieux
 */

#ifndef TEMPMAPPROPMINER_H_
#define TEMPMAPPROPMINER_H_

#include <set>
#include <ltlast/allnodes.hh>

namespace texada {

std::set<const spot::ltl::formula*> mine_map_property_type(std::string formula_string, std::string trace_source);
} /* namespace texada */

#endif /* TEMPMAPPROPMINER_H_ */
