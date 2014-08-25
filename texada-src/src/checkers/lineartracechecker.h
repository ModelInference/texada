/*
 * lineartracechecker.h
 *
 *  Created on: May 19, 2014
 *      Author: clemieux
 */

#ifndef LINEARTRACECHECKER_H_
#define LINEARTRACECHECKER_H_

#include <ltlast/allnodes.hh>
#include "../trace/stringevent.h"
#include "boolbasedchecker.h"
#include <iostream>

#include "../instantiation-tools/pregeninstantspool.h"

namespace texada {


/**
 * Class to check whether an LTL formula is true on a trace in the
 * form of a vector.
 *
 */
class linear_trace_checker : public bool_based_checker<const string_event*>{
public:
	linear_trace_checker() {};
	virtual ~linear_trace_checker() {};

	bool check_on_trace(const spot::ltl::formula* node, const string_event *trace);

private:

	virtual bool ap_check(const spot::ltl::atomic_prop* node,
	            const string_event* trace_pt, std::set<int> trace_ids = std::set<int>());
    virtual bool until_check(const spot::ltl::binop* node,
            const string_event* trace_pt, std::set<int> trace_ids = std::set<int>());
    virtual bool release_check(const spot::ltl::binop* node,
            const string_event* trace_pt, std::set<int> trace_ids = std::set<int>());
    virtual bool strongrelease_check(const spot::ltl::binop* node,
                const string_event* trace_pt, std::set<int> trace_ids = std::set<int>());
    virtual bool weakuntil_check(const spot::ltl::binop* node,
            const string_event* trace_pt, std::set<int> trace_ids = std::set<int>());
    virtual bool globally_check(const spot::ltl::unop* node,
            const string_event* trace_pt, std::set<int> trace_ids = std::set<int>());
    virtual bool finally_check(const spot::ltl::unop* node,
            const string_event* trace_pt, std::set<int> trace_ids = std::set<int>());
    virtual bool next_check(const spot::ltl::unop* node,
            const string_event* trace_pt, std::set<int> trace_ids = std::set<int>());



};

vector<map<string, string>> valid_instants_on_traces(
        const spot::ltl::formula * prop_type,
        instants_pool_creator * instantiator,
        shared_ptr<set<vector<string_event>>> traces);

} /* namespace texada */

#endif /* LINEARTRACECHECKER2_H_ */
