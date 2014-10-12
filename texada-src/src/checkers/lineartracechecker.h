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
#include "statistic.h"
#include "finding.h"

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

	statistic check_on_trace(const spot::ltl::formula* node, const string_event *trace);

private:

	virtual statistic ap_check(const spot::ltl::atomic_prop* node,
	            const string_event* trace_pt, std::set<int> trace_ids = std::set<int>());
    virtual statistic until_check(const spot::ltl::binop* node,
            const string_event* trace_pt, std::set<int> trace_ids = std::set<int>());
    virtual statistic release_check(const spot::ltl::binop* node,
            const string_event* trace_pt, std::set<int> trace_ids = std::set<int>());
    virtual statistic strongrelease_check(const spot::ltl::binop* node,
                const string_event* trace_pt, std::set<int> trace_ids = std::set<int>());
    virtual statistic weakuntil_check(const spot::ltl::binop* node,
            const string_event* trace_pt, std::set<int> trace_ids = std::set<int>());
    virtual statistic globally_check(const spot::ltl::unop* node,
            const string_event* trace_pt, std::set<int> trace_ids = std::set<int>());
    virtual statistic finally_check(const spot::ltl::unop* node,
            const string_event* trace_pt, std::set<int> trace_ids = std::set<int>());
    virtual statistic next_check(const spot::ltl::unop* node,
            const string_event* trace_pt, std::set<int> trace_ids = std::set<int>());
    virtual bool is_short_circuiting(statistic result);

};

vector<finding> valid_instants_on_traces(
        const spot::ltl::formula * prop_type,
        instants_pool_creator * instantiator,
        shared_ptr<set<vector<string_event>>> traces);

vector<finding> valid_instants_on_traces(
        const spot::ltl::formula * prop_type,
        instants_pool_creator * instantiator,
        shared_ptr<set<vector<string_event>>> traces,
        int sup_threshold,
        int sup_pot_threshold,
        float conf_threshold,
        bool global_thresholds,
        bool print_stats);

} /* namespace texada */

#endif /* LINEARTRACECHECKER2_H_ */
