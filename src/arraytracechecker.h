/*
 * arraytracechecker2.h
 *
 *  Created on: May 19, 2014
 *      Author: clemieux
 */

#ifndef ARRAYTRACECHECKER_H_
#define ARRAYTRACECHECKER_H_

#include <ltlast/allnodes.hh>
#include <string>
#include <iostream>

namespace texada {


/**
 * Class to check whether an LTL formula is true on a trace.
 *
 */
class array_trace_checker {
public:
	array_trace_checker();
	virtual ~array_trace_checker();

	bool check(const spot::ltl::formula* node, const std::string *trace);

    bool check(const spot::ltl::atomic_prop* node, const std::string *trace);
    bool check(const spot::ltl::constant* node, const std::string *trace);
    bool check(const spot::ltl::binop* node, const std::string *trace);
    bool check(const spot::ltl::unop* node,  const std::string *trace);
    bool check(const spot::ltl::multop* node,  const std::string *trace);

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
    };

    const std::string END_VAR = "EndOfTraceVar";
};

} /* namespace texada */

#endif /* ARRAYTRACECHECKER2_H_ */
