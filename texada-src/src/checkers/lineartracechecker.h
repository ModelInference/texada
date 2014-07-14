/*
 * lineartracechecker.h
 *
 *  Created on: May 19, 2014
 *      Author: clemieux
 */

#ifndef LINEARTRACECHECKER_H_
#define LINEARTRACECHECKER_H_

#include <ltlast/allnodes.hh>
#include "../parsing/stringevent.h"
#include <iostream>

#include "../instantiation-tools/instantspoolcreator.h"

namespace texada {


/**
 * Class to check whether an LTL formula is true on a trace in the
 * form of a vector.
 *
 */
class linear_trace_checker {
public:
	linear_trace_checker();
	virtual ~linear_trace_checker();

	bool check(const spot::ltl::formula* node, const string_event *trace);

	shared_ptr<vector<instants_pool_creator::inst_fxn>> check_instants_on_trace(shared_ptr<vector<instants_pool_creator::inst_fxn>> instantiations,
			const spot::ltl::formula* formula, const string_event* trace);
private:
    bool check(const spot::ltl::atomic_prop* node, const string_event *trace);
    bool check(const spot::ltl::constant* node, const string_event *trace);
    bool check(const spot::ltl::binop* node, const string_event *trace);
    bool check(const spot::ltl::unop* node,  const string_event *trace);
    bool check(const spot::ltl::multop* node,  const string_event *trace);

    /**
     * both the next types are not supported.
     */
    bool check(const spot::ltl::automatop* node) {
    	std::cerr << "Type automatop unsupported. \n";
    	    	return false;
    }
    bool check(const spot::ltl::bunop* node) {
    	std::cerr << "Type bunop unsupported. \n";
    	return false;
    }



};

} /* namespace texada */

#endif /* LINEARTRACECHECKER2_H_ */
