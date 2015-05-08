/*
 * otfinstantspool.h
 *
 *  Created on: Jul 7, 2014
 *      Author: clemieux
 */

#ifndef OTFINSTANTSPOOL_H_
#define OTFINSTANTSPOOL_H_

#include <ltlast/allnodes.hh>
#include "instantspoolcreator.h"
#include "../trace/event.h"
#include <string>
#include <set>
#include <map>

namespace texada {
using std::vector;

/**
 * Instantiation pool creator which generates each next instantiation on the fly
 */

class otf_instants_pool : public instants_pool_creator {

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

	// keeps track of each "level" we're adding to
	// the instantiation map
	vector<iter_store> iteration_tracker;

	void set_up_iteration_tracker();


public:
	otf_instants_pool(shared_ptr<set<string>>  events, const spot::ltl::formula* f, bool allow_reps, bool opt_order,
	        shared_ptr<spot::ltl::atomic_prop_set> set, vector<string> exclude_events);
	virtual ~otf_instants_pool();
	vector<map<string, string>> return_valid_instants(const spot::ltl::formula * node,
	        set<vector<event>>);
	shared_ptr<map<string,string>>  get_next_instantiation();

};

} /* namespace texada */

#endif /* OTFINSTANTSPOOL_H_ */
