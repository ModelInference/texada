/*
 * truncatingchecker.h
 *
 *  Created on: Jul 7, 2014
 *      Author: clemieux
 */

#ifndef TRUNCATINGCHECKER_H_
#define TRUNCATINGCHECKER_H_

#include <ltlast/allnodes.hh>
#include "lineartracechecker.h"
#include <string>
#include <set>
#include <map>

namespace texada {

/**
 * Checker combining instantiation creation and checking so
 * as to truncate the instantiation space
 */

class truncating_checker {

private:

    // structure to help create instantiations on the fly,
    // to allow a single pass through.
	struct iter_store{
	    // at which points do we need to switch the
	    // mapto variable
		int switchvar;
		// formula variable which will be substituted
		string mapfrom;
		// current position in the set of substituting
		// variables
		set<string>::iterator mapto;
	};

	// all unique events in the traces
	set<string> events;

	// keeps track of each "level" we're adding to
	// the instantiation map
	vector<iter_store> iteration_tracker;

	void set_up_iteration_tracker(const spot::ltl::formula* node);

	bool meets_trunc_criteria(unsigned long pos);

public:
	truncating_checker(const spot::ltl::formula* node, set<string>);
	virtual ~truncating_checker();
	// For now, using traces in linear form
	vector<map<string, string>> return_valid_instants(const spot::ltl::formula * node,
	        set<vector<string_event>>);
};

} /* namespace texada */

#endif /* TRUNCATINGCHECKER_H_ */
