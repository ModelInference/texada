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

/**
 * Class to check whether an LTL formula holds on a trace in the form of a map.
 * Note that this class uses LONG_MAX as infinity. Once we get to traces that
 * are longer than that, all the long will have to be change to long long.
 */
class map_trace_checker {
public:
	map_trace_checker(std::map<string_event,std::vector<long>>);
	virtual ~map_trace_checker();
	bool check_on_trace(const spot::ltl::formula *);
	struct interval {
		long start = 0;
		long end = LONG_MAX;
	};
	/**
	 * This class uses relative positions to check to occurrence of events. As
	 * such, it has three extra groups of functions: find first, last and all
	 * occurrence(s). These take in the formula and an interval and first the
	 * first, last or all occurrence(s) of the formula in that interval.
	 */
private:
	std::map<string_event,std::vector<long>> trace_map;
	bool check(const spot::ltl::formula *, interval);
	bool check(const spot::ltl::atomic_prop *, interval);
	bool check(const spot::ltl::constant *, interval);
	bool check(const spot::ltl::unop *, interval);
	bool check(const spot::ltl::multop *, interval);
	bool check(const spot::ltl::binop *, interval);

	long find_first_occurrence(const spot::ltl::formula*, interval);
	long find_first_occurrence(const spot::ltl::multop*, interval);
	long find_first_occurrence(const spot::ltl::unop*, interval);
	long find_first_occurrence(const spot::ltl::constant*, interval);
	long find_first_occurrence(const spot::ltl::binop*, interval);
	long find_first_occurrence(const spot::ltl::atomic_prop*,interval);

	long find_last_occurrence(const spot::ltl::formula*, interval);
	long find_last_occurrence(const spot::ltl::atomic_prop*, interval);
	long find_last_occurrence(const spot::ltl::multop*, interval);
	long find_last_occurrence(const spot::ltl::unop*, interval);
	long find_last_occurrence(const spot::ltl::constant*, interval);
	long find_last_occurrence(const spot::ltl::binop*, interval);


    /**
     * automatop and bunop are not supported.
     */
    bool check(const spot::ltl::automatop* node) {
    	std::cerr << "Type automatop unsupported. \n";
    	    	return false;
    }
    bool check(const spot::ltl::bunop* node) {
    	std::cerr << "Type bunop unsupported. \n";
    	return false;
    }
	long find_first_occurrence(const spot::ltl::automatop*){
    	std::cerr << "Type automatop unsupported. \n";
    	    	return -1;
    }
	long find_first_occurrence(const spot::ltl::bunop*){
    	std::cerr << "Type bunop unsupported. \n";
    	    	return -1;
    }

	long find_last_occurrence(const spot::ltl::automatop*){
    	std::cerr << "Type automatop unsupported. \n";
    	    	return -1;
    }
	long find_last_occurrence(const spot::ltl::bunop*){
    	std::cerr << "Type bunop unsupported. \n";
    	    	return -1;
    }

};

} /* namespace texada */

#endif /* MAPTRACECHECKER_H_ */
