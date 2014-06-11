/*
 * maptracechecker.h
 *
 *  Created on: Jun 10, 2014
 *      Author: clemieux
 */

#ifndef MAPTRACECHECKER_H_
#define MAPTRACECHECKER_H_
#include "stringevent.h"
#include <map>
#include <vector>
#include <ltlast/allnodes.hh>
#include <climits>

namespace texada {

class map_trace_checker {
public:
	map_trace_checker(std::map<string_event,std::vector<long>>);
	virtual ~map_trace_checker();
	bool check_on_trace(const spot::ltl::formula *);
	struct interval {
		long start = 0;
		long end = LONG_MAX;
	};
private:
	std::map<string_event,std::vector<long>> trace_map;
	bool check(const spot::ltl::formula *, interval);
	bool check(const spot::ltl::atomic_prop *, interval);
	bool check(const spot::ltl::constant *, interval);
	bool check(const spot::ltl::unop *, interval);
	bool check(const spot::ltl::multop *, interval);
	bool check(const spot::ltl::binop *, interval);
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
	long find_first_occurrence(const spot::ltl::formula*, interval);
	long find_first_occurrence(const spot::ltl::multop*, interval);
	long find_first_occurrence(const spot::ltl::unop*, interval);

public:
	long find_first_occurrence(const spot::ltl::atomic_prop*,interval);
};

} /* namespace texada */

#endif /* MAPTRACECHECKER_H_ */
