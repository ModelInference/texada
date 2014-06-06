/*
 * arrayinstantitator.h
 *
 *  Created on: May 21, 2014
 *      Author: clemieux
 */

#ifndef ARRAYINSTANTITATOR_H_
#define ARRAYINSTANTITATOR_H_

#include <set>
#include <string>
#include <map>
#include <ltlast/allnodes.hh>
#include <ltlvisit/apcollect.hh>

namespace texada {

/**
 * class which creates an array of instantiating functions (i.e. maps) given
 * the number of variables in an LTL formula and a set of events from traces.
 * [it now actually returns a vector, but that's just a wrapper for an array...]
 */
class array_instantiator {

	//declared up here so that return_instantiations can be
private:
	int size;
public:

	struct inst_fxn{
		std::map<std::string, std::string> inst_map;
		bool validity = true;
	};

	array_instantiator(std::set<std::string>& events,
			spot::ltl::atomic_prop_set);
	virtual ~array_instantiator();
	void instantiate_array();
	void traverse_and_fill(std::string event, int i, int k);
	std::vector<inst_fxn> return_instantiations() const;

private:
	std::vector<inst_fxn> return_array;
	spot::ltl::atomic_prop_set formula_vars;
	std::set<std::string> events;
	int length;

};

} /* namespace texada */

#endif /* ARRAYINSTANTITATOR_H_ */
